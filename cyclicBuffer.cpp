#include "cyclicBuffer.h"

cyclicBuffer:: cyclicBuffer(int size = 0){
    if(size > 0){
        this->arrayOfPaths = new char*[size];
        for(int i = 0; i < size; i++){
            this->arrayOfPaths[i] = NULL;
        }
        this->length = size;
        this->head = 0;
        this->tail = 0;
        isFull = 0;
    }else{  
        cout << "Size is not a positive number\n";
    }
}

cyclicBuffer:: ~cyclicBuffer(){
        while(this->empty() != 1){
            free(this->arrayOfPaths[this->tail]);
            this->arrayOfPaths[this->tail] = NULL;
            this->tail = (this->tail + 1)%this->length;
            this->isFull = 0;
        }
        delete[] this->arrayOfPaths;
}

int cyclicBuffer:: insertToCyclicBuffer(char* path){
    if(this->full() == 1){
        cout << "Buffer is full\n";
        return 0;
    }

    if(this->arrayOfPaths[this->head] != NULL){
        free(this->arrayOfPaths[this->head]);
        this->arrayOfPaths[this->head] = NULL;
    }

    this->arrayOfPaths[this->head] = (char*) malloc(sizeof(char*)*(strlen(path)+1));
    strcpy(this->arrayOfPaths[this->head], path);
    this->head = (this->head + 1)%this->length;
    
    if(this->head == this->tail) isFull = 1;
    return 1;
}

int cyclicBuffer:: readFromCyclicBuffer(char** givePath){
    if(this->empty() == 1){
        cout << "Empty buffer\n";
        return -1;
    }

    *givePath = (char*) malloc(sizeof(char*)*(strlen(this->arrayOfPaths[this->tail])+1));
    strcpy(*givePath,this->arrayOfPaths[this->tail]);
    free(this->arrayOfPaths[this->tail]);
    this->arrayOfPaths[this->tail] = NULL;
    this->tail = (this->tail + 1)%this->length;
    this->isFull = 0;
    return 0;
}


int cyclicBuffer:: full(){
    return isFull == 1 ? 1:0;
}

int cyclicBuffer:: empty(){
    return (this->isFull != 1 && (this->head == this->tail)) ? 1:0;
}