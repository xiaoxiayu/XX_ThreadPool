
#include "xx_threadpool_win.h"

typedef struct _THREAD_CONTEXT
{
	CXXWorkQueue* pWorkQueue;
	void*       pThreadData;
} THREAD_CONTEXT,*PTHREAD_CONTEXT;

bool CXXWorkQueue::Create(const unsigned int  nNumberOfThreads,
	void*         *ThreadData      /*=NULL*/) {
	m_pWorkItemQueue = new WorkItemQueue();
	if(NULL == m_pWorkItemQueue ) {
		return false;
	}

	m_phSincObjectsArray[SEMAPHORE_INDEX] = CreateSemaphore(NULL,0,LONG_MAX,NULL);
	if(m_phSincObjectsArray[SEMAPHORE_INDEX] == NULL) {
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		return false;
	}

	m_phSincObjectsArray[ABORT_EVENT_INDEX] = CreateEvent(NULL,TRUE,FALSE,NULL);
	if(m_phSincObjectsArray[ABORT_EVENT_INDEX]  == NULL)
	{
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
		return false;
	}

	InitializeCriticalSection(&m_CriticalSection);

	m_phThreads = new HANDLE[nNumberOfThreads];
	if(m_phThreads == NULL) {
		delete m_pWorkItemQueue;
		m_pWorkItemQueue = NULL;
		CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
		CloseHandle(m_phSincObjectsArray[ABORT_EVENT_INDEX]);
		DeleteCriticalSection(&m_CriticalSection);
		return false;
	}
	unsigned int i;
	m_nNumberOfThreads = nNumberOfThreads;
	DWORD dwThreadId;
	PTHREAD_CONTEXT pThreadsContext;

	for(i = 0 ; i < nNumberOfThreads ; i++ ) {
		pThreadsContext = new THREAD_CONTEXT;
		pThreadsContext->pWorkQueue  = this;
		pThreadsContext->pThreadData = ThreadData == NULL? NULL : ThreadData[i];

		m_phThreads[i] = CreateThread(NULL,
			0,
			CXXWorkQueue::ThreadFunc,
			pThreadsContext,
			0,
			&dwThreadId);
		if(m_phThreads[i] == NULL) {
			delete pThreadsContext;
			m_nNumberOfThreads = i;
			Destory(5);
			return false;
		}
	}
	return true;
}

int CXXWorkQueue::InsertWorkItem(CXXThreadWorker* pWorkItem) {
	assert(pWorkItem != NULL);

	EnterCriticalSection(&m_CriticalSection);

	m_pWorkItemQueue->push(pWorkItem);

	LeaveCriticalSection(&m_CriticalSection);

	if (!ReleaseSemaphore(m_phSincObjectsArray[SEMAPHORE_INDEX],1,NULL)) {
		assert(false);
		return false;
	}
	return true;
}

CXXThreadWorker*  CXXWorkQueue::RemoveWorkItem() {
	CXXThreadWorker* pWorkItem;

	EnterCriticalSection(&m_CriticalSection);

	pWorkItem = m_pWorkItemQueue->front();
	m_pWorkItemQueue->pop();

	LeaveCriticalSection(&m_CriticalSection);
	assert(pWorkItem != NULL);
	return pWorkItem;
}

unsigned long __stdcall CXXWorkQueue::ThreadFunc( void*  pParam ) {
	PTHREAD_CONTEXT       pThreadContext =  (PTHREAD_CONTEXT)pParam;
	CXXThreadWorker*         pWorkItem      = NULL;
	CXXWorkQueue*           pWorkQueue     = pThreadContext->pWorkQueue;
	void*                 pThreadData    = pThreadContext->pThreadData;
	DWORD dwWaitResult;
	for(;;) {
		dwWaitResult = WaitForMultipleObjects(NUMBER_OF_SYNC_OBJ,pWorkQueue->m_phSincObjectsArray,FALSE,INFINITE);

		switch(dwWaitResult - WAIT_OBJECT_0) {
		case ABORT_EVENT_INDEX:
			delete pThreadContext;
			return 0;
		case SEMAPHORE_INDEX:
			pWorkItem = pWorkQueue->RemoveWorkItem();
			if(pWorkItem == NULL) {
				assert(false);
				break;
			}

			pWorkItem->DoWork(pThreadData);
			break;
		default:
			assert(false);
			delete pThreadContext;
			return 0;
		}
	}

	delete pThreadContext;
	return 1;
}

int CXXWorkQueue::GetThreadTotalNum() {
	return m_nNumberOfThreads;
}

int CXXWorkQueue::GetWorekQueueSize() {
	EnterCriticalSection(&m_CriticalSection);
	int iWorkQueueSize = m_pWorkItemQueue->size();

	LeaveCriticalSection(&m_CriticalSection);
	return iWorkQueueSize;
}

void CXXWorkQueue::Destory(int iWaitSecond) {
	while(0 != GetWorekQueueSize()) {
		Sleep(iWaitSecond*1000);
	}

	if(!SetEvent(m_phSincObjectsArray[ABORT_EVENT_INDEX])) {
		assert(false);
		return;
	}

	//wait for all the threads to end
	WaitForMultipleObjects(m_nNumberOfThreads,m_phThreads,true,INFINITE);

	//clean queue
	while(!m_pWorkItemQueue->empty()) {
		m_pWorkItemQueue->front()->Abort();
		m_pWorkItemQueue->pop();
	}
	delete m_pWorkItemQueue;
	m_pWorkItemQueue = NULL;
	CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
	CloseHandle(m_phSincObjectsArray[ABORT_EVENT_INDEX]);
	DeleteCriticalSection(&m_CriticalSection);
	//close all threads handles
	for(int i = 0 ; i < m_nNumberOfThreads ; i++)
		CloseHandle(m_phThreads[i]);
	delete[] m_phThreads;
}

void CXXWorkQueue::DestoryForce() {
	if(!SetEvent(m_phSincObjectsArray[ABORT_EVENT_INDEX])) {
		assert(false);
		return;
	}
	//wait for all the threads to end
//	WaitForMultipleObjects(m_nNumberOfThreads,m_phThreads,true,INFINITE);
	//clean queue
	while(!m_pWorkItemQueue->empty()) {
		m_pWorkItemQueue->front()->Abort();
		m_pWorkItemQueue->pop();
	}
	delete m_pWorkItemQueue;
	m_pWorkItemQueue = NULL;
	CloseHandle(m_phSincObjectsArray[SEMAPHORE_INDEX]);
	CloseHandle(m_phSincObjectsArray[ABORT_EVENT_INDEX]);
	DeleteCriticalSection(&m_CriticalSection);
	//close all threads handles
	for(int i = 0 ; i < m_nNumberOfThreads ; i++)
		CloseHandle(m_phThreads[i]);
	delete[] m_phThreads;
}