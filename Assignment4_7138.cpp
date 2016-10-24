#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 10
#define PRODUCER_NUM  1
#define CONSUMER_NUM  0

int * bufArray = new int[BUFFER_SIZE];

int bufArrLength = sizeof(bufArray) / sizeof(int);

DWORD WINAPI ThreadDecision(LPVOID p);
int GenerateNumber();

int main(void) {

	int numSem = 5;
	int numOfProducers = 5;
	int numOfConsumers = 5;

	BOOL producerOrConsumer;

	DWORD producerThreadID;
	DWORD consumerThreadID;

	HANDLE * producers = new HANDLE[numOfProducers];
	HANDLE * consumers = new HANDLE[numOfConsumers];

	printf("In  	Out\n");

	INT pNum = PRODUCER_NUM;

	for (int i = 0; i < numOfProducers; i++) {
		producers[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadDecision,
			(LPVOID)&pNum,
			0,
			&producerThreadID
		);

		if (producers[i] == NULL) {
			printf("CreateThread error %d", GetLastError());
			return 1;
		}

	}

	INT cNum = CONSUMER_NUM;

	for (int i = 0; i < numOfConsumers; i++) {
		consumers[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadDecision,
			(LPVOID)&cNum,
			0,
			&consumerThreadID
		);

		if (consumers[i] == NULL) {
			printf("CreateThread error %d", GetLastError());
			return 1;
		}

	}

	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE);
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE);

	return 0;

}

DWORD WINAPI ThreadDecision(LPVOID p) {
	
	INT * flag = (INT*)p;
	DWORD wait;
	HANDLE sem;
	sem = CreateSemaphore(
		NULL,
		5,
		5,
		NULL
	);

	if (sem == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}

	while (true) {
		if (*flag == 1) {
			printf("%d\n", GenerateNumber());
			ReleaseSemaphore(sem, 0, NULL);
		} else {
			WaitForSingleObject(sem, 0L);
			printf("		Consumer\n");
		}
		Sleep(rand() % 500 + 500);
	}

	return TRUE;
}

int GenerateNumber() { return rand() % 1000 + 1000; }