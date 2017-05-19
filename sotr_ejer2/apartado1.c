#include <sys/time.h>
#include <sys/signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

void t1();
void t2();
void t3();
void t4();
void t5();

void espera_activa( int tiempo) {

	time_t t;

	//Bucle de espera 
    t = time(0) + tiempo;
    while(time(0) < t);
}

void get_actual_time(){
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ("%s", asctime (timeinfo) );
}
/*
static void normalizar(struct timespec* tm) {

	if (tm->tv_nsec >= 1000000000L

}*/

void error() {
	printf("ERROR!\n");
}

int main(){

	struct itimerspec required, old;
	struct timespec first, period;
	struct sigevent sig;
	int signum;
	timer_t timer;
	sigset_t set;
	uint8_t ciclo = 0x00;
	uint8_t mask = 0x01;

	//======================
	// CREAR TEMPORIZADOR
	//======================

	// Preparamos las señales
	if (sigemptyset(&set) != 0) error();
	if (sigaddset(&set, SIGALRM) != 0) error();

	// Asociamos señal y temporizador
	sig.sigev_notify = SIGEV_SIGNAL;
	sig.sigev_signo = SIGALRM;
	sig.sigev_value.sival_ptr = &timer;

	if (clock_gettime(CLOCK_MONOTONIC, &first) != 0) error(); // No necesario si usamos tiempos relativos
	if (timer_create(CLOCK_MONOTONIC, &sig, &timer) != 0) error();

	first.tv_sec = 1; // o first.tv_sec + 3 con tiempos absolutos
	first.tv_nsec = 0;
	period.tv_sec = 10;
	period.tv_nsec = 0;
	required.it_value = first;
	required.it_interval = period;

	if (timer_settime(timer, 0, &required, &old) != 0)
		error();

	pthread_sigmask(SIG_BLOCK,&set,NULL);
	
	//======================
	// ESPERAR SEÑAL
	//======================
	while(1) {
		if (sigwait(&set, &signum) != 0) error();

		// segundo ciclo 
		if (ciclo){
			t1();
			t2();
			t3();
			t4();
		}

		// primer ciclo
		else {
			t1();
			t2();
			t5(); // este entra en este ciclo por webis
		}

		ciclo = (ciclo+1) & mask;
	}

	// Destruir timers: funcion timer_delete
	timer_delete(timer);
	return 0;
}

void t1() {
	printf("Soy la tarea 1\n");
	printf("Ejecucion comenzada en: ");
	get_actual_time();

	espera_activa(3);

	printf("Ejecucion terminada en: ");
	get_actual_time();
}

void t2() {
	printf("Soy la tarea 2\n");
	printf("Ejecucion comenzada en: ");
	get_actual_time();

	espera_activa(3);	

	printf("Ejecucion terminada en: ");
	get_actual_time();
}
void t3() {
	printf("Soy la tarea 3\n");
	printf("Ejecucion comenzada en: ");
	get_actual_time();

	espera_activa(2);

	printf("Ejecucion terminada en: ");
	get_actual_time();
}
void t4(){
	printf("Soy la tarea 4\n");
	printf("Ejecucion comenzada en: ");
	get_actual_time();
	
	espera_activa(2);

	printf("Ejecucion terminada en: ");
	get_actual_time();
}
void t5(){
	printf("Soy la tarea 5\n");
	printf("Ejecucion comenzada en: ");
	get_actual_time();
	
	espera_activa(2);

	printf("Ejecucion terminada en: ");
	get_actual_time();
}