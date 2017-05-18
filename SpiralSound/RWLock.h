#pragma once

#include <pthread.h>

class RWLock {
	public:
		RWLock();
		~RWLock();
		void rlock(void);
		void runlock(void);
		void wlock(void);
		void wunlock(void);

	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		int readers, writers, wrwait;
};

class RLocker {
	public:
		RLocker(RWLock *l){
			lock = l;
			lock->rlock();
		}
		~RLocker(void){
			lock->runlock();
		}
	private:
		RWLock *lock;
};

class WLocker {
	public:
		WLocker(RWLock *l){
			lock = l;
			lock->wlock();
		}
		~WLocker(void){
			lock->wunlock();
		}
	private:
		RWLock *lock;
};
