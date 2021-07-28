
/*
Student Name: 		Matthew Burley
Student ID:		191847490
GitHub Login ID:	rusty-p-shackleford

Student Name: 		Michael Burley
Student ID: 		191821710
GitHub Login ID:	michael-k-burley

GitHub Repository URL: https://github.com/rusty-p-shackleford/Assignment4
*/

//Imports
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//Global Variables
const int true = 1;
const int false = 0;
int NUM_RESOURCES;
int NUM_CUSTOMERS = 0;	
int LINE_MAX_CHAR = 10;
int EXPECTED_NUM_CMDS = 6;

//Function Declarations
void initBanker();
void freeBanker();
char** readFileIntoArr();
void interact();

int NotFindSafeSequence();
int requestResource();
int releaseResource();

void printAllResources();
void printAvailableArr();
void printMaximumArr();
void printAllocatedArr();
void printNeededArr();
void printCustomerInfo();

struct Bank {	//NOTE: maximum = allocated + needed

	int *available;
	int **maximum;
	int **allocated;
	int **needed;
	
	//Can only have one flexible array member at end of structs
} banker;

int NotFindSafeSequence(int printSafeSeq){

	int work[NUM_RESOURCES];
	int finish[NUM_CUSTOMERS];
	int safeSeq[NUM_CUSTOMERS];
	int currentIndex = 0;
	int lastCompletedIndex = 0;
	int count = 0;
	int ready = true;

	//Populate work with values from available
	for(int i = 0; i < NUM_RESOURCES; i++){
	
		work[i] = banker.available[i];
	}

	//Populate finish for each customer to false
	for(int i = 0; i < NUM_CUSTOMERS; i++){
			
		finish[i] = false;
	}

	do{
		//If customer with current index is not finished
		if(finish[currentIndex] == false){

			ready = true;

			//Check if customer needs less resources than what is available
			for(int i = 0; i < NUM_RESOURCES; i++){

				if(banker.needed[currentIndex][i] > work[i]){

					ready = false;

				}

			}
	
			//If customer needs resources less than or equal to what is available
			if(ready == true){

				//Assign to work array the currently allocated customers resources
				for(int i = 0; i < NUM_RESOURCES; i++){

					work[i] += banker.allocated[currentIndex][i];

				}

				//Set customer finished to true now
				finish[currentIndex] = true;

				//Add the index of the now finished customer to the safe sequence array
				safeSeq[count] = currentIndex;
				count++;
				
				//Set the last completed index to the index of the last customer to finish
				lastCompletedIndex = currentIndex;

				//Print customer into for current index and available work array
				printCustomerInfo(currentIndex, work);

			}
		}
		//Get index of next customer
		currentIndex = (currentIndex + 1) % NUM_CUSTOMERS;

	}while(currentIndex != lastCompletedIndex);	//If current index is equal to last completed then full loop done without finishing a new customer

	//If all customers not finished then in unsafe state
	for(int i = 0; i < NUM_CUSTOMERS; i++){

		if(finish[i] == false){

			return -1;

		}

	}
		

	//If this far then there exists a safe sequence
	if(printSafeSeq){
	
		//Print safeSeq here, along with printCustomerInfo
		printf("<");
		for(int i = 0; i < NUM_CUSTOMERS; i++){

			printf("%d", safeSeq[i]);
		
			if(i != NUM_CUSTOMERS-1){
				printf(", ");
			}
		}
		printf(">\n\n");

	}

	return 0;	//Safe Sequence found
}	

int requestResource(int custNum, int *userInputArr){

	//Check to make sure that banker has available all resources being requested and customer isn't trying to request more resources than its maximum
	for(int i = 0; i < NUM_RESOURCES; i++){

		if(userInputArr[i] > banker.available[i] || (userInputArr[i] + banker.allocated[custNum][i]) > banker.maximum[custNum][i]){

			return -1; // Error

		}
	}

	//If this far then banker has all resources that are being requested
	for(int i = 0; i < NUM_RESOURCES; i++){	


		//Remove request resources from available pool
		banker.available[i] -= userInputArr[i];

		//Assign requested resources to requesting customer	
		banker.allocated[custNum][i] += userInputArr[i];
	
		//Decrement the amount of the resource the current customer needs by the amount being assigned
		banker.needed[custNum][i] -= userInputArr[i];
		
	}

	//If cannot find a safe sequence then release resources
	

	return 0;	//Successful allocation
}

int releaseResource(int custNum, int *userInputArr){

	//Check to make sure that customer has alloted all resources they are trying to release
	for(int i = 0; i < NUM_RESOURCES; i++){

		if(userInputArr[i] > banker.allocated[custNum][i]){

			return -1; // Error

		}
	}

	//If this far then customer has all resources it is trying to release
	for(int i = 0; i < NUM_RESOURCES; i++){

		
		//Unallocated resources from customer who is releasing
		banker.allocated[custNum][i] -= userInputArr[i];

		//And assign freed resources back to available pool
		banker.available[i] += userInputArr[i];

		//Increment the amount of the resource the current customer needs by the amount being released
		banker.needed[custNum][i] += userInputArr[i];
		
	}

	return 0;	//Successful release	
}
