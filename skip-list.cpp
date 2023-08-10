#include "skip-list.h"
#include "cstring"

skipList::skipList(int sizeArray){
    this->arrayOfSLists = new skipListNode*[1];
    this->arrayOfSLists[0] = NULL;
    this->sizeOfArray = 1;
    this->prob = p; // p = probability... check header file...
}

int skipList:: getSizeOfArray(){
    return this->sizeOfArray; // number of actual levels...
}

float skipList:: getProb(){
    //probability
    return this->prob;
}

int skipList::search(int citizenID, int deletion){
    
    if (citizenID > 0){
        
        skipListNode* temp = this->arrayOfSLists[this->sizeOfArray - 1]; // go to last level...
        skipListNode* temp2 = NULL;
        int index = this->sizeOfArray - 1;
        int i = 0;

        while(1){
            
            // we know we have to stop when we have arrived at the last level 
            // of the skip list...
            if(temp->getId() != citizenID && (temp->down == NULL && temp->next == NULL || 
            temp->down == NULL && temp->getId() > citizenID || 
            index < 0 )){
                return 0;
            }
            if(temp->getId() < citizenID){
                
                // we have to go right...

                if(temp->next == NULL){
                    // go down...
                    temp = temp->down;
                    i++;
                    index--;
                }else{
                    temp2 = temp;
                    temp = temp->next;
                }

            }else if (temp->getId() > citizenID){
                
                // citizen might be on the left...
                if(i == 0){
                    index--;                    
                    temp = this->arrayOfSLists[index];
                }else{
                    temp = temp2->down;
                    i++;
                    index--;
                }

            }else{

                // we have found our citizen...
                // we have to go to level 0 in order to return
                // its value...
                // however my implementation is such
                // that it provides a person pointer for every level...

                person* pr = temp->getPerson();
                if(deletion == 1){
                    
                    // delete all nodes from here and below...
                    
                    skipListNode* temp3 = temp;
                    skipListNode* temp4 = temp3;

                    int counter = index;
                    while(temp3 != NULL && counter >= 0){
                        
                        if(temp2 == NULL){

                            this->arrayOfSLists[counter] = temp3->next;
                            temp3 = temp3->down;
                            temp4->next = NULL;
                            delete temp4;
                            temp4 = temp3;

                            if(this->arrayOfSLists[counter] == NULL && counter > 0){
                                // i have to lower the level...
                                
                                skipListNode** SL = new skipListNode*[this->sizeOfArray - 1];
                                this->sizeOfArray --;

                                for(int i = 0; i < this->sizeOfArray; i++){
                                    SL[i] = this->arrayOfSLists[i];
                                }

                                delete[] this->arrayOfSLists;
                                this->arrayOfSLists = SL;
                            }else if(counter == 0 && this->arrayOfSLists[counter] == NULL){
                                
                                delete[] this->arrayOfSLists;
                                this->arrayOfSLists =NULL;
                                this->sizeOfArray = 0;

                            }

                        }else{
                            temp2->next = temp3->next;
                            temp3 = temp3->down;
                            delete temp4;
                            temp4 = temp3;
                            temp2 = temp2->down;
                            while(temp2!=NULL && temp2->next != NULL){
                                if(temp2->next->getId() == citizenID){
                                    break;
                                }
                                temp2 = temp2->next;
                            }

                        }
                        counter--;
                    }
                }
                return 1;
            }
            
        }
    }else{
        cout << "Sorry, give me a proper ID number > 0 and <= 9999\n";
    }
    return 0;
}

void skipList::printSkipList(){

    for(int i = this->sizeOfArray - 1; i >= 0; i--){
        skipListNode* sNode = this->arrayOfSLists[i];
        while(sNode!=NULL){    
            cout << sNode->getId()<<"->";
            sNode = sNode->next;
        }
        cout << endl;
    }
    cout<<endl;

}

void skipList:: printData(){

    if(this->arrayOfSLists == NULL){
        cout << "Sorry, this virus does not have any unvaccinated citizens.\n";
        return;
    }
    skipListNode* sNode = this->arrayOfSLists[0];
    if(sNode == NULL) {
        cout << "Sorry, this virus does not have any unvaccinated citizens.\n";
        return;
    }
    
    while(sNode!=NULL){    
        person* pr = sNode->getPerson();
        cout << pr->getId() << " " << pr->getName() << " " << pr->getSurname() << " " << *pr->getCountry() << " " << pr->getAge() << endl; 
        sNode = sNode->next;
    }

    cout << endl;
}

