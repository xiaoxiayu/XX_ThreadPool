#ifndef _XX_THREADPOOL_H_
#define _XX_THREADPOOL_H_

#if _MSC_VER && !__INTEL_COMPILER
#include "xx_threadpool_win.h"
#else
#include "xx_threadpool_unix.h"
#endif

CXXWorkQueue* XX_Thread_CreateWorkQueue(int iMaxThreadNum);

int			XX_Thread_InsertWorkItem(const CXXWorkQueue* pWorkQueue, CXXThreadWorker* pWorker);

void		XX_Thread_Destory(CXXWorkQueue* pWorkQueue, int iWaitInterval=1);

void		XX_Thread_DestoryForce(CXXWorkQueue* pWorkQueue);

#endif // _XX_THREADPOOL_H_







