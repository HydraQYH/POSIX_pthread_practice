#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <pthread.h>
#define N 5

struct params
{
    int a;
    char b;
    float c;
    double d;
};


void func_para_int_0(int val)
{
    // 参数为int 返回值类型为void 报warning
    printf("This is a thread function with arg: %d\n", val);
}

void* func_para_int_1(int val)
{
    // 参数为int 返回值类型为void*
    printf("This is a thread function with arg: %d\n", val);
    pthread_exit(0);
}

// void* func_para_int_2(int val)
// {
//     // 在线程入口函数内部声明局部变量（存储在当前线程栈上）
//     // 并将其地址转换为void*返回 在主线程中会出现段错误
//     int tmp = -1 * val;
//     printf("This is a thread function with arg: %d\n", val);
//     return (void*)&tmp;
// }

void* func_para_int_3(int val)
{
    int* p = (int*)malloc(sizeof(int));
    *p = val;
    printf("This is a thread function with arg: %d\n", val);
    return (void*)p;
}

void* func_para_void_p(void* p_arg)
{
    printf("This is a thread function with arg: %d\n", *(int*)p_arg);
    return p_arg;
}

void* function(void* p_struct)
{
    printf("Thread execute...\n");
    struct params* p = (struct params*)p_struct;
    printf("Thread param.a: %d\n", p->a);
    p->a = 20;
    printf("Thread param.a: %c\n", p->b);
    p->b = 'R';
    printf("Thread param.a: %f\n", p->c);
    p->c = 12.34;
    printf("Thread param.a: %f\n", p->d);
    p->d = 98.0;
    printf("Thread exit...\n");
    return p_struct;
}

int main(void)
{
    pthread_t tokens[N];
    int data[N];
    for (int i = 0; i < N; i++)
    {
        data[i] = i;
    }
    
    int* p_ret = NULL;
    void* p_tmp = NULL;
    
    // for (int i = 0; i < N; i++)
    // {
    //     // pthread_create系统调用第三个参数为函数指针
    //     // 该函数指针的类型为 void *(*start_rtn)(void)
    //     // 其返回类型应尽量限制为void*
    //     pthread_create(&tokens[i], NULL, func_para_int_3, i);
    // }

    // for (int i = 0; i < N; i++)
    // {
    //     pthread_join(tokens[i], (void**)&p_ret);
    //     printf("This is main thread, return value is %d\n", *p_ret);
    //     free(p_ret);
    // }

    // for (size_t i = 0; i < N; i++)
    // {
    //     pthread_create(&tokens[i], NULL, func_para_void_p, (void**)&data[i]);
    // }
    
    // for (size_t i = 0; i < N; i++)
    // {
    //     pthread_join(tokens[i], &p_tmp);
    //     printf("Main thread get value %d\n", *(int*)p_tmp);
    // }
    pthread_t t;
    struct params info = {1, 'c', 5.3, 14.2};
    pthread_create(&t, NULL, function, (void*)&info);
    pthread_join(t, &p_tmp);
    printf("Main Thread execute...\n");
    // struct params* p = (struct params*)p_tmp;
    struct params* p = &info;
    printf("Main Thread param.a: %d\n", p->a);
    printf("Main Thread param.a: %c\n", p->b);
    printf("Main Thread param.a: %f\n", p->c);
    printf("Main Thread param.a: %f\n", p->d);
    printf("Main Thread exit...\n");
    return 0;
}

