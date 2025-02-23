//
// jlu操作系统实验1
// 进程与线程 - 基于fork
// yusy 21221116
//

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include "unistd.h"
#include "string.h"

char r_buf[4]; // 读缓冲
char w_buf[4]; // 写缓冲
int pipe_fd[2];
pid_t pid1, pid2, pid3, pid4;

int producer(int id);
int consumer(int id);

int main(int argc, char **argv) {
    if (pipe(pipe_fd) < 0) {
        printf("pipe create error\n");
        exit(-1);
    } else {
        // 创建子进程
        printf("pipe is created successfully!\n");
        if ((pid1 = fork()) == 0) producer(1);
        if ((pid2 = fork()) == 0) producer(2);
        if ((pid3 = fork()) == 0) consumer(1);
        if ((pid4 = fork()) == 0) consumer(2);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // 等待4个子进程全部结束放出四个结束信号后父进程才退出
    int i, pid, status;
    for (i = 0; i < 4; i++) {
        pid = wait(&status);
    }

    exit(0);
}

int producer(int id) {
    printf("producer %d is running\n", id);

    close(pipe_fd[0]); // 关闭读指针

    for (int i = 1; i < 10 ; i++) {
        sleep(3);

        if (id == 1) {
            // 生产者1
            strcpy(w_buf, "aaa\0");
        } else {
            // 生产者2
            strcpy(w_buf, "bbb\0");
        }

        // 写管道
        if (write(pipe_fd[1], w_buf, 4) == -1) {
            printf("write to pipe error\n");
        }
    }

    close(pipe_fd[1]); // 关闭写指针

    printf("producer %d is over!\n", id);

    exit(id);
}

int consumer(int id)
{
    printf("consumer %d is running\n", id);

    close(pipe_fd[1]); // 关闭写指针

    if (id == 1) {
        // 消费者1
        strcpy(w_buf, "ccc\0");
    } else {
        // 消费者2
        strcpy(w_buf, "ddd\0");
    }

    while (1) {
        sleep(1);
        strcpy(r_buf, "eee\0");

        // 从管道中读到消息
        if (read(pipe_fd[0], r_buf, 4) == 0) {
            break;
        }
        printf("consumer %d gets %s, while the w_buf is %s\n", id, r_buf, w_buf);
    }

    close(pipe_fd[0]); // 关闭读指针

    printf("consumer %d is over!\n", id);

    exit(id);
}