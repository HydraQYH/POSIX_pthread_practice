#include <stdio.h>
#include <pthread.h>

void* thread(void* arg)
{
    printf("This is a new thread, the arg is %d\n", *(int*)arg);
    *(int*)arg = 0;
    return arg;
}

int main(void)
{
    pthread_t th;
    int ret;
    int arg = 10;
    int* thread_ret = NULL;
    ret = pthread_create(&th, NULL, thread, &arg);
    if (ret != 0)
    {
        printf("Can not create a new thread.\n");
        return -1;
    }
    
    printf("This is main thread.\n");
    pthread_join(th, (void**)&thread_ret);
    printf("Thread return is %d\n", *(int*)(thread_ret));
    return 0;
}

