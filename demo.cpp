#include "xx_threadpool.h" 

#include <stdio.h>
#include <iostream>

class CMyTest : public CXXThreadWorker {
public:
	void   DoWork(void* pThreadContext);
	void   Abort ();
};

void CMyTest::DoWork(void* pThreadContext) {              // ���̻߳��������е�����.
	std::cout << "ThreadID: " << GetThreadID() << std::endl;
    Sleep(1000);
       //  ASSERT_EQ(0, 1) << "ThreadID: " << GetThreadID();
}

void CMyTest::Abort() {        // �̱߳����ٺ�û�����е���������е����
	std::cout << "Thread Abort.";
}

int main(int argc, char** argv) {
	CXXWorkQueue* pWorkQueue = XX_Thread_CreateWorkQueue(3/*, pThreadDataArray*/);	// ����3 ���߳�

	CMyTest *pRunItem = new CMyTest();	// �����࣬�̳���CXXThreadWorker����������DoWork�µĶ�������߳����С�Ҳ����һ���߳�һ���ࡣ

	for (int i = 0; i < 10; i++) {		// ����10����������3��3�������У���Ϊ���洴����3���̡߳�
		XX_Thread_InsertWorkItem(pWorkQueue, pRunItem);	// ��������Ĺ��̿��Ը��ݲ��������Լ���װ��
	}
	XX_Thread_Destory(pWorkQueue /* ,1*/);		// ���١���ȴ�ȫ��������������٣��ȴ���ʱ�����ɵ�2������������Ĭ��Ϊ1��.
	// FXQA_Thread_DestoryForce(pWorkQueue);	// ���١����ȴ�������ûȫ������Ҳȫ������

	delete pRunItem;

	return 1;
}