int skipList::insertToSkipList(person* pr, int lvl, skipListNode* downNode){ // int lvl = 0 default
    // node should contain a person* instance...
    
    skipListNode* temp;
    skipListNode* sNode = new skipListNode(pr);
    temp = this->arrayOfSLists[lvl];
    
    sNode->down = downNode;
    if(temp == NULL){

        // the very first node in this level
        this->arrayOfSLists[lvl] = sNode;
        this->checkAndProceed(pr,sNode,lvl);
        return 1;

    }else{

        // we have to insert it in a sorted way based on its ID...
        skipListNode* temp2 = temp;
        int i = 0;
        
        while(temp != NULL){

            if(temp->getId() > sNode->getId() && i == 0){
                
                // case when sNode has to replace
                // temp as the first element of the list...

                this->arrayOfSLists[lvl] = sNode;
                sNode->next = temp;

                // check if we need to insert it to upper level... 
                this->checkAndProceed(pr,sNode,lvl); 
                return 1;

            }else if(temp->getId() > sNode->getId()){

                temp2->next = sNode;
                sNode->next = temp;
                // check if we need to insert it to upper level...
                this->checkAndProceed(pr, sNode, lvl);
                return 1;

            }else if(temp->getId() == sNode->getId()){

                cout << "This citizen already exists in this Skip List\n";
                return 0;

            }
            temp2 = temp;
            temp = temp->next;
            i++;

        }        
        temp2->next = sNode;

        // check if we need to insert it to upper level...
        this->checkAndProceed(pr, sNode,lvl);
        return 1;
    }
}

int skipList::checkAndProceed(person* pr, skipListNode* thisNode,int lvl){
    float ran = ((double) rand() / RAND_MAX);

    if(lvl >= this->maxLevels) return 1;

    if(ran > prob){

        // we insert to upper level...
        
        if(this->sizeOfArray > lvl+1){

            //that means, our current array already has the upper lvl...
            // do not allocate and proceed...
            this->insertToSkipList(pr, ++lvl, thisNode);

        }else{

            // we need to create the new level... 
            // hold the current size...
            int prevNumOfLvls = this->sizeOfArray;

            //increase the number of lvls
            this->sizeOfArray++; 

            // malloc the new array with increased number of cells
            skipListNode** newArray = new skipListNode*[this->sizeOfArray];
            // copy the old array to the new...
            int i;

            for(i = 0; i < prevNumOfLvls; i++){
                newArray[i] = this->arrayOfSLists[i];
            }

            // malloc the new lvl
            // recurr
            
            newArray[i] = NULL;
            delete [] this->arrayOfSLists;

            this->arrayOfSLists = NULL;

            this->arrayOfSLists = new skipListNode*[this->sizeOfArray];

            for(i = 0; i < this->sizeOfArray; i++){
                this->arrayOfSLists[i] = newArray[i];
            }

            delete[] newArray;
            newArray = NULL; 
            
            this->insertToSkipList(pr, ++lvl, thisNode);
            
        }
    }
}

int skipList::deleteNode(int citizenId){
    // should delete node with citizenId from every level...
    if(this->search(citizenId, 1) == 0) return 0;
    return 1;
}

skipList::~skipList(){
    
    for(int i = 0; i < this->sizeOfArray; i++){
        skipListNode* sNode = this->arrayOfSLists[i];
        skipListNode* temp;
        while(sNode != NULL){
            temp = sNode;
            sNode = sNode->next;
            delete temp;
        }
    }
    if(this->arrayOfSLists != NULL) delete [] this->arrayOfSLists;
    this->arrayOfSLists = NULL;
}

int skipListNode::getId(){
    return this->id;
}

person* skipListNode:: getPerson(){
    return this->pr;
}

skipListNode::skipListNode(person* pr){
    this->pr = pr;
    this->id = pr->getId();
    this->next = NULL;
    this->down = NULL;
}

skipListNode::skipListNode(skipListNode* sNode){
    this->pr = sNode->pr;
    this->id = sNode->id;
    this->next = NULL;
    this->down = NULL;
}

skipListNode:: ~skipListNode(){
    this->next = NULL;
    this->down = NULL;
    this->pr = NULL;
}