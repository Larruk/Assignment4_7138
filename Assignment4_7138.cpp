#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 10
#define PRODUCER_NUM  1
#define CONSUMER_NUM  0

int front = 0;
int rear = 0;
int * buffer = new int[BUFFER_SIZE];

int numOfProducers = 5;
int numOfConsumers = 5;
HANDLE * producers = new HANDLE[numOfProducers];
HANDLE * consumers = new HANDLE[numOfConsumers];

DWORD WINAPI ThreadCheck(LPVOID p);
void Enqueue();
void Dequeue();
int GenerateNumber();
bool IsEmpty();
bool IsFull();

int main(void) {

	DWORD producerThreadID;
	DWORD consumerThreadID;

	printf("In  	Out\n");

	INT pNum = PRODUCER_NUM;
	INT cNum = CONSUMER_NUM;

	for (int i = 0; i < numOfProducers; i++) {
		producers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&pNum, 0 ,&producerThreadID);

		consumers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&cNum, 0, &consumerThreadID);

		if (producers[i] != NULL) 
			WaitForSingleObject(consumers[i], INFINITE);

		if (consumers[i] != NULL) 
			WaitForSingleObject(producers[i], INFINITE);

	}

	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE);
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE);

	return 0;
}

DWORD WINAPI ThreadCheck(LPVOID p) {

	INT * flag = (INT*)p;
	int numSem = 5;
	DWORD wait;
	HANDLE sem;
	sem = CreateSemaphore(NULL, 1, 1, NULL);

	while (true) {
		if (*flag == 1) {
			Enqueue();
			ReleaseSemaphore(sem, 0, NULL);
		}
		else {
			Dequeue();
			ReleaseSemaphore(sem, 0, NULL);
		}
		Sleep(rand() % 500 + 500);
	}

	return TRUE;
}

void Enqueue() {

	if (!IsFull()) {
		buffer[rear] = GenerateNumber();
		printf("%d\n", buffer[rear]);
		rear++;
	}

}

void Dequeue() {
	if(!IsEmpty()){
		int numToPrint = buffer[0];
		printf("		%d\n", numToPrint);

		for (int i = 1; i < BUFFER_SIZE; i++)
			buffer[i - 1] = buffer[i];
			
		rear--;
	}
}

int GenerateNumber() { return rand() % 1000 + 1000; }

bool IsEmpty() { return front == rear; }

bool IsFull() { return rear == BUFFER_SIZE - 1; }
