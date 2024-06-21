//
// jlu操作系统实验2
// 实时调度算法 - EDF&RMS
// yusy 21221116
//

#include <math.h>
#include <sched.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>

typedef struct {
    char task_id;
    int call_num;   // 任务发生次数
    int ci;         // Ci初始值
    int ti;         // Ti初始值
    int ci_left;    // Ci当前值
    int ti_left;    // Ti当前值
    int flag;       // 是否活跃，0为否，2为是
    int arg;
    pthread_t th;   // 任务对应的线程
} task;

void proc(int* args);
void* idle();
int select_proc(int alg);

int task_num = 0;
int idle_num = 0;

int alg; // 所选算法，1为EDF，2为RMS
int curr_proc = -1;
int demo_time = 100; // 演示时间

task* tasks;
pthread_mutex_t proc_wait[100];
pthread_mutex_t main_wait, idle_wait;
float sum = 0;
pthread_t idle_proc;

int main() {
    pthread_mutex_init(&main_wait, NULL);
    pthread_mutex_lock(&main_wait);
    pthread_mutex_init(&idle_wait, NULL);
    pthread_mutex_lock(&idle_wait);

    printf("Please input number of real time tasks:\n"); // 输入实时调度任务数

    scanf("%d", &task_num);
    tasks = (task*) malloc(task_num * sizeof(task));

    for (int i = 0; i < task_num; i++) {
        pthread_mutex_init(&proc_wait[i], NULL);
        pthread_mutex_lock(&proc_wait[i]);
    }

    for (int i = 0; i < task_num; i++) {
        printf("Please input task id, followed by Ci & Ti:\n");
        getchar();
        scanf("%c,%d,%d,", &tasks[i].task_id, &tasks[i].ci, &tasks[i].ti);
        tasks[i].ci_left = tasks[i].ci;
        tasks[i].ti_left = tasks[i].ti;
        tasks[i].flag = 2;
        tasks[i].arg = i;
        tasks[i].call_num = 1;
        sum += (float) tasks[i].ci / (float) tasks[i].ti;
    }

    printf("Please input algorithm, 1 for EDF, 2 for RMS: ");
    getchar();
    scanf("%d", &alg);
    printf("Please input demo time: ");
    scanf("%d", &demo_time);

    double r = 1; // EDF

    // RMS
    if (alg == 2) {
        double a1 = log(2) / task_num;
        double r1 = exp(a1);
        r = ((double) task_num) * ( r1 - 1 );
        printf("r is %lf\n", r);
    }

    // 不可调度
    if (sum > r) {
        printf("(sum = %lf > r = %lf), not schedulable!\n", sum, r);
        exit(2);
    }

    // 创建闲逛进程
    pthread_create(&idle_proc, NULL, (void*) idle, NULL);

    // 创建实时任务线程
    for (int i = 0; i < task_num; i++) {
        pthread_create(&tasks[i].th, NULL, (void*) proc, &tasks[i].arg);
    }

    for (int i = 0; i < demo_time; i++) {
        if ((curr_proc = select_proc(alg)) != -1) {
            // 根据调度算法选择线程
            pthread_mutex_unlock(&proc_wait[curr_proc]); // 唤醒被选中的线程
            pthread_mutex_lock(&main_wait); // 主线程等待
        } else {
            // 无可运行任务，选择闲逛进程
            pthread_mutex_unlock(&idle_wait); // 唤醒闲逛线程
            pthread_mutex_lock(&main_wait); // 主线程等待
        }

        for (int j = 0; j < task_num; j++) {
            if (--tasks[j].ti_left == 0) {
                // 继续下一周期
                tasks[j].ti_left = tasks[j].ti;
                tasks[j].ci_left = tasks[j].ci;
                pthread_create(&tasks[j].th, NULL, (void*)proc, &tasks[j].arg);
                tasks[j].flag = 2;
            }
        }
    }
    printf("\n");

    // sleep(10);

    return 0;
}

void proc(int* args) {
    while(tasks[*args].ci_left > 0) {
        pthread_mutex_lock(&proc_wait[*args]);      // 等待被调度

        if (idle_num != 0) {
            printf("idle(%d)", idle_num);
            idle_num = 0;
        }

        printf("%c%d", tasks[*args].task_id, tasks[*args].call_num);

        tasks[*args].ci_left--; // 执行一个时间单位

        if (tasks[*args].ci_left == 0) {
            printf("(%d)\n", tasks[*args].ci);
            tasks[*args].flag = 0;
            tasks[*args].call_num++;
        }

        pthread_mutex_unlock(&main_wait); // 唤醒主线程
    }
}

void* idle() {
    while (1) {
        pthread_mutex_lock(&idle_wait); // 等待被调度
        printf("->"); // 空耗一个时间单位
        idle_num++;
        pthread_mutex_unlock(&main_wait); // 唤醒主控线程
    }
}

int select_proc(int alg) {
    int temp1, temp2;
    temp1 = 10000;
    temp2 = -1;

    if ((alg == 2) && (curr_proc != -1) && (tasks[curr_proc].flag != 0)) {
        return curr_proc;
    }

    for (int j = 0; j < task_num; j++) {
        if (tasks[j].flag == 2) {
            switch (alg) {
                // EDF
                case 1:
                    if (temp1 > tasks[j].ti_left) {
                        temp1 = tasks[j].ti_left;
                        temp2 = j;
                    }
                    break;
                // RMS
                case 2:
                    if (temp1 > tasks[j].ti) {
                        temp1 = tasks[j].ti;
                        temp2 = j;
                    }
                    break;
            }
        }
    }
    return temp2;
}