#ifndef _XX_THREADPOOL_UNIX_H_
#define _XX_THREADPOOL_UNIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include <queue>

class CXXThreadWorker
{
public:
    virtual void   DoWork(void* pThreadContext) = 0;
    virtual void   Abort () = 0;
	CXXThreadWorker *next;

	virtual unsigned long GetThreadID() {	
#if defined(__APPLE__)
		return (unsigned long)pthread_self();
#else
#	if defined(_M_IX86 )
		return (unsigned long) syscall(224);
#	else
		return (unsigned long) syscall(186);
#	endif
#endif
	}
};

typedef struct {
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
    CXXThreadWorker *queue_head;
    int shutdown;
    pthread_t *threadid;
    int max_thread_num;
    int cur_queue_size;
} CThreadPool;


class CXXWorkQueue {
	static CThreadPool *pool;
public:
	virtual ~CXXWorkQueue() {};

	void Create(int max_thread_num, void** pThreadDataArray=NULL);

	int InsertWorkItem(CXXThreadWorker* newworker);

	void Destory(int iWaitSecond=1);

	void DestoryForce();

	static void* thread_routine (void *arg);
};

#endif // _XX_THREADPOOL_UNIX_H_







