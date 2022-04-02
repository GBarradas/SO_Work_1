#include <stdio.h>
#include <stdbool.h>
#define QUEUE_SIZE 10000
typedef bool Boolean;
typedef struct so SO;
typedef struct queues Queue;
typedef struct program Program;

enum Queues{ready, block};
enum States
{
    EXIT,
    READY,
    RUN,
    BLOCKED,
    NONCREATE,
    NEW,
    FINISH,

};

 struct program{
    int now;                                                                                                          
    int state;
    int start;                                                                                                          
    int cycle[11];
}; 

struct queues{
    int ready_queue[QUEUE_SIZE];
    int ready_front;
    int ready_tail;                                                               
    int block_queue[QUEUE_SIZE];
    int block_front;
    int block_tail;                                                               

};

struct so{
    int instant;
    int number_of_programs;
    int quantum_time;
    Program programs[11];
    Queue queues;
};

SO OS;

int front(int queue_number) {                                                                                          
    if (queue_number == 0) {                                                                                          
        if (OS.queues.ready_front != -1)
            return OS.queues.ready_queue[OS.queues.ready_front];
    } else if (queue_number == 1) {                                                                                    
        if (OS.queues.block_front != -1)
            return OS.queues.block_queue[OS.queues.block_front];
    } 
    return -1;
}

void enqueue(int id, int queue_number) {                                                                            
    if (queue_number == 0) {                                                                                           
        if (OS.queues.ready_tail == QUEUE_SIZE - 1)
            return;

        if (OS.queues.ready_front == -1)
            OS.queues.ready_front = 0;

        ++OS.queues.ready_tail;
        OS.queues.ready_queue[OS.queues.ready_tail] = id;
    } else if (queue_number == 1) {                                                                                    
        if (OS.queues.block_tail == QUEUE_SIZE - 1)
            return;

        if (OS.queues.block_front == -1)
            OS.queues.block_front = 0;
        ++OS.queues.block_tail;
        OS.queues.block_queue[OS.queues.block_tail] = id;
    }
}

int dequeue(int queue_number) {                                                                                        
    int aux;
    if (queue_number == 0) {                                                                                           
        if (OS.queues.ready_front == -1)
            return -1;

        aux = OS.queues.ready_queue[OS.queues.ready_front];
        ++OS.queues.ready_front;

        if (OS.queues.ready_front > OS.queues.ready_tail) {
            OS.queues.ready_front = -1;
            OS.queues.ready_tail = -1;
        }

        return aux;
    } else if (queue_number == 1) {
        if (OS.queues.block_front == -1)
            return -1;

        aux = OS.queues.block_queue[OS.queues.block_front];
        ++OS.queues.block_front;

        if (OS.queues.block_front > OS.queues.block_tail) {
            OS.queues.block_front = -1;
            OS.queues.block_tail = -1;
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

void changeProgram(int id) {
    Boolean currently_running = false;
    int aux;
    switch (getState(id)) {
        case 0:                      
            setState(id, 6);   

            break;
        case 1:                                                                                                        
            dequeue(0);
            setState(id, 2);                                                                             
            ++OS.programs[id].now;

            break;
        case 2:                                                                                                       
                if (OS.programs[id].cycle[OS.programs[id].now + 1] == 0)
                    setState(id, 0);
                else {
                    enqueue(id, 1);
                    setState(id, 3);
                    ++OS.programs[id].now;
                }
                if ((aux = front(0)) != -1) {
                    dequeue(0);
                    setState(aux, 2);                                                                            
                    ++OS.programs[aux].now;
                }
            
            break;

        case 3:                                                                                   
            aux = dequeue(1);
            if (aux != -1) {
                setState(aux, 1);                                                                            
                enqueue(aux, 0);
            }
            

            break;
        case 4:                                                                                        
            setState(id, 5);                                                                      

            break;
        case 5:                                                                                                       
            currently_running = 0;
            for (int i = 0; i < OS.number_of_programs; ++i) {
                if (getState(i) == 2)
                    currently_running=true;
            }
            if(!currently_running){
                setState(id, 2);
                ++OS.programs[id].now;
            } else{
                setState(id, 1);                                  
                enqueue(id, 0);
            }

            break;
        default:
            break;
    }
}

void quantumChangeProgram(int id) {                                                                   
    int ready = front(0);                                             
    if (ready != -1) {
        dequeue(0);
        setState(ready, 2);                   
        ++OS.programs[ready].now;
        enqueue(id, 0);
        setState(id, 1);                          
        OS.programs[id].now--;
    }
   
}

void printProgramState(int state)
{
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
    for (int i = 0; i < OS.number_of_programs; ++i)
        if (OS.programs[i].start == 0)
            setState(i, NEW);

    OS.instant = 1;
    int quantum_unity = OS.quantum_time;

    while (OS.instant != 0) {
        Boolean is_running = false;
        int numOfExecutinfPrograms = 0;
        int running_program;

        printf("|    %2d |", OS.instant);

        for (int i = 0; i < OS.number_of_programs; ++i) {

            if ((getState(i) == 2) || (front(1) == i && getState(i) == 3))      
                OS.programs[i].cycle[OS.programs[i].now]--;

            printProgramState(getState(i));
            if(getState(i) == 2){
            is_running = true;
            quantum_unity--;
            running_program = i;
            }

        }
        printf("\n");

        for (int i = 0; i < OS.number_of_programs; ++i) {

            if (getState(i) == 0)                                                                                
                changeProgram(i);

            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && getState(i) == 2) {                                    
                if (front(0) == -1)                                              
                    is_running = true;
                changeProgram(i);
                quantum_unity = OS.quantum_time;
            }

            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && getState(i) == 3)         
                changeProgram(i);

            if (getState(i) != 6)                                                     
                ++numOfExecutinfPrograms;
        }
        if (!is_running && front(0) != -1)                                                                     
            changeProgram(front(0));

      

        if (quantum_unity == 0) {                                                                                  
            quantumChangeProgram(running_program);
            quantum_unity = OS.quantum_time;
        }

        for (int i = 0; i < OS.number_of_programs; ++i) {
            if (getState(i) == 5)                          
                changeProgram(i);

            if (OS.programs[i].start == OS.instant && getState(i) == 4)                                        
                changeProgram(i);
        }

        ++OS.instant;
        if (numOfExecutinfPrograms == 0) {                                                             
            printf("|    %2d |", OS.instant);

            for (int i = 0; i < OS.number_of_programs; ++i)
                printf(" ----- |");

            OS.instant = 0;
        }

    }

    printf("\n");
}



int main() {
    int programas[3][10] = {
 {0, 3, 1, 2, 2, 4, 0, 0, 0, 0 } ,
 {1, 4, 2, 4, 1, 1, 0, 0, 0, 0 } ,
 {3, 2, 1, 6, 1, 3, 1, 1, 0, 0 } };
    int number_of_rows = sizeof(programas) / sizeof(programas[0]);
    int number_of_columns = sizeof(programas[0]) / sizeof(programas[0][0]);

    OS.quantum_time = 3;
    OS.number_of_programs = number_of_rows;
    OS.queues.block_front = -1;
    OS.queues.block_tail = -1;
    OS.queues.ready_front = -1;
    OS.queues.ready_tail = -1;
    printf("|Instate|");
//         " -P0%- |"
    for (int i = 0; i < number_of_rows; ++i) {
        OS.programs[i].now = 0;
        setState(i, 4);

        for (int j = 0; j < number_of_columns; ++j)
            OS.programs[i].cycle[j] = programas[i][j];

        OS.programs[i].start = OS.programs[i].cycle[0];
    }
    for( int i = 0; i < number_of_rows; ++i ){
        if(i<9)
            printf(" -P0%d- |",i+1);
        else
           printf(" -P%d- |",i+1);
    }
    printf("\n|:-----:|");
    for( int i = 0; i < number_of_rows; ++i ){
        printf(":-----:|");
    }

    OS.instant = 0;

    printf("\n");

    run();

    return 0;
}