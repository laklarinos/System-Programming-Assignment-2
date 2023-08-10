#ifndef LINKED_H    
#define LINKED_H

    #include "person.h"
    #include "bloom-filter.h"
    #include <string>
    using namespace std;
    class skipList;
    class skipListNode;

    class linkedListNode{
        private:
            void* thing; //either a person* object or a string* depending on the type of hash
            char type;
        public:

// some attributes
            bloomFilter* BF; // linked list nodes for virus hash table, have a pointer to askipList and a  
                             // bloomFilter. For every other type of hash, citizen hash and country hash,
                             // these attributes are not implemented...
            skipList* VSL; //vaccinated skipList
            skipList* NVSL; // not vaccinated SkipList
            linkedListNode* next;
// helpers            
            char getType();
            //void initHashType(char c);
            void* getThing();
            int checkVSL();
            int checkNVSL();
            date* getDateOfVaccination(string virus);

// skip list methods
            int initSkipList(int sListSize);
            int initNotVaccinatedSkipList(int sListSize);

            int insertToNotVaccinatedSkipList(person * pr);
            int insertToSkipList(person* pr);

// bloom filter methods           
            int initBloom(int bloomSize);
            int insertToBloom(string str);

// insertion helpers
            int insertToList(person* citizen, linkedListNode** node);
            string* insertToList(string str, linkedListNode** node);
            int insertToList(linkedListNode* node, linkedListNode** head);

// more helpers o.O            
            int isEmpty();
            person* search(int citizenID);
            int search(string str);

// con/de structors
            linkedListNode(person* citizen);
            linkedListNode(string str);
            ~linkedListNode();
    };
    #include "skip-list.h"
#endif