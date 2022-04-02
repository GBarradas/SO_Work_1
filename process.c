#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lib.h"

typedef bool Boolean;
typedef struct node *Node;
typedef struct queue Queue;
typedef struct program Program;
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

struct program
{
    int id;
    int now;
    enum States state;
    int timeToStart;
    int cycle[10];
};

struct so
{
    int instance;
    int numOfPrograms;
    int quantum_time;
    int ready[10];
    int ready_front;
    int ready_rear;
    int ready_size;
    Program programs[10];
};

struct node
{
    int porogram_id;
    Node *next;
};

/*struct queue
{
    int size;
    int Head;
    int Rear;
    Program elements[10];
};*/
SO OperatingSystem;

void inicializeReady(){
    OperatingSystem.ready_front = 0;
    OperatingSystem.ready_rear = 0;
    OperatingSystem.ready_size = 0;
}


Boolean readyIsEmpty()
{
    return OperatingSystem.ready_size == 0;
}

void EnqueueReady(Program P) 
{
    OperatingSystem.ready[OperatingSystem.ready_rear] = P.id;
    ++OperatingSystem.ready_rear;
    ++OperatingSystem.ready_size;
}

Program peekReady()
{
    if (!readyIsEmpty())
    {
        return OperatingSystem.programs[OperatingSystem.ready[0]];
    }
}

Program DequeueReady()
{
    if (!readyIsEmpty())
    {
        Program aux = peekReady();
        for (int i = 0; i < OperatingSystem.ready_rear - 1; ++i)
        {
            OperatingSystem.ready[i] = OperatingSystem.ready[i + 1];
        }
        return aux;
    }
}


void change(Program P)
{
    

        Boolean current_running;

        Program aux;
        switch (P.state)
        {
        case READY:
            DequeueReady();
            P.state = RUN;
            ++P.now;
            break;
        case RUN:
            if (P.cycle[P.now + 1] == 0)
            {
                P.state = FINISH;
            }
            else
            {
                // Enqueue(P, block);
                P.state = BLOCKED;
                P.now++;
            }
            if (!readyIsEmpty())
            {
                aux = DequeueReady();
                aux.state = RUN;
                ++aux.now;
            }
            break;
        case BLOCKED:
           
                P.state = READY;
                EnqueueReady(P);
            
            break;
        case NEW:
            current_running = false;
            for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
            {
                if (OperatingSystem.programs[i].state == RUN)
                {
                    current_running = true;
                }
            }
            if (!current_running)
            {
                P.state = RUN;
                ++P.now;
            }
            else
            {
                P.state = READY;
                EnqueueReady(P);
            }
            break;
        case NONCREATE:
            P.state = NEW;
            break;
        default:
            break;
        }
    
}

void change_quantum(Program P)
{
    // printf("quatum change-%d\n",P.id);
    Program aux;
    if (!readyIsEmpty())
    {
        aux = DequeueReady();
        aux.state = RUN;
        ++aux.now;
        EnqueueReady(P);
        P.state = READY;
        --P.now;
    }
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

void run()
{
    printf("2ning\n");
    for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
    {
        Program P = OperatingSystem.programs[i];
        if (P.timeToStart == 0)
        {
            P.state = NEW;
        }
    }
    SO copy = OperatingSystem;
    OperatingSystem.instance = 1;
    int quantum_program = OperatingSystem.quantum_time;
    while (OperatingSystem.instance != 0)
    {
        copy = OperatingSystem;
        Boolean program_running = false;
        int numOfProgramsExecuting = 0;
        Program programRunning;
        Program P;
        printf("%2d|", OperatingSystem.instance);

        for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
        {
            P = OperatingSystem.programs[i];

            //printf("%d %d", P.now, P.id);
            //(programs[i].state == 2) || (front(1) == i && programs[i].state == 3)
            if (P.state == RUN ||  P.state == BLOCKED) // pode faltar uma condição
                --P.cycle[P.now];
            
            printProgramState(P.state);
            if (P.state == RUN)
            {
                program_running = true;
                --quantum_program;
                programRunning = P;
            }
        }
        printf("\n");
        for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
        {
            P = OperatingSystem.programs[i];
            if (P.cycle[P.now] == 0 && P.state == RUN)
            {
                if (readyIsEmpty())
                    program_running = false;
                change(P);
                quantum_program = OperatingSystem.quantum_time;
            }
            if (P.cycle[P.now] == 0 && P.state == BLOCKED)
                change(P);
            if (P.state != FINISH)
                ++numOfProgramsExecuting;
        }

        if (!program_running && !readyIsEmpty())
            change(peekReady());
        if (quantum_program == 0)
        {
            change_quantum(programRunning);
            quantum_program = OperatingSystem.quantum_time;
        }

        for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
        {
            P = OperatingSystem.programs[i];
            if (P.state == NEW)
                change(P);
            if (P.timeToStart == OperatingSystem.instance && P.state == NONCREATE)
                change(P);
        }
        OperatingSystem.instance++;
        if (numOfProgramsExecuting == 0)
        {
            printf("   %2d|", OperatingSystem.instance);
            for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
            {
                printf("-----|");
            }
            OperatingSystem.instance = 0;
        }
    }
}

void main()
{
    printf("hello");
    int programas[3][10] = {
        {0, 3, 1, 2, 2, 4, 0, 0, 0, 0},
        {1, 4, 2, 4, 1, 1, 0, 0, 0, 0},
        {3, 2, 1, 6, 1, 3, 1, 1, 0, 0}};
    int rows = 3;
    int cols = 10;

    OperatingSystem.numOfPrograms = rows;
    OperatingSystem.quantum_time = 3;
    inicializeReady();
    
    for (int i = 0; i < rows; ++i)
    {
        Program P ;
        P.id = i; // para remover
        P.now = 0;
        P.state = NONCREATE;
        for (int j = 0; j < 10; ++j)
        {
            //P.cycle[j] = malloc(sizeof(int));
            P.cycle[j] = (int)programas[i][j];
        }
        P.timeToStart = P.cycle[0];
        OperatingSystem.programs[i] = P;
    }
    printf("%d %d %d\n", OperatingSystem.instance, OperatingSystem.numOfPrograms, OperatingSystem.quantum_time);
    for (int i = 0; i < OperatingSystem.numOfPrograms; ++i)
    {
        Program P = OperatingSystem.programs[i];
        printf("%d %d-%d - ", P.id, P.timeToStart, P.now);
        for (int j = 0; j < 10; ++j)
        {
            printf("%d ", P.cycle[j]);
        }
        printf("\n");
    }
    SO copy = OperatingSystem;
    printf("2\n");
    run();
}