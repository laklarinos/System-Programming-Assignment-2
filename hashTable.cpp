#include "hashTable.h"
#include <string>
#include <cstring>
#include <iostream>
using namespace std;
        int hashTable:: hashFunction(int x){
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            return x % this->buckets;
        }

        char hashTable:: getType(){
            // type of hash is either s or i
            // s for string based hash and i
            // for integer type hash...
            return this->type;
        }

        int hashTable:: hashFunction(string str){
            
            int n = str.length();
            char char_array[n + 1];
            strcpy(char_array, str.c_str());
            unsigned int hash = 0;
            for (int i = 0 ; char_array[i] != '\0' ; i++)
            {
                hash = 31*hash + char_array[i];
            }
            return hash % this->getNumBuckets();
        }

        linkedListNode** hashTable:: getArray(){
            return this->array;
        }

        int hashTable:: getNumBuckets(){
            return this->buckets;
        }

        bloomFilter* hashTable:: getBFOfVirus(string nameOfVirus){
            if(this->type == 's'){
                int key = hashFunction(nameOfVirus);
                if(key >= this->getNumBuckets() || key < 0) return NULL;
                linkedListNode* temp = this->array[key];
                while(temp!=NULL){
                    string* str = (string*)temp->getThing();
                    if(*str == nameOfVirus){
                        return temp->BF;
                    }
                    temp=temp->next;
                }
                return NULL;
            }else{
                return NULL;
            }
        }

        int hashTable:: isEmpty(){
            if(buckets==0){
                return 1;
            }
            return 0;
        }

        int hashTable:: insertToHash(person* citizen){
            this->type = 'i';
            //get the id
            int key = hashFunction(citizen->getId());
            return this->array[key]->insertToList(citizen,&array[key]);
        }

        int hashTable:: insertToHash(linkedListNode* node){
            int key;
            if(this->type == 's'){
                string str = *(string*)node->getThing();
                key = this->hashFunction(str);
            }else if(this->type == 'i'){
                person* pr = (person*) node->getThing();
                int id = pr->getId();
                key = this->hashFunction(id);
            }
            //cout <<key<<" "<<str<<endl;
            return this->array[key]->insertToList(node, &array[key]);
        }

        string* hashTable:: insertToHash(string str){
            this->type = 's';
            int key = this->hashFunction(str);
            string * res = this->array[key]->insertToList(str,&array[key]);
            return res;
        }
                
        hashTable::hashTable(int buckets){
            if (buckets == 0){
                this->array = NULL; 
                this->buckets = 0;
                return;
            }
            this->buckets = buckets;
            this->array = new linkedListNode*[buckets];
            for(int i = 0; i < buckets; i++){
                this->array[i] = NULL;
            }
            this->type = ' ';
        }

    
        int hashTable:: deleteNodeNotVaccSkipList(int citizenId,string virus){
            
            // to delete a node a node from a not vacc skip list...

            if(this->type == 's') {
                if(this->search(virus) == 1){
                    // virus exists...
                    int key = this->hashFunction(virus);
                    linkedListNode* temp = this->array[key];
                    while(temp != NULL){
                        
                        string str = *((string*)temp->getThing());
                        if(str == virus){
                            // found virus...
                            if(temp->NVSL->deleteNode(citizenId) == 0){
                                return 0;
                            }

                            if(temp->NVSL->getSizeOfArray() == 0){
                                delete temp->NVSL;
                                temp->NVSL = NULL;
                            }
                            return 1;
                        }
                        temp = temp->next;
                    }
                }
            }
        }

        void hashTable:: printData(){
            cout << "\n--------------------------------\n";            
            for(int i = 0; i < this->buckets; i++){
                if(this->array[i] != NULL){
                    linkedListNode* temp = this->array[i];
                    if(this->type == 'i'){
                        person* citizen;
                        while(temp != NULL){
                            citizen = (person*)temp->getThing();
                            cout<<citizen->getId()<<"->";
                            temp = temp->next;
                        }
                        cout << endl;
                    }else if (this->type == 's'){
                        string str;
                        while(temp != NULL){
                            str = *((string*)temp->getThing()); 
                            temp = temp->next;
                            cout << str<<"->";
                        } 
                        cout << endl;
                    }
                }else{
                    cout<<"NULL"<<endl;
                }
            }
            cout << "--------------------------------\n\n";
        }

        person* hashTable:: search(int citizenID){
            // find citizen...
            if(this->type == 'i'){
                int hashValue = hashFunction(citizenID);
                linkedListNode* temp = array[hashValue];
                person* citizen = temp->search(citizenID);
                if(citizen == NULL){
                    //cout<<"Sorry, we could not find citizen with id: "<<citizenID<<endl;
                    return NULL;
                }else{
                    //citizen->printData();
                    return citizen;
                }
            }else{
                cout << "This operation needs hash table with int ID\n";
                return NULL;
            }
        }

        int hashTable:: search(string str){
            // find virus/ country...
            if(this->type == 's'){
                int hashValue = hashFunction(str);
                linkedListNode* temp = array[hashValue];
                int res = temp->search(str);
                if(res == 0){
                    //cout<<"Sorry, we could not find string: "<<str<<endl;
                    return 0;
                }else{
                    return 1;
                }
            }else{
                cout << "This operation needs hash table with string\n";
                return 0;
            }
        }

        hashTable::~hashTable(){
            if(this->type == 'i'){
                if(this->array != NULL){
                    for(int i = 0; i < this->buckets; i++){
                        linkedListNode* temp = this->array[i];
                        linkedListNode* temp2;
                        while( temp!= NULL){
                            
                            temp2 = temp;
                            temp = temp2->next;
                            delete temp2;
                            
                        }            
                    }
                    delete [] this->array;
                }

            }else if (this->type == 's'){
               
                if(this->array!= NULL){
                    for(int i = 0; i < this->buckets; i++){
                        
                        linkedListNode* temp = this->array[i];
                        linkedListNode* temp2;
                        while( temp!= NULL){

                            temp2 = temp;
                            temp = temp->next;
                            delete temp2;

                        }            

                    }
                    delete[] this->array;
                }
            }
        }

        hashTable & hashTable :: operator= ( const hashTable & HT )	{
            for(int i = 0; i < HT.buckets; i++){
                //cout << "HHHHHHH\n";
                linkedListNode* temp = HT.array[i];
                int j = 0;
                while(temp!=NULL){
                    if(temp->getType() == 'p' && j == 0){
                        this->type = 'i';
                    }else if(temp->getType() == 's' && j == 0){
                        this->type = 's';
                    }
                    linkedListNode* temp2 = temp->next;
                    HT.array[i] = temp2;
                    temp->next = NULL;
                    this->insertToHash(temp);
                    temp = temp2;
                }
            }
        }

        void hashTable:: populationStatus(hashTable* virusHash,string virus, string country,date* date1, date* date2, int age1, int age2){
            
            // if dates are provided, just find all those vaccinated during this period date1-date2, who are
            // from this specific country, and divide them by the total number of those vaccinated and not
            // vaccinated who are also from this specific country

            
            int key = virusHash->hashFunction(virus);
            linkedListNode* temp = virusHash->array[key];
            if(temp == NULL){
                cout << "Sorry, we do not have data for this virus.\n";
                return ;
            }
            
            int numVaccOrNotVacc = 0;
            int numVaccDate1Date2 = 0;
            int totalVacc = 0;
            int vaccInAge = 0;

            
            while(temp != NULL){

                // first, find the virus we want 
                string str = *(string*)temp->getThing();
                
                if(str == virus){
                    
                    skipList* SL = temp->VSL;
                    skipListNode* temp2;
            
                    if(country != " "){

                        // if country is given...

                        // if skip list of this virus is not NULL
                        if(SL != NULL) {

                            temp2 = SL->arrayOfSLists[0];

                            // go through the skip list and 
                            // find all those from this country...
                            while(temp2 != NULL){
                                
                                person* pr = temp2->getPerson();
                            
                                if(*(pr->getCountry()) == country){
                                    // citizen is in vacc skip list and is from 
                                    // the country i want
                                    numVaccOrNotVacc++;
                                    int error = 0;
                                    
                                    // each citizen may have more than one viruses
                                    // check them all and find the one we want...
                                    for(int i = 0; i < pr->getNumViruses(); i++ ){
                                        
                                        string virus2 = *pr->virusArray[i];

                                        if(virus == virus2){
                                            
                                            // if dates are provided...
                                            if(date1 != NULL && date2 != NULL){
                                                
                                                // chech if citizens date of vaccination
                                                // was done between date1 and date2
                                                date* date = pr->arrayOfVaccDates[i];

                                                if(date->empty() == 0) {
                                                    totalVacc++;
                                                    // it's not 0-0-0000
                                                    int k = *date >= *date1;
                                                    int j = *date <= *date2;

                                                    if(k == 1 && j == 1 ){
                            
                                                        if(age2!=0){
                                                            // if age restriction is given...

                                                            if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                                // if age of citizen is between
                                                                // age1 and age2
                                                                numVaccDate1Date2++;
                                                            }

                                                        }else{
                                                            // if not...
                                                            numVaccDate1Date2++;
                                                        }
                                                        error = 1;
                                                        break;
                                                    }
                                                }
                                            }else{
                                                // if dates are not given...

                                                totalVacc++;
                                                if(age2 != 0){
                                                    // if age restriction is given
                                                    if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                        // check if dates are right...
                                                        numVaccDate1Date2++;
                                                    }
                                                }else{
                                                    // if not
                                                    numVaccDate1Date2++;
                                                }                                             
                                            }
                                        }
                                    }
                                    if(error == 1){
                                        temp2 = temp2->next;
                                        continue;
                                    }
                                }
                                temp2 = temp2->next;   
                            }
                        }
                        skipList* NSL = temp->NVSL;

                        // check not vaccinated skip list...

                        if(NSL != NULL){
                            
                            temp2 = NSL->arrayOfSLists[0];

                            // for each person in the skip list...
                            while(temp2 != NULL){

                                person* pr = temp2->getPerson();
                                // if he is from the country we want
                                if(*pr->getCountry() == country){
                                    numVaccOrNotVacc++;
                                }
                                temp2 = temp2->next;
                            }
                        }
                        
                        if(numVaccOrNotVacc == 0){
                            cout << "Error, 0 number of total citizens\n";
                            cout << country << " " << "0 0%\n"; 
                            return ;
                        }

                        double percentage =(double) numVaccDate1Date2 / numVaccOrNotVacc;
                        if(age2  == 0){
                            cout << country << " " << totalVacc << " " << percentage*100 << "%" << endl;
                        }else{
                            cout << numVaccDate1Date2 << " " << percentage*100 << "%" << endl;
                        }
                            
                        return ;

                    }else{

                        // if country is not given...
                        // go to skip list vacc, and do the same but for every country...

                        string* checkedCountries = NULL;
                        int prevIndex;
                        int flag = 0;
                        int i = 0;
                        
                        // if skip list is not null
                        if(SL != NULL){
                            
                            temp2 = SL->arrayOfSLists[0];
                            skipListNode* head = temp2;

                            // go to all citizens in this skip list
                            while(temp2 != NULL){
                                
                                int numVaccOrNotVacc = 0;
                                int numVaccDate1Date2 = 0;
                                int totalVacc = 0;

                                person *pr = temp2->getPerson();
                                
                                // take the country of this specific person...
                                string currentCountry = *pr->getCountry();
                            
                                skipListNode* temp3 = head;
                                
                                if(i == 0){

                                    checkedCountries = new string[i+1];
                                    checkedCountries[0] = currentCountry;
                                    prevIndex = i+1;

                                }else{
                                    
                                    // this procedure anytime that a new country
                                    // is encountered, it saves it in an array...
                                    // i do so in order not to check the same country twice...
                                    // For every new country that is not already in my array
                                    // I look out for the citizens that come from it...

                                    int error = 0;
                                    //check if current country is not checked.
                                    for(int i = 0; i< prevIndex; i++ ){

                                        if(checkedCountries[i] == currentCountry){
                                            // country already checked...
                                            // move on
                                            
                                            temp2 = temp2->next;

                                            // if have completely searched vaccinated 
                                            // skip list for this virus once...
                                            // we should now check its not vaccinated
                                            // skip list in order to have all the countries 
                                            // of the virus...

                                            if(temp2 == NULL && flag == 0){

                                                skipList* tempS = temp->NVSL;

                                                if(tempS != NULL) {   

                                                    temp2 = tempS->arrayOfSLists[0];

                                                }
                                                flag = 1;
                                                error = 1;
                                                break;

                                            }
                                            error = 1; 
                                            break;
                                        }

                                    }

                                    if(error) continue;

                                    // if here, country has not been checked...
                                    prevIndex++;

                                    // copy paste whole array to a new one
                                    string* newArray = new string[prevIndex];
                                    int j;
                                    
                                    for(j = 0; j < prevIndex - 1; j++){
                                        newArray[j] = checkedCountries[j];
                                    }

                                    newArray[j] = currentCountry;
                                    delete[] checkedCountries;
                                    checkedCountries = newArray;

                                }

                                                                    
                                while(temp3!=NULL && age2 == 0){
                                    
                                    // for every node of level 0 find every
                                    // node that has same country
                                    
                                    
                                    if(temp3->getId() != temp2->getId()){
                                        
                                        person* pr = temp3->getPerson();

                                        if(*(pr->getCountry()) == currentCountry){
                                            // citizen is in vacc skip list and is from the country i want
                                            numVaccOrNotVacc++;
                                            
                                            int error1 = 0;
                                            
                                            // because a citizen may have multiple viruses...
                                            // find the one we want...
                                            for(int i = 0; i < pr->getNumViruses(); i++ ){

                                                string virus2 = *pr->virusArray[i];
                                                
                                                if(virus == virus2){

                                                    if(date1 != NULL && date2 !=NULL){

                                                        // if dates are provided...

                                                        date* date = pr->arrayOfVaccDates[i];
                                                        if(date->empty() == 0) {
                                                            totalVacc++;
                                                            // it's not 0-0-0000
                                                            int i = *date >= *date1;
                                                            int j = *date <= *date2;
                                                
                                                            if(i == 1 && j == 1 ){
                                                                
                                                                if(age2!=0){
                                                                    // if we have age restriction...
                                                                    if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                                        numVaccDate1Date2++;
                                                                    }

                                                                }else{
                                                                    
                                                                    numVaccDate1Date2++;

                                                                }
                                                                error1 = 1;
                                                                break;
                                                            }

                                                        }
                                                    }else{
                                                        totalVacc++;
                                                        if(age2 != 0){
                                                        // we have age upper bound
                                                            if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                                numVaccDate1Date2++;
                                                            }
                                                        }else{
                                                            // no age upper/lower bound
                                                            numVaccDate1Date2++;
                                                        }
                                                    }
                                                }
                                            }
                                            if(error1){
                                                temp3 = temp3->next;
                                                continue;
                                            }
                                        }

                                    }else{
                                        
                                        date* dateOfVacc = NULL;
                                        for(int k = 0; k < pr->getNumViruses(); k++){
                                            if(*(pr->virusArray[k]) == virus){
                                                
                                                dateOfVacc = pr->arrayOfVaccDates[k];
                                                break;
                                            }

                                        }
                                        
                                        if(age2 != 0){
                                        // we have age upper bound
                                            if(date1 != NULL && date2 != NULL){
                                                int i = *dateOfVacc >= *date1;
                                                int j = *dateOfVacc <= *date2;
                                                if(i == 1 && j == 1){
                                                    if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                        numVaccDate1Date2++;
                                                    }
                                                }
                                            }else{
                                                if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                    numVaccDate1Date2++;
                                                }
                                            }

                                        }else{
                                            // no age upper/lower bound
                                            if(date1!=NULL && date2 != NULL){

                                                int i = *dateOfVacc >= *date1;
                                                int j = *dateOfVacc <= *date2;
                                                if(i == 1 && j ==1){
                                                    numVaccDate1Date2++;
                                                }
                                            }else{
                                                numVaccDate1Date2++;
                                            }
                                        }
                                        numVaccOrNotVacc++;
                                        totalVacc ++;
                                    }
                                    temp3 = temp3->next;
                                }

                                // check Not Vaccinated Skip List
                                // of virus and find those mot vaccinated
                                // for the virus...
                                skipList* NSL = temp->NVSL;

                                if(NSL != NULL){
                                    skipListNode* temp4 = NSL->arrayOfSLists[0];

                                    while(temp4 != NULL){
                                        person* pr = temp4->getPerson();
                                        if(*pr->getCountry() == currentCountry){
                                            numVaccOrNotVacc++;
                                        }
                                        temp4 = temp4->next;
                                    }
                                }
                                    
                                
                                if(numVaccOrNotVacc == 0 && age2 == 0){

                                    cout << "Error, 0 number of total citizens\n";
                                    temp2 = temp2->next;
                                    i++;
                                    
                                }else{
                                    double percentage =(double) numVaccDate1Date2 / numVaccOrNotVacc;
                                    if(age2 == 0){
                                        cout << currentCountry << " " << totalVacc << " " << percentage*100 << "%" << endl;
                                    }/*else{
                                        cout <<  numVaccDate1Date2 << " " << percentage*100 << "%" << endl;
                                    }*/
                                    temp2 = temp2->next;
                                    i++;
                                }

                                // if temp2 == NULL, we have searched all the skipList
                                // we know have to check not vaccinated skip list
                                // in order to find the countries of the virus 
                                // that have only not vaccinated citizens...

                                if(temp2 == NULL && flag == 0){
                                    
                                    skipList* tempS = temp->NVSL;
                                    
                                    if(tempS != NULL) {   
                                        
                                        temp2 = tempS->arrayOfSLists[0];

                                    }else{

                                        // if age2 != 0 
                                        // popStatusByAge()
                                        // for every country we found... 
                                        if(age2!=0){
                                            for(int i = 0; i < prevIndex; i++){                                
                                                this->popStatusByAge(virusHash, virus, checkedCountries[i], date1, date2);
                                                cout << endl;
                                            }

                                            delete [] checkedCountries;
                                            return ;
                                        }else{
                                            return;
                                        }
                                    }
                                    flag = 1;
                                    continue;

                                }
                            }

                        }else{

                            // if SL of this virus is NULL...
                            // check not vaccinated skip list...

                            skipList* NVSL = temp->NVSL;
                            temp2 = NVSL->arrayOfSLists[0];
                            skipListNode* head = temp2;

                            while(temp2 != NULL){
                                // go to every node of level 0... 
                                // find all the different countries and all their citizens

                                int numVaccOrNotVacc = 0;
                                int numVaccDate1Date2 = 0;
                                int totalVacc = 0;

                                person *pr = temp2->getPerson();
                                
                                string currentCountry = *pr->getCountry();
                            
                                skipListNode* temp3 = head;
                                
                                if(i == 0){

                                    checkedCountries = new string[i+1];
                                    checkedCountries[0] = currentCountry;
                                    prevIndex = i+1;

                                }else{
                                    int error = 0;
                                    //check if current country is not checked.
                                    for(int i = 0; i< prevIndex; i++ ){

                                        if(checkedCountries[i] == currentCountry){
                                            // country already checked...
                                            // move on
                                            
                                            temp2 = temp2->next;

                                            if(temp2 == NULL && flag == 0){

                                                skipList* tempS = temp->NVSL;

                                                if(tempS != NULL) {   

                                                    temp2 = tempS->arrayOfSLists[0];

                                                }
                                                flag = 1;
                                                error = 1;
                                                break;

                                            }
                                            error = 1; 
                                            break;
                                        }

                                    }

                                    if(error) continue;
                                    // if here, country has not been checked...
                                    prevIndex++;
                                    // copy paste whole array to a new one
                                    string* newArray = new string[prevIndex];
                                    int i;
                                    for(i = 0; i < prevIndex - 1; i++){
                                        newArray[i] = checkedCountries[i];
                                    }
                                    newArray[i] = currentCountry;
                                    delete[] checkedCountries;
                                    checkedCountries = newArray;

                                }
                                    
                                while(temp3!=NULL){
                                    // for every node of level 0 find every node 
                                    // that has same country
                                   
                                    if(temp3->getId() != temp2->getId()){

                                        person* pr = temp3->getPerson();
                                    
                                        if(*(pr->getCountry()) == currentCountry){
                                            // citizen is in vacc skip list and is from the country i want
                                            numVaccOrNotVacc++;
                                            // in order to increase vaccinated
                                            // we should check if the date
                                            // is in the limit.
                                            int error1 = 0;
                                            for(int i = 0; i < pr->getNumViruses(); i++ ){
                                                string virus2 = *pr->virusArray[i];
                                                if(virus == virus2){
                                                    if(date1 != NULL && date2 !=NULL){
                                                        date* date = pr->arrayOfVaccDates[i];
                                                        if(date->empty() == 0) {
                                                            totalVacc++;
                                                            // it's not 0-0-0000
                                                            int i = *date >= *date1;
                                                            int j = *date <= *date2;
                                                            
                                                            if(i == 1 && j == 1 ){
                                                                if(age2!=0){
                                                                    if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                                        numVaccDate1Date2++;
                                                                    }
                                                                }else{
                                                                    numVaccDate1Date2++;
                                                                }
                                                                error1 = 1;
                                                                break;
                                                            }

                                                        }
                                                    }else{
                                                        totalVacc++;
                                                        if(age2 != 0){
                                                        // we have age upper bound
                                                            if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                                numVaccDate1Date2++;
                                                            }
                                                        }else{
                                                            // no age upper/lower bound
                                                            numVaccDate1Date2++;
                                                        }
                                                    }
                                                }
                                            }
                                            if(error1){
                                                temp3 = temp3->next;
                                                continue;
                                            }
                                        }
                                    }else{
                                        if(age2 != 0){
                                        // we have age upper bound
                                            if(pr->getAge() >= age1 && pr->getAge() <= age2){
                                                numVaccDate1Date2++;
                                            }
                                        }else{
                                            // no age upper/lower bound
                                            numVaccDate1Date2++;
                                        }
                                        numVaccOrNotVacc++;
                                        totalVacc ++;
                                    }
                                    temp3 = temp3->next;
                                }
                                if(numVaccOrNotVacc == 0){

                                    cout << "Error, 0 number of total citizens\n";
                                    temp2 = temp2->next;
                                    i++;
                                    
                                }else{

                                    double percentage =(double) numVaccDate1Date2 / numVaccOrNotVacc;
                                    // if age2 != 0 this implies that populationStatus was
                                    // called from popStatusByAge as both these methods make use of 
                                    // each other...
                                    // thus if age2 == 0 you can print ...
                                    if(age2 == 0){
                                        cout << currentCountry << " " << totalVacc << " " << percentage*100 << "%" << endl;
                                    }/*else{
                                        cout <<  numVaccDate1Date2 << " " << percentage*100 << "%" << endl;
                                    }*/
                                    temp2 = temp2->next;
                                    i++;
                                }
                            }
                            
                        }

                        // if age2!=0 that means 
                        // we have age restriction and thus, that this method was
                        // called from popStatusByAge method. In other words, we are not done
                        // we need to do the same procedure for all the different age groups
                        // 0, 20 - 20, 40 etc...

                        if(age2 != 0){
                            
                            // we use the countries array which contains
                            for(int i = 0; i < prevIndex; i++){                                
                                this->popStatusByAge(virusHash, virus, checkedCountries[i], date1, date2);
                                cout << endl;
                            }

                            delete [] checkedCountries;
                            return ;
                        }

                    }
                } 
                temp = temp->next;
            }
        }

        void hashTable:: popStatusByAge(hashTable* virusHash,string virus, string country,date* date1, date* date2){
            // exactly the same as populationStatus but with ages...
            if(country != " "){
                cout << country <<endl;
                cout <<" 0-20 ";
                this->populationStatus(virusHash, virus, country, date1, date2, 0, 20);
                cout <<" 21-40 ";
                this->populationStatus(virusHash, virus, country, date1, date2, 21, 40);
                cout <<" 41-60 ";
                this->populationStatus(virusHash, virus, country, date1, date2, 41, 60);
                cout << " 60+ ";
                this->populationStatus(virusHash, virus, country, date1, date2, 60, 1000);
            }else{
                // take country by country...
                this->populationStatus(virusHash, virus, " ", date1, date2, 0, 100);
            }
        }




        int hashTable:: checkVaccinationBloom(int id ,string virus){
            if(this->type == 's'){

                string idToString = to_string(id);
                int key = this->hashFunction(virus);
                linkedListNode* temp = this->array[key];
                
                while(temp!=NULL){
                    string* str = (string*)temp->getThing();
                    if(*str == virus){
                        int res = temp->BF->search(idToString);
                        if(res){
                            cout << "\n----------------------------------------------------------\n";
                            cout << "Citizen with ID: "<<id<<" may have been vaccinated for "<<virus<<endl;
                            cout << "----------------------------------------------------------\n\n";
                            return 1;
                        }
                    }
                    temp = temp->next; 
                }
                cout << "\n----------------------------------------------------------\n";
                cout << "Citizen with ID: "<<id<<" has not been vaccinated for "<<virus<<endl;
                cout << "----------------------------------------------------------\n\n";

            }else{
                cout << "This operations requires given hash table to be string based.\n";
                return 0;
            }
            return 0;
        }

        int hashTable:: insertToBloom(person* pr, string virus){
            // I have a person and I want to insert him to 
            // respective bloom filter...
            //string virus = *pr->getVirus();
            int ok = 0;
            int counter;
            for(int i = 0; i< pr->getNumViruses(); i++){
                if(*pr->virusArray[i] == virus){
                    counter = i;
                    ok = 1;
                    break;
                }
            }
            if(! ok ){
                return -1;
            }

            if(pr->arrayVaccinated[counter] == "NO"){
                cout << "Sorry, bloom filter contains vacc people\n";
                return 0;
            }

            if(this->type == 's'){

                string idToString = to_string(pr->getId());
                int key = this->hashFunction(virus);
                linkedListNode* temp = this->array[key];

                while(temp!=NULL){
                    string* str = (string*)temp->getThing();
                    if(*str == virus){
                        string id = to_string(pr->getId());
                        if(temp->BF == NULL){
                            temp->initBloom(100000);
                            temp->insertToBloom(id);
                            return 1;
                        }
                        temp->insertToBloom(id);
                        return 1;
                    }
                    temp = temp->next; 
                }
            }else{
                cout << "This operations requires given hash table to be string based.\n";
                return 0;
            }
            return 0;
        }



        int hashTable:: insertToSkipList(person* pr, string virus){
            int key = this->hashFunction(virus);
            int ok = 0;
            int counter;
    
            for(int i = 0; i< pr->getNumViruses(); i++){
                if(*pr->virusArray[i] == virus){
                    counter = i;
                    ok = 1;
                    break;
                }
            }
            if(! ok ){
                return -1;
            }
            if(key > this->getNumBuckets() || key < 0){
                cout << "Sorry, value does not belong to this Hash Table\n";
                return -1;
            }

            linkedListNode* temp = this->array[key];
            while(temp != NULL){
                string str2 = *(string*)temp->getThing();
                if(str2 == virus){

                    if(pr->arrayVaccinated[counter] == "NO"){
                        if(temp->checkNVSL() == 0){
                            temp->insertToNotVaccinatedSkipList(pr);
                        }else{
                            temp->initNotVaccinatedSkipList(0);
                            temp->insertToNotVaccinatedSkipList(pr);
                        }
                        return 1;
                    }else if(pr->arrayVaccinated[counter] == "YES"){
                        if(temp->checkVSL() == 0){
                            temp->insertToSkipList(pr);
                        }else{
                            temp->initSkipList(0);
                            temp->insertToSkipList(pr);
                        }
                        return 1;
                    }
                }
                temp = temp->next;
            }
            return 0;
        }

       int hashTable:: initSkipListBloomFilter(int bloomSize, hashTable* virusHash, int sListSize) {

            if(this->type == 'i'){

                // for every bucket of citizen hash...
                for(int i = 0; i<this->getNumBuckets(); i++){

                    linkedListNode* temp = this->array[i];
                    // for every node of this linked list...
                    while(temp!=NULL){
                        person* pr = (person*)temp->getThing();
                        // for every virus of the citizen...
                        for(int j = 0; j < pr->getNumViruses(); j++){ 
                            string* virus = (string*)pr->virusArray[j];
                            
                            // if yes... insert to vac skip list...
                            if(pr->arrayVaccinated[j] == "YES"){    
                                
                                string idToString = to_string(pr->getId());
                                int key = virusHash->hashFunction(*virus);
                                linkedListNode* temp2 = virusHash->array[key];

                                // find the list node that contains this virus...
                                // in order to access its skip list...
                                while(temp2 != NULL){
                                    
                                    string str =*(string*)temp2->getThing();
                                    if(str == (*virus)){

                                        if(temp2->BF == NULL) temp2->initBloom(bloomSize);
                                        temp2->insertToBloom(idToString);

                                        if(temp2->VSL == NULL) temp2->initSkipList(sListSize);
                                        temp2->insertToSkipList(pr);
                                        break;

                                    }
                                    temp2 = temp2 -> next;

                                }

                            }else if (pr->arrayVaccinated[j] == "NO"){
                                // go to not vacc skip list...

                                int key = virusHash->hashFunction(*virus);
                                linkedListNode* temp2 = virusHash->array[key];

                                // find the list node that contains this virus...
                                // in order to access its not vacc skip list...
                                while(temp2 != NULL){

                                    string str =*(string*)temp2->getThing();
                                    if(str == (*virus)){    
                                        
                                        if(temp2->NVSL == NULL) temp2->initNotVaccinatedSkipList(sListSize);
                                        temp2->insertToNotVaccinatedSkipList(pr);
                                        break;

                                    }
                                    temp2 = temp2 -> next;

                                }

                            }
                        }   
                        temp = temp->next;
                    }
                }
                return 1;

           }else{

               cout << "This operation requires integer based Hash Table.\n";
               return 0;

           }
        }

        void hashTable::printVaccinatedSkipList(string str){
            int key = this->hashFunction(str);
            if(key > this->getNumBuckets() || key < 0){
                cout << "Sorry, value does not belong to this Hash Table\n";
                return;
            }    
            cout << "Vaccinated Skip List for Virus: "<<str<<endl;
            linkedListNode* temp = this->array[key];
            while(temp != NULL){
                string str2 = *(string*)temp->getThing();
                if(str2 == str){
                    //cout << "FOUND\n";
                    temp->VSL->printSkipList();
                    break;
                }
                //cout << "HHHHH\n";
                temp = temp->next;
            }
        }

        void hashTable:: printNotVaccinatedSkipList(string str){
            int key = this->hashFunction(str);
            if(key > this->getNumBuckets() || key < 0){
                cout << "Sorry, value does not belong to this Hash Table\n";
                return ;
            }    
            cout << "Not Vaccinated Skip List for Virus: "<<str<<endl;
            linkedListNode* temp = this->array[key];
            while(temp != NULL){
                string str2 = *(string*)temp->getThing();
                if(str2 == str){
                    //cout << "FOUND\n";
                    //temp->NVSL->printSkipList();
                    if(temp->NVSL != NULL){
                        temp->NVSL->printData();
                    }else{
                        cout << "Sorry, Not vaccinated skip list for virus "<< str <<" is empty.\n";
                    }
                    break;
                }
                //cout << "HHHHH\n";
                temp = temp->next;
            }
        }

        int hashTable:: searchVaccSkipList(int citizenID, hashTable *citizenHash,string str){
            if(this->type == 's'){    
                
                // str == virus given...
                person* pr = citizenHash->search(citizenID);
                if(pr == NULL){ 
                    cout<<"Sorry, citizen with id: "<<citizenID<<" does not exist\n";
                    return -1;
                }

                if(str != " "){

                    // if virus is given...  
                    
                    int key = this->hashFunction(str);
                    
                    if(key > this->getNumBuckets() || key < 0){
                        cout << "Sorry, value does not belong to this Hash Table\n";
                        return -1;
                    }

                    linkedListNode* temp = this->array[key];
                    
                    while(temp != NULL){
                        string str2 = *(string*)temp->getThing();
                        if(str2 == str){
                            
                            if(temp->checkVSL() == 0 ){
                                
                                if(temp->VSL->search(citizenID) == 1){
                                    // uncomment if you want to generate output

                                    //cout << "\n--------------------------------------------------------\n";
                                    //cout << "Citizen with ID: "<<citizenID<<" has been vaccinated for "<<str<<endl;
                                    //pr->printVaccDate(str);
                                    //cout << "--------------------------------------------------------\n\n";                               
                                    return 1;
                                }
                            }
                        }
                        temp = temp->next;
                    }
                    
                    //if(this->searchNotVaccSkipList(citizenID, citizenHash, str) == 1) return 2;
                    //cout << "Sorry, did not find the citizen neither in Vcc nor in Not Vacc skip list.\n";
                    return 0;
                }else{  
                    // if virus is not given...
                        
                    for(int i = 0; i < pr->getNumViruses(); i++){

                        string virus = *(pr->virusArray[i]);
                        int key = this->hashFunction(virus);
                        linkedListNode* temp = this->array[key];

                        while(temp != NULL){
                            string str2 = *(string*)temp->getThing();
                            if(str2 == virus){
                                if(temp->checkVSL() == 0){ 
                                    if(temp->VSL->search(citizenID) == 1){
                                        cout << "\n--------------------------------------------------------\n";
                                        cout << "Citizen with ID: "<<citizenID<<" has been vaccinated for "<<virus<<endl;
                                        pr->printVaccDate(str2);
                                        cout << "--------------------------------------------------------\n\n";
                                    }else{
                                        if(temp->checkNVSL() == 0){
                                            if(temp->NVSL->search(citizenID) == 1){                       
                                                cout << "\n--------------------------------------------------------\n";
                                                cout << "Citizen with ID: "<<citizenID<<" has not been vaccinated for "<<virus<<endl;
                                                cout << "--------------------------------------------------------\n\n";
                                            }
                                        }
                                    }
                                }else if(temp->checkNVSL() == 0){
                                    if(temp->NVSL->search(citizenID) == 1){     
                                        cout << "\n--------------------------------------------------------\n";
                                        cout << "Citizen with ID: "<<citizenID<<" has not been vaccinated for "<<virus<<endl;
                                        cout << "--------------------------------------------------------\n\n";
                                    }
                                }               
                            }
                            temp = temp->next;
                        }
                    }
                    return 0;
                }
            }else{
                cout << "Sorry, this operataion needs string based hash.\n";
                return 0;
            }
        }

        int hashTable:: searchNotVaccSkipList(int citizenID ,hashTable* citizenHash ,string str){
            if(this->type == 's'){    
                person* pr = citizenHash->search(citizenID);
                if(pr == NULL){ 
                    cout<<"Sorry, citizen with id: "<<citizenID<<" does not exist\n";
                    return -1;
                }
                if(str != " "){    

                    int key = this->hashFunction(str);
                    if(key > this->getNumBuckets() || key < 0){
                        cout << "Sorry, value does not belong to this Hash Table\n";
                        return -1;
                    }
                    // I can upgrade this code by using the citizenHash table to get acces to 
                    // virus pointer instead of looking for it...
                    /// later

                    linkedListNode* temp = this->array[key];
                    while(temp != NULL){
                        string str2 = *(string*)temp->getThing();
                        if(str2 == str){
                            if(temp->checkNVSL() == 0){
                                if(temp->NVSL->search(citizenID) == 1){
                                    // uncomment if you want to generate proper output...

                                    //cout << "\n--------------------------------------------------------\n";
                                    //cout << "Citizen with ID: "<<citizenID<<" has not been vaccinated for "<<str<<endl;
                                    //cout << "--------------------------------------------------------\n\n";
                                    return 1;
                                }
                            }
                        }
                        temp = temp->next;
                    }
                    return 0;
                }else{
                        person* pr = citizenHash->search(citizenID);
                        if(pr == NULL){ 
                            cout<<"Sorry, citizen with id: "<<citizenID<<" does not exist\n";
                            return -1;
                        }
                        for(int i = 0; i < pr->getNumViruses(); i++){
                            
                            string virus = *(pr->virusArray[i]);
                            int key = this->hashFunction(virus);
                            linkedListNode* temp = this->array[key];

                            while(temp != NULL){
                                string str2 = *(string*)temp->getThing();
                                if(str2 == virus){
                                    if(temp->checkNVSL() == 0){ 
                                        if(temp->NVSL->search(citizenID) == 1){                        
                                            cout << "\n--------------------------------------------------------\n";
                                            cout << "Citizen with ID: "<<citizenID<<" has not been vaccinated for "<<virus<<endl;
                                            cout << "--------------------------------------------------------\n\n";
                                            return 1;
                                            break;
                                        }

                                    }                            
                                }
                                temp = temp->next;
                            }

                        }
                        return 0;
                }
                return 0;
            }else{
                cout << "Sorry, this operataion needs string based hash.\n";
                return 0;
            }
        }

        date* hashTable:: getDateOfVaccination(int citizenID, string virus){
            
            if(this->type == 'i'){
                
                int key = hashFunction(citizenID);
                if(key < 0 || key >= this->getNumBuckets()){
                    return NULL;
                }
                
                linkedListNode* temp = this->array[key];
                while(temp!= NULL){
                    person* pr = (person*)temp->getThing();
                    if(pr->getId() == citizenID){
                        date* dateReceived;
                        dateReceived = temp->getDateOfVaccination(virus);
                        return dateReceived;
                    }
                    temp = temp->next;
                }
            }else{
                return NULL;
            }
        }

        int hashTable::checkHash(){
            
            int linksCount = 0;
            int nullCount = 0;

            for(int i = 0; i < this->buckets; i++){
                
                int flag = 0;
                linkedListNode* temp = this->array[i]; 

                while(temp!=NULL){
                    
                    flag = 1;
                    linksCount++;
                    temp = temp->next;
                }
                

                if(flag == 0){
                    // it is NULL
                    nullCount++;
                }
            }
            // Now we have the total amount of nodes for all lists
            // We compute an average and if the average exceeds 
            // a certain number lets say 3 list nodes per list...
            // we have to increase the size of the hash table.
            
            if(nullCount > this->buckets / 2){
                //cout<<"TOO MANY NULLS, DECREASE\n";
                return (this->buckets / 2);
            }else if(nullCount > 0 &&(linksCount / (this->buckets - nullCount)) >= 3){
                //cout << "TOO MANY LINKS, INCREASE\n";
                return (this->buckets * linksCount / (this->buckets - nullCount) );
            }else if(nullCount < this->buckets / 5){
                if(linksCount / (this->buckets) >= 2){
                    return (this->buckets * linksCount / (this->buckets - nullCount) );
                }
            }
            return 0;
        }
