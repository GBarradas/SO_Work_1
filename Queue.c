#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include"process.c"
#include"fatal.h"


typedef bool Boolean;
typedef struct node *Node;
typedef struct queue * Queue;

struct node {
    int element;
    Node next;
};
struct queue{
    int Size;
    Node Head;
    Node Rear;
};

Queue InicializeQueue(Node N){
    Queue Q;
    Q= malloc(sizeof(Queue));
    if(Q == NULL){
        FatalError("Out of space!!");
    }
    Q->Size=1;
    Q->Head=N;
    Q->Rear=N;
    return Q;
}

Boolean isEmpty(Queue Q){
    return Q->Size==0;
}
void add (Queue Q,Node N){
    Q->Size++;
    Q->Rear->next=N;
    Q->Rear=N;

}
Node getFirt(Queue Q){
    if(isEmpty(Q))
        Error("Queue is empty!!");
    return Q->Head;
}
Node removeFirst(Queue Q){
    if(isEmpty(Q))
        Error("Queue is empty!!");
    Node a= Q->Head;
    Q->Head=a->next;
    Q->Size--;
    return a;
}
Node atPos(Queue Q , int Position){
    if(Position>Q->Size)
        Error("Position out of bounds");
    Node aux=Q->Head;
    Position--;
    while(Position !=0){
        Position--;
        aux=aux->next;
    }
    return aux;
}
int getProcess(Node N){
    return N->element;
}
Node createNode(int  P){
    Node N = malloc(sizeof(Node));
    N->element=P;
    N->next=NULL;
    return N;
}


void main(){
   
    Node p=createNode(0);
    Queue q= InicializeQueue(p);
    add(q,p);
    p=createNode(20);
    add(q,p);
    printf("%d  %d\n",q->Head->element,q->Rear->element);

}