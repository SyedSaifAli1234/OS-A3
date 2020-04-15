#include <stdio.h>       
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>     
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

char* buffer;
char* buffer2;
sem_t* cp;
sem_t* fp;


// void* CoronaPatients(void* param){

// 	sem_wait(pcp_sem);
// 	*pcp = *pcp + 1;
// 	printf("Potential Corona Virus Patiens count = %d\n", *pcp);
// 	sem_post(pcp_sem);


// 	int random = rand() % 2;
// 	printf("random = %d\n",random);

// 	if(random == 0){
// 		sem_post(fp);
// 		printf("Flu patient incremented\n");

// 		sem_wait(pcp_sem);
// 		*pcp = *pcp - 1;
// 		printf("PCP decremented\n");
// 		sem_post(pcp_sem);	
// 	}
// 	else if(random == 1){
// 		sem_post(cp);
// 		printf("Corona patient incremented\n");

// 		sem_wait(pcp_sem);
// 		*pcp = *pcp - 1;
// 		printf("PCP decremented\n");
// 		sem_post(pcp_sem);	

// 		int sum = 0;
// 		while(1){
// 		sum = rand()%2 + rand()%2;

// 			if (sum == 0){
// 				sem_wait(cp);
// 				printf("Corona patient has been treated! :D\n");
// 				break;
// 			}
// 		sum = 0;
// 	    }

// 	}
// 	pthread_exit(0);

// }


int main(){


	int shmid_buff1 = shmget(121212, 1024, 0644|IPC_CREAT);										//Shared memory for Buffer 1
	if (shmid_buff1 < 0){
		perror ("Shared memory not created\n");
		exit(0);
	}
	buffer = (char*) shmat(shmid_buff1, NULL, 0);								
	if (buffer < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	*buffer = NULL;



	int shmid_buff2 = shmget(131313, 1024, 0644|IPC_CREAT); 									//Shared memory for Buffer 2
	if (shmid_buff2 < 0){
		perror ("Shared memory not created\n");
		exit(0);
	}
	buffer2 = (char*) shmat(shmid_buff2, NULL, 0);								
	if (buffer2 < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	*buffer2 = NULL;

	



	// result = sem_unlink("sem_pcp");																//Detaching and Deleting
	// if ( result < 0){
	// 	perror("Error in removing semaphore sem_pcp\n");
	// 	exit(0);
	// }

	// result = shmdt(pcp);
	// if ( result < 0){
	// 	perror("Error in removing semaphore sem_pcp\n");
	// 	exit(0);
	//}
	shmctl(shmid_buff1, IPC_RMID, 0);
	shmctl(shmid_buff2, IPC_RMID, 0);
    //sem_destroy(pcp_sem);

	printf("Works\n");
	return 0;
}

