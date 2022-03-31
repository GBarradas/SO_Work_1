#include<stdlib.h>
#include<stdio.h>


typedef struct process * Process;
typedef struct so *SO;
enum States
{
    NEW,
    READY,
    RUN,
    BLOCKED,
    EXIT,
    
};

struct process{
    int a;
};