#ifndef TASKS_H_
#define TASKS_H_

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*taskFunc)(int);
} task;


#endif
