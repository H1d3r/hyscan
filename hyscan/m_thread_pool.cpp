#include "m_thread_pool.h"


ThreadPool::ThreadPool(DWORD dwMinThreadNum, DWORD dwMaxThreadNum)
{
	// �����������TaskQueue
	this->taskQueue = new TaskQueue();
	
	do{
		// ��ʼ����Ա����
		this->dwMinThreadNum = dwMinThreadNum;
		this->dwMaxThreadNum = dwMaxThreadNum;
		this->workThreadNum = 0;
		
		// �����������߳�
		this->consumeThread = new thread[this->dwMaxThreadNum]; // 50 0-49 
		RtlZeroMemory(this->consumeThread, sizeof(thread)*this->dwMaxThreadNum);
		
		if (this->consumeThread != NULL){
			for (DWORD i = 0; i<this->dwMaxThreadNum; i++){
				this->consumeThread[i] = thread(ThreadPool::work, this);
				this->aliveThreadNum++;
			}
			cout << "[+] Create ConsumeThread Successed" << endl;
		}else{
			cout << "[-] Create ManagerThread Failed" << endl;
			break;
		}

		// �����������߳�
		this->managerThread = thread(ThreadPool::manage, this);
		if (this->consumeThread){
			cout << "[+] Create ManagerThread Successed" << endl;
		}else{
			cout << "[-] Create ConsumeThread Failed" << endl;
			break;
		}
		
	} while (0);

}

ThreadPool::~ThreadPool()
{
	// this->bShutDown = TRUE;
	
	// ����Ҫ�Ե�ǰ�̳߳��л����ڵ��߳̽�������
	this->conditionVariable.notify_all();
	
	// ���ڴ���ͷ�
	if (this->consumeThread != NULL){
		delete[] this->consumeThread;
	}
	
	if (this->taskQueue != NULL){
		delete this->taskQueue;
		this->taskQueue = NULL;
	}
}

void ThreadPool::addTask(Task task){
	if (this->bShutDown){
		return;
	}
	
	this->taskQueue->addTask(task); 
	// ÿ�����������Ļ������Խ���֪ͨ���������߳�
	conditionVariable.notify_one();
}
//////////////////////////////////////
void ThreadPool::addTask(callbackFunc func, map<string, string>* mArgs){
	if (this->bShutDown){
		return;
	}

	this->taskQueue->addTask(func, mArgs);
	// ÿ�����������Ļ������Խ���֪ͨ���������߳�
	conditionVariable.notify_one();
}

void ThreadPool::addTask(weakCallbackFunc func, map<string, string>* mArgs){
	if (this->bShutDown){
		return;
	}

	this->taskQueue->addTask(func, mArgs);
	// ÿ�����������Ļ������Խ���֪ͨ���������߳�
	conditionVariable.notify_one();
}
//////////////////////////////////////

DWORD ThreadPool::getWorkThreadNum(){
	return this->workThreadNum;
}

DWORD ThreadPool::getAliveThreadNum(){
	return this->aliveThreadNum;
}

DWORD ThreadPool::getQueueTaskSize(){
	return this->taskQueue->getQueueTaskSize();
}

// �����ߺ���
void ThreadPool::work(void* pool){
	
	// ���ѵĻ�����ȡ����������е�����������������Ѽ���
	ThreadPool* threadPool = (ThreadPool*)pool;
	
	while (1){
		threadPool->threadMutex.lock();
		while (threadPool->taskQueue->getQueueTaskSize() == 0 && !threadPool->bShutDown){
			threadPool->conditionVariable.wait(threadPool->threadMutex);
			// ������̵߳��������������Ӧ��Ҫ��ɱ�����߳���������Դ
			if (threadPool->exitThreadNum > 0){
				threadPool->exitThreadNum--;
				
				if (threadPool->aliveThreadNum > threadPool->dwMinThreadNum)
				{
					threadPool->aliveThreadNum--;
					threadPool->threadMutex.unlock();
					for (DWORD i = 0; i < threadPool->dwMaxThreadNum; i++){
						if (threadPool->consumeThread[i].get_id() == this_thread::get_id()){
							RtlZeroMemory(&threadPool->consumeThread[i], sizeof(thread));
							return;
						}
					}
				}
			}
		}

		// �����ʱ�̳߳ص�bShutDown��True�Ļ�����ô���ʾ�Ѿ�����Ҫ�̳߳��ˣ���ô��ǰ�������̳߳ض���Ҫ������ɱ
		if (threadPool->bShutDown){
			threadPool->aliveThreadNum--;
			threadPool->threadMutex.unlock();
			for (DWORD i = 0; i < threadPool->dwMaxThreadNum; i++){
				if (threadPool->consumeThread[i].get_id() == this_thread::get_id()){
					RtlZeroMemory(&threadPool->consumeThread[i], sizeof(thread));
					return;
				}
			}
		}
		
		// ����ִ��
		Task task = threadPool->getTask();
		threadPool->workThreadNum++;
		threadPool->threadMutex.unlock();

		// 		task.func(task.mArgs);
		if (task.mArgs->size() == 1){
			string ipAddr;
			map<string, string>::iterator it = task.mArgs->find("ipAddr");
			if (it != task.mArgs->end())
				ipAddr = it->second;
			task.func(ipAddr);
		}
		else if (task.mArgs->size() == 5){
			string serverName;
			string domainName;
			string domainUsername;
			string domainPassword;
			string personalPassword;
			map<string, string>::iterator it;
			it = task.mArgs->find("serverName");
			if (it != task.mArgs->end())
				serverName = it->second;
			it = task.mArgs->find("domainName");
			if (it != task.mArgs->end())
				domainName = it->second;
			it = task.mArgs->find("domainUsername");
			if (it != task.mArgs->end())
				domainUsername = it->second;
			it = task.mArgs->find("domainPassword");
			if (it != task.mArgs->end())
				domainPassword = it->second;
			it = task.mArgs->find("personalPassword");
			if (it != task.mArgs->end())
				personalPassword = it->second;
			task.weakFunc(serverName, domainName, domainUsername, domainPassword, personalPassword);
		}
		delete task.mArgs;
		task.mArgs = nullptr;
		threadPool->threadMutex.lock();
		threadPool->workThreadNum--;
		threadPool->threadMutex.unlock();
	}
}

Task ThreadPool::getTask(){
	return this->taskQueue->getTask();
}

// �����ߺ���

void ThreadPool::manage(void* arg){
	ThreadPool* threadPool = (ThreadPool*)arg;
	DWORD dwQueueSizeCount = 0;
	// ����̳߳�û�йر�, ��һֱ���
	while (!threadPool->bShutDown)
	{
		// ÿ��5s���һ��
		Sleep(1000);
		
		threadPool->threadMutex.lock();
		DWORD dwQueueSize = threadPool->taskQueue->getQueueTaskSize();
		DWORD dwAliveThreadNum = threadPool->getAliveThreadNum();
		DWORD dwWorkThreadNum = threadPool->getWorkThreadNum();
		threadPool->threadMutex.unlock();

		if (dwQueueSize == 0)
			dwQueueSizeCount++;
		if (dwQueueSizeCount == THREAD_CLOSE_NUM)
		{
			threadPool->bShutDown = TRUE;
			threadPool->conditionVariable.notify_all();
			return;
		}
		
		// ÿ�ε�������/��ɱ5���߳�
		DWORD dwAdjustThreadNum = 5;

#ifdef DEBUG
		cout << "[+] TaskSize		----> " << dwQueueSize << endl;
		cout << "[+] workThreadNum	----> " << threadPool->getWorkThreadNum() << endl;
		cout << "[+] aliveThreadNum ----> " << threadPool->getAliveThreadNum() << endl;
#else
#endif
		// ��ǰ������� > �����߳��� && �����߳��� < ����̸߳���
		if (dwQueueSize > dwAliveThreadNum && dwAliveThreadNum < threadPool->dwMaxThreadNum)
		{
			threadPool->threadMutex.lock();
			// ���ʱ�����Ҫ����߳���������
			DWORD dwNum = 0;
			for (DWORD i = 0; i < threadPool->dwMaxThreadNum
				&& dwNum < dwAdjustThreadNum 
				&& dwAliveThreadNum < threadPool->dwMaxThreadNum; i++)
			{
				if (*(DWORD*)&threadPool->consumeThread[i] == 0)
				{
					threadPool->consumeThread[i] = thread(ThreadPool::work, threadPool);
					threadPool->aliveThreadNum++;
					dwNum++;
				}
			}
			threadPool->threadMutex.unlock();
		}
		
		// ���ٶ�����߳�
		// �����߳�*2 < �����߳���Ŀ && �����߳��� > ��С�߳�����
		if (dwWorkThreadNum * 2 < dwAliveThreadNum && dwAliveThreadNum > threadPool->dwMinThreadNum)
		{
			threadPool->threadMutex.lock();
			threadPool->exitThreadNum = dwAdjustThreadNum;
			threadPool->threadMutex.unlock();
			for (DWORD i = 0; i < dwAdjustThreadNum; i++)
			{
				// ֪ͨ�������߳�
				threadPool->conditionVariable.notify_one();
			}
		}
	}
}
