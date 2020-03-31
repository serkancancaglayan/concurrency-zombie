/*	Serkan Can Caglayan
*	31/03/2020
*	18120205021
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h> // rand() , malloc()
#include <unistd.h> // sleep()


/*number of zombies in the game: 
you should synchronize threads editing this variable*/
int zombieCounter;

int killed_zombieCounter;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

/* Keeps track of number of zombies entered.*/
void zombieEntered(){
	zombieCounter++;
}
/* Keeps track of number of zombies killed.*/
void zombieKilled(){
	killed_zombieCounter++;
	zombieCounter--;
}
/* Returns true if number of zombies in the room are 
greater than or equal to 100.*/
int tooManyZombiesInTheRoom(){
	return zombieCounter >= 100;
}

/*Returns true if more than 100 zombies have been killed.*/
int killed100Zombies(){
	return killed_zombieCounter >= 100;
}

/* Returns true if there is at least one zombies in the room.*/
int zombiesExist(){
	return zombieCounter > 0;
}
/*Returns the count of number of zombies killed.*/
int getKilledCount(){
	return killed_zombieCounter;
}

/* Returns the count of number of zombies in the room.*/
int getInTheRoomCount(){
	return zombieCounter;
}
/*doorman thread*/
void *doorMan(void *n){
	while(1){
		int random = rand() % 100;
		//%50 chance
		if(random < 50){
			pthread_mutex_lock(&m);
			if(tooManyZombiesInTheRoom()){
				printf("Too many zombies in the room...\n");
				exit(EXIT_FAILURE);
			}
			else if(killed100Zombies()){
				printf("Slayer killed 100 zombies!\n");
				exit(EXIT_SUCCESS);
			}
			zombieEntered();
			//printf("Doorman let a zombie in %d\n", getInTheRoomCount());
			pthread_mutex_unlock(&m);
		}
		sleep(0.002);
	}
	pthread_exit(NULL);
}
/*slayer thread*/
void *slayer(void *n){
	while(1){
		pthread_mutex_lock(&m);
		if(tooManyZombiesInTheRoom()){
			printf("Too many zombies in the room...\n");
			exit(EXIT_FAILURE);
		}
		else if(killed100Zombies()){
			printf("Slayer killed 100 zombies!\n");
			exit(EXIT_SUCCESS);
		}
		if(zombiesExist()){
			zombieKilled();
			//printf("Slayer killed a zombie %d\n",getInTheRoomCount());
		}
		pthread_mutex_unlock(&m);
		sleep(0.002);
	}
	pthread_exit(NULL);
}
/*simulator main thread*/
int main(int argc, char **argv){
	int n = atoi(argv[1]);

	//thread id array for doorMan threads
	pthread_t * tids_doorman = malloc(n * sizeof(pthread_t));
	
	//creating doorMan threads
	for(int i = 0; i < n; i++){
		pthread_create(&tids_doorman[i], NULL, doorMan, NULL);
	}

	//holds slayer thread's thread id
	pthread_t tid_slayer;

	//creating and joining slayer thread
	pthread_create(&tid_slayer, NULL, slayer, NULL);
	pthread_join(tid_slayer, NULL);

	//joining doorMan threads
	for(int i = 0; i < n; i++){
		pthread_join(tids_doorman[i], NULL);
	}

	return 0;
}
