#ifndef HASH_H
#define HASH_H
#include <string>
#include "person.h"
#include "linkedList.h"
#define VIRUSARRAY 5
#define COUNTRYARRAY 5
using namespace std;
    class hashTable{
        private: 
            
            int buckets;
            char type;
        public:
            linkedListNode** array;
            // helpers...
            int checkHash();
            char getType();
            linkedListNode** getArray();
            int hashFunction(int x);
            int hashFunction(string str);
            int getNumBuckets();
            int isEmpty();
            void printData();
            void populationStatus(hashTable* virusHash,string virus, string country = " ",date* date1 = NULL, date* date2 = NULL, int age1 = 0, int age2 = 0);
            void popStatusByAge(hashTable* virusHash,string virus, string country = " ",date* date1 = NULL, date* date2 = NULL);
            bloomFilter* getBFOfVirus(string nameOfVirus);
            date* getDateOfVaccination(int citizenID, string virus);


            // for bloom...
            int checkVaccinationBloom(int id ,string virus);
            int initSkipListBloomFilter(int bloomSize, hashTable* virusHash, int sListSize = 0);
            int insertToBloom(person* pr, string virus);


            // for skipList...
            void printVaccinatedSkipList(string str);
            void printNotVaccinatedSkipList(string str);
            int searchVaccSkipList(int citizenID, hashTable* citizenHash, string str = " ");
            int searchNotVaccSkipList(int citizenID, hashTable* citizenHash, string str = " ");
            int insertToSkipList(person* pr, string virus);
            int deleteNodeNotVaccSkipList(int citizenId, string virus);
            
            // insert and search...
            int insertToHash(person* citizen);
            string* insertToHash(string str);
            int insertToHash(linkedListNode* node);
            person* search(int citizenID);
            int search(string str);
            

            // de/con - structors
            hashTable(int buckets);
            //hashTable(const hashTable& HT, int updateBuckets);
            hashTable & operator= ( const hashTable & other );
            ~hashTable();
    };
#endif