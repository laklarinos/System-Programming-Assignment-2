#ifndef _HELPERS
#define _HELPERS

    #include "person.h"
    #include "linkedList.h"
    #include "hashTable.h"
    #include "bloom-filter.h"
    #include "helpers.h"
    #include <ctime>
    #include <fstream>
    #include <iostream>  
    #include <string>
    #include <sstream>
    #include <cstring>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <cstdlib>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include "travelRequest.h"
    #include <netdb.h>
    #include <time.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include "cyclicBuffer.h"
    using namespace std;

    int inputCheck(hashTable**, hashTable**, hashTable**, int, person**);
    string* readInput(int* );
    date* dateConverter(string);
    int checkDateInput(string date);
    string firstCap(string str);
    string toUpper(string str);
    string** updateArray(string** virusArray, string* newVirus, int* length);
    
#endif