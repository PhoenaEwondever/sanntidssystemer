#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "network/udp.h"
#include "time/time.h"

UDPConn* conn;

float Kp = 10.0;
float Ki = 800.0;
int sample_time = 1000 * 1000 * 5; // nanoseconds
float dt = 0.005; // seconds. make sure that this is the same time as in the line above.
float integral = 0.0;

float y;
static sem_t y_sem, signal_sem;

// function to parse the GET_ACK. returns float y value. 
float parse_get(char * str, int len){
	char * pch;
	pch = strtok (str,":");
	pch = strtok (NULL,":");
	return atof(pch);
}

float regulator(float rval, float yval){
	float error = (rval - yval);
	integral = integral + dt * error; 
	return Kp * error + Ki * integral;
}

void* controller_th(void*ptr){
	float ref = 1.0;
	char sendBuf[64];
	
	struct timespec interval = timespec_normalized(0, sample_time);
		
	struct timespec currenttime;
	struct timespec waketime;
	struct timespec onesec = timespec_normalized(1, 0);
	
	clock_gettime(CLOCK_REALTIME, &currenttime);
	struct timespec halftime = timespec_add(currenttime, onesec);
		
	while(1){
		clock_gettime(CLOCK_REALTIME, &currenttime);
		
		if (timespec_cmp(currenttime, halftime) > 0 && ref == 1.0){
			ref = 0.0;
		}
	
		sprintf(sendBuf, "GET");    
		udpconn_send(conn, sendBuf);

		sem_wait(&y_sem);
		
		float u = regulator(ref, y);
		
		sprintf(sendBuf, "SET:%f", u);
		udpconn_send(conn, sendBuf);
		
		waketime = timespec_add(currenttime, interval);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
}

void* recv_th(void*ptr){
	char recvBuf[64];
	memset(recvBuf, 0, sizeof(recvBuf));
	
	while(1){
		udpconn_receive(conn, recvBuf, sizeof(recvBuf));

		if(strcmp(recvBuf, "SIGNAL") == 0){
			sem_post(&signal_sem);
		} else {
			y = parse_get(recvBuf, sizeof(recvBuf));
			sem_post(&y_sem);
		}
	}
}

void* signal_th(void*ptr){
	char sendBuf[64];
	
	while(1){
		sem_wait(&signal_sem);
		sprintf(sendBuf, "SIGNAL_ACK");    
		udpconn_send(conn, sendBuf);
	}
}

int main(){
	
	sem_init(&y_sem, 0, 0);
	sem_init(&signal_sem, 0, 0);
	
	conn = udpconn_new("10.100.23.145", 9999);
	pthread_t controller_thread, recv_thread, signal_thread;
	
	struct timespec currenttime;
	clock_gettime(CLOCK_REALTIME, &currenttime);
	struct timespec twosec = timespec_normalized(2, 0);
	struct timespec endtime = timespec_add(currenttime, twosec);
	
	char sendBuf[64];
	sprintf(sendBuf, "START");  
	udpconn_send(conn, sendBuf);\
	
	pthread_create(&recv_thread, NULL, recv_th, NULL);
	pthread_create(&controller_thread, NULL, controller_th, NULL);
	pthread_create(&signal_thread, NULL, signal_th, NULL);
	
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &endtime, NULL);
	
	pthread_cancel(controller_thread);
	pthread_cancel(recv_thread);
	pthread_cancel(signal_thread);
	
	sprintf(sendBuf, "STOP");    
	udpconn_send(conn, sendBuf);
	udpconn_delete(conn);
	
	sem_destroy(&y_sem);
	sem_destroy(&signal_sem);
}





