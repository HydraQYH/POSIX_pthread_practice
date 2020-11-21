#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct params {
    int* p_s;
    int* p_d;
};

pthread_mutex_t mutex;
pthread_cond_t cond;

void* producer(void* p_arg)
{   
    struct params* p = (struct params*)p_arg;
    // 生产者生产一个数据（产生一个事件）
    // 流程：
    //      1. 加锁
    //      2. 生产数据
    //      3. 向消费者发送信号
    //      4. 解锁
    printf("producer starting...\n");
    // pthread_mutex_lock(&mutex);
    // *p->p_s = 820;
    // pthread_cond_signal(&cond);
    // pthread_mutex_unlock(&mutex);
    for (int i = 0; i < 2; i++)
    {
        pthread_mutex_lock(&mutex);
        printf("Producer %d\n", i);
        *p->p_s = 820 + i * 56;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    
    printf("producer exit...\n");
    return (void*)p->p_s;
}

void* consumer(void* p_arg)
{
    struct params* p = (struct params*)p_arg;
    // 消费者消费一个数据
    // 流程：
    //      1. 加锁
    //      2. 等待生产者发送信号
    //      3. 处理生产者生产的数据
    //      4. 解锁
    printf("consumer starting...\n");
    // pthread_mutex_lock(&mutex);
    // pthread_cond_wait(&cond, &mutex);
    // *p->p_d = *p->p_s;
    // pthread_mutex_unlock(&mutex);
    for (int i = 0; i < 2; i++)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("Consumer %d\n", i);
        *p->p_d = *p->p_s;
        pthread_mutex_unlock(&mutex);
    }
    
    printf("consumer exit...\n");
    return (void*)p->p_d;
}

int main(void)
{
    int source = -1;
    int destination = -1;
    struct params args;
    args.p_s = &source;
    args.p_d = &destination;
    void* producer_ret = NULL;
    void* consumer_ret = NULL;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t token_p, token_c;
    /*
    * 问题1:假设生产者先执行，并且先拿到锁，进而优先产生数据，那么，生产者产生数据完毕后会通知消费者处理数据，
    * 但是此时有一种情况，就是消费者根本就没有拿到锁，也没有执行wait，这时会产生什么样的现象？
    * 实验1：先创建生产者线程，然后延时5秒，再创建消费者线程，观察现象。
    * 现象1：消费者线程hang住。
    * 经验：生产者调用singal或broadcast时，一定要有正在处于wait的线程才行，
    * 假设如果没有任何线程处于wait状态，这次singal就会被浪费
    */
    
    pthread_create(&token_c, NULL, consumer, (void*)&args);
    for (size_t i = 0; i < 5; i++)
    {
        printf("%lus\n", 5 - i);
        sleep(1);
    }
    pthread_create(&token_p, NULL, producer, (void*)&args);

    /**
     * 问题2:假设有如下执行顺序：
     *  1.消费者进入wait状态并释放锁
     *  2.生产者获得锁并生产数据
     *  3.生产者通知消费者处理生产的数据
     *  4.生产者释放锁
     *  5.生产者再次获得锁（消费者没有竞争过生产者抢到锁）
    */

    pthread_join(token_p, &producer_ret);
    pthread_join(token_c, &consumer_ret);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Main Thread: source is %d, destination is %d\n", *args.p_s, *args.p_d);
    return 0;
}

