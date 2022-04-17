#include <stdio.h>
#include <stdbool.h>
#include "main.h"

SO OS;              //Crear SO

Boolean isEmpty(enum Queues Q){     //Verifica se queues a queue esta vazia
    if(Q == ready)
        return OS.queues.readyFront == -1;
    else if(Q == block)
        return OS.queues.blockFront == -1;
}

int peek(enum Queues Q) {                   //devolve o primeiro elemento dda queue                                                                        
    if (Q == ready) {                                                                                          
        if (!isEmpty(ready))
            return OS.queues.readyPrograms[OS.queues.readyFront];
    } else if (Q == block) {                                                                                    
        if (!isEmpty(block))
            return OS.queues.blockPrograms[OS.queues.blockFront];
    } 
    return -1;
}

void enqueue(int id, enum Queues Q) {                        //coloca um emento na queue                                                    
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

int dequeue(enum Queues Q) {                //retira um processo da queue                                                                        
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

enum States getState(int id){       //devolve o estado de um processo
    return OS.programs[id].state;
}

void setState(int id, enum States state){       //define o estado de um processo
    OS.programs[id].state = state;
}

int getCycle(int id, int Position){             //devolve o valor do ciclo em que o processo se encontra
    return OS.programs[id].cycle[Position];
}

int getNow(int id){                             //devolve o a posição do ciclo em que o processo se encontra agora
    return OS.programs[id].now;
}

void changeProgram(int id) {            //função que analiza o estado atual e muda o estado
    Boolean programRunning = false;
    int aux;
    switch (getState(id)) {
        case EXIT:                      //processo terminou
            setState(id, FINISH);   

            break;
        case READY:                    //ready-> Run                                                                                           
            dequeue(0);
            setState(id, RUN);                                                                             
            ++OS.programs[id].now;

            break;
        case RUN:                                                                                                       
            if (getCycle(id,getNow(id)+1) == 0)     //caso proximo ciclo igual a 0 o processo terminou e vai para exit
                setState(id, EXIT);
            else {                          //Casos contrario o processo vai para block
                enqueue(id, block);
                setState(id, BLOCKED);
                ++OS.programs[id].now;
            }
            if (!isEmpty(ready)) {          //caso a queue ready não esteja vazia
                aux = dequeue(ready);       // vamos por um processo de ready em run
                setState(aux, RUN);                                                                            
                ++OS.programs[aux].now;
            }
            break;

        case BLOCKED:                                                                                      
            if (!isEmpty(block)) {          //passa de block para ready
                aux = dequeue(block);
                setState(aux, READY);                                                                            
                enqueue(aux, ready);
            }
            

            break;
        case NONCREATE:                         //processo não existe e passa a NEW                                                                  
            setState(id, NEW);                                                                      

            break;
        case NEW:                                                                                                       
            programRunning = 0;
            for (int i = 0; i < OS.numOfPrograms; ++i) {
                if (getState(i) == RUN)
                    programRunning=true;                //determina se existe algum processo em RUN
            }
            if(!programRunning){            // se não houver o processo passa automaticamente oara RUN
                setState(id, RUN);
                ++OS.programs[id].now;
            } else{                     //caso contario passa paraa a queue ready
                setState(id, READY);                                  
                enqueue(id, ready);
            }

            break;
        default:
            break;
    }
}

void quantumChangeProgram(int id) {                        //mudança ocorre quando um processo esta mais o quantum time no processador                                           
    if (!isEmpty(ready)) {
        int aux = dequeue(ready);
        setState(aux, RUN);                   
        ++OS.programs[aux].now;
        enqueue(id, ready );
        setState(id, READY);                          
        --OS.programs[id].now;
    }
   
}

void printProgramState(enum States state) {         //Print estados
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

void run() {                                        //responsavel pela execução dos processos
    for (int i = 0; i < OS.numOfPrograms; ++i)
        if (getCycle(i,0) == 0)
            setState(i, NEW);               //inicializa os processos que ja pode ser executados

    OS.instante = 1;
    int quantumProgram = OS.quantumTime;

    while (true) {
        Boolean isProgramRunning = false;
        int numOfExecutinfPrograms = 0;
        int runningProgram;

        printf("|    %2d |", OS.instante);

        for (int i = 0; i < OS.numOfPrograms; ++i) {        //printar todos os processos
            if(getState(i) == NONCREATE && getNow(i) ==0 ){
                --OS.programs[i].cycle[0];                  //decrementar o ciclo atual
            }
            if ((getState(i) == RUN) || (peek(block) == i && getState(i) == BLOCKED))      
                --OS.programs[i].cycle[getNow(i)];

            printProgramState(getState(i));
            if(getState(i) == RUN){
            isProgramRunning = true;
                if(OS.isRR){        // caso RR e program == RUN decrementar o quantum do processo atual
                --quantumProgram;
                runningProgram = i;
                }
            }

        }
        printf("\n");

        for (int i = 0; i < OS.numOfPrograms; ++i) {    //analisar e trocar o estados dos programas

            if (getState(i) == EXIT)                                                                                
                changeProgram(i);               //caso no estado Exit -> FINISH

            if (getCycle(i,getNow(i)) == 0 && getState(i) == RUN) {       //caso o ciclo run tenha chegado ao fim                              
                if (isEmpty(ready))                                              
                    isProgramRunning = true;
                changeProgram(i);
                if(OS.isRR)
                    quantumProgram = OS.quantumTime;
            }

            if (getCycle(i,getNow(i)) == 0 && getState(i) == BLOCKED)     //caso o ciclo block tenha chegado ao fim
                changeProgram(i);

            if (getState(i) != FINISH)                                                     
                ++numOfExecutinfPrograms;
        }
        if (!isProgramRunning && !isEmpty(ready))                   //caso não haja programa em run e haja programas em ready vamos colocar esse processo em run                                                     
            changeProgram(peek(0));

      

        if (OS.isRR && quantumProgram == 0) {                //rest quantum_time                                                                  
            quantumChangeProgram(runningProgram);
            quantumProgram = OS.quantumTime;
        }

        for (int i = 0; i < OS.numOfPrograms; ++i) {
            if (getState(i) == NEW)                 //caso new mudar estado         
                changeProgram(i);

            if (getCycle(i,0) == 0 && getState(i) == NONCREATE)      // caso esteja no instate do processo entrar no processador                                  
                changeProgram(i);
        }

        if (numOfExecutinfPrograms == 0) {                      //caso o numero de programa executando seja 0                                       
           // printf("|    %2d |", ++OS.instante);
                                                                //printa um ultimo instante e termina
           // for (int i = 0; i < OS.numOfPrograms; ++i)
           //     printf(" ----- |");

            break;
        }
        ++OS.instante;

    }

    printf("\n");
}

int main() {
    int programas[3][10] = {            //programas dados pelo stor
        {0, 3, 1, 2, 2, 4, 0, 0, 0, 0 } ,
        {1, 4, 2, 4, 1, 1, 0, 0, 0, 0 } ,
        {3, 2, 1, 6, 1, 3, 1, 1, 0, 0 } 
    };
    int rows = 3;       
    int cols = 10;

    OS.quantumTime = 3;         //inicializar o SO
    OS.numOfPrograms = rows;
    OS.queues.blockFront = -1;
    OS.queues.blockRear = -1;
    OS.queues.readyFront = -1;
    OS.queues.readyRear = -1;
    printf("|Instate|");
//         " -P0%- |"
    for (int i = 0; i < rows; ++i) {
        OS.programs[i].now = 0;
        setState(i, 4);             //set all program states to NOTCREATE

        for (int j = 0; j < cols; ++j)
            OS.programs[i].cycle[j] = programas[i][j];      //passar o ciclo para o prcesso

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
    OS.isRR = true;
    printf("\n");

    run();
    printf("  \n---  \n# Execution Finish!!  \n");
}
