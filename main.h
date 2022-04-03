#define QUEUE_SIZE 500
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
    int readyPrograms[QUEUE_SIZE];
    int readyFront;
    int readyRear;                                                               
    int blockPrograms[QUEUE_SIZE];
    int blockFront;
    int blockRear;                                                               

};

struct so{
    int instante;
    int numOfPrograms;
    int quantumTime;
    Program programs[11];
    Queue queues;
};

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