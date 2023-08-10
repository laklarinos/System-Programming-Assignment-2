#ifndef _CYCLIC_BUFFER
#define _CYCLIC_BUFFER
    #include "helpers.h"
    class cyclicBuffer{
        public:
            char** arrayOfPaths;
            int head;
            int tail;
            int length;
            int isFull;
            cyclicBuffer(int size);
            ~cyclicBuffer();
            int insertToCyclicBuffer(char* path);
            int readFromCyclicBuffer(char** givePath);
            int full();
            int empty();
    };

#endif