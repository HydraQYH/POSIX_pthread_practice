#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 100

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
        // pthread_cond_signal(&cond); // 若消费者处于睡眠状态，唤醒消费者
        if (g_count == 1)
        {
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    return p_args;
}

void* function2(void* p_args)
{
    int* p = (int*)p_args;
    int remain = N;
    while (remain != 0)
    {
        pthread_mutex_lock(&mutex);
        while (g_count == 0)
        {
            printf("Consumer thread sleep, remain %d elements\n", remain);
            pthread_cond_wait(&cond, &mutex);
            printf("Consumer thread wake up, remain %d elements\n", remain);
        }
        // g_count > 0 说明buffer中有需要处理的值
        p[--g_count] = -1;
        printf("Consumer Thread delete element %d\n", g_count);
        --remain;
        pthread_mutex_unlock(&mutex);
    }
    return p_args;
}

int main(void)
{
    pthread_t tokens[2];
    int* p_buffer = (int*)malloc(sizeof(int) * N);
    void* p_f1_ret = NULL;
    void* p_f2_ret = NULL;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&tokens[1], NULL, function2, (void*)p_buffer);
    sleep(1);
    pthread_create(&tokens[0], NULL, function1, (void*)p_buffer);
    pthread_join(tokens[1], &p_f2_ret);
    pthread_join(tokens[0], &p_f1_ret);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    free(p_buffer);
    return 0;
}

