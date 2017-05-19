#include <stdio.h>
#include <unistd.h>
#define __USE_UNIX98
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>

pthread_mutex_t mutex, mutex2, mutex3;
time_t init_time;
int cnt, cnt2;

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

/*
*	CLAVE
*
*	Funcion para pillar el tiempo actual de ejecucion
*
*   @return segundos que han pasado desde el comienzo de la ejecucion
*/
int get_the_time(){
	return (time(NULL) - init_time);
}

void *tareaA( void * args) {
	printf("%d::A::voy a dormir\n", get_the_time());
	sleep(3);
	printf("%d::A::me despierto y pillo mutex\n", get_the_time());
	pthread_mutex_lock(&mutex);
	printf("%d::A::incremento valor\n", get_the_time());
	++cnt;
	printf("%d::A::desbloqueo mutex\n", get_the_time());
	pthread_mutex_unlock(&mutex);
	printf("%d::A::FINISH\n", get_the_time());
}

void *tareaM( void * args) {
	printf("\t%d::M::me voy a dormir\n", get_the_time());
	sleep(2);

	printf("\t%d::M::me despierto y pillo mutex2\n", get_the_time());
	pthread_mutex_lock(&mutex2);
	printf("\t%d::M::pillo mutex3\n", get_the_time());
	pthread_mutex_lock(&mutex3);
	
	printf("\t%d::M::incremento variable cnt2\n", get_the_time());
	++cnt2;

	printf("\t%d::M::libero mutex3\n", get_the_time());
	pthread_mutex_unlock(&mutex3);
	printf("\t%d::M::libero mutex2\n", get_the_time());
	pthread_mutex_unlock(&mutex2);

	printf("\t%d::M::FINSIH\n", get_the_time());
}

void *tareaB( void * args) {
	printf("\t\t%d::B::me voy a dormir\n", get_the_time());
	sleep(1);

	printf("\t\t%d::B::me despierto y pillo mutex3\n", get_the_time());
	pthread_mutex_lock(&mutex3);
	printf("\t\t%d::B::espera activa\n", get_the_time());
	espera_activa(3);

	printf("\t\t%d::B::pillo mutex2\n", get_the_time());
	pthread_mutex_lock(&mutex2);
	printf("\t\t%d::B::incremento cnt2\n", get_the_time());
	++cnt2;
	
	printf("\t\t%d::B::suelto mutex2\n", get_the_time());
	pthread_mutex_unlock(&mutex2);
	printf("\t\t%d::B::suelto mutex3\n", get_the_time());
	pthread_mutex_unlock(&mutex3);


	printf("\t\t%d::B::FINISH\n", get_the_time());
}

int main() {

	pthread_t hebraA, hebraM, hebraB;
	pthread_attr_t attr;
	pthread_mutexattr_t attrM, attrM2, attrM3;
	struct sched_param prio;

	time( &init_time);
	cnt = 0;
	cnt2 = 0;

	printf("===DEBUG MODO \"ON\"===\nEXEC_TIME::THREAD_TAG::VERBOSE_INFO\n\n");

	// ====================
	// MUTEX
	// ====================

	pthread_mutexattr_init(&attrM);
	pthread_mutexattr_init(&attrM2);
	pthread_mutexattr_init(&attrM3);
	
	if( pthread_mutexattr_setprotocol(&attrM, PTHREAD_PRIO_INHERIT) != 0) {
		printf("ERROR en __set_protocol\n");
		exit(-1);
	}
	if( pthread_mutexattr_setprioceiling(&attrM, 3) != 0){
		printf("ERROR en __setprioceiling\n");
	}

	if( pthread_mutexattr_setprotocol(&attrM2, PTHREAD_PRIO_INHERIT) != 0) {
		printf("ERROR en __set_protocol\n");
		exit(-1);
	}
	if( pthread_mutexattr_setprioceiling(&attrM2, 2) != 0){
		printf("ERROR en __setprioceiling\n");
	}

	if( pthread_mutexattr_setprotocol(&attrM3, PTHREAD_PRIO_INHERIT) != 0) {
		printf("ERROR en __set_protocol\n");
		exit(-1);
	}
	if( pthread_mutexattr_setprioceiling(&attrM3, 2) != 0){
		printf("ERROR en __setprioceiling\n");
	}

	// Sin ningun tipo de prioridad, todos usaran los mismos attrM
	pthread_mutex_init(&mutex, &attrM);
	pthread_mutex_init(&mutex2, &attrM2);
	pthread_mutex_init(&mutex3, &attrM3);

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