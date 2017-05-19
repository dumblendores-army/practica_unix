#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// OTRO cosas del mac...
#ifndef TIMER_ABSTIME
#define 	TIMER_ABSTIME   0x01
#endif

void t1();
void t2();
void t3();
void t4();
void t5();

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
*	Funcion para pillar el tiempo actual y mostrarlo por consola
*
*/
void get_actual_time(){
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ("%s", asctime (timeinfo) );
}

/*
*	CLAVE
*	
*	Temporización mediante relojes. Cosas del profesor
*
*	@param tm variable de tiempo a normalizar
*/
static void normalizar(struct timespec* tm)
{
    if (tm->tv_nsec >= 1000000000L) {
        tm->tv_sec += (tm->tv_nsec / 1000000000L);
        tm->tv_nsec = (tm->tv_nsec % 1000000000L);
    } else if (tm->tv_nsec <= -1000000000L) {
        tm->tv_sec += - (-tm->tv_nsec / 1000000000L);
        tm->tv_nsec = - (-tm->tv_nsec % 1000000000L);
    }
    if ((tm->tv_sec > 0)&&(tm->tv_nsec < 0)) {
        tm->tv_nsec += 1000000000L;
        tm->tv_sec -= 1;
    } else if ((tm->tv_sec < 0)&&(tm->tv_nsec > 0)) {
        tm->tv_nsec -= 1000000000L;
        tm->tv_sec += 1;
    }
    assert((tm->tv_sec>=0 && tm->tv_nsec>=0 && tm->tv_nsec<1000000000L)
           ||(tm->tv_sec<0 && tm->tv_nsec>-1000000000L && tm->tv_nsec<=0));
}

void error() {
	printf("ERROR!\n");
}

int main(){

	// CLAVE variables minimas para el funcionamiento
	struct timespec next_activation;

	// OTRO variables para mi forma de hacer el ejercicio
	uint8_t ciclo = 0x00;
	uint8_t mask = 0x01;

	//======================
	// CREAR TEMPORIZADOR
	//======================

	if( clock_gettime(CLOCK_MONOTONIC, &next_activation))
		error();
	
	//======================
	// ESPERAR SEÑAL
	//======================
	while(1) {

		next_activation.tv_sec += 10; // Calculamos siguiente activacion
		normalizar(&next_activation); // Normalizamos

		// Nos ciclamos
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

		ciclo = (ciclo+1) & mask; // esto son mierdas mias
		if( clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL))
			error();

	}

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
