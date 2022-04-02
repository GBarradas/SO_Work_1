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
    int start;
    enum States state;
    int cycle[11];

}; 

struct so{
    int instance;
    int numOfPrograms;
    int quantum_time;
    Queue queues;
    Program programs[10];
};

SO OS;

int peek(enum Queues Q){
    switch (Q)
    {
        case ready:
            return OS.queues.ready_programs[0];
            break;
        case block:
            return OS.queues.block_programs[0];
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
            for(int i = 0; i <OS.queues.ready_rear - 1; ++i){
                OS.queues.ready_programs[i] = OS.queues.ready_programs[i + 1];
            }
            --OS.queues.ready_rear;
            --OS.queues.ready_size;
            break;
        case block:
            result = peek(block);
            for(int i = 0; i <OS.queues.block_rear - 1; ++i){
                OS.queues.block_programs[i] = OS.queues.block_programs[i + 1];
            }
            --OS.queues.block_rear;
            --OS.queues.block_size;
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
            OS.queues.ready_programs[OS.queues.ready_rear] = id;
            ++OS.queues.ready_rear;
            ++OS.queues.ready_size;
            break;
        case block:
            OS.queues.block_programs[OS.queues.block_rear] = id;
            ++OS.queues.block_rear;
            ++OS.queues.block_size;
        default:
            break;
    }
}

void state_change(int program_id) {                                                                         // Makes a change from a state to another to the program passed as argument
    int currently_running;
    int aux;
    switch (OS.programs[program_id].state)
    {
    case 0:                                    // Exit
        OS.programs[program_id].state = 6; // EXT --> --- (finished program)

        break;
    case 1: // Ready
        Dequeue(0);
        OS.programs[program_id].state = 2; // RDY --> RUN
        OS.programs[program_id].now++;

        break;
    case 2: // Run
            // Round Robin Standard (RR)
        if (OS.programs[program_id].cycle[OS.programs[program_id].now + 1] == 0)
            OS.programs[program_id].state = 0; // RUN --> EXT
        else
        {
            Enqueue(program_id, 1);
            OS.programs[program_id].state = 3; // RUN --> BLK
            OS.programs[program_id].now++;
        }

        aux = peek(0);

        if (aux != -1)
        {
            Dequeue(0);
            OS.programs[aux].state = 2; // RDY --> RUN
            OS.programs[aux].now++;
        }

        break;

    case 3: // Block
            // Round Robin Standard (RR)
        aux = Dequeue(1);

        if (aux != -1)
        {
            OS.programs[aux].state = 1; // BLK --> RDY
            Enqueue(aux, 0);
        }

        break;
    case 4:                                    // (not created program)
        OS.programs[program_id].state = 5; //     --> NEW

        break;
    case 5: // New
        currently_running = 0;
        for (int i = 0; i < OS.numOfPrograms; i++)
        {
            if (OS.programs[i].state == 2)
                currently_running = 1;
        }
        if (currently_running == 0)
        {
            OS.programs[program_id].state = 2;
            OS.programs[program_id].now++;
        }
        else
        {
            OS.programs[program_id].state = 1; // NEW --> RDY
            Enqueue(program_id, 0);
        }

        break;
    case 7: // Aux
        if (peek(2) != -1)
        {
            aux = Dequeue(2);
            OS.programs[aux].state = 2; // AUX --> RUN
            OS.programs[aux].now++;
        }
        else
        {
            Dequeue(0);
            OS.programs[program_id].state = 2; // RDY --> RUN
            OS.programs[program_id].now++;
        }

        break;
    default:
        break;
    }
}

void quantum_change(int program_id) {                                                                       // makes the change from RUN to RDY when a program has spent all of its quantum time in the programor
    int ready = peek(0);

                                                                                                     // Round Robin Standard (RR)
    if (ready != -1) {
        Dequeue(0);
        OS.programs[ready].state = 2;                                                                                  // RDY --> RUN
        OS.programs[ready].now++;
        Enqueue(program_id, 0);
        OS.programs[program_id].state = 1;                                                                             // RUN --> RDY
        OS.programs[program_id].now--;
    }
    
}

void printProgramState(enum States state)
{
    switch (state)
    {
    case 5:
        printf(" NEW |");
        break;
    case 1:
        printf("READY|");
        break;
    case 2:
        printf(" RUN |");
        break;
    case 3:
        printf("BLOCK|");
        break;
    case 0:
        printf(" EXIT|");
        break;
    case 6:
        printf("-----|");
        break;
    case 4:
        printf("     |");
        break;
    default:
        break;
    }
}

