#ifndef _XX_THREADPOOL_WIN_H_
#define _XX_THREADPOOL_WIN_H_

#include <queue>
#include <windows.h>
#include <assert.h>

//#include<vcl.h>
class CXXWorkQueue;


class CXXThreadWorker
{
public:
	virtual void   DoWork(void* pThreadContext)    = 0;
	virtual void   Abort () = 0;
	friend CXXWorkQueue;

	virtual unsigned long  GetThreadID() {	
		return GetCurrentThreadId(); 
	}
};


typedef std::queue<CXXThreadWorker*>           WorkItemQueue,*PWorkItemQueue;

/*------------------------------------------------------------------------
CXXWorkQueue
This is the WorkOueue class also known as thread pool,
the basic idea of this class is creating thread that are waiting on a queue
of work item when the queue is inserted with items the threads wake up and
perform the requered work and go to sleep again.
------------------------------------------------------------------------*/
class  CXXWorkQueue
{
public:
	virtual ~CXXWorkQueue(){};
	bool Create(const unsigned int       nNumberOfThreads,
		void*                    *pThreadDataArray             = NULL);
	int InsertWorkItem(CXXThreadWorker* pWorkItem);
	void Destory(int iWairSecond=1);
	void DestoryForce();
	int GetThreadTotalNum();

private:
	static unsigned long __stdcall ThreadFunc( void* pParam );
	CXXThreadWorker* RemoveWorkItem();
	int GetWorekQueueSize();
	enum{
		ABORT_EVENT_INDEX = 0,
		SEMAPHORE_INDEX,
		NUMBER_OF_SYNC_OBJ,
	};
	
	PHANDLE                  m_phThreads;
	unsigned int             m_nNumberOfThreads;
	void*                    m_pThreadDataArray;
	HANDLE                   m_phSincObjectsArray[NUMBER_OF_SYNC_OBJ];
	CRITICAL_SECTION         m_CriticalSection;
	PWorkItemQueue           m_pWorkItemQueue;
};

#endif // _XX_THREADPOOL_WIN_H_



