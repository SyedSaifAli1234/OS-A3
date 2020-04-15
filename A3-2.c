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
	//printf("1\n"); 




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
	//printf("2\n");




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
	//printf("3\n");





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
	//printf("4\n");










	if(fork() == 0){																	//Child1
		//A say read and enter in buffer1
		printf("In Process A\n");
		FILE *fp;
	    char line[11];
	    line[10] = '\0';

	    fp = fopen("file-1.txt", "r");
   	    fgets(line, 11, (FILE*)fp);
	    printf("%s\n", line);
	    fclose(fp);
	    
	    sem_wait(buff1);
	    for(int i=0; i<10; i++){
	    	buffer[i] = line[i];
	    	printf("%c", buffer[i]);
	    }

	    sem_post(buff1);
	    printf("\nLeaving Process A after writing to buffer1\n");
	    exit(0);
	}	
	else{
		wait(NULL);																		//Parent
		if(fork() == 0){
		//B say read and enter in buffer1												//Child2
			printf("In Process B\n");
			FILE *fp;
		    char line[11];
		    line[10] = '\0';

		    fp = fopen("file-2.txt", "r");
	   	    fgets(line, 11, (FILE*)fp);
		    printf("%s\n", line);
		    fclose(fp);
		    
		    sem_wait(buff1);
		    int j = 10;
		    for(int i=0; i<10; i++){
		    	buffer[j] = line[i];
		    	printf("%c", buffer[j]);
		    	j++;
		    }

		    sem_post(buff1);
		    printf("\nLeaving Process B after writing to buffer1\n");
		    printf("%s\n", buffer);
		    exit(0);
		}
		else{
			wait(NULL);																			//Parent
			if(fork() == 0){		
			//C say read from buffer1 to buffer 2										//Child3
				printf("\nIn Process C\n");
				sem_wait(buff1);
				sem_wait(buff2);
				for(int i = 0; i<20; i++){
					buffer2[i] = buffer[i];
				}
				printf("Buffer 1 = %s\n", buffer);
				printf("Buffer 2 = %s\n", buffer2);
				sem_post(buff1);
				sem_post(buff2);
				exit(0);
			}
			else{	
				wait(NULL);																//Parent
				if(fork() == 0){														//Child4
					//D reads from Buffer 2 and prints
					printf("In Process D\n");
					sem_wait(buff2);
					
					for(int i = 0; i<20; i++){
						buffer2[i] = buffer[i];
						printf("%c", buffer2[i]);
					}
					sem_post(buff2);
					printf("\n");
					exit(0);
				}
				else{
					wait(NULL);
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

	printf("\nEverything works\n");
	return 0;
}

