
    #include "bloom-filter.h"
    #include <iostream>
    #include <string>
    #include <cstring>

    using namespace std;

    unsigned long bloomFilter:: djb2(unsigned char *str) {
        int c;
        unsigned long hash = 5381;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; // hash * 33 + c

        return hash;
    }

    unsigned long bloomFilter:: sdbm(unsigned char *str) {
        int c;
        unsigned long hash = 0;

        while ((c = *str++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }

    unsigned long bloomFilter:: hash_i(unsigned char *str, unsigned int i) {
	    return ((djb2(str) + i * sdbm(str) + i * i));
    }

    bloomFilter::bloomFilter(int bytes){

        this->array = new uint8_t[bytes];
        this->length = bytes;
        
        for(int i = 0; i < this->length; i++){
            this->array[i] = 0;
        }
        
    }

    bloomFilter::~bloomFilter(){
        delete [] this->array;
        this->array = NULL;
        this->length = 0;
    }

    int bloomFilter::getLength(){
        return this->length;
    }

    int bloomFilter::search(string str){
        int n = str.length();
        char char_array[n + 1];
        char_array[n+1] = '\0';
        strcpy(char_array, str.c_str());
        int K = 16;
        int values[K];
        
        for(int i = 0; i < K; i++){
            values[i] = hash_i((unsigned char*)char_array, i) % (this->length * 8);
        }

        for(int i = 0; i < K; i++){
            int k;
            k = this->array[(values[i] / 8)] & (1 << values[i] % 8);
            if(!k){
                return 0;
            }
        }
        return 1;
    }   

    bloomFilter:: bloomFilter(uint8_t* array, int bloomSize){
        this->array = new uint8_t[bloomSize];
        this->length = bloomSize;
        for(int i = 0; i < bloomSize; i++){
            this->array[i] = array[i];
        }
    }

    uint8_t* bloomFilter::getArray(){
        return this->array;
    }

    int bloomFilter::insert(string str){
        int n = str.length();
        char char_array[n + 1];
        char_array[n+1] = '\0';
        strcpy(char_array, str.c_str());
        int K = 16;
        int values[K];
        for(int i = 0; i < K ; i++){
            values[i] = hash_i((unsigned char*)char_array, i) % (8 * this->length);
        }
        for(int i = 0; i < K; i++){
            this->array[(values[i] / 8)] |= (1 << values[i] % 8);
        }
    }
