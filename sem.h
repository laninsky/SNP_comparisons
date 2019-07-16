#ifndef SEM_H
#define SEM_H

// ------------------------------
// Linux Specific Code
// ------------------------------
#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#define semaphore sem_t

typedef unsigned char bool;
#define true 1
#define false 0 

// Wait for a semaphore
void semaphore_wait(semaphore *s) {
	// Sem wait
	sem_wait(s);
}

// Signal a semaphore
void semaphore_signal(semaphore *s) {
	// Sem post
	sem_post(s);
}

// Create a semaphore initialised to value
void semaphore_create(semaphore *s, int value) {
	// Initialise Semaphore
	sem_init(s, 0, value);
}
#endif
// ------------------------------

// ------------------------------
// macOS Specific Code
// ------------------------------
#ifdef __APPLE__

#include <pthread.h>
#include <unistd.h>
#include <dispatch/dispatch.h>
#define semaphore dispatch_semaphore_t

// Wait for a semaphore
void semaphore_wait(semaphore *s) {
	// Dispatch wait
	dispatch_semaphore_wait(*s, DISPATCH_TIME_FOREVER);
}

// Signal a semaphore
void semaphore_signal(semaphore *s) {
	// Dispatch signal
	dispatch_semaphore_signal(*s);
}

// Create a semaphore initialised to value
void semaphore_create(semaphore *s, int value) {
	// Dispatch create semaphore
	(*s) = dispatch_semaphore_create(value);
}
#endif
// ------------------------------

// ------------------------------
// UNIX Specific Code
// ------------------------------
#if defined(__APPLE__) || defined(__linux__)

// Create a thread (pthreads)
void create_thread(void *(*func)(void*), void *args) {
	// Create Thread - POSIX API
	pthread_t thread;
	pthread_create(&thread, NULL, func, args);
}

#endif

#endif // LIFT_H_