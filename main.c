#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef bool Boolean;
typedef struct queues Queue;
typedef struct program Program;
typedef struct so SO;

enum Queues{ready, block};
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

struct queues{
    int ready_programs[10];
    int ready_rear;
    int ready_size;

    int block_programs[10];
    int block_rear;
    int block_size;
};

struct program{
    int id;
    int now;
    enum States state;
    int cycle[10];

};
struct so{
    int instance;
    int numOfPrograms;
    int quantum_time;
    Queue queues;
    Program programs[10];
};

SO Op_sys;
Queue inicializeQueue(){
    Queue a;
    a.block_rear = 0;
    a.block_size = 0;
    a.ready_rear = 0;
    a.ready_size = 0;
}
Boolean isEmpty(enum Queues Q){
    switch (Q)
    {
        case ready:
            return Op_sys.queues.ready_size == 0;
            break;
        case block:
            return Op_sys.queues.block_size == 0;
            break;
        default:
            break;
    }
}

int peek(enum Queues Q){
    switch (Q)
    {
        case ready:
            return Op_sys.queues.ready_programs[0];
            break;
        case block:
            return Op_sys.queues.block_programs[0];
            break;
        default:
            break;
    }
}

int Dequeue(enum Queues Q){
     int result;
    switch (Q)
    {
        case ready:
            result = peek(ready);
            for(int i = 0; i <Op_sys.queues.ready_rear - 1; ++i){
                Op_sys.queues.ready_programs[i] = Op_sys.queues.ready_programs[i + 1];
            }
            --Op_sys.queues.ready_rear;
        case block:
            result = peek(block);
            for(int i = 0; i <Op_sys.queues.block_rear - 1; ++i){
                Op_sys.queues.block_programs[i] = Op_sys.queues.block_programs[i + 1];
            }
            --Op_sys.queues.block_rear;
            break;
        default:
            break;
    }
    return result;
}

void Enqueue(int id, enum Queues Q){
    switch (Q)
    {
        case ready:
            Op_sys.queues.ready_programs[Op_sys.queues.ready_rear] = id;
            ++Op_sys.queues.ready_rear;
            ++Op_sys.queues.ready_size;
        case block:
            Op_sys.queues.block_programs[Op_sys.queues.block_rear] = id;
            ++Op_sys.queues.block_rear;
            ++Op_sys.queues.block_size;
        default:
            break;
    }
}

enum States getState (int id){
    return Op_sys.programs[id].state;
}

void setState(int id, enum States toSet){
    Op_sys.programs[id].state = toSet;
}

void printProgramState(enum States state)
{
    switch (state)
    {
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
    case NONCREATE:
        printf("     |");
        break;
    default:
        break;
    }
}

void change(int id){
    Boolean current_running;
    int aux;
    int now = Op_sys.programs[id].now;
    switch (getState(id))
    {
        case NEW:
            current_running = false;
            for (int id = 0; id < Op_sys.numOfPrograms; ++id){
                if(getState(id) == RUN)
                    current_running = true;
            }
            if(current_running){
                setState(id,READY);
                Enqueue(id,ready);
            }
            else
            {
                setState(id,RUN);
                ++Op_sys.programs[id].now;
            }
            break;
        case READY:
            Dequeue(ready);
            setState(id,RUN);
            ++Op_sys.programs[id].now;
            break;
        case RUN:
            if(Op_sys.programs[id].cycle[now + 1] == 0){
                setState(id,FINISH);
            }
            else{
                Enqueue(id, block);
                setState(id, BLOCKED);
                ++Op_sys.programs[id].now;
            }
            if(!isEmpty(ready)){
                aux = Dequeue(ready);
                setState(aux, RUN);
                ++Op_sys.programs[aux].now;
            }
            break;
        case BLOCKED:
            setState(id, READY);
            Enqueue(id, ready);
            break;
        case EXIT:
            setState(id, FINISH);
            break;
        case NONCREATE:
            setState(id, NEW);
            break;

        default:
            break;
    }

}
void change_quantum(int id)
{
    if(!isEmpty(ready)){
        int aux = Dequeue(ready);
        setState(aux, RUN);
        ++Op_sys.programs[aux].now;
        Enqueue(id, ready);
        setState(id , READY);
        --Op_sys.programs[id].now;


    }
}


