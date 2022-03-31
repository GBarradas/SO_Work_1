#include<stdlib.h>
#include<stdio.h>
#include"Queue.c"


typedef struct program * Program;
typedef struct so *SO;
enum States
{
    NEW,
    READY,
    RUN,
    BLOCKED,
    EXIT,
    FINISH,
    NOTCREATE,

};

struct program{
    int now;
    enum States state;
    int timeToStatrt;
    int cycle[10];

};
struct so{
    int instance;
    int numOfPrograms;
    int quantum_time;
    Queue ready;
    Queue block;
    Program programs[];
};
SO OperatingSystem;

void change(Program P){
    Boolean current_running;
    Program aux;
    switch (P->state)
    {
    case READY:
        Dequeue(OperatingSystem->ready);
        P->state = RUN;
        ++P->now;
        break;
    case RUN:
        if(P->cycle[P->now+1]==0){
            P->state = EXIT;
        }
        else{
          Enqueue(P,OperatingSystem->block);
          P->state = BLOCKED;
          ++P->now;
        }
        if(!isEmpty(OperatingSystem->ready)){
            aux = peek(OperatingSystem->ready);
            Dequeue(OperatingSystem->ready);
            P->state = RUN;
            ++P->now;
        }
        break;
    case BLOCKED:
        if(!isEmpty(OperatingSystem->block)){
            aux = peek(OperatingSystem->block);
            Dequeue(OperatingSystem->block);
            aux->state = READY;
            Enqueue(aux, OperatingSystem->ready);
        }
        break;
    case NEW:
        current_running = false;
        for(int i = 0; i < OperatingSystem->numOfPrograms ;++i){
            if(OperatingSystem->programs[i]->state == RUN ){
                current_running = true;
            }
        }
        if(current_running){
            P->state = RUN;
            ++P->now;
        }
        else{
            P->state = READY;
            Enqueue(P,OperatingSystem->ready);
        }
        break;
    case NOTCREATE:
        P->state = NEW;
        break;
    default:
        break;
    }
}

void change_quantum(Program P){
    Program aux;
    if(!isEmpty(OperatingSystem->ready)){
        aux = peek(OperatingSystem->ready);
        Dequeue(OperatingSystem->ready);
        aux->state = RUN;
        ++aux->now;
        Enqueue(P,OperatingSystem->block);
        P->state = BLOCKED;
        --P->now;
    }
}
void printProgramState(enum States state){
    switch(state){
         case NEW:
            printf(" NEW |");
            break;
        case READY:
            printf("READY|");
            break;
        case RUN:
            printf(" RUN |");
            break;
        case BLOCKED:
            printf("BLOCK|");
            break;
        case EXIT:
            printf(" EXIT|");
            break;
        case FINISH:
            printf("-----|");
            break;
        case NOTCREATE:
            printf("     |");
            break;
        default:
            break;
    }
}
void run (){
    for(int i = 0; i < OperatingSystem->numOfPrograms; ++i){
        if(OperatingSystem->programs[i]->timeToStatrt == 0 ){
            OperatingSystem->programs[i]->state = NEW;
        }
    }
    OperatingSystem->instance = 1;
    int quantum_program = OperatingSystem->quantum_time;
    while(OperatingSystem->instance != 0){
        Boolean program_running = false;
        int numOfProgramsExecuting = 0;
        Program programRunning;
        Program P;
        printf("%2d",OperatingSystem->instance);

        for( int i = 0; i<OperatingSystem->numOfPrograms ;++i){
            P = OperatingSystem->programs[i];
            if(P->state == RUN ||P->state == BLOCKED )      //pode faltar uma condição
                --P->cycle[P->now];

            printProgramState(P->state);
            if(P->state == RUN){
                program_running = true;
                --quantum_program;
                programRunning = P;
            }
            if(P->cycle[P->now] == 0 && P->state == 2){
                if(isEmpty(OperatingSystem->ready))
                    program_running = false;
                change(P);
                quantum_program =OperatingSystem->quantum_time;
            }
            if(P->cycle[P->now] == 0 && P->state == BLOCKED)
                change(P);
        }
        printf("/n");
        
    }

}