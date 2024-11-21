#ifndef TASKS_H_
#define TASKS_H_

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*taskFunc)(int);
} task;


unsigned int map_value(unsigned int aFirst, unsigned int aSecond, unsigned int bFirst, unsigned int bSecond, unsigned int inVal)
{
        return bFirst + (long((inVal - aFirst))*long((bSecond-bFirst)))/(aSecond - aFirst);
}

#endif