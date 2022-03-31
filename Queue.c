#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include"fatal.h"

typedef bool Boolean;
typedef struct node *Node;
typedef struct queue *Queue;

struct node{
    Program element;
    Node *next;
};

struct queue{
    int size;
    Node *Head;
    Node *Rear;
};

Queue inicializeQueue(){
    Queue Q = malloc( sizeof(Queue) ) ;
    Q->size = 0;
    Q->Head = NULL;
    Q->Rear = NULL;
    return Q;
}

Boolean isEmpty(Queue Q){
    return Q->size == 0;
}

void Enqueue(Program P, Queue Q){
    Node N = malloc( sizeof(Node));
    Node aux;
    N->element = P;
    N->next = NULL;
    if(Q->size == 0){
        Q->Head = N;
        Q->Rear = N ;
        ++Q->size;
    }
    else{
        aux = Q->Rear;
        aux->next = N;
        Q->Rear = N;
        ++Q->size;
    }
}

Program peek(Queue Q){
    Node aux = Q->Head;
    return aux->element;
}

void Dequeue(Queue Q){
    if(!isEmpty(Q)){
        if(Q->size == 1){
            Node aux = Q->Head;
            Q->Head = NULL;
            Q->Rear = NULL;
            --Q->size;
            free(aux);
        }
        else{
            Node aux = Q->Head;
            Q-> Head = aux->next;
            --Q->size;
            free(aux);
        }
    }
}

