#include <stdio.h>
#include <stdbool.h>
#define QUEUE_SIZE 10000
typedef bool Boolean;
typedef struct so SO;

enum States
{
    NEW,
    READY,
    RUN,
    BLOCKED,
    EXIT,
    NONCREATE,
    FINISH,

};

typedef struct {
    int now;                                                                                                           // index of the program sub-cycle that is currently in
    int state;
    int start;                                                                                                          // starting instant of time
    int cycle[11];
} program; 

struct so{
    int instant;
    int number_of_programs;
    int quantum_time;
    program programs[11];

    int ready_queue[QUEUE_SIZE];
    int ready_front;
    int ready_tail;                                                               
    int block_queue[QUEUE_SIZE];
    int block_front;
    int block_tail;                                                               
};

SO OS;

int front(int queue_number) {                                                                                           // returns the element on the first position of the queue
    if (queue_number == 0) {                                                                                            // ready queue
        if (OS.ready_front != -1)
            return OS.ready_queue[OS.ready_front];
    } else if (queue_number == 1) {                                                                                     // block queue
        if (OS.block_front != -1)
            return OS.block_queue[OS.block_front];
    } 
    return -1;
}

int getState(int id){
    return OS.programs[id].state;
}

void setState(int id, int state){
    OS.programs[id].state = state;
}

void enqueue(int id, int queue_number) {                                                                             // places the element passed as argument on the last position of the queue
    if (queue_number == 0) {                                                                                            // ready queue
        if (OS.ready_tail == QUEUE_SIZE - 1)
            return;

        if (OS.ready_front == -1)
            OS.ready_front = 0;

        ++OS.ready_tail;
        OS.ready_queue[OS.ready_tail] = id;
    } else if (queue_number == 1) {                                                                                     // block queue
        if (OS.block_tail == QUEUE_SIZE - 1)
            return;

        if (OS.block_front == -1)
            OS.block_front = 0;
        ++OS.block_tail;
        OS.block_queue[OS.block_tail] = id;
    }
}

int dequeue(int queue_number) {                                                                                         // returns the element in the first position of the queue and removes it
    int aux;
    if (queue_number == 0) {                                                                                            // ready queue
        if (OS.ready_front == -1)
            return -1;

        aux = OS.ready_queue[OS.ready_front];
        ++OS.ready_front;

        if (OS.ready_front > OS.ready_tail) {
            OS.ready_front = -1;
            OS.ready_tail = -1;
        }

        return aux;
    } else if (queue_number == 1) {                                                                                     // block queue
        if (OS.block_front == -1)
            return -1;

        aux = OS.block_queue[OS.block_front];
        ++OS.block_front;

        if (OS.block_front > OS.block_tail) {
            OS.block_front = -1;
            OS.block_tail = -1;
        }

        return aux;
    }
    return -1;
}

void state_change(int id) {                                                                         // Makes a change from a state to another to the program passed as argument
    int currently_running;
    int aux;
    switch (getState(id)) {
        case 0:                                                                                                         // Exit
            setState(id, 6);                                                                             // EXT --> --- (finished program)

            break;
        case 1:                                                                                                         // Ready
            dequeue(0);
            setState(id, 2);                                                                             // RDY --> RUN
            ++OS.programs[id].now;

            break;
        case 2:                                                                                                         // Run                                                                                         // Round Robin Standard (RR)
                if (OS.programs[id].cycle[OS.programs[id].now + 1] == 0)
                    setState(id, 0);                                                                     // RUN --> EXT
                else {
                    enqueue(id, 1);
                    setState(id, 3);                                                                     // RUN --> BLK
                    ++OS.programs[id].now;
                }

                aux = front(0);

                if (aux != -1) {
                    dequeue(0);
                    setState(aux, 2);                                                                            // RDY --> RUN
                    ++OS.programs[aux].now;
                }
            
            break;

        case 3:                                                                                                         // Block                                                                                         // Round Robin Standard (RR)
            aux = dequeue(1);
            if (aux != -1) {
                setState(aux, 1);                                                                            // BLK --> RDY
                enqueue(aux, 0);
            }
            

            break;
        case 4:                                                                                                         // (not created program)
            setState(id, 5);                                                                             //     --> NEW

            break;
        case 5:                                                                                                          // New
            currently_running = 0;
            for (int i = 0; i < OS.number_of_programs; ++i) {
                if (getState(i) == 2)
                    currently_running=1;
            }
            if(currently_running==0){
                setState(id, 2);
                ++OS.programs[id].now;
            } else{
                setState(id, 1);                                                                         // NEW --> RDY
                enqueue(id, 0);
            }

            break;
        case 7:                                                                                                         // Aux
            if (front(2) != -1) {
                aux = dequeue(2);
                setState(aux, 2);                                                                                // AUX --> RUN
                ++OS.programs[aux].now;
            } else {
                dequeue(0);
                setState(id, 2);                                                                         // RDY --> RUN
                ++OS.programs[id].now;
            }

            break;
        default:
            break;
    }
}

void quantum_change(int id) {                                                                       // makes the change from RUN to RDY when a program has spent all of its quantum time in the programor
    int ready = front(0);                                                                                            // Round Robin Standard (RR)
    if (ready != -1) {
        dequeue(0);
        setState(ready, 2);                                                                                  // RDY --> RUN
        ++OS.programs[ready].now;
        enqueue(id, 0);
        setState(id, 1);                                                                             // RUN --> RDY
        OS.programs[id].now--;
    }
   
}

void printProgramState(int state)
{
    switch (state)
    {
    case 5:
        printf("  NEW  |");
        break;
    case 1:
        printf(" READY |");
        break;
    case 2:
        printf("  RUN  |");
        break;
    case 3:
        printf(" BLOCK |");
        break;
    case 0:
        printf("  EXIT |");
        break;
    case 6:
        printf(" ----- |");
        break;
    case 4:
        printf("       |");
        break;
    default:
        break;
    }
}

void run() {                                                                                // simulates the running of the 5 state model and prints every instant's information
    for (int i = 0; i < OS.number_of_programs; ++i)
        if (OS.programs[i].start == 0)
            setState(i, 5);

    OS.instant = 1;
    int quantum_unity = OS.quantum_time;

    while (OS.instant != 0) {
        Boolean is_running = false;
        int number_of_executing_programs = 0;
        int running_program;

        printf(" %4d |", OS.instant);

        for (int i = 0; i < OS.number_of_programs; ++i) {

            if ((getState(i) == 2) || (front(1) == i && getState(i) == 3))                                  // If program is running or program is blocked --> decrement program block cycle
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

            if (getState(i) == 0)                                                                                 // If program is exiting --> terminate the program
                state_change(i);


            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && getState(i) == 2) {                                    // If program cycle is 0 and program state is running --> change the state of the program                                                                                      // Round Robin Standard (RR)
                if (front(0) == -1)                                                                                 // If ready queue is empty
                    is_running = true;
                state_change(i);
                quantum_unity = OS.quantum_time;
            }

            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && getState(i) == 3)                                      // If program cycle is 0 and program state is blocked --> change the state of the program
                state_change(i);

            if (getState(i) != 6)                                                                                 // If program isn't finished --> increment number_of_executing_programs variable
                ++number_of_executing_programs;
        }


                                                                                             // Round Robin Standard (RR)
        if (!is_running && front(0) != -1)                                                                      // If no programs are running and ready queue isn't empty --> change the state of the queue program
            state_change(front(0));

      

        if (quantum_unity == 0) {                                                                                       // If the quantum cycle ends --> change running program and reset quantum cycle
            quantum_change(running_program);
            quantum_unity = OS.quantum_time;
        }

        for (int i = 0; i < OS.number_of_programs; ++i) {
            if (getState(i) == 5)                                                                                 // If program is new --> put the program in ready queue
                state_change(i);

            if (OS.programs[i].start == OS.instant && getState(i) == 4)                                                 // If program isn't created --> create the program
                state_change(i);
        }

        ++OS.instant;
        if (number_of_executing_programs == 0) {                                                                        // If there are no programs executing
            printf(" %4d |", OS.instant);

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
    OS.block_front = -1;
    OS.block_tail = -1;
    OS.ready_front = -1;
    OS.ready_tail = -1;
    printf("Instate|");

    for (int i = 0; i < number_of_rows; ++i) {
        OS.programs[i].now = 0;
        setState(i, 4);

        for (int j = 0; j < number_of_columns; ++j)
            OS.programs[i].cycle[j] = programas[i][j];

        OS.programs[i].start = OS.programs[i].cycle[0];
    }
    for( int i = 0; i < number_of_columns; ++i ){

    }

    OS.instant = 0;

    printf("\n");

    run();

    return 0;
}