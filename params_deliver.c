#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 5

struct params {
    int id;
    int* p_value;
};

void* thread_func(void* p_arg)
{
    struct params* p_tmp = (struct params*)p_arg;
    printf("Thread %d start...\n", p_tmp->id);
    *p_tmp->p_value = N - p_tmp->id;
    return (void*)p_tmp->p_value;
}

int main(void)
{
    pthread_t tokens[N];
    struct params args[N];
    for (size_t i = 0; i < N; i++)
    {
        args[i].id = i;
        args[i].p_value = (int*)malloc(sizeof(int));
    }
    
    for (size_t i = 0; i < N; i++)
    {
        pthread_create(&tokens[i], NULL, thread_func, (void*)&args[i]);
    }

    void* p_result = NULL;
    for (size_t i = 0; i < N; i++)
    {
        pthread_join(tokens[i], &p_result);
        printf("Main thread get value %d from thread %zu\n", *(int*)p_result, i);
    }
    

    for (size_t i = 0; i < N; i++)
    {
        free(args[i].p_value);
    }
    
    return 0;
}