void run(int quantum_time, int is_vrr) {                                                                                // simulates the running of the 5 state model and prints every instant's information
    for (int i = 0; i < OS.numOfPrograms; i++)
        if (OS.programs[i].start == 0)
            OS.programs[i].state = 5;

    OS.instance = 1;
    int quantum_unity = quantum_time;

    while (OS.instance != 0) {
        int is_running = 0;
        int number_of_executing_programs = 0;
        int running_program;

        printf("  %2d |", OS.instance);

        for (int i = 0; i < OS.numOfPrograms; i++) {

            if ((OS.programs[i].state == 2) || (peek(1) == i && OS.programs[i].state == 3))                                  // If program is running or program is blocked --> decrement program block cycle
                OS.programs[i].cycle[OS.programs[i].now]--;

            printProgramState(OS.programs[i].state);
                
            if(OS.programs[i].state == 2){
                is_running = 1;
                quantum_unity--;
                running_program = i;
            }

                
        }

        for (int i = 0; i < OS.numOfPrograms; i++) {

            if (OS.programs[i].state == 0)                                                                                 // If program is exiting --> terminate the program
                state_change(i);


            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && OS.programs[i].state == 2) {                                    // If program cycle is 0 and program state is running --> change the state of the program
                if (is_vrr == 0) {                                                                                      // Round Robin Standard (RR)
                    if (peek(0) == -1)                                                                                 // If ready queue is empty
                        is_running = 0;
                } else if (is_vrr == 1) {                                                                               // Virtual Round Robin (VRR)
                    if (peek(2) == -1)                                                                                 // If aux queue is empty
                        if (peek(0) == -1)                                                                             // If ready queue is empty
                            is_running = 0;
                }

                state_change(i);
                quantum_unity = quantum_time;
            }

            if (OS.programs[i].cycle[OS.programs[i].now] == 0 && OS.programs[i].state == 3)                                      // If program cycle is 0 and program state is blocked --> change the state of the program
                state_change(i);

            if (OS.programs[i].state != 6)                                                                                 // If program isn't finished --> increment number_of_executing_programs variable
                number_of_executing_programs++;
        }

        printf("%d\n",number_of_executing_programs);

        if (is_vrr == 0) {                                                                                              // Round Robin Standard (RR)
            if (is_running == 0 && peek(0) != -1)                                                                      // If no programs are running and ready queue isn't empty --> change the state of the queue program
                state_change(peek(0));

        } else if (is_vrr == 1) {                                                                                       // Virtual Round Robin (VRR)
            if (is_running == 0 && peek(2) != -1)                                                                      // If no programs are running and aux queue isn't empty --> change the state of the queue program
                state_change(peek(2));
            else if (is_running == 0 && peek(0) != -1)                                                                 // If no programs are running and ready queue isn't empty --> change the state of the queue program
                state_change(peek(0));
        }

        if (quantum_unity == 0) {                                                                                       // If the quantum cycle ends --> change running program and reset quantum cycle
            quantum_change(running_program);
            quantum_unity = quantum_time;
        }

        for (int i = 0; i < OS.numOfPrograms; i++) {
            if (OS.programs[i].state == 5)                                                                                 // If program is new --> put the program in ready queue
                state_change(i);

            if (OS.programs[i].start == OS.instance && OS.programs[i].state == 4)                                                 // If program isn't created --> create the program
                state_change(i);
        }

        OS.instance++;
        if (number_of_executing_programs == 0) {                                                                        // If there are no programs executing
            printf("  %2d |", OS.instance);

            for (int i = 0; i < OS.numOfPrograms; i++)
                printf("  ---  |");

            OS.numOfPrograms = 0;
        }

    }

    printf("\nALL PROGRAMS HAVE TERMINATED.");
}



void main(){
    int programas[5][10] = {
            {0, 3, 1, 2, 2, 4, 1, 1, 1, 1 },
            {1, 2, 4, 2, 4, 2, 0, 0, 0, 0 },
            {3, 1, 6, 1, 6, 1, 6, 1, 0, 0 },
            {3, 6, 1, 6, 1, 6, 1, 6, 0, 0 },
            {5, 9, 1, 9, 0, 0, 0, 0, 0, 0 }
    };
    int rows = 3;
    int cols = 10;
    OS.numOfPrograms = rows;
    OS.quantum_time = 3;
    //OS.queues = inicializeQueue();
    for(int i = 0; i < OS.numOfPrograms; ++i){
        OS.programs[i].id = i;
        OS.programs[i].now = 0;
        OS.programs[i].state = 4;
        for(int j = 0; j < 10 ;++j){
            OS.programs[i].cycle[j] = programas[i][j];
        }
        OS.programs[i].start=OS.programs[i].cycle[0];
    }
    SO copy = OS;
    printf("   instante|p1|p1-cycle|p1-sate|p2|p2-cycle|p2-sate|p3|p3-cycle|p3-sate|number of execut|\n|-|-|-|-|-|-|-|-|-|-|-|\n");
    run(OS.quantum_time,0);
}
