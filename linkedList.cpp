#include "linkedList.h"
#include <iostream>
#include <string>

using namespace std;

        void* linkedListNode:: getThing(){
            return this->thing;
        }

        char linkedListNode:: getType(){
            return this->type;
        }

        /*void linkedListNode:: initHashType(char c){
            this->HT = c;
        }

        char linkedListNode:: getHashType(){
            return this->HT;
        }*/

        int linkedListNode:: insertToSkipList(person* pr){
            // insert a person to skip list...
            if(this->VSL == NULL){
                //cout << "Skip List was not initialized previously. Default size is 4 levels.\n";
                this->initSkipList(1);
            } 
            this->VSL->insertToSkipList(pr);
        }

        

        int linkedListNode:: insertToNotVaccinatedSkipList(person* pr){

            // insert a person to a not vacc skip list...
            if(this->NVSL == NULL){
                //cout << "Skip List was not initialized previously. Default size is 4 levels.\n";
                this->initNotVaccinatedSkipList(1);
            } 
            this->NVSL->insertToSkipList(pr);

        }

        int linkedListNode:: initSkipList(int sListSize){
            //default size is 1
            if(sListSize == 0){
                this->VSL = new skipList(1);
                return 1;
            }
            this->VSL = new skipList(sListSize);
            return 1;
        }   
        
        int linkedListNode:: initNotVaccinatedSkipList(int sListSize){
            //default size if 1
            if(sListSize == 0){
                this->NVSL = new skipList(1);
                return 1;
            }
            this->NVSL = new skipList(sListSize);
            return 1;
        }

        int linkedListNode:: insertToBloom(string str){

            // insert a citizen to bloom...
            // its Id is given as a string...
            if(this->BF == NULL){
                //cout << "Bloom Filter was not initialized previously. Default size is 800000 bits.\n";
                this->initBloom(100000);
            }
            this->BF->insert(str);
        }

        int linkedListNode:: initBloom(int bloomSize){
            this->BF = new bloomFilter(bloomSize);
            return 1;
        }

        date* linkedListNode:: getDateOfVaccination(string virus){
            if(this->type == 'p'){
                // then its a citizen....
                person* pr = (person*) this->getThing();
                date* dateReceived = pr->getDateOfVaccination(virus);
                return dateReceived;
            }
        }

        int linkedListNode:: insertToList(person* citizen, linkedListNode** node){

            // insert a person to linked list...
            
            if(this == NULL){
                //if not collision
                *node = new linkedListNode(citizen);
            }else{
                // if collision
                linkedListNode* temp = this;
                linkedListNode* temp2 = NULL;
                while (temp!=NULL){

                    person* pers = (person*)temp->getThing();

                    if(pers->getId() == citizen->getId()){
                        
                        bool statement = pers->getName() != citizen->getName() || pers->getSurname() != citizen->getSurname()
                         || pers->getAge() != citizen->getAge() || *(pers->getCountry()) != *(citizen->getCountry());
                        static int i = 0;
                        if(statement){            
                            
                            // if given record has same ID with another saved record but differ in any one of the attributes
                            // name, surname, age, country... Illegal duplicate -> REJECT
                            cout << "Error, this citizen already exists with different info."<<endl;
                            citizen->printData();                            
                            return 0;

                        }else{                        
                            
                            // exact same citizen has 2 records for the same virus, drop the second...
                            for(int k = 0; k < citizen->getNumViruses(); k++){
                                for(int lamda = 0; lamda < pers->getNumViruses(); lamda++){
                                    if(*(citizen->virusArray[k]) == *(pers->virusArray[lamda])){
                                        cout << "Error, this citizen's record for this virus already exists."<<endl;
                                        //citizen->printData();
                                        return 0;
                                    }
                                }
                            }

                            // two records for the same person but different virus...
                            if(citizen->arrayVaccinated[0] == "NO"){
                                pers->updateArrays(citizen->virusArray[0], citizen->arrayVaccinated[0]);
                            }else{
                                pers->updateArrays(citizen->virusArray[0], citizen->arrayVaccinated[0], citizen->arrayOfVaccDates[0]);
                            }
                            delete citizen;
                            return 2;

                        }
                    }
                    temp2 = temp;
                    temp = temp->next;
                }
                temp2->next = new linkedListNode(citizen);
            }
            return 1;
        }

        int linkedListNode::insertToList(linkedListNode* node, linkedListNode** head){

            // insert a linked list node to linked list...
            if(this == NULL){
                (*head) = node;
            }else{
                linkedListNode* temp = this;
                linkedListNode* temp2 = NULL;
                while (temp != NULL){
                   //cout << "HELLO ";
                    temp2 = temp;
                    temp = temp->next;
                }
                temp2->next = node;
            }
            return 1;
        }

        string* linkedListNode:: insertToList(string str, linkedListNode** node){       
            // insert a country, virus 
            // to linked list...     
            if(this == NULL){     

                //if not collision
                *node = new linkedListNode(str);
                return (string*)(*node)->getThing();      
                
            }else{

                // if collision
                linkedListNode* temp = this;
                int flag = 0;
                linkedListNode* temp2 = NULL;

                while (temp!=NULL){

                    string str1 = *(string*)temp->getThing(); 
                    if(str1 == str){
                        flag = 1;
                        break;
                    }
                    temp2 = temp;
                    temp = temp->next;

                }

                if (flag == 0){
                    temp = new linkedListNode(str);
                    temp2->next = temp;
                    return (string*)temp->getThing();
                }

                return (string*)temp->getThing();
            }
        }

        int linkedListNode:: isEmpty(){
            return this == NULL;
        }

        person* linkedListNode:: search(int citizenID){
            linkedListNode* temp = this;
            person* citizen;
            while(temp!=NULL){
                citizen = (person*)temp->thing;
                if(citizen->getId() == citizenID){
                        return ((person*)temp->getThing());
                }
                temp = temp->next;
            }
            return NULL;
        }

        int linkedListNode:: search(string str){
            linkedListNode* temp = this;
            string* str2;
            while(temp!=NULL){
                str2 = (string*)temp->thing;
                if(*str2 == str){
                    return 1;
                }
                temp = temp->next;
            }
            return 0;
        }

        int linkedListNode::checkVSL(){
            if(this->VSL == NULL){
                return 1;
            }
            return 0;
        }
        int linkedListNode::checkNVSL(){
            if(this->NVSL == NULL){
                return 1;
            }
            return 0;
        }

        linkedListNode::linkedListNode(person* citizen){
            this->thing = citizen;
            this->next = NULL;
            this->BF = NULL;
            this->VSL = NULL;
            this->NVSL = NULL;
            this->type = 'p';
        }

        linkedListNode::linkedListNode(string str){
            this->thing = new string(str);
            this->next = NULL;
            this->initBloom(100000);
            this->VSL = NULL;
            this->NVSL = NULL;
            this->type = 's';
        }

        linkedListNode::~linkedListNode(){
            if(this->type == 's'){
                delete (string*)this->thing;  

                if(this->VSL != NULL){
                    delete VSL;
                    VSL = NULL;
                }

                
                if(this->NVSL != NULL){
                    delete NVSL;
                    NVSL = NULL;
                }


                if(this->BF != NULL){
                    delete BF;
                    BF = NULL;
                }
                

            }else if(this->type == 'p'){
                delete (person*)this->thing;
                this->next = NULL;
            }

        }
