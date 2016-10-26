#include <windows.h>
#include <stdio.h>

//creation of constants
#define BUFFER_SIZE 10
#define PRODUCER_NUM  1
#define CONSUMER_NUM  0

//global variables
const int front = 0;
int rear = 0;
int * buffer = new int[BUFFER_SIZE]; //creation of buffer array
int numOfProducers = 5;
int numOfConsumers = 5;
HANDLE * producers = new HANDLE[numOfProducers];
HANDLE * consumers = new HANDLE[numOfConsumers];

//Win32 DWORD types to thread IDs
DWORD producerThreadID; //holds producer thread IDs
DWORD consumerThreadID; //holds consumer thread IDs

int numSem = 5; //maximum number of semaphores
INT pNum = PRODUCER_NUM; //hold Producer number (1)
INT cNum = CONSUMER_NUM; //hold Consumer number (0)

DWORD WINAPI ThreadCheck(LPVOID p);
void Enqueue();
void Dequeue();
int GenerateNumber();
bool IsEmpty();
bool IsFull();

int main(void) {
	printf("In  	Out\n"); //Initial required print statement

	HANDLE threadSem = CreateSemaphore(NULL, numSem, numSem, L"Global\\sem1"); //create the thread semaphores
	HANDLE readSem = CreateSemaphore(NULL, 1, 1, L"Global\\sem2"); 	//create a binary semaphore
	HANDLE writeSem = CreateSemaphore(NULL, 1, 1, L"Global\\sem3"); //create a binary semaphore

	for (int i = 0; i < numOfProducers; i++) {

		//create producer threads
		producers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&pNum, 0, &producerThreadID);

		//create consumer threads
		consumers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&cNum, 0, &consumerThreadID);

	}

	//wait for all the threads to stop
	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE); //Wait for all producer threads
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE); //Wait for all consumer threads

	// Upon closing, make sure to deallocate handles
	for (int j = 0; j < numOfProducers; j++) {
		CloseHandle(producers[j]);
		CloseHandle(consumers[j]);
	}

	CloseHandle(threadSem);

	return 0;
}

//check if thread is producer or consumer thread
DWORD WINAPI ThreadCheck(LPVOID p) {
	INT * flag = (INT*)p;

	// Find the semaphore handle and open it with full access
	HANDLE tSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\sem1");
	HANDLE rSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\sem2");
	HANDLE wSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\sem3");
	srand(GetCurrentThreadId());

	while (true) {
		DWORD dwWaitResult = WaitForSingleObject(tSem, 0L);

		switch (dwWaitResult)
		{
			// The semaphore object was signaled. 1 = producer, 0 = consumer
			case WAIT_OBJECT_0:
					if (*flag == 1) {
						// Wait for the write semaphore
						while (true) {
							DWORD dwWriteWait = WaitForSingleObject(wSem, 0L);

							if (dwWriteWait == WAIT_OBJECT_0) {
								Enqueue();

								// Release the semaphore when done writing
								if (!ReleaseSemaphore(wSem, 1, NULL))
								{
									printf("ReleaseSemaphore error: %d\n", GetLastError());
								}
								break;
							}
						}						
					}
					else {
						// Wait for the read semaphore
						while (true) {
							DWORD dwReadWait = WaitForSingleObject(rSem, 0L);

							if (dwReadWait == WAIT_OBJECT_0) {
								Dequeue();

								// Release the semaphore when done writing
								if (!ReleaseSemaphore(rSem, 1, NULL))
								{
									printf("ReleaseSemaphore error: %d\n", GetLastError());
								}
								break;
							}
						}
					}

					// Release the semaphore when task is finished
					if (!ReleaseSemaphore(tSem, 1, NULL))
					{
						printf("ReleaseSemaphore error: %d\n", GetLastError());
					}
				break;

			// The semaphore was nonsignaled, so a time-out occurred.
			case WAIT_TIMEOUT:
				//printf("Thread %d: wait timed out\n", GetCurrentThreadId());
				break;
		}
		if (GetKeyState(0x43) && GetKeyState(VK_CONTROL))
			return TRUE;
		Sleep(rand() % 500 + 500);
	}

	return TRUE;
}

void Enqueue() {
	// Add a random number to the queue and increment the rear
	Sleep(100); // Simulate work
	if (!IsFull()) {
		buffer[rear] = GenerateNumber();
		printf("%d\n", buffer[rear]);
		rear++;
	}
	else {
		//printf("Enqueue failed\n");
	}
}

void Dequeue() {
	// If the array is not empty, remove by shifting everything	
	Sleep(100); // Simulate work
	if (!IsEmpty()) {
		printf("	%d\n", buffer[front]);

		// Shift the array over 1
		for (int i = 1; i < BUFFER_SIZE; i++)
			buffer[i - 1] = buffer[i];

		rear--;
	}
	else {
		//printf("Dequeue failed\n");
	}
}

int GenerateNumber() { return rand() % 1000 + 1000; }

bool IsEmpty() { return front == rear; }

bool IsFull() { return rear == BUFFER_SIZE - 1; }