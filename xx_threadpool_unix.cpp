
#include "xx_threadpool_unix.h"

CThreadPool* CXXWorkQueue::pool = NULL;

typedef struct _THREAD_CONTEXT {
	CXXWorkQueue* pWorkQueue;
	void*       pThreadData;
} THREAD_CONTEXT,*PTHREAD_CONTEXT;

void CXXWorkQueue::Create(int max_thread_num, void* *ThreadData) {
    pool = new CThreadPool();
    pthread_mutex_init (&(pool->queue_lock), NULL);
    pthread_cond_init (&(pool->queue_ready), NULL);
    pool->queue_head = NULL;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->shutdown = 0;
    //pool->threadid =
    //    (pthread_t *) malloc (max_thread_num * sizeof (pthread_t));
    pool->threadid = new pthread_t[max_thread_num];

    PTHREAD_CONTEXT pThreadsContext;
    for (int i = 0; i < max_thread_num; i++) { 
	pThreadsContext = new THREAD_CONTEXT;
	pThreadsContext->pWorkQueue  = this;
	pThreadsContext->pThreadData = ThreadData == NULL? NULL : ThreadData[i];
        pthread_create(&(pool->threadid[i]), NULL, thread_routine, pThreadsContext);
    }
}

int CXXWorkQueue::InsertWorkItem(CXXThreadWorker* newworker) {
    newworker->next = NULL;
 
    pthread_mutex_lock (&(pool->queue_lock));

    CXXThreadWorker *member = pool->queue_head;
    if (member != NULL) {
        while (member->next != NULL)
            member = member->next;
        member->next = newworker;
    } else {
        pool->queue_head = newworker;
    }

    assert (pool->queue_head != NULL);
    pool->cur_queue_size++;
    pthread_mutex_unlock (&(pool->queue_lock));
 
    pthread_cond_signal (&(pool->queue_ready));
    return 0;
}

void CXXWorkQueue::Destory(int iWaitSecond) {
    if (pool->shutdown)
        return; 

    while (pool->cur_queue_size != 0) {
        sleep(1 * iWaitSecond);
    }
    pool->shutdown = 1;

    pthread_cond_broadcast (&(pool->queue_ready));

    for (int i = 0; i < pool->max_thread_num; i++)
        pthread_join (pool->threadid[i], NULL);

    delete[] pool->threadid;

	while ((pool->queue_head != NULL) && 
		(pool->cur_queue_size != 0)) {
			CXXThreadWorker* worker = pool->queue_head;
			worker->Abort();
			delete pool->queue_head;
			pool->queue_head = NULL;
	}

    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    
    delete pool;
    pool = NULL;

    return;
}

void CXXWorkQueue::DestoryForce() {
	if (pool->shutdown)
		return; 

	pool->shutdown = 1;

	pthread_cond_broadcast (&(pool->queue_ready));

	for (int i = 0; i < pool->max_thread_num; i++)
		pthread_join (pool->threadid[i], NULL);

	delete[] pool->threadid;

	while ((pool->queue_head != NULL) && 
		(pool->cur_queue_size != 0)) {
			CXXThreadWorker* worker = pool->queue_head;
			worker->Abort();
			delete pool->queue_head;
			pool->queue_head = NULL;
	}

	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_ready));

	delete pool;
	pool = NULL;

	return;
}

void* CXXWorkQueue::thread_routine (void *pParam) {
    //printf ("starting thread 0x%x\n", pthread_self ());
	PTHREAD_CONTEXT       pThreadContext =  (PTHREAD_CONTEXT)pParam;
	CXXWorkQueue*           pWorkQueue     = pThreadContext->pWorkQueue;
	void*                 pThreadData    = pThreadContext->pThreadData;
    while (1)
    {
        pthread_mutex_lock (&(pool->queue_lock));
        
        while (pool->cur_queue_size == 0 && !pool->shutdown) {
            //printf ("thread 0x%x is waiting/n", pthread_self ());
            pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock));
        }

        /* Destory */
        if (pool->shutdown) {
            pthread_mutex_unlock (&(pool->queue_lock));
            //printf ("thread 0x%x will exit/n", pthread_self ());
            pthread_exit (NULL);
        }
        //printf ("thread 0x%x is starting to work/n", pthread_self ());

        assert (pool->cur_queue_size != 0);
        assert (pool->queue_head != NULL);
        
        pool->cur_queue_size--;
        CXXThreadWorker* worker = pool->queue_head;
        pool->queue_head = worker->next;

        pthread_mutex_unlock (&(pool->queue_lock));

		worker->DoWork(pThreadData);
    }

    pthread_exit (NULL);
}
