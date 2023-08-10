#ifndef BLOOM_
#define BLOOM_   
    #include <string>
    using namespace std;
    class bloomFilter{
        private:
            uint8_t *array;
            int length;
        public:
            bloomFilter(int bytes = 100000);
            bloomFilter(uint8_t* array, int bloomSize);
            ~bloomFilter();
            int getLength();
            uint8_t* getArray();
            int search(string str);
            int insert(string str);
            //int insertHash(hashTable* HT);
            unsigned long hash_i(unsigned char *str, unsigned int i);
            unsigned long sdbm(unsigned char *str);
            unsigned long djb2(unsigned char *str);
    };

#endif