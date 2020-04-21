#include <stdio.h>          // printf(), scanf()               
#include <stdlib.h>         // exit(), malloc(), free(), rand()
#include <sys/types.h>      // key_t, sem_t, pid_t      
#include <sys/shm.h>        // shmat(), IPC_RMID in shmctl()      
#include <errno.h>          // errno, ECHILD            
#include <semaphore.h>      // sem_open(), sem_destroy(), sem_wait().. 
#include <fcntl.h>          // O_CREAT, O_EXEC 
#include <unistd.h>         // fork
#include <sys/wait.h>       // wait

char* Buffer1;
sem_t* Buffer1_sem;

char* Buffer2;
sem_t* Buffer2_sem;


int main(){

    //Creating Shared Memory for buffer1 
    //0644 = Permission, size is of an integer value so only one value is stored in shared mem (potentialCPatients)
    int shmid_buffer1 = shmget(101010, 1024, 0644|IPC_CREAT); 
    //printf("Shmid_buffer1 : %d\n", shmid_buffer1);

    if (shmid_buffer1 < 0){
        perror("Shared memory for buffer1 not created successfully\n");
        exit(0);
    }

    //Attaching shared memory segment (identified by shmid_buffer1) to the address space of the calling process
    //2nd parameter is NULL so the system chooses a suitable unused address at which to attach the shared mem segment 
    Buffer1 = (char*) shmat(shmid_buffer1, NULL, 0);

    if (Buffer1 < 0){
        perror("Shared memory for Buffer 1 not attached successfully\n");
        exit(1);
    } 
    printf("Shared mem work for Buffer 1 done\n");






    //Creating Shared Memory for buffer2
    //0644 = Permission, size is of an integer value so only one value is stored in shared mem (potentialCPatients)
    int shmid_buffer2 = shmget(121212, 1024, 0644|IPC_CREAT); 
    //printf("Shmid_buffer2 : %d\n", shmid_buffer2);

    if (shmid_buffer2 < 0){
        perror("Shared memory for buffer2 not created successfully\n");
        exit(0);
    }

    //Attaching shared memory segment (identified by shmid_buffer1) to the address space of the calling process
    //2nd parameter is NULL so the system chooses a suitable unused address at which to attach the shared mem segment 
    Buffer2 = (char*) shmat(shmid_buffer2, NULL, 0);

    if (Buffer2 < 0){
        perror("Shared memory for Buffer 2 not attached successfully\n");
        exit(1);
    }
    printf("Shared mem work for Buffer 2 done\n");






    //Creating Shared Memory for buffer1 semaphore
    //0644 = Permission, size is of an integer value so only one value is stored in shared mem (potentialCPatients)
    int shmid_buffer1_sem = shmget(131313, 1024, 0644|IPC_CREAT); 
    //printf("Shmid_buffer1_sem : %d\n", shmid_buffer1_sem);

    if (shmid_buffer1_sem < 0){
        perror("Shared memory for buffer1 semaphore not created successfully\n");
        exit(0);
    }

    //Attaching shared memory segment (identified by shmid_buffer1_sem) to the address space of the calling process
    //2nd parameter is NULL so the system chooses a suitable unused address at which to attach the shared mem segment 
    Buffer1_sem = (sem_t*) shmat(shmid_buffer1_sem, NULL, 0);

    if (Buffer1_sem < 0){
        perror("Shared memory for Buffer 1 semaphore not attached successfully\n");
        exit(1);
    }    

    Buffer1_sem = sem_open("Buffer1_sem", O_CREAT|O_EXCL, 0644, 1);
    sem_unlink("Buffer1_sem"); 
    printf("Shared mem work for Buffer 1 semaphore done\n");







    //Creating Shared Memory for buffer2 semaphore
    //0644 = Permission, size is of an integer value so only one value is stored in shared mem (potentialCPatients)
    int shmid_buffer2_sem = shmget(141414, 1024, 0644|IPC_CREAT); 
    //printf("Shmid_buffer2_sem : %d\n", shmid_buffer2_sem);

    if (shmid_buffer2_sem < 0){
        perror("Shared memory for buffer2 semaphore not created successfully\n");
        exit(0);
    }

    //Attaching shared memory segment (identified by shmid_buffer2_sem) to the address space of the calling process
    //2nd parameter is NULL so the system chooses a suitable unused address at which to attach the shared mem segment 
    Buffer2_sem = (sem_t*) shmat(shmid_buffer2_sem, NULL, 0);

    if (Buffer2_sem < 0){
        perror("Shared memory for Buffer 2 semaphore not attached successfully\n");
        exit(1);
    }

    Buffer2_sem = sem_open("Buffer2_sem", O_CREAT|O_EXCL, 0644, 1);
    sem_unlink("Buffer2_sem"); 
    printf("Shared mem work for Buffer 2 semaphore done\n"); 





    
    pid_t ProcessA, ProcessB, ProcessC, ProcessD;

	ProcessA = fork();

	if (ProcessA == 0) {
		printf("In ProcessA -> Child1");  // Child A code 
		// Process A needs to read 10 characters from FileA.txt and place it in buffer1

		FILE *fileA = fopen("file-1.txt", "r");
		if (fileA == NULL){
			perror("FileA failed to open\n");
			exit(0);	
		}
		else{ printf("FileA opened\n"); }

		//Accessing Buffer1 so don't let any other to access it

		sem_wait(Buffer1_sem);

		int i;
		for(i = 0; i < 10; i++){
			Buffer1[i] = fgetc(fileA);	
		}
		printf("Buffer1 after ProcessA: %s\n", Buffer1);

		sem_post(Buffer1_sem);

		fclose(fileA);
		printf("PrcoessA is done\n");
		exit(0);

	} else {
		wait(NULL); //Wait for ProcessA
		printf("In Parent"); // Parent Code 
	    ProcessB = fork();

	    if (ProcessB == 0) {
	    	printf("In ProcessB-> Child2"); //Child B code

	    	// Process B needs to read 10 characters from FileB.txt and place it in buffer1 after ProcessA 10 characters

			FILE *fileB = fopen("file-2.txt", "r");
			if (fileB == NULL){
				perror("FileB failed to open\n");
				exit(0);	
			}
			else{ printf("FileB opened\n"); }

			//char fileBstuff[11]; // 11 because last for null character
			//memset(fileBstuff,'\0',sizeof(fileBstuff)); 

			//Accessing Buffer1 so don't let any other to access it

			sem_wait(Buffer1_sem);

			int i;
			
			for(i = 10; i < 20; i++){
				Buffer1[i] = fgetc(fileB);	
			}
			printf("Buffer1 after ProcessA: %s\n", Buffer1);

			sem_post(Buffer1_sem);

			fclose(fileB);
			printf("Work of Process B is done\n");
			exit(0);

	    } else {
	    	wait(NULL);//Wait for ProcessB
	    	printf("In Parent"); //Parent Code
	        ProcessC = fork();

	        if (ProcessC == 0){
	        	printf("In ProcessC -> Child3"); // Child C Code 

	        	sem_wait(Buffer1_sem);
	        	sem_wait(Buffer2_sem);

	        	int i;	        	

	        	for(i = 0; i < 20; i++){
	        		Buffer2[i] = Buffer1[i];	
	        	}

	        	printf("Buffer1 after Process C: %s\n", Buffer1);
	        	printf("Buffer2 after Process C: %s\n", Buffer2);

	        	sem_post(Buffer2_sem);
	        	sem_post(Buffer1_sem);

	        	printf("All work of Process C is done\n");
	        	exit(0);

	        }else {
	        	printf("In Parent"); // Parent Code
	        	ProcessD = fork();

	        	if (ProcessD == 0){
	        		printf("In ProcessD -> Child4");	// Child D Code

	        		sem_wait(Buffer2_sem);

	        		printf("Buffer2 in Process D: %s", Buffer2);

	        		sem_post(Buffer2_sem);
	        		printf("All work in ProcessD is done\n");
	        		exit(0);
	        	}
	        	else{
	        		printf("In parrent\n");
	        		wait(NULL);
	        	}
	        }
	    }
	} 


	


    //Cleaning shared mem stuff
    shmdt(Buffer1);
    shmdt(Buffer2);
    shmdt(Buffer1_sem);
    shmdt(Buffer2_sem);
    printf("Shmdt for all shared mem done\n");

    shmctl(shmid_buffer1, IPC_RMID, 0);
    shmctl(shmid_buffer2, IPC_RMID, 0);
    shmctl(shmid_buffer1_sem, IPC_RMID, 0);
    shmctl(shmid_buffer2_sem, IPC_RMID, 0);
    printf("shmctl done for all shared mem\n");

    //delete semaphores
    sem_destroy(Buffer1_sem);
    sem_destroy(Buffer2_sem);
    printf("sem_destroy for semaphores done\n");

	return 0;
}