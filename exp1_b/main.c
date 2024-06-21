//
// jlu操作系统实验1
// 进程与线程 - 基于clone
// yusy 21221116
//

#define _GNU_SOURCE
#include "unistd.h"
#include "pthread.h"
#include "sched.h"
#include "semaphore.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int producer(void* args);
int consumer(void* args);
pthread_mutex_t mutex;
sem_t product;
sem_t warehouse;

char buffer[8][4];
int bp = 0;

int main(int argc, char** argv) {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&product, 0, 0);
    sem_init(&warehouse, 0, 8); // 缓冲区大小为 8

    int clone_flag = CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES;
    char* stack;

    for (int arg = 0; arg < 2; arg++) {
        stack = (char*) malloc(4096);
        clone((void*) producer, &(stack[4095]), clone_flag, (void*) &arg);

        stack = (char*) malloc(4096);
        clone((void*) consumer, &(stack[4095]), clone_flag, (void*) &arg);

//        printf("%d\n", arg);
        sleep(1);
    }
    sleep(60);
    exit(1);
}

int producer(void* args){
    int id = *((int*) args);

    for (int i = 0; i < 10; i++) {
        sleep(i + 1);
        sem_wait(&warehouse); // 若满，等待消费者取走物品
        pthread_mutex_lock(&mutex); // 互斥操作
        if (id == 0) {
            strcpy(buffer[bp], "aaa\0");
        } else {
            strcpy(buffer[bp], "bbb\0");
        }
        bp++;
        printf("producer %d produce %s in %d\n", id, buffer[bp - 1], bp - 1);
        pthread_mutex_unlock(&mutex);
        sem_post(&product); // 唤醒消费者（若有）
    }
    printf("producer %d is over!\n", id);
}

int consumer(void* args) {
    int id = *((int*) args);

    for (int i = 0; i < 10; i++) {
        sleep(10 - i);
        sem_wait(&product); // 若满，等待消费者取走物品
        pthread_mutex_lock(&mutex); // 互斥操作
        bp--;
        printf("consumer %d get %s in %d\n", id, buffer[bp], bp);
        strcpy(buffer[bp], "zzz\0");
        pthread_mutex_unlock(&mutex);
        sem_post(&warehouse); // 唤醒消费者（若有）
    }
    printf("consumer %d is over!\n", id);
}