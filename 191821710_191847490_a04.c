
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

int main(int argc, char *argv[]){

	if(argc == 1){
		printf("Error: Main -- Command line arguments not provided.\n");
		return -1;
	}else{
		NUM_RESOURCES = argc - 1;	//Since first arg is program execute command (i.e. ./a04)
	}
	
	//Read file contents into multi dimensional char array
	char** customersMaximums = readFileIntoArr();

	//Print Initial Output
	printf("Number of Customers: %d\n", NUM_CUSTOMERS);

	//Initialize banker with maximum values read from file
	initBanker(customersMaximums);

	//Read command line arguments into bankers available array and print currently available resources
	printf("Currently Available Resources: ");
	for(int i = 0; i < NUM_RESOURCES; i++){	

		sscanf(argv[i+1], "%d", &(banker.available[i]));	//Needs to be (i+1) since argv[0] is ./a04.c 
		printf("%d ", banker.available[i]);
	}

	//Print Remaining Initial Output
	printf("\nMaximum Resources from File:\n");
	printMaximumArr(0);

	//Prompt and Handle User Input
	interact();

	//Free memory for dynamically allocated arrays
	freeBanker();

	printf("Program Exited Successfully.\n");

	return 0;
}

void initBanker(char** fileArray){
	
	banker.available = (int*) calloc(NUM_RESOURCES, sizeof(int));

	banker.maximum = (int**) malloc(NUM_CUSTOMERS * sizeof(int*));
	banker.allocated = (int**) malloc(NUM_CUSTOMERS * sizeof(int*));
	banker.needed = (int**) malloc(NUM_CUSTOMERS * sizeof(int*));

	for(int i = 0; i < NUM_CUSTOMERS; i++){
	
		banker.maximum[i] = (int*) calloc(NUM_RESOURCES, sizeof(int));
		banker.allocated[i] = (int*) calloc(NUM_RESOURCES, sizeof(int));
		banker.needed[i] = (int*) calloc(NUM_RESOURCES, sizeof(int));
	}

	//Initialize maximum array
	char *line, *token;

	//Loops for every line (sub array) in file Array
	for(int i = 0; i < NUM_CUSTOMERS; i++){

		int k = 0;
		//Get current line from multi-dimensional maximum needs array
		line = fileArray[i];

		//Break up line into tokens and use those values to populate the ith customers maximum needs array
		while ((token = strtok_r(line , ",", &line)) != NULL){
  
			sscanf(token, "%d", &(banker.maximum[i][k]));

			//Set the ammount each customer needs to their maximum, as nothing has yet to be allocated
			banker.needed[i][k] = banker.maximum[i][k];
			k++;
		}
	}
}

void freeBanker(){

	//Free dynamically allocated sub arrays
	for (int i = 0; i < NUM_CUSTOMERS; i++){

		free(banker.maximum[i]);
		free(banker.allocated[i]);
		free(banker.needed[i]);
	}

	//Free dynamically allocated array
	free(banker.available);
	banker.available = NULL;

	//Free dynamically allocated arrays of pointers
	free(banker.maximum);
	free(banker.allocated);
	free(banker.needed);
	
	banker.maximum = NULL;
	banker.allocated = NULL;
	banker.needed = NULL;
}


char** readFileIntoArr(){

	FILE *in = fopen("sample4_in.txt", "r");

	if(!in){
		printf("ERROR: initMaxArr - opening input file failed\n");
	}

	//Read file contents into multidimensional array
	char** file = (char**) malloc(0);

	while(!feof(in)){
	
		//Pre-increment NUM_CUSTOMERS to realloc number of file lines dynamically
		file = (char**) realloc(file, ++NUM_CUSTOMERS * sizeof(char*)); 

		//Each line is at most LINE_MAX_CHAR
		char* line = (char*) malloc(LINE_MAX_CHAR * sizeof(char));	

		//Read line from file into line and if not empty line (null)
		if(fgets(line, LINE_MAX_CHAR, in) != NULL){

			//Assign appropriate file double pointer to current line pointer
			file[NUM_CUSTOMERS-1] = line;
		}	
	}
	fclose(in);

	return file;
}

void interact(){

	char *cmdLineInput = NULL;
	size_t len = 0;
	char *userInputArr[EXPECTED_NUM_CMDS];
	int *resourceArr = (int*) calloc(NUM_RESOURCES, sizeof(int));

	do{	//Prompt user to enter command
		printf("Enter Command: ");
		
		//Get user input from command line
		getline(&cmdLineInput, &len, stdin);

		//Eliminate newline character from command line input sring
		cmdLineInput[strlen(cmdLineInput)-1] ='\0';

		int userInputArrLength = 0;
		char *token = strtok(cmdLineInput, " ");
		//Put command line input into an array
		while(token != NULL){	
			userInputArr[userInputArrLength++] = token;
			token = strtok(NULL, " ");
		}

		//Get pertinent information from user Input Array
		char *command = userInputArr[0];
		int customerNumber;

		if(userInputArrLength > 1){
			//Get more pertinent information from user Input Array	
			customerNumber = atoi(userInputArr[1]);
		}

		if( (strcmp(command, "*") == 0) || (strcmp(command, "Run") == 0) ){
		
			if(strcmp(command, "*") == 0){
				printAllResources();
			}else{

				//Returns 0 if safe sequence found and -1 if not
				NotFindSafeSequence(1);	
			}
		
		}else if( (userInputArrLength == EXPECTED_NUM_CMDS) && ((customerNumber >= 0) && (customerNumber <= NUM_CUSTOMERS)) ){

			//Read userInput into resource array as ints
			for(int i = 0; i < NUM_RESOURCES; i++){
			
				sscanf(userInputArr[i+2], "%d", &(resourceArr[i]));	//Plus 2 is necessary offset as command is 0 and customer number is 1
			}
			
			if(strcmp(command, "RQ") == 0){

				//Request Resources
				int val = requestResource(customerNumber, resourceArr);
				
				//If requested resources not initiall allocated OR if was allocated but in unsafe state then
				if(val == -1){
					printf("Request Unsuccessful\n");
				}else{
					printf("Request Successful\n");
				}

			}else if(strcmp(command, "RL") == 0){

				//Release Resources
				int val = releaseResource(customerNumber, resourceArr);
				
				if(val == -1){
					printf("Release Unsuccessful\n");
				}else{
					printf("Release Successful\n");
				}

			}else{
			
				printf("Error: Command Not Recognized. Please try again or enter \"EXIT\" to quit the program.\n");
			}
	
		}else if(strcmp(command, "EXIT") != 0){

			printf("ERROR: The Command as Entered is Invalid. Please try again or enter \"EXIT\" to quit the program.\n");
		}

	}while(strcmp(userInputArr[0], "EXIT") != 0);

	free(resourceArr);
	//Must free as getline allocates memory to a buffer that must be freed
	free(cmdLineInput);
}
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
	if(NotFindSafeSequence(0)){
		
		releaseResource(custNum, userInputArr);
		return -1;
	}

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

void printAllResources(){

	printAvailableArr(1);
	printMaximumArr(1);
	printAllocatedArr(1);
	printNeededArr(1);
}

void printAvailableArr(int printHeader){

	//Print Current State of Available Array
	if(printHeader){
		printf("Currently State of Available Array: ");
	}
	for(int i = 0; i < NUM_RESOURCES; i++){
		printf("%d ", banker.available[i]);
	}
	printf("\n");
}

void printMaximumArr(int printHeader){

	//Print Current State of Maximum Array
	if(printHeader){
		printf("Current State of Maximum Array.\n");
	}	
	for(int i = 0; i < NUM_CUSTOMERS; i++){
		for(int j = 0; j < NUM_RESOURCES; j++){
			printf("%d", banker.maximum[i][j]);

			if(j != NUM_RESOURCES-1){
				printf(",");
			}	
		}	
		printf("\n");	
	}
}

void printAllocatedArr(int printHeader){

	//Print Current State of Allocated Array
	if(printHeader){
		printf("Current State of Allocated Array.\n");
	}
	for(int i = 0; i < NUM_CUSTOMERS; i++){
		for(int j = 0; j < NUM_RESOURCES; j++){
			printf("%d", banker.allocated[i][j]);	

			if(j != NUM_RESOURCES-1){
				printf(", ");
			}
		}	
		printf("\n");	
	}
}

void printNeededArr(int printHeader){

	//Print Current State of Needed Array
	if(printHeader){
		printf("Current State of Needed Array.\n");
	}
	for(int i = 0; i < NUM_CUSTOMERS; i++){
		for(int j = 0; j < NUM_RESOURCES; j++){
			printf("%d", banker.needed[i][j]);

			if(j != NUM_RESOURCES-1){
				printf(", ");
			}
		}	
		printf("\n");	
	}
}

//This function may not be entirely correct
void printCustomerInfo(int customerNumber, int work[NUM_RESOURCES]){

	printf("--> Customer/Thread %d\n", customerNumber);
	
	printf("Allocated:\t");
	for(int j = 0; j < NUM_RESOURCES; j++){

		printf("%d ", banker.allocated[customerNumber][j]);
	}
	printf("\n");

	printf("Needed:\t\t");
	for(int j = 0; j < NUM_RESOURCES; j++){

		printf("%d ", banker.needed[customerNumber][j]);
	}
	printf("\n");

	printf("Available:\t");
	for(int i = 0; i < NUM_RESOURCES; i++){

		printf("%d ", work[i] - banker.allocated[customerNumber][i]);
	}
	printf("\n");

	printf("Thread has started\n");
	printf("Thread has finished\n");
	printf("Thread is releasing resources\n");

	printf("New Available:\t");
	for(int i = 0; i < NUM_RESOURCES; i++){

		printf("%d ", work[i]);
	}
	printf("\n\n");
}
