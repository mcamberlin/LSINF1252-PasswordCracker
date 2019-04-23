#include <stdio.h>
#include <pthread.h>
#define NTHREAD 4


//pthread_mutex_t mutex;
long global = 0;
int increment(int i);


int increment(int i)
{
	return i+1;
}

void* func(void* param)
{
	pthread_mutex_t mutex;
	for(int i=0; i<1000; i++)
	{
		pthread_mutex_lock(&mutex);
		global = increment(global);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}


typedef struct hash{
	char hash[32];
}hash;

void* funcThreadEnDehorsDeMain()
{
	pthread_t thread[NTHREAD];
	int err;
	for(int i=0; i<NTHREAD; i++)
	{
		printf("creation d'un thread");
		err=pthread_create(&(thread[i]),NULL,&func,NULL);
		if(err!=0)
			printf("pthread_create");
	}
	/*...*/
	for(int i=NTHREAD-1;i>=0;i--)
	{
		err=pthread_join(thread[i],NULL);
		if(err!=0)
			printf("pthread_join");
	}

}


int main()
{

	printf("la taille d'un hash est : %ld\n",sizeof(hash));
	pthread_t thread;
	int err;
	for(int i=0; i<1; i++)
	{
		err=pthread_create(&(thread),NULL,&funcThreadEnDehorsDeMain,NULL);
		if(err!=0)
			printf("pthread_create");
	}
	/*...*/
	for(int i=1;i>0;i--)
	{
		err=pthread_join(thread,NULL);
		if(err!=0)
			printf("pthread_join");
	}
	printf("global: %ld\n",global);
}
