#include <stdio.h>
#include <pthread.h>
#define NTHREAD 4


pthread_mutex_t mutex;
long global = 0;
int increment(int i);


int increment(int i)
{
	return i+1;
}

void* func(void* param)
{
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


int main()
{

	printf("la taille d'un hash est : %ld\n",sizeof(hash));
	pthread_t thread[NTHREAD];
	int err;
	for(int i=0; i<NTHREAD; i++)
	{
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
	printf("global: %ld\n",global);
}
