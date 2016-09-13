#include "xx_threadpool.h" 

#include <stdio.h>
#include <iostream>

class CMyTest : public CXXThreadWorker {
public:
	void   DoWork(void* pThreadContext);
	void   Abort ();
};

void CMyTest::DoWork(void* pThreadContext) {              // 多线程环境下运行的内容.
	std::cout << "ThreadID: " << GetThreadID() << std::endl;
    Sleep(1000);
       //  ASSERT_EQ(0, 1) << "ThreadID: " << GetThreadID();
}

void CMyTest::Abort() {        // 线程被销毁后，没被运行的任务会运行到这里。
	std::cout << "Thread Abort.";
}

int main(int argc, char** argv) {
	CXXWorkQueue* pWorkQueue = XX_Thread_CreateWorkQueue(3/*, pThreadDataArray*/);	// 创建3 个线程

	CMyTest *pRunItem = new CMyTest();	// 创建类，继承自CXXThreadWorker，这个类里的DoWork下的东西会多线程运行。也可以一个线程一个类。

	for (int i = 0; i < 10; i++) {		// 创建10个任务，它会3个3个的运行，因为上面创建了3个线程。
		XX_Thread_InsertWorkItem(pWorkQueue, pRunItem);	// 创建任务的过程可以根据测试需求自己封装。
	}
	XX_Thread_Destory(pWorkQueue /* ,1*/);		// 销毁。会等待全部任务结束再销毁，等待的时间间隔由第2个参数决定，默认为1秒.
	// FXQA_Thread_DestoryForce(pWorkQueue);	// 销毁。不等待，任务没全运行完也全结束。

	delete pRunItem;

	return 1;
}
