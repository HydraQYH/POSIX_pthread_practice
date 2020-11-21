#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 100
#define CT 4

struct params {
    int* p_buffer;
    int id;
};

pthread_mutex_t mutex;
pthread_cond_t cond;
int g_count = 0;

void* function1(void* p_args)
{
    int* p = (int*)p_args;
    // 一共向buffer中添加了N个数字
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_lock(&mutex);
        *(p + g_count) = g_count;
        g_count++;
        printf("Producer Thread add elemnet %d\n", g_count - 1);
        if (g_count == 1)
        {
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    return p_args;
}

void* function2(void* p_args)
{
    struct params *p = (struct params *)p_args;
    int remain = N / CT;
    while (remain != 0)
    {
        pthread_mutex_lock(&mutex);
        while (g_count == 0)
        {
            printf("Consumer %d thread sleep, remain %d elements\n", p->id, remain);
            pthread_cond_wait(&cond, &mutex);
            printf("Consumer %d thread wake up, remain %d elements\n", p->id, remain);
        }
        // g_count > 0 说明buffer中有需要处理的值
        p->p_buffer[--g_count] = -1;
        printf("Consumer %d Thread delete element %d\n", p->id, g_count);
        --remain;
        pthread_mutex_unlock(&mutex);
    }
    printf("Consumer %d Thread exit...\n", p->id);
    return p_args;
}

int main(void)
{
    pthread_t producer;
    pthread_t consumer[CT];
    struct params data[CT];
    int* p_buffer = (int*)malloc(sizeof(int) * N);
    void* p_f1_ret = NULL;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    for (int i = 0; i < CT; i++)
    {
        data[i].p_buffer = p_buffer;
        data[i].id = i;
    }
    
    for (size_t i = 0; i < CT; i++)
    {
        pthread_create(&consumer[i], NULL, function2, (void*)&data[i]);
    }
    sleep(1);
    pthread_create(&producer, NULL, function1, (void*)p_buffer);
    for (size_t i = 0; i < CT; i++)
    {
        void* p_f2_ret = NULL;
        pthread_join(consumer[i], &p_f2_ret);   
    }
    pthread_join(producer, &p_f1_ret);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    free(p_buffer);
    return 0;
}

