#ifndef SKIP_
#define SKIP_
    #define maxSkipListLevel 50
    #define p 0.4
    #include "linkedList.h"
    class skipList{
        private:
            
            int sizeOfArray;
            int maxLevels = maxSkipListLevel;
            float prob;
        public:
        skipListNode** arrayOfSLists;
        // con/de structors
            skipList(int sizeArray);
            ~skipList();
        
        //helpers
            int getSizeOfArray();
            float getProb();
            int deleteNode(int citizenId);
            int checkAndProceed(person* pr, skipListNode* thisNode,int lvl = 0);
            int insertToSkipList(person* pr, int lvl = 0, skipListNode* downNode = NULL);
            int search(int citizenID = 0, int deletion = 0);
        
        //printing
            void printSkipList();
            void printData();
    };

    class skipListNode{
        private:
            person* pr;
            int id;
        public:
            skipListNode* next;
            skipListNode* down;
            int getId();
            person* getPerson();
            skipListNode(person* pr);
            ~skipListNode();
            skipListNode(skipListNode* sNode);
    };

#endif