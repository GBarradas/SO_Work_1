#include <stdio.h>
#include <stdbool.h>
#include "main.h"

SO OS;

Boolean isEmpty(enum Queues Q){
    if(Q == ready)
        return OS.queues.readyFront == -1;
    else if(Q == block)
        return OS.queues.blockFront == -1;
}

int peek(enum Queues Q) {                                                                                          
    if (Q == ready) {                                                                                          
        if (!isEmpty(ready))
            return OS.queues.readyPrograms[OS.queues.readyFront];
    } else if (Q == block) {                                                                                    
        if (!isEmpty(block))
            return OS.queues.blockPrograms[OS.queues.blockFront];
    } 
    return -1;
}

void enqueue(int id, enum Queues Q) {                                                                            
    if (Q == ready) {                                                                                           
        if (OS.queues.readyRear == QUEUE_SIZE - 1)
            return;

        if (OS.queues.readyFront == -1)
            OS.queues.readyFront = 0;

        ++OS.queues.readyRear;
        OS.queues.readyPrograms[OS.queues.readyRear] = id;
    } else if (Q == block) {                                                                                    
        if (OS.queues.blockRear == QUEUE_SIZE - 1)
            return;

        if (OS.queues.blockFront == -1)
            OS.queues.blockFront = 0;
        ++OS.queues.blockRear;
        OS.queues.blockPrograms[OS.queues.blockRear] = id;
    }
}

int dequeue(enum Queues Q) {                                                                                        
    int aux;
    if (Q == ready) {                                                                                           
        if (isEmpty(ready))
            return -1;

        aux = OS.queues.readyPrograms[OS.queues.readyFront];
        ++OS.queues.readyFront;

        if (OS.queues.readyFront > OS.queues.readyRear) {
            OS.queues.readyFront = -1;
            OS.queues.readyRear = -1;
        }

        return aux;
    } else if (Q == block) {
        if (isEmpty(block))
            return -1;

        aux = OS.queues.blockPrograms[OS.queues.blockFront];
        ++OS.queues.blockFront;

        if (OS.queues.blockFront > OS.queues.blockRear) {
            OS.queues.blockFront = -1;
            OS.queues.blockRear = -1;
        }

        return aux;
    }
    return -1;
}

enum States getState(int id){
    return OS.programs[id].state;
}

void setState(int id, enum States state){
    OS.programs[id].state = state;
}

int getCycle(int id, int Position){
    return OS.programs[id].cycle[Position];
}

int getNow(int id){
    return OS.programs[id].now;
}

void changeProgram(int id) {
    Boolean programRunning = false;
    int aux;
    switch (getState(id)) {
        case EXIT:                      
            setState(id, FINISH);   

            break;
        case READY:                                                                                                        
            dequeue(0);
            setState(id, RUN);                                                                             
            ++OS.programs[id].now;

            break;
        case RUN:                                                                                                       
            if (getCycle(id,getNow(id)+1) == 0)
                setState(id, EXIT);
            else {
                enqueue(id, block);
                setState(id, BLOCKED);
                ++OS.programs[id].now;
            }
            if (!isEmpty(ready)) {
                aux = dequeue(ready);
                setState(aux, RUN);                                                                            
                ++OS.programs[aux].now;
            }
            break;

        case BLOCKED:                                                                                   
            if (!isEmpty(block)) {
                aux = dequeue(block);
                setState(aux, READY);                                                                            
                enqueue(aux, ready);
            }
            

            break;
        case NONCREATE:                                                                                        
            setState(id, NEW);                                                                      

            break;
        case NEW:                                                                                                       
            programRunning = 0;
            for (int i = 0; i < OS.numOfPrograms; ++i) {
                if (getState(i) == RUN)
                    programRunning=true;
            }
            if(!programRunning){
                setState(id, RUN);
                ++OS.programs[id].now;
            } else{
                setState(id, READY);                                  
                enqueue(id, ready);
            }

            break;
        default:
            break;
    }
}

void quantumChangeProgram(int id) {                                                                   
    if (!isEmpty(ready)) {
        int aux = dequeue(ready);
        setState(aux, RUN);                   
        ++OS.programs[aux].now;
        enqueue(id, ready );
        setState(id, READY);                          
        --OS.programs[id].now;
    }
   
}

