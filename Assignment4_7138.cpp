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

	int numOfProducers = 5;
	int numOfConsumers = 5;

	DWORD producerThreadID;
	DWORD consumerThreadID;

	HANDLE * producers = new HANDLE[numOfProducers];
	HANDLE * consumers = new HANDLE[numOfConsumers];

	printf("In  	Out\n");

	INT pNum = PRODUCER_NUM;
	INT cNum = CONSUMER_NUM;

	for (int i = 0; i < numOfProducers; i++) {
		producers[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadDecision,
			(LPVOID)&pNum,
			0,
			&producerThreadID
		);

		if (consumers[i] != NULL) {
			WaitForSingleObject(producers[i], INFINITE);
		}

	}

	

	for (int i = 0; i < numOfConsumers; i++) {
		consumers[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadDecision,
			(LPVOID)&cNum,
			0,
			&consumerThreadID
		);

		if (consumers[i] != NULL) {
			WaitForSingleObject(producers[i], INFINITE);
		}

	}

	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE);
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE);

	return 0;

}

DWORD WINAPI ThreadDecision(LPVOID p) {

	INT * flag = (INT*)p;
	int numSem = 5;
	DWORD wait;
	HANDLE sem;
	sem = CreateSemaphore(
		NULL,
		1,
		1,
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
		}
		else {
			printf("		Consumer\n");
			ReleaseSemaphore(sem, 0, NULL);
		}
		Sleep(rand() % 500 + 500);
	}

	return TRUE;
}

int GenerateNumber() { return rand() % 1000 + 1000; }
