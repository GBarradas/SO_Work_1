#define QUEUE_SIZE 500
typedef bool Boolean;
typedef struct so SO;
typedef struct queues Queue;
typedef struct program Program;

enum Queues{ready, block};  //2 queues que existem
enum States         //Varios estados que um processo pode tomar
{
    EXIT,
    READY,
    RUN,
    BLOCKED,
    NONCREATE,
    NEW,
    FINISH,

};

struct program{         //process
    int now;                      //posição do array(cycle) em que se encontra                                                                                       
    enum States state;                      //estado em que esta
    int start;                              //ciclo em que começa
    int cycle[11];                          //array do programa que é lido na main do main.c
}; 

struct queues{                          //queues array e posição do 1º elemento de cada queue e do ultimo
    int readyPrograms[QUEUE_SIZE];
    int readyFront;
    int readyRear;                                                               
    int blockPrograms[QUEUE_SIZE];
    int blockFront;
    int blockRear;                                                               

};

struct so{                  // struct do sisOpe guarda tudo o que é necessario para o So

    int instante;               //instante do So
    int numOfPrograms;           
    int quantumTime;            // quantum time para o round Robin
    Program programs[11];       //Todos os programas do So
    Queue queues;                 //Queues do so
    Boolean isRR;                  //Diz nos se é Round Robin ou se é FIFO
};
                //Funções usadas e declaradas em main.c
Boolean isEmpty(enum Queues Q);
int peek(enum Queues Q);
void enqueue(int id, enum Queues Q);
int dequeue(enum Queues Q);
enum States getState(int id);
void setState(int id, enum States state);
int getCycle(int id, int Position);
int getNow(int id);
void changeProgram(int id);
void quantumChangeProgram(int id);
void printProgramState(enum States state);
void run();
