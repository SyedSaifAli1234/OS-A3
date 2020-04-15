#include <stdio.h>       
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>     
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

int* pcp;
sem_t* cp;
sem_t* fp;
sem_t* pcp_sem;


void* CoronaPatients(void* param){

	sem_wait(pcp_sem);
	*pcp = *pcp + 1;
	printf("Potential Corona Virus Patiens count = %d\n", *pcp);
	sem_post(pcp_sem);


	int random = rand() % 2;
	printf("random = %d\n",random);

	if(random == 0){
		sem_post(fp);
		printf("Flu patient incremented\n");

		sem_wait(pcp_sem);
		*pcp = *pcp - 1;
		printf("PCP decremented\n");
		sem_post(pcp_sem);	
	}
	else if(random == 1){
		sem_post(cp);
		printf("Corona patient incremented\n");

		sem_wait(pcp_sem);
		*pcp = *pcp - 1;
		printf("PCP decremented\n");
		sem_post(pcp_sem);	

		int sum = 0;
		while(1){
		sum = rand()%2 + rand()%2;

			if (sum == 0){
				sem_wait(cp);
				printf("Corona patient has been treated! :D\n");
				break;
			}
		sum = 0;
	    }

	}
	pthread_exit(0);

}


int main(){																			//Main

	int number;
	printf("Enter the number of patients : ");
	scanf("%d", &number);
	printf("Number = %d\n", number);
	pthread_t id[number];


	int shmid_pcp = shmget(121212, sizeof(int), 0644|IPC_CREAT); 					//Shared memory for PCP var
	if (shmid_pcp < 0){
		perror ("Shared memory not created\n");
		exit(0);
	}
	pcp = (int*) shmat(shmid_pcp, NULL, 0);								
	if (pcp < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	*pcp = 0;


	
	int shmid_cp = shmget(131313, 1024, 0644|IPC_CREAT); 							//Shared memory for CP 
	if (shmid_cp < 0){
		perror ("Shared memory for cp not created\n");
		exit(0);
	}
	cp = (sem_t*) shmat(shmid_cp, NULL, 0);								

	if ( cp < 0){
		perror ("Shared memory not attached\n");
		exit(0);
	}
	
	sem_open("sem_cp", O_CREAT|O_EXCL, 0644, 0);
	int result = sem_unlink("sem_cp");
	if ( result < 0){
		perror("Error in removing semaphore sem_cp\n");
		exit(0);
	}	




	int shmid_fp = shmget(141414, 1024, 0644|IPC_CREAT); 							//Shared memory for FP
	if ( shmid_fp < 0){
		perror ("Shared memory for fp not created\n");
		exit(0);
	}

	fp = (sem_t*) shmat(shmid_fp, NULL, 0);
	if ( fp < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}

	sem_open("sem_fp", O_CREAT|O_EXCL, 0644, 0);
	result = sem_unlink("sem_fp");
	if ( result < 0){
		perror("Error in removing semaphore sem_fp\n");
		exit(0);
	}


	

	int shmid_pcp_sem = shmget(151515, 1024, 0644|IPC_CREAT); 						//Shared memory for PCP
	if ( shmid_pcp_sem < 0){
		perror ("Shared memory for pcp_sem not created\n");
		exit(0);
	}
	pcp_sem = (sem_t*) shmat(shmid_pcp_sem, NULL, 0);
	if ( pcp_sem < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}

	sem_open("sem_pcp", O_CREAT|O_EXCL, 0644, 1);
	result = sem_unlink("sem_pcp");
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp\n");
		exit(0);
	}
	


    int i;

	for(i = 0; i< number; i++){														//Creating Threads
		if (pthread_create(&id[i], NULL, CoronaPatients, NULL) < 0) {
	    	printf("Thread not created\n");
	  	}
	  	sleep(1);
  	}


	for(i = 0; i< number; i++){														//Returning from Threads
	  	if (pthread_join(id[i], NULL) < 0) {
	    	printf("Thread not created\n");
	  	}
	}

	printf("PCP %d\n", *pcp);														





	result = shmdt(pcp);															//Detaching Semaphores
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp\n");
		exit(0);
	}

	result = shmdt(cp);
	if ( result < 0){
		perror("Error in removing semaphore sem_cp\n");
		exit(0);
	}

	result = shmdt(fp);
	if ( result < 0){
		perror("Error in removing semaphore sem_fp\n");
		exit(0);
	}
	

	result = shmdt(pcp_sem);
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp_sem\n");
		exit(0);
	}

	shmctl(shmid_pcp, IPC_RMID, 0);													//Clearing Semaphore from mem
	shmctl(shmid_fp, IPC_RMID, 0);
	shmctl(shmid_cp, IPC_RMID, 0);
	shmctl(shmid_pcp_sem, IPC_RMID, 0);

    sem_destroy(cp);																//Destroying Semaphores
    sem_destroy(fp);
    sem_destroy(pcp_sem);

	return 0;
}