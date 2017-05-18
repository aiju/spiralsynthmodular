#include "RWLock.h"
#include <iostream>

RWLock::RWLock(void)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	readers = 0;
	writers = 0;
	wrwait = 0;
}

RWLock::~RWLock(void)
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

void
RWLock::rlock(void)
{
	pthread_mutex_lock(&mutex);
	while(writers > 0 || wrwait > 0)
		pthread_cond_wait(&cond, &mutex);
	readers++;
	pthread_mutex_unlock(&mutex);
}

void
RWLock::runlock(void)
{
	pthread_mutex_lock(&mutex);
	if(writers != 0){
		std::cerr << "Lock " << this << ": wlock -> runlock\n";
		writers = 0;
		pthread_cond_broadcast(&cond);
	}else if(readers <= 0)
		std::cerr << "Lock " << this << ": double unlock\n";
	else if(--readers == 0)
		pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
}

void
RWLock::wlock(void)
{
	pthread_mutex_lock(&mutex);
	while(readers != 0 || writers != 0){
		wrwait++;
		pthread_cond_wait(&cond, &mutex);
		wrwait--;
	}
	writers = 1;
	pthread_mutex_unlock(&mutex);
}

void
RWLock::wunlock(void)
{
	pthread_mutex_lock(&mutex);
	if(readers != 0){
		std::cerr << "Lock " << this << ": rlock -> wunlock\n";
		if(--readers == 0)
			pthread_cond_broadcast(&cond);
	}else if(writers == 0)
		std::cerr << "Lock " << this << ": double unlock\n";
	else{
		writers = 0;
		pthread_cond_broadcast(&cond);
	}
	pthread_mutex_unlock(&mutex);
}
