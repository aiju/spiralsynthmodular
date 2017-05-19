#include "RWLock.h"
#include <iostream>

RWLock::RWLock(void)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	readers = 0;
	writers = 0;
	wrwait = 0;
	writer = pthread_self();
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
	if(writers == 0 || !pthread_equal(pthread_self(), writer))
		while(writers > 0 || wrwait > 0)
			pthread_cond_wait(&cond, &mutex);
	readers++;
	pthread_mutex_unlock(&mutex);
}

void
RWLock::runlock(void)
{
	pthread_mutex_lock(&mutex);
	if(readers <= 0)
		std::cerr << "Lock " << this << ": double unlock\n";
	else if(--readers == 0)
		pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
}

void
RWLock::wlock(void)
{
	pthread_mutex_lock(&mutex);
	while(readers != 0 || writers != 0 && !pthread_equal(pthread_self(), writer)){
		wrwait++;
		pthread_cond_wait(&cond, &mutex);
		wrwait--;
	}
	writers++;
	writer = pthread_self();
	pthread_mutex_unlock(&mutex);
}

void
RWLock::wunlock(void)
{
	pthread_mutex_lock(&mutex);
	if(writers <= 0)
		std::cerr << "Lock " << this << ": double unlock\n";
	else
		if(--writers == 0)
			pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
}