void printProgramState(enum States state) {
    switch (state)
    {
    case NEW:
        printf("  NEW  |");
        break;
    case READY:
        printf(" READY |");
        break;
    case RUN:
        printf("  RUN  |");
        break;
    case BLOCKED:
        printf(" BLOCK |");
        break;
    case EXIT:
        printf("  EXIT |");
        break;
    case FINISH:
        printf(" ----- |");
        break;
    case NONCREATE:
        printf("       |");
        break;
    default:
        break;
    }
}

void run() {                                                                             
    for (int i = 0; i < OS.numOfPrograms; ++i)
        if (getCycle(i,0) == 0)
            setState(i, NEW);

    OS.instante = 1;
    int quantumProgram = OS.quantumTime;

    while (true) {
        Boolean isProgramRunning = false;
        int numOfExecutinfPrograms = 0;
        int runningProgram;

        printf("|    %2d |", OS.instante);

        for (int i = 0; i < OS.numOfPrograms; ++i) {
            if(getState(i) == NONCREATE && getNow(i) ==0 ){
                --OS.programs[i].cycle[0];
            }
            if ((getState(i) == RUN) || (peek(block) == i && getState(i) == BLOCKED))      
                --OS.programs[i].cycle[getNow(i)];

            printProgramState(getState(i));
            if(getState(i) == RUN){
            isProgramRunning = true;
            --quantumProgram;
            runningProgram = i;
            }

        }
        printf("\n");

        for (int i = 0; i < OS.numOfPrograms; ++i) {

            if (getState(i) == EXIT)                                                                                
                changeProgram(i);

            if (getCycle(i,getNow(i)) == 0 && getState(i) == RUN) {                                    
                if (isEmpty(ready))                                              
                    isProgramRunning = true;
                changeProgram(i);
                quantumProgram = OS.quantumTime;
            }

            if (getCycle(i,getNow(i)) == 0 && getState(i) == BLOCKED)         
                changeProgram(i);

            if (getState(i) != FINISH)                                                     
                ++numOfExecutinfPrograms;
        }
        if (!isProgramRunning && !isEmpty(ready))                                                                     
            changeProgram(peek(0));

      

        if (quantumProgram == 0) {                                                                                  
            quantumChangeProgram(runningProgram);
            quantumProgram = OS.quantumTime;
        }

        for (int i = 0; i < OS.numOfPrograms; ++i) {
            if (getState(i) == 5)                          
                changeProgram(i);

            if (getCycle(i,0) == 0 && getState(i) == NONCREATE)                                        
                changeProgram(i);
        }

        if (numOfExecutinfPrograms == 0) {                                                             
            printf("|    %2d |", ++OS.instante);

            for (int i = 0; i < OS.numOfPrograms; ++i)
                printf(" ----- |");

            break;
        }
        ++OS.instante;

    }

    printf("\n");
}

int main() {
    int programas[3][10] = {
        {0, 3, 1, 2, 2, 4, 0, 0, 0, 0 } ,
        {1, 4, 2, 4, 1, 1, 0, 0, 0, 0 } ,
        {3, 2, 1, 6, 1, 3, 1, 1, 0, 0 } 
    };
    int rows = sizeof(programas) / sizeof(programas[0]);
    int cols = sizeof(programas[0]) / sizeof(programas[0][0]);

    OS.quantumTime = 3;
    OS.numOfPrograms = rows;
    OS.queues.blockFront = -1;
    OS.queues.blockRear = -1;
    OS.queues.readyFront = -1;
    OS.queues.readyRear = -1;
    printf("|Instate|");
//         " -P0%- |"
    for (int i = 0; i < rows; ++i) {
        OS.programs[i].now = 0;
        setState(i, 4);

        for (int j = 0; j < cols; ++j)
            OS.programs[i].cycle[j] = programas[i][j];

        OS.programs[i].start = OS.programs[i].cycle[0];
    }
    for( int i = 0; i < rows; ++i ){
        if(i<9)
            printf(" -P0%d- |",i+1);
        else
           printf(" -P%d- |",i+1);
    }
    printf("\n|:-----:|");
    for( int i = 0; i < rows; ++i ){
        printf(":-----:|");
    }

    OS.instante = 0;

    printf("\n");

    run();
    printf("  \n---  \n# Execution Finish!!  \n");
}