#include <stdio.h>
#include <unistd.h>
#define __USE_UNIX98
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>

pthread_mutex_t mutex;
int cnt;

/* 
*	CLAVE
*	
*  	Funcion para hacer una espera activa
*
*  	@param tiempo tiempo en segundos que queremos que espere la funcion
*/
void espera_activa( int tiempo) {

	time_t t;

	//Bucle de espera 
    t = time(0) + tiempo;
    while(time(0) < t);
}

void *tareaA( void * args) {
	printf("tareaA::voy a dormir\n");
	sleep(3);
	printf("tareaA::me despierto y pillo mutex\n");
	pthread_mutex_lock(&mutex);
	printf("tareaA::incremento valor\n");
	++cnt;
	printf("tareaA::desbloqueo mutex\n");
	pthread_mutex_unlock(&mutex);
	printf("tareaA::FINISH\n");
}

void *tareaM( void * args) {
	printf("\ttareaM::me voy a dormir\n");
	sleep(5);
	printf("\ttareaM::me despierto y hago espera activa\n");
	espera_activa(15);
	printf("\ttareaM::FINSIH\n");
}

void *tareaB( void * args) {
	printf("\t\ttareaB::me voy a dormir\n");
	sleep(1);
	printf("\t\ttareaB::me despierto y pillo mutex\n");
	pthread_mutex_lock(&mutex);
	printf("\t\ttareaB::espera activa\n");
	espera_activa(7);
	printf("\t\ttareaB::incremento cnt\n");
	++cnt;
	printf("\t\ttareaB::suelto mutex\n");
	pthread_mutex_unlock(&mutex);
	printf("\t\ttareaB::FINISH\n");
}

int main() {

	pthread_t hebraA, hebraM, hebraB;
	pthread_attr_t attr;
	pthread_mutexattr_t attrM;
	struct sched_param prio;

	cnt = 0;

	// ====================
	// MUTEX
	// ====================

	pthread_mutexattr_init(&attrM);
	
	if( pthread_mutexattr_setprotocol(&attrM, PTHREAD_PRIO_INHERIT) != 0) {
		printf("ERROR en __set_protocol\n");
		exit(-1);
	}
	if( pthread_mutexattr_setprioceiling(&attrM, 3) != 0){
		printf("ERROR en __setprioceiling\n");
	}

	pthread_mutex_init(&mutex, &attrM);

	// ====================
	// THREADS
	// ====================
	if( pthread_attr_init( &attr) != 0) {
		printf("ERROR en __attr_init\n");
		exit(-1);
	}
	if( pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED) != 0){
		printf("ERROR __setinheritsched\n");
		exit(-1);
	}
	if( pthread_attr_setschedpolicy( &attr, SCHED_FIFO) != 0) {
		printf("ERROR __setschedpolicy\n");
		exit(-1);
	}

	int error;

	// Prioridades
	prio.sched_priority = 1;
	if( pthread_attr_setschedparam(&attr, &prio) != 0) {
		printf("ERROR __attr_setschedparam %d\n", error);
		exit(-1);
	}
	if( (error=pthread_create(&hebraB, &attr, tareaB, NULL)) != 0) {
		printf("ERROR __pthread_create \ttipo: %d\n", error);
		exit(-1);
	}

	prio.sched_priority = 2;
	if( pthread_attr_setschedparam(&attr, &prio) != 0) {
		printf("ERROR __attr_setschedparam\n");
		exit(-1);
	}
	if( pthread_create(&hebraM, &attr, tareaM, NULL) != 0) {
		printf("ERROR __pthread_create\n");
		exit(-1);
	}

	prio.sched_priority = 3;
	if( pthread_attr_setschedparam(&attr, &prio) != 0) {
		printf("ERROR __attr_setschedparam\n");
		exit(-1);
	}
	if( pthread_create(&hebraA, &attr, tareaA, NULL) != 0) {
		printf("ERROR __pthread_create\n");
		exit(-1);
	}

	pthread_join(hebraA, NULL);
	pthread_join(hebraM, NULL);
	pthread_join(hebraB, NULL);

	return 0;
}