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


int main(int argc, char** argv){

	printf("argv[1] : %c\n", *argv[1]);
	int number = *argv[1] -48;
	printf("Number = %d\n", number);
	pthread_t id[number];


	//Step 1 - create shared mem for pcp so that we can use the integer pcp one at a time
	//printf("Starting\n");
	int shmid_pcp = shmget(121212, sizeof(int), 0644|IPC_CREAT); 
	if (shmid_pcp < 0){
		perror ("Shared memory not created\n");
		exit(0);
	}
	//printf("Shared Memory created for pcp\n");

	pcp = (int*) shmat(shmid_pcp, NULL, 0);								
	if (pcp < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	//printf("Shared memory for int* pcp has been attached\n");
	*pcp = 0;



	// Step 2 Shared mem create for semaphore cp
	int shmid_cp = shmget(131313, 1024, 0644|IPC_CREAT); 
	if (shmid_cp < 0){
		perror ("Shared memory for cp not created\n");
		exit(0);
	}
	//printf("Shared memory created for CoronaPatients\n");
	cp = (sem_t*) shmat(shmid_cp, NULL, 0);								

	if ( cp < 0){
		perror ("Shared memory not attached\n");
		exit(0);
	}
	//printf("Shared memory for CP Semaphore has been attached\n");
	sem_open("sem_cp", O_CREAT|O_EXCL, 0644, 0);
	int result = sem_unlink("sem_cp");
	if ( result < 0){
		perror("Error in removing semaphore sem_cp\n");
		exit(0);
	}
	//printf("Sem CP has been unlinked\n");






	//Step 3 Shared mem create for semaphore FP

	int shmid_fp = shmget(141414, 1024, 0644|IPC_CREAT); 
	if ( shmid_fp < 0){
		perror ("Shared memory for fp not created\n");
		exit(0);
	}
	//printf("Shmget done\n");
	fp = (sem_t*) shmat(shmid_fp, NULL, 0);
	if ( fp < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	//printf("Shmat done\n");
	sem_open("sem_fp", O_CREAT|O_EXCL, 0644, 0);
	result = sem_unlink("sem_fp");
	if ( result < 0){
		perror("Error in removing semaphore sem_fp\n");
		exit(0);
	}
	//printf("Sem FP has been unlinked\n");






	//Step 4 shared mem for pcp_sem

	int shmid_pcp_sem = shmget(151515, 1024, 0644|IPC_CREAT); 
	if ( shmid_pcp_sem < 0){
		perror ("Shared memory for pcp_sem not created\n");
		exit(0);
	}
	//printf("Shmget done\n");

    //Att shared mem to process1
	// void *shmat(int shmid, const void *shmaddr, int shmflg);
	pcp_sem = (sem_t*) shmat(shmid_pcp_sem, NULL, 0);

	if ( pcp_sem < 0){
		perror ("Shared memory att not done\n");
		exit(0);
	}
	//printf("Shmat done\n");

	// initial value of semaphore 
	// sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
	sem_open("sem_pcp", O_CREAT|O_EXCL, 0644, 1);//intial value 1 so that any process can use integer pcp

	//sem_unlink - remove a named semaphore
	// int sem_unlink(const char *name);
	result = sem_unlink("sem_pcp");
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp\n");
		exit(0);
	}
	//printf("semunlink done\n");




    //creating threads now - after all shared mem ork is done
    int i;

	for(i = 0; i< number; i++){
		if (pthread_create(&id[i], NULL, CoronaPatients, NULL) < 0) {
	    	printf("Thread not created\n");
	  	}
	  	sleep(1);
  	}


	for(i = 0; i< number; i++){
	  	if (pthread_join(id[i], NULL) < 0) {
	    	printf("Thread not created\n");
	  	}
	}

	printf("PCP %d\n", *pcp);








	result = shmdt(pcp);
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp\n");
		exit(0);
	}
	//printf("Shmdt done\n");

	result = shmdt(cp);
	if ( result < 0){
		perror("Error in removing semaphore sem_cp\n");
		exit(0);
	}
	//printf("Shmdt done\n");

	result = shmdt(fp);
	if ( result < 0){
		perror("Error in removing semaphore sem_fp\n");
		exit(0);
	}
	//printf("Shmdt done\n");

	result = shmdt(pcp_sem);
	if ( result < 0){
		perror("Error in removing semaphore sem_pcp_sem\n");
		exit(0);
	}
	printf("Shmdt done\n");

	shmctl(shmid_pcp, IPC_RMID, 0);
	shmctl(shmid_fp, IPC_RMID, 0);
	shmctl(shmid_cp, IPC_RMID, 0);
	shmctl(shmid_pcp_sem, IPC_RMID, 0);
	//printf("R\n");


    //delete all semaphores
    sem_destroy(cp);
    sem_destroy(fp);
    sem_destroy(pcp_sem);
    printf("E\n");

	return 0;
}