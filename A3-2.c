#include <stdio.h>       
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>     
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

char* buffer;
char* buffer2;
sem_t* buff1;
sem_t* buff2;


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
	printf("1\n"); 




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
	printf("2\n");




	int shmid_buff1_sem = shmget(141414, 1024, 0644|IPC_CREAT); 								//Semaphore for Buff1 shared memory
	if (shmid_buff1_sem < 0){
		perror ("Shared memory for buff1_sem not created\n");
		exit(0);
	}
	buff1 = (sem_t*) shmat(shmid_buff1_sem, NULL, 0);								

	if (buff1 < 0){
		perror ("Shared memory not attached\n");
		exit(0);
	}
	sem_open("sem_buff1", O_CREAT|O_EXCL, 0644, 1);
	int result = sem_unlink("sem_buff1");
	if (result < 0){
		perror("Error in unlinking semaphore buff1_sem\n");
		exit(0);
	}	
	printf("3\n");





	int shmid_buff2_sem = shmget(151515, 1024, 0644|IPC_CREAT); 								//Semaphore for Buff2 shared memory
	if (shmid_buff2_sem < 0){
		perror ("Shared memory for buff2_sem not created\n");
		exit(0);
	}
	buff2 = (sem_t*) shmat(shmid_buff2_sem, NULL, 0);								

	if (buff2 < 0){
		perror ("Shared memory not attached\n");
		exit(0);
	}
	sem_open("sem_buff2", O_CREAT|O_EXCL, 0644, 1);
	result = sem_unlink("sem_buff2");
	if (result < 0){
		perror("Error in unlinking semaphore buff2_sem\n");
		exit(0);
	}
	printf("4\n");










	if(fork() == 0){																	//Child1
		//A say read and enter in buffer1
	}	
	else{																				//Parent
		if(fork() == 0){																//Child2
			//B say read and enter in buffer1
		}
		else{																			//Parent
			if(fork() == 0){															//Child3
				//Buffer 1 se C reads and enter in Buffer 2
			}
			else{																		//Parent
				if(fork() == 0){														//Child4
					//D reads from Buffer 2 and prints
				}
			}
		}
	}























	result = shmdt(buff1);																		//Detaching and Destroying here
	if (result < 0){
		perror("Error in removing semaphore buff1_sem\n");
		exit(0);
	}
	result = shmdt(buff2);
	if (result < 0){
		perror("Error in removing semaphore buff2_sem\n");
		exit(0);
	}
	result = shmdt(buffer);
	if (result < 0){
		perror("Error in removing semaphore buff2_sem\n");
		exit(0);
	}
	result = shmdt(buffer2);
	if (result < 0){
		perror("Error in removing semaphore buff2_sem\n");
		exit(0);
	}

	
	shmctl(shmid_buff1, IPC_RMID, 0);
	shmctl(shmid_buff2, IPC_RMID, 0);
	shmctl(shmid_buff1_sem, IPC_RMID, 0);
	shmctl(shmid_buff2_sem, IPC_RMID, 0);
    sem_destroy(buff1);
    sem_destroy(buff2);

	printf("Works\n");
	return 0;
}

