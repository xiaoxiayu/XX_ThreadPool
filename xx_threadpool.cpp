
#if _MSC_VER && !__INTEL_COMPILER
#include "xx_threadpool_win.cpp"
#else
#include "xx_threadpool_unix.cpp"
#endif

CXXWorkQueue* XX_Thread_CreateWorkQueue(int iMaxThreadNum) {
	CXXWorkQueue* pWorkQueue = new CXXWorkQueue();
	pWorkQueue->Create(iMaxThreadNum);

	return pWorkQueue;
}

int			XX_Thread_InsertWorkItem(const CXXWorkQueue* pWorkQueue, CXXThreadWorker* pWorker) {
	if (pWorkQueue == NULL)
		return -1;

	if (pWorker == NULL) 
		return -1;

	return ((CXXWorkQueue*)pWorkQueue)->InsertWorkItem(pWorker);
}

void		XX_Thread_Destory(CXXWorkQueue* pWorkQueue, int iWaitInterval) {
	if (pWorkQueue == NULL)
		return;

	pWorkQueue->Destory(iWaitInterval);
	delete pWorkQueue;
	pWorkQueue = NULL;
}

void		XX_Thread_DestoryForce(CXXWorkQueue* pWorkQueue) {
	if (pWorkQueue == NULL)
		return;

	pWorkQueue->DestoryForce();
	delete pWorkQueue;
	pWorkQueue = NULL;
}