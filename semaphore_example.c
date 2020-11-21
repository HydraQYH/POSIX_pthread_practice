#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define N 5

struct params {
    int* p_value;
    int id;
};

sem_t sem;
pthread_mutex_t mutexs[N];

void* thread_function(void* p_args)
{
    struct params* p = (struct params*)p_args;
    int index_id = p->id % N;
    sem_wait(&sem); // 控制同时最多只能有N个thread进入当前代码段
    printf("Thread %d go into critical region\n", p->id);
    if (p->id == index_id)
    {
        pthread_mutex_lock(&mutexs[index_id]);
        *p->p_value = p->id;
        pthread_mutex_unlock(&mutexs[index_id]);
    }
    else
    {
        pthread_mutex_lock(&mutexs[index_id]);
        *p->p_value = -1;
        pthread_mutex_unlock(&mutexs[index_id]);
    }
    sleep(5);
    printf("Thread %d exit critical region\n", p->id);
    sem_post(&sem);
    return p_args;
}

int main(void)
{
    int values[N];
    pthread_t tokens[2 * N];
    struct params thread_args[2 * N];
    /**
     * int sem_init (sem_t *sem, int pshared, unsigned int value);
     * 第二个参数是0，表示它是当前里程的局部信号量，否则，其它进程就能够共享这个信号量。
    */
    int code = sem_init(&sem, 0, N);
    if (code != 0)
    {
        printf("semaphore init failed, exit from main thread with error code %d\n", code);
        return 0;
    }
    
    for (int i = 0; i < 2 * N; i++)
    {
        thread_args[i].id = i;
        thread_args[i].p_value = values + (i % N);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_init(&mutexs[i], NULL);
    }
    
    for (int i = 0; i < 2 * N; i++)
    {
        pthread_create(&tokens[i], NULL, thread_function, (void*)&thread_args[i]);
    }

    void* p_tmp = NULL;
    for (int i = 0; i < 2 * N; i++)
    {
        pthread_join(tokens[i], &p_tmp);
    }
    
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_destroy(&mutexs[i]);
    }
    sem_destroy(&sem);
    return 0;
}

