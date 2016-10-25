// assignment4__5973.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//standard library header files calls
#include <windows.h>
#include <stdio.h>

//creation of constants
#define BUFFER_SIZE 10 //declare max buffer array length
#define PRODUCER_NUM  1 //number assigned to producer thread
#define CONSUMER_NUM  0 //number assignmed to consumer thread

//global variables
const int front = 0; //constant int front for the front of the array
int rear = 0; //holds rear of the queue
int * buffer = new int[BUFFER_SIZE]; //creation of buffer array
int numOfProducers = 5; //maximum number of producer threads
int numOfConsumers = 5; //maximum number of consumer threads
HANDLE * producers = new HANDLE[numOfProducers]; //creation of producer threads
HANDLE * consumers = new HANDLE[numOfConsumers]; //creation of consumer threads
												 //function prototypes
DWORD WINAPI ThreadCheck(LPVOID p); //check if thread is producer or consumer thread
void Enqueue(); //add number to buffer queue
void Dequeue(); //take number away from queue
int GenerateNumber(); //generate a random number between 1000 and 2000
bool IsEmpty(); //check if buffer is empty
bool IsFull(); //check if buffer is full

			   //main function
int main(void) {

	//Win32 DWORD types to thread IDs
	DWORD producerThreadID; //holds producer thread IDs
	DWORD consumerThreadID; //holds consumer thread IDs

	int numSem = 1; //maximum number of semaphores

	printf("In  		Out\n"); //Initial required print statement

							 //create Win32 INTs to pass to the ThreadCheck function
	INT pNum = PRODUCER_NUM; //hold Producer number (1)
	INT cNum = CONSUMER_NUM; //hold Consumer number (0)
	HANDLE sem = CreateSemaphore(NULL, numSem, numSem, L"Global\\sem1"); //create the semaphore	

	//Iterate to create producer and consumer threads
	for (int i = 0; i < numOfProducers; i++) {

		//create producer threads
		producers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&pNum, 0, &producerThreadID);

		//create consumer threads
		consumers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&cNum, 0, &consumerThreadID);

	}

	//wait for all the threads to stop
	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE); //Wait for all producer threads
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE); //Wait for all consumer threads

	return 0;
}

//check if thread is producer or consumer thread
DWORD WINAPI ThreadCheck(LPVOID p) {
	INT * flag = (INT*)p; //Cast parameter p to a Win32 INT
	HANDLE sem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"Global\\sem1");
	srand(GetCurrentThreadId());

	while (true) {
		DWORD dwWaitResult = WaitForSingleObject(sem, rand() % 500 + 500);

		switch (dwWaitResult)
		{
			// The semaphore object was signaled.
			case WAIT_OBJECT_0:
				if (*flag == 1) { //if the number is from a producer, add a number to the queue
					Enqueue();
				}
				else {
					Dequeue();
				}

				// Simulate thread spending time on task
				Sleep(5);

				// Release the semaphore when task is finished
				if (!ReleaseSemaphore(sem, 1, NULL))
				{
					printf("ReleaseSemaphore error: %d\n", GetLastError());
				}
				break;

				// The semaphore was nonsignaled, so a time-out occurred.
			case WAIT_TIMEOUT:
				//printf("Thread %d: wait timed out\n", GetCurrentThreadId());
				break;
		}

		Sleep(rand() % 500 + 500);

	}

	return TRUE;
}

//add number to buffer queue
void Enqueue() {
	if (!IsFull()) { //if the queue is not full
		buffer[rear] = GenerateNumber(); //set buffer[rear] to a random number
		printf("%d: %d\n", GetCurrentThreadId(), buffer[rear]); //print the rear
		rear++; //increment the rear
	}
}

//take number away from queue
void Dequeue() {
	Sleep(100); //sleep for a tenth of a second to show that a queue was implemented
	if (!IsEmpty()) { //if the queue is not empty
		printf("		%d: %d\n", GetCurrentThreadId(), buffer[front]); //print the number at the front
		for (int i = 1; i < BUFFER_SIZE; i++) //move all the array values up one
			buffer[i - 1] = buffer[i];

		rear--; //decrement the rear
	}
}

//generate a random number between 1000 and 2000
int GenerateNumber() { return rand() % 1000 + 1000; }

//check if buffer is empty
bool IsEmpty() { return front == rear; }

//check if buffer is full
bool IsFull() { return rear == BUFFER_SIZE - 1; }