void run() {                                                                                // simulates the running of the 5 state model and prints every instant's information
    for (int id = 0; id < Op_sys.numOfPrograms; ++id)
        if (Op_sys.programs[id].cycle[0] == 0)
            setState(id, NEW);

    Op_sys.instance = 1;
    int quantum_program = Op_sys.quantum_time;
    int now;
    SO copy = Op_sys;
    while (Op_sys.instance != 0) {
        Boolean programRunning = false;
        int numOfProgramsExecuting = 0;
        int program_Running;
        printf(" %4d |", Op_sys.instance);
        copy = Op_sys;
        for (int id = 0; id < Op_sys.numOfPrograms; ++id) {
            if(getState(id) == RUN ||
            getState(id) == NONCREATE ||
            (getState(id) == BLOCKED && id == peek(block))){
                now = Op_sys.programs[id].now;
                --Op_sys.programs[id].cycle[now];
            }
            printf("%2d| %2d| ",now, Op_sys.programs[id].cycle[now] );
            printProgramState(getState(id));
            if(getState(id) == RUN){
                programRunning = true;
                --quantum_program;
                program_Running = id;
            }

    
        }
        copy = Op_sys;

        for (int id = 0; id < Op_sys.numOfPrograms; ++id) {
            now = Op_sys.programs[id].now;
            if (getState(id) == EXIT)                                                                                 // If program is exiting --> terminate the program
                change(id);


            if (Op_sys.programs[id].cycle[now] <= 0 && getState(id) == RUN) {                                    // If program cycle is 0 and program state is running --> change the state of the program                                                                                   // Round Robin Standard (RR)
                    if (isEmpty(ready))                                                                                 // If ready queue is empty
                        programRunning = false;
            }

            if (Op_sys.programs[id].cycle[now] <= 0 && getState(id) == BLOCKED)                                      // If program cycle is 0 and program state is blocked --> change the state of the program
                change(id);

            if (getState(id) != FINISH)                                                                                 // If program isn't finished --> increment numOfProgramsExecuting variable
                numOfProgramsExecuting++;
        }

        printf("%d %d %d|\n",numOfProgramsExecuting,programRunning, quantum_program);

                                                                                                   // Round Robin Standard (RR)
        if (!programRunning && !isEmpty(ready))                                                                      // If no programs are running and ready queue isn't empty --> change the state of the queue program
            change(peek(ready));

        if (quantum_program == 0) {                                                                                       // If the quantum cycle ends --> change running program and reset quantum cycle
            change_quantum(program_Running);
            quantum_program = Op_sys.quantum_time;
        }

        for (int id = 0; id < Op_sys.numOfPrograms; id++) {
            if (getState(id) == NEW)                                                                                 // If program is new --> put the program in ready queue
                change(id);

            if (Op_sys.programs[id].cycle[0] <= 0 && getState(id) == NONCREATE)                                                 // If program isn't created --> create the program
                change(id);
        }

        ++Op_sys.instance;
        if (numOfProgramsExecuting == 0) {                                                                        // If there are no programs executing
            printf("%d|", Op_sys.instance);

            for (int i = 0; i < Op_sys.numOfPrograms; i++)
                printf("  ---  |");

            Op_sys.instance = 0;
        }

    }

    printf("\nALL PROGRAMS HAVE TERMINATED.");
}


void main(){
    int programas[3][10] = {
        {0, 3, 1, 2, 2, 4, 0, 0, 0, 0},
        {1, 4, 2, 4, 1, 1, 0, 0, 0, 0},
        {3, 2, 1, 6, 1, 3, 1, 1, 0, 0}};
    int rows = 3;
    int cols = 10;
    Op_sys.numOfPrograms = rows;
    Op_sys.quantum_time = 3;
    //Op_sys.queues = inicializeQueue();
    for(int i = 0; i < Op_sys.numOfPrograms; ++i){
        Op_sys.programs[i].id = i;
        Op_sys.programs[i].now = 0;
        Op_sys.programs[i].state = NONCREATE;
        for(int j = 0; j < 10 ;++j){
            Op_sys.programs[i].cycle[j] = programas[i][j];
        }
    }
    SO copy = Op_sys;
    printf("   instante|p1|p1-cycle|p1-sate|p2|p2-cycle|p2-sate|p3|p3-cycle|p3-sate|number of execut|\n|-|-|-|-|-|-|-|-|-|-|-|\n");
    run();
}
