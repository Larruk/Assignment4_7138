/*
	Programmer: 7138
	Class Number: COP 4610
	Class Name: Operating Systems
	CRN: 80604
	Professor: Dr. Ingrid Buckley, PhD
	Assignment Number: 4
	Assignment Purpose: To apply Semaphores using the Win32 API
	File Name: Assignment4_7138.cpp
	File Purpose: To apply the purpose of this assignment and be the main folder to hold everything
*/

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

	printf("In  	Out\n"); //Initial required print statement

	//create Win32 INTs to pass to the ThreadCheck function
	INT pNum = PRODUCER_NUM; //hold Producer number (1)
	INT cNum = CONSUMER_NUM; //hold Consumer number (0)

	//Iterate to create producer and consumer threads
	for (int i = 0; i < numOfProducers; i++) {

		//create producer threads
		producers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&pNum, 0, &producerThreadID);

		//create consumer threads
		consumers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCheck, (LPVOID)&cNum, 0, &consumerThreadID);

		//wait for producer threads to release locks
		if (producers[i] != NULL) 
			WaitForSingleObject(consumers[i], INFINITE);

		//wait for consumer threads to release locks
		if (consumers[i] != NULL) 
			WaitForSingleObject(producers[i], INFINITE);

	}

	//wait for all the threads to stop
	WaitForMultipleObjects(numOfProducers, producers, TRUE, INFINITE); //Wait for all producer threads
	WaitForMultipleObjects(numOfConsumers, consumers, TRUE, INFINITE); //Wait for all consumer threads

	return 0;
}

//check if thread is producer or consumer thread
DWORD WINAPI ThreadCheck(LPVOID p) {

	INT * flag = (INT*)p; //Cast parameter p to a Win32 INT
	int numSem = 1; //maximum number of semaphores
	HANDLE sem = CreateSemaphore(NULL, numSem, numSem, NULL); //create the semaphore

	//infinitely check if the number passed belonged  a producer or consumer
	while (true) {
		if (*flag == 1) { //if the number is from a producer, add a number to the queue
			Enqueue();
			ReleaseSemaphore(sem, 0, NULL); //release the semaphore
		} else { //if the number is from a consumer, remove a number from the queue
			Dequeue();
			ReleaseSemaphore(sem, 0, NULL); //release the semaphore
		}
		Sleep(rand() % 500 + 500); //Sleep for a random amount of time between 500 and 1000 milliseconds
	}

	return TRUE;
}

//add number to buffer queue
void Enqueue() {
	if (!IsFull()) { //if the queue is not full
		buffer[rear] = GenerateNumber(); //set buffer[rear] to a random number
		printf("%d\n", buffer[rear]); //print the rear
		rear++; //increment the rear
	}
}

//take number away from queue
void Dequeue() { 
	Sleep(100); //sleep for a tenth of a second to show that a queue was implemented
	if(!IsEmpty()){ //if the queue is not empty
		printf("	%d\n", buffer[front]); //print the number at the front
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
