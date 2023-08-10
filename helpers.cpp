#include "helpers.h"

int inputCheck(hashTable** virusHash, hashTable** citizenHash, hashTable** countryHash, int personArraySize, person** personArray){
    // commands...
    
    string wholeCommand;
    string command;
    string opt1 = "/vaccineStatusBloom";
    string opt2 = "/vaccineStatus";
    string opt3 = "/populationStatus";
    string opt4 = "/popStatusByAge";
    string opt5 = "/insertCitizenRecord";
    string opt6 = "/vaccineNow";
    string opt7 = "/list-nonVaccinated-Persons";
    string opt8 = "/exit";

    while(1){
        
        int counter = 0;
        string* arrayOfStrings = readInput(&counter);
        cout << endl;

        if(arrayOfStrings == NULL){
            cout <<"\nSorry, did not get that one, please check your commands and try again.\n";
            cout <<"The commands are:\n";
            cout << opt1 << endl;
            cout << opt2 << endl;
            cout << opt3 << endl;
            cout << opt4 << endl;
            cout << opt5 << endl;
            cout << opt6 << endl;
            cout << opt7 << endl;
            cout << opt8 << endl;
            continue;
        }

        if(arrayOfStrings[0] == opt1){
            if(counter - 1 < 2 || counter - 1 > 2){
                cout << "A citizenID and a virus Name should be given with this command\n";
                delete [] arrayOfStrings;
                continue;
            }else{
                string str = arrayOfStrings[1];
                int length = arrayOfStrings[1].length();
                for(int i = 0; i < length; i++){
                    if(str[i] < '0' || str[i] > '9'){
                        cout << "First parameter should be a citizen id (integer).\n";
                        delete[] arrayOfStrings;
                        continue;
                    }
                }          
                
                int key = stoi(arrayOfStrings[1]);
                //string virus = toUpper(arrayOfStrings[2]);
                string virus = arrayOfStrings[2];
                if(key < 0){
                    cout << "Sorry, we could not find citizen with id: "<<key<<endl;
                    delete [] arrayOfStrings;
                    continue;
                }

                
                (*virusHash)->checkVaccinationBloom(key,virus);
                delete[] arrayOfStrings;
                continue;
            }
        }else if(arrayOfStrings[0] == opt2){

            if(counter - 1 < 1 || counter - 1 > 2){

                cout << "A citizenID and a virus Name should be given with this command\n";
                delete [] arrayOfStrings;
                continue; 

            }else if(counter - 1 == 2){
                string str = arrayOfStrings[1];
                int length = arrayOfStrings[1].length();
                int error = 0;
                for(int i = 0; i < length; i++){
                    if(str[i] < '0' || str[i] > '9'){
                        cout << "First parameter should be a citizen id (integer).\n";
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }  
                if(error) break;
                int key = stoi(arrayOfStrings[1]);
                string virus = toUpper(arrayOfStrings[2]);
                if(key < 0){
                    cout << "Sorry, we could not find citizen with id: "<<key<<endl;
                    delete [] arrayOfStrings;
                    continue;
                }

                if((*virusHash)->search(virus) == 0){
                    cout << "Sorry, did not find this virus.\n";
                    delete [] arrayOfStrings;
                    continue;
                }

                if((*virusHash)->searchVaccSkipList(key,*citizenHash,virus) == 0) {
                    (*virusHash)->searchNotVaccSkipList(key,*citizenHash,virus);
                }

                delete [] arrayOfStrings;
                continue;
            }else if (counter - 1 == 1){

                string str = arrayOfStrings[1];
                int length = arrayOfStrings[1].length();
                for(int i = 0; i < length; i++){
                    if(str[i] < '0' || str[i] > '9'){
                        cout << "First parameter should be a citizen id (integer).\n";
                        delete[] arrayOfStrings;
                        continue;
                    }
                }  
                
                int key = stoi(arrayOfStrings[1]);

                if(key < 0){
                    cout << "Sorry, we could not find citizen with id: "<<key<<endl;
                    delete [] arrayOfStrings;
                    continue;
                }
                (*virusHash)->searchVaccSkipList(key,*citizenHash," ");

            }

        }else if(arrayOfStrings[0] == opt3){
            
            if(counter - 1 < 1){
                // if equals 3 then we certainly know something is wrong
                cout<<"Sorry, command \" "<<arrayOfStrings[0]<<" \" expects at least a virus name along side with 2 (date 1 < date 2).\n";
                delete [] arrayOfStrings;
                continue;
            }
            
            // first case, user provides virus, 2 dates, and country...
            // second case user provides virus with 2 dates...
            // i do exactly the same but for every country...

            string country = firstCap(arrayOfStrings[1]);

            // in order to check if first parameter is a proper country...
            // let's check if it is in country virus... 
            // because we use hashTable, the cost will be negligible
            
            if((*countryHash)->search(country) == 1) {

                if(counter - 2 == 0){
                    // if only a country is given...
                    cout << "Sorry, I need a virus.\n";
                    delete [] arrayOfStrings;
                    continue;
                }
                // then it is proper country...
                string virus = toUpper(arrayOfStrings[2]);

                // lets's check if sec param is proper virus...
                if((*virusHash)->search(virus) == 1){
                    
                    // if it is ...
                    // check the dates...
                    
                    if(counter - 1 != 2 && counter - 1 != 4){
                        // user has provided a proper country and a right virus but 1 date.
                        cout <<" Sorry, 2 or 0 dates are needed.\n";
                        delete [] arrayOfStrings;
                        continue;
                    }
                    
                    if(counter - 1 == 4){

                        if(checkDateInput(arrayOfStrings[3]) || checkDateInput(arrayOfStrings[4])){
                            cout << "Sorry, problematic date(s).\n";
                            delete [] arrayOfStrings;
                            continue;
                        }

                        date* date1 = dateConverter(arrayOfStrings[3]);
                        date* date2 = dateConverter(arrayOfStrings[4]);


                        (*citizenHash)->populationStatus(*virusHash, virus, country, date1, date2);
                        delete date1;
                        delete date2;
                        delete [] arrayOfStrings;
                        continue;
                    }

                }else{
                    
                    cout<<"Sorry, non-existent virus.\n";
                    delete [] arrayOfStrings;
                    continue;

                }
               
                (*citizenHash)->populationStatus(*virusHash , virus, country, NULL, NULL);

                delete [] arrayOfStrings;
                continue;

            }else{
                // let's check if it is virus
                
                string virus = toUpper(arrayOfStrings[1]);

                if((*virusHash)->search(virus) == 1){
                    // user's first param was a virus...
                    // now we expect 2 dates...
                    if(counter - 1 != 3 && counter - 1 != 1){

                        // user has provided a right virus but 1 date.
                        cout << "Sorry, 2 or 0 dates are needed.\n";
                        delete [] arrayOfStrings;
                        continue;

                    }

                    if(counter - 1 == 3){

                        if(checkDateInput(arrayOfStrings[2]) || checkDateInput(arrayOfStrings[3])){
                            cout << "Sorry, problematic date(s).\n";
                            delete [] arrayOfStrings;
                            continue;
                        }
                        date* date1 = dateConverter(arrayOfStrings[2]);
                        date* date2 = dateConverter(arrayOfStrings[3]);

                        (*citizenHash)->populationStatus(*virusHash, virus, " ", date1, date2);
                        delete date1;
                        delete date2;
                        delete [] arrayOfStrings;
                        continue;
                        
                    }

                    // if user does not provide dates... 
                    (*citizenHash)->populationStatus(*virusHash , virus," ", NULL, NULL);
                    delete [] arrayOfStrings;         
                    continue;

                }else{
                    
                    cout<<"Sorry, non-existent virus.\n";    
                    delete [] arrayOfStrings;         
                    continue;

                }
            }
        }else if(arrayOfStrings[0] == opt4){  

            if(counter - 1 < 1){
                // if equals 3 then we certainly know something is wrong
                cout<<"Sorry, command \" "<<arrayOfStrings[0]<<" \" expects at least a virus name along side with 2 (date 1 < date 2).\n";
                delete [] arrayOfStrings;
                continue;
            }
            
            // first case, user provides virus, 2 dates, and country...
            // second case user provides virus with 2 dates...
            // i do exactly the same but for every country...

            string country = arrayOfStrings[1];

            // in order to check if first parameter is a proper country...
            // let's check if it is in country virus... 
            // because we use hashTable, the cost will be negligible

            if((*countryHash)->search(country) == 1) {

                // then it is proper country...
                string virus = arrayOfStrings[2];

                // lets's check if sec param is proper virus...
                if((*virusHash)->search(virus) == 1){
                    
                    // if it is ...
                    // check the dates...
                    
                    if(counter - 1 != 2 && counter - 1 != 4){
                        // user has provided a proper country and a right virus but 1 date.
                        cout <<" Sorry, 2 or 0 dates are needed.\n";
                        delete [] arrayOfStrings;
                        continue;
                    }
                    
                    if(counter - 1 == 4){

                        if(checkDateInput(arrayOfStrings[3]) || checkDateInput(arrayOfStrings[4])){
                            cout << "Sorry, problematic date(s).\n";
                            delete [] arrayOfStrings;
                            continue;
                        }

                        date* date1 = dateConverter(arrayOfStrings[3]);
                        date* date2 = dateConverter(arrayOfStrings[4]);


                        (*citizenHash)->popStatusByAge(*virusHash, virus, country, date1, date2);
                        delete date1;
                        delete date2;
                        delete [] arrayOfStrings;
                        continue;
                    }

                    // if just virus... do exactly the same as above but with no dates...
                    // that means you should print the total amount o vaccinated and the percentage of
                    // them.
                    
                    
                    (*citizenHash)->popStatusByAge(*virusHash, virus," ", NULL, NULL);
                }else{
                    
                    cout<<"Sorry, non-existent virus.\n";
                    delete [] arrayOfStrings;
                    continue;

                }

                // everything good with date...
                
                (*citizenHash)->popStatusByAge(*virusHash , virus, country, NULL, NULL);
                delete [] arrayOfStrings;
                continue;


            }else{
                // let's check if it is virus
                
                string virus = arrayOfStrings[1];
                if((*virusHash)->search(virus) == 1){
                    // user's first param was a virus...
                    // now we expect 2 dates...
                    if(counter - 1 != 3 && counter - 1 != 1){

                        // user has provided a proper country and a right virus but 1 date.
                        cout << "Sorry, 2 or 0 dates are needed.\n";
                        delete [] arrayOfStrings;
                        continue;

                    }

                    if(counter - 1 == 3){
                        //cout <<" HELLP\n";
                        if(checkDateInput(arrayOfStrings[2]) || checkDateInput(arrayOfStrings[3])){
                            cout << "Sorry, problematic date(s).\n";
                            delete [] arrayOfStrings;
                            continue;
                        }
                        date* date1 = dateConverter(arrayOfStrings[2]);
                        date* date2 = dateConverter(arrayOfStrings[3]);
                        
                        (*citizenHash)->popStatusByAge(*virusHash, virus, " ", date1, date2);

                        delete date1;
                        delete date2;
                        delete [] arrayOfStrings;
                        continue;
                        
                    }

                    // if user does not provide dates... 

                    (*citizenHash)->popStatusByAge(*virusHash , virus," ", NULL, NULL);
                    delete [] arrayOfStrings;
                    continue;
                }else{
                    
                    cout<<"Sorry, non-existent virus.\n";
                    delete [] arrayOfStrings;
                    continue;

                }
            }
        }else if(arrayOfStrings[0] == opt5){
            
            // user gives id, name, surname, country, age, virus, yes / no, [date]
            // case 1, user gives right input...
            // case 2, user gives faulty input (something missing)..
            // case 3, user gives record with id that already exist but with different info...
            
            // check input...
            if(counter - 1 != 8 && counter - 1 != 7){
                cout<<"Sorry, command \" "<<arrayOfStrings[0]<<" \" expects record: ID, Name, Surname, Country, Age, Virus, Yes/No, Date(if yes).\n";          
                delete [] arrayOfStrings;
                continue;
            }

            // check if id has arithmetic value...
            string id = arrayOfStrings[1];

            int length = arrayOfStrings[1].length();
            int error = 0;
            for(int i = 0; i < length; i ++){
                if(id[i] < '0' || id[i] > '9'){
                    cout << "Sorry, Id must be integer.\n";
                    error = 1;
                    break;
                }
            }

            if(error){
                delete[] arrayOfStrings;
                continue;
            }

            // check if age is integer...
            string age = arrayOfStrings[5];

            int lengthAge = arrayOfStrings[5].length();
            error = 0;
            for(int i = 0; i < lengthAge; i++){
                if(age[i] < '0' && age[i] > '9'){
                    cout << "Sorry, Age must be integer.\n";
                    error = 1;
                    break;
                }
            }
            if(error){
                delete[] arrayOfStrings;
                continue;
            }

            
            if(lengthAge > 3){
                cout << "Sorry, max age is 120 years old\n";
                delete [] arrayOfStrings;
                continue;
            }
            // if age is integer... check if there exists such a citizen...
            
            int idInt = stoi(id);
            int ageInt = stoi(age);
            // check if this user already exists in citizen hash...
            person* pr = (*citizenHash)->search(idInt);
            string name = arrayOfStrings[2];
            string surname = arrayOfStrings[3];
            string country = firstCap(arrayOfStrings[4]);
            string virus = toUpper(arrayOfStrings[6]);
            string vacc = toUpper(arrayOfStrings[7]);
            
            if(vacc == "YES" && counter - 1 != 8){
                cout << "Sorry, I expect id, name, surname, country, age, YES, date.\n";
                delete [] arrayOfStrings;
                continue;
            }

            if(vacc == "NO" && counter - 1 != 7){
                cout << "Sorry, I expect id, name, surname, country, age, NO.\n";
                delete [] arrayOfStrings;
                continue;
            }

            if(vacc != "YES" && vacc != "NO"){
                cout << "Sorry, I expect a YES or NO.\n";
                delete[] arrayOfStrings;
                continue;
            }

            if(pr == NULL){
                // person with that ID does not exist...
                // create instance of person, check date
                // check virus, country
                error = 0;
                int countryLength = country.length();
                for(int i = 0; i < countryLength; i++){
                    if(country[i] < 'A' || (country[i] < 'a' && country[i] > 'Z') || country[i] > 'z'){
                        cout << "Sorry, country "<< country <<" must be a proper string.\n";
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }
                if(error) continue;
                    // country does not exist... 

                string* addressCountry = (*countryHash)->insertToHash(country);
                // also check if hash table needs re-size...
                int updateBuckets = (*countryHash)->checkHash();
            
                // check if it needs re-orientation...
                if(updateBuckets > 0 ){
                    hashTable* updateCountryHash;
                    updateCountryHash = new hashTable(updateBuckets);
                    (*updateCountryHash) = (**countryHash); 

                    linkedListNode** array = (*countryHash)->getArray();
                    for(int i = 0; i < (*countryHash)->getNumBuckets(); i++){
                        delete array[i];
                    }
                    delete [] array;
                    (*countryHash)->array = NULL;
                    delete *countryHash;
                    
                    *countryHash = updateCountryHash;
                }
                    
                error = 0;
                int virusLength = virus.length();
                for(int i = 0; i < virusLength; i++){
                    if(virus[i] != '-' && (virus[i] < 'A') || (virus[i] < 'a' && virus[i] > 'Z') || 
                    (virus[i] > 'z') && virus[i] > '9' && virus[i] <'0'){
                        cout << "Sorry, virus must be a proper string.\n";
                        cout << country << endl;
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }

                if(error) continue;
                   
                string* addressVirus = (*virusHash)->insertToHash(virus);
                updateBuckets = (*virusHash)->checkHash();
                // check if it needs re-orientation...
                if(updateBuckets > 0 ){
                    hashTable* updateVirusHash;
                    updateVirusHash = new hashTable(updateBuckets);
                    (*updateVirusHash) = (**virusHash); 
                    linkedListNode** array = (*virusHash)->getArray();
                    for(int i = 0; i < (*virusHash)->getNumBuckets(); i++){
                        delete array[i];
                    }
                    delete [] array;
                
                    (*virusHash)->array = NULL;
                    delete *virusHash;
                    *virusHash = updateVirusHash;
                }

                if(counter - 1 == 8){
                    // we have id, name, surname, age, country, virus, vaccinatedStatus , date
                    // check if arrayOfStrings[7] proper date...
                    if(arrayOfStrings[7] != "YES"){
                        cout<<"Sorry, I expected a YES.\n";
                        delete[] arrayOfStrings;
                        continue;
                    }
                    
                    // it's yes
                    int res = checkDateInput(arrayOfStrings[8]);

                    if(res == 0){

                        // date is correct...
                        date* date1 = dateConverter(arrayOfStrings[8]);
                        
                        // everything is right, we can now proceed to insertion...   
                        person* pr = new person(idInt, name, surname, ageInt,vacc, addressVirus, addressCountry, date1);
                        // now i need to insert to bloom filter and to skip list...

                        (*citizenHash)->insertToHash(pr);
                        updateBuckets = (*citizenHash)->checkHash();
                        // check if it needs re-orientation...
                        if(updateBuckets > 0 ){
                            hashTable* updateCitizenHash;
                            updateCitizenHash = new hashTable(updateBuckets);
                            (*updateCitizenHash) = (**citizenHash); 
                            linkedListNode** array = (*citizenHash)->getArray();
                            for(int i = 0; i < (*citizenHash)->getNumBuckets(); i++){
                                delete array[i];
                            }
                            delete [] array;
                        
                            (*citizenHash)->array = NULL;
                            delete *citizenHash;
                            *citizenHash = updateCitizenHash;
                        }

                        if((*virusHash)->insertToBloom(pr, virus) == 0){
                            cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                            delete  date1;
                            delete [] arrayOfStrings;
                            continue;
                        }

                        if((*virusHash)->insertToSkipList(pr, virus) == 0){
                            cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                            delete  date1;
                            delete [] arrayOfStrings;
                            continue;
                        }

                        delete [] arrayOfStrings;
                        continue;
                        
                    }else{

                        if(res == 1)
                            cout << "Sorry, date should be of type DD-MM-YYYY.\n";
                        if(res == 2)
                            cout << "Sorry, day number should be 1-30, month 1-12, year 1901 - 2021.\n";
                        delete [] arrayOfStrings;
                        continue;

                    }
                }else{
                    // if 7 params
                    if(vacc == "YES"){
                        // if it is yes, I would also expect a date, thus 8 params...
                        // error...
                        cout << "Sorry, not enough parametres.\n";
                        delete[] arrayOfStrings;
                        continue;
                    }

                    person* pr = new person(idInt, name, surname, ageInt,vacc, addressVirus, addressCountry);
                    (*citizenHash)->insertToHash(pr);

                    updateBuckets = (*citizenHash)->checkHash();
                    // check if it needs re-orientation...
                    if(updateBuckets > 0 ){
                        hashTable* updateCitizenHash;
                        updateCitizenHash = new hashTable(updateBuckets);
                        (*updateCitizenHash) = (**citizenHash); 
                        linkedListNode** array = (*citizenHash)->getArray();
                        for(int i = 0; i < (*citizenHash)->getNumBuckets(); i++){
                            delete array[i];
                        }
                        delete [] array;
                    
                        (*citizenHash)->array = NULL;
                        delete *citizenHash;
                        *citizenHash = updateCitizenHash;
                    }

                    if((*virusHash)->insertToSkipList(pr, virus) == 0){
                        cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                        delete pr;
                        delete [] arrayOfStrings;
                        continue;
                    }

                    pr->updateArrays(addressVirus, vacc, NULL);
                    delete [] arrayOfStrings;
                    continue;
                }
                
            }else{
                
                // SUCH ID ALREADY EXISTS...
                error = 0;
                int countryLength = country.length();
                for(int i = 0; i < countryLength; i++){
                    if(country[i] < 'A' || (country[i] < 'a' && country[i] > 'Z') || country[i] > 'z'){
                        cout << "Sorry, country must be a proper string.\n";
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }
                
                if(error) continue;

                if(pr->getName() == name && pr->getSurname() == surname && pr->getAge() == ageInt 
                    && *pr->getCountry() == country){
                    
                    // IF all info is same proceed...
                    
                    if(counter - 1 == 8){
                        // if 8 parametres are given
                        if(vacc == "NO"){
                            // and vacc == NO then (param #7) then the 8th param is
                            // obviously wrong...
                            cout<<"Sorry, did not expect anything after \"NO\" .\n";
                            delete[] arrayOfStrings;
                            continue;
                        }

                        //check date
                        if(checkDateInput(arrayOfStrings[8]) == 0){
                            // else if vacc == YES
                            // check if 8th param is a proper date...

                            date* date1 = dateConverter(arrayOfStrings[8]);
                            

                            // insert country to hash, if country already exists, 
                            // it returns the pointer to that country...
                            // else, it inserts it in hash table and returns address...
                            // string* addressCountry = (*countryHash)->insertToHash(country);
                            string* addressVirus= (*virusHash)->insertToHash(virus);

                            (*virusHash)->printData();

                            // also check if hash table needs re-size...
                            int updateBuckets = (*countryHash)->checkHash();

                            updateBuckets = (*virusHash)->checkHash();
                            hashTable* updateVirusHash;

                            if(updateBuckets > 0 ){
                                updateVirusHash = new hashTable(updateBuckets);
                                (*updateVirusHash) = (**virusHash); 
                                linkedListNode** array = (*virusHash)->getArray();
                                for(int i = 0; i < (*virusHash)->getNumBuckets(); i++){
                                    delete array[i];
                                }
                                delete [] array;
                                (*virusHash)->array = NULL;
                                delete *virusHash;
                                *virusHash = updateVirusHash;
                            }
                            
                            // check if for this citizen there exist such a statement for this virus...
                            if((*virusHash)->searchVaccSkipList(idInt, *citizenHash,virus) == 1){
                                cout << "Sorry, duplication error. This citizen is already vaccined.\n";
                                delete date1;
                                delete [] arrayOfStrings;
                                continue;
                            }
                            
                            if((*virusHash)->searchNotVaccSkipList(idInt, *citizenHash, virus) == 1){
                                cout << "Sorry, duplication error. Citizen is already not vaccinated for virus: "<<virus << endl;
                                cout << "In order to change its status, use /vaccineNow.\n";
                                delete date1;
                                delete [] arrayOfStrings;
                                continue;
                            }
                            // if not...
                            // insert it...
                            pr->updateArrays(addressVirus, vacc, date1);
                            if((*virusHash)->insertToBloom(pr, virus) == 0){

                                cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                                delete  date1;
                                delete [] arrayOfStrings;
                                continue;

                            }

                            if((*virusHash)->insertToSkipList(pr, virus) == 0){
                                cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                                delete  date1;
                                delete [] arrayOfStrings;
                                continue;
                            }            
                            

                            delete [] arrayOfStrings;
                            continue;      
                        }

                    }else{

                        // if 7 params are given, command and 7 more...
                        if(vacc == "YES"){
                            // if it is yes, I would also expect a date, thus 8 params...
                            // error...
                            cout << "Sorry, not enough parametres.\n";
                            delete[] arrayOfStrings;
                            continue;
                        }

                        // if else, proceed...
                        string* addressVirus= (*virusHash)->insertToHash(virus);
                        int updateBuckets = (*virusHash)->checkHash();
                        hashTable* updateVirusHash;

                        if(updateBuckets > 0 ){
                            updateVirusHash = new hashTable(updateBuckets);
                            (*updateVirusHash) = (**virusHash); 
                            linkedListNode** array = (*virusHash)->getArray();
                            for(int i = 0; i < (*virusHash)->getNumBuckets(); i++){
                                delete array[i];
                            }
                            delete [] array;
                            (*virusHash)->array = NULL;
                            delete *virusHash;
                            *virusHash = updateVirusHash;
                        }
                        
                        if((*virusHash)->searchNotVaccSkipList(idInt, *citizenHash, virus) == 0){
                            
                            if((*virusHash)->searchVaccSkipList(idInt, *citizenHash, virus) == 1){
                                cout << "Sorry, duplication error. Citizen is already vaccinated for virus: "<<virus << endl;
                                delete [] arrayOfStrings;
                                continue;
                            }

                            pr->updateArrays(addressVirus, vacc, NULL);
                            if((*virusHash)->insertToSkipList(pr, virus) == 0){
                                cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                                delete [] arrayOfStrings;
                                continue;
                            }

                           
                        }else{
                            cout << "Sorry, this citizen is already not vaccined for this virus.\n";
                            delete [] arrayOfStrings;
                            continue;
                        }
                    }
                }else{
                    
                    cout << "Error, citizen with this ID already exists but with different info\n";
                    pr->printData();
                    delete [] arrayOfStrings;
                    continue;

                }
            }

        }else if( arrayOfStrings[0] == opt6 ){
            
            //citizenID firstName lastName country age virusName 
            
            // check input...
            if(counter - 1 != 6){
                cout<<"Sorry, command \" "<<arrayOfStrings[0]<<" \" expects record: ID, Name, Surname, Country, Age, Virus.\n";          
                delete [] arrayOfStrings;
                continue;
            }

            // check if id has arithmetic value...
            string id = arrayOfStrings[1];

            int length = arrayOfStrings[1].length();
            int error = 0;

            for(int i = 0; i < length; i ++){
                if(id[i] < '0' || id[i] > '9'){
                    cout << "Sorry, Id must be integer.\n";
                    error = 1;
                    break;
                }
            }

            if(error){
                delete[] arrayOfStrings;
                continue;
            }

            // check if age is integer...
            string age = arrayOfStrings[5];

            int lengthAge = arrayOfStrings[5].length();
            error = 0;
            for(int i = 0; i < lengthAge; i++){
                if(age[i] < '0' || age[i] > '9'){
                    cout << "Sorry, Age must be integer.\n";
                    error = 1;
                    break;
                }
            }
            if(error){
                delete[] arrayOfStrings;
                continue;
            }

            
            if(lengthAge > 3){
                cout << "Sorry, max age is 120 years old\n";
                delete [] arrayOfStrings;
                continue;
            }
            // if age is integer... check if there exists such a citizen...
            
            int idInt = stoi(id);
            int ageInt = stoi(age);
            // check if this user already exists in citizen hash...
            person* pr = (*citizenHash)->search(idInt);
            string name = arrayOfStrings[2];
            string surname = arrayOfStrings[3];
            string country = firstCap(arrayOfStrings[4]);
            string virus = toUpper(arrayOfStrings[6]);

            if(pr == NULL){
                // person with that ID does not exist...
                // create instance of person
                // check virus, country
                error = 0;
                int countryLength = country.length();
                for(int i = 0; i < countryLength; i++){
                    if(country[i] < 'A' || (country[i] < 'a' && country[i] > 'Z') || country[i] > 'z' && country[i] != '-'){
                        cout << country[i] <<endl;
                        cout << "Sorry, country must be a proper string.\n";
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }
                if(error) continue;

                string* addressCountry = (*countryHash)->insertToHash(country);
                // also check if hash table needs re-size...

                int updateBuckets = (*countryHash)->checkHash();
                // check if it needs re-orientation...
                if(updateBuckets > 0 ){
                    hashTable* updateCountryHash;
                    updateCountryHash = new hashTable(updateBuckets);
                    (*updateCountryHash) = (**countryHash); 
                    linkedListNode** array = (*countryHash)->getArray();
                    for(int i = 0; i < (*countryHash)->getNumBuckets(); i++){
                        delete array[i];
                    }
                    delete [] array;
                    (*countryHash)->array = NULL;
                    delete *countryHash;
                    *countryHash = updateCountryHash;
                }
                                               
                string* addressVirus = (*virusHash)->insertToHash(virus);
                updateBuckets = (*virusHash)->checkHash();
                // check if it needs re-orientation...
                if(updateBuckets > 0 ){

                    hashTable* updateVirusHash;
                    updateVirusHash = new hashTable(updateBuckets);
                    (*updateVirusHash) = (**virusHash); 
                    linkedListNode** array = (*virusHash)->getArray();
                    for(int i = 0; i < (*virusHash)->getNumBuckets(); i++){
                        delete array[i];
                    }
                    delete [] array;
                    (*virusHash)->array = NULL;
                    delete *virusHash;
                    *virusHash = updateVirusHash;
                }

                // we have id, name, surname, age, country, virus, date
                time_t t = std::time(0);  
                tm* now = std::localtime(&t);

                int year = now->tm_year + 1900;
                int month = now->tm_mon + 1;
                int day = now->tm_mday;

                date* date1 = new date(day,month,year);
                
                // everything is right, we can now proceed to insertion...   
                person* pr = new person(idInt, name, surname, ageInt,"YES",addressVirus, addressCountry, date1);
                // now i need to insert to bloom filter and to skip list...

                (*citizenHash)->insertToHash(pr);

                if((*virusHash)->insertToBloom(pr, virus) == 0){
                    cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                    delete pr;
                    delete  date1;
                    delete [] arrayOfStrings;
                    continue;   
                }

                if((*virusHash)->insertToSkipList(pr, virus) == 0){
                    cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                    delete pr;
                    delete  date1;
                    delete [] arrayOfStrings;
                    continue;
                }

                delete[] arrayOfStrings;
                continue;

            }else{

                // SUCH ID ALREADY EXISTS...
                error = 0;
                int countryLength = country.length();
                for(int i = 0; i < countryLength; i++){
                    if(country[i] != '-' && country[i] < 'A' || (country[i] < 'a' && country[i] > 'Z') || country[i] > 'z'){
                        cout << "Sorry, country must be a proper string.\n";
                        delete[] arrayOfStrings;
                        error = 1;
                        break;
                    }
                }
                
                if(error) continue;

                if(pr->getName() == name && pr->getSurname() == surname && pr->getAge() == ageInt 
                    && *pr->getCountry() == country){

                    time_t t = std::time(0);  
                    tm* now = std::localtime(&t);

                    int year = now->tm_year + 1900;
                    int month = now->tm_mon + 1;
                    int day = now->tm_mday;
                    date* date1 = new date(day, month, year);

                    // insert country to hash, if country already exists, 
                    // it returns the pointer to that country...
                    // else, it inserts it in hash table and returns address...
                     // check if for this citizen there exist such a statement for this virus...
                    if((*virusHash)->searchVaccSkipList(idInt, *citizenHash, virus) == 1){

                        cout << "Sorry, duplication error. This citizen is already vaccined.\n";
                        delete [] arrayOfStrings;
                        delete date1;
                        continue;

                    }
                    
                    string* addressCountry = (*countryHash)->insertToHash(country);
                    string* addressVirus = (*virusHash)->insertToHash(virus);
                    
                    // also check if hash table needs re-size...
                    int updateBuckets = (*countryHash)->checkHash();
                    
                    updateBuckets = (*virusHash)->checkHash();
                    hashTable* updateVirusHash;
                   
                    if(updateBuckets > 0 ){

                        updateVirusHash = new hashTable(updateBuckets);
                        (*updateVirusHash) = (**virusHash); 
                        linkedListNode** array = (*virusHash)->getArray();
                        for(int i = 0; i < (*virusHash)->getNumBuckets(); i++){
                            delete array[i];
                        }
                        delete [] array;
                        (*virusHash)->array = NULL;
                        delete *virusHash;
                        *virusHash = updateVirusHash;

                    }

                    
                    //if not...
                    //insert it...
                    if((*virusHash)->searchNotVaccSkipList(idInt, *citizenHash, virus) == 1){
                        
                        if((*virusHash)->deleteNodeNotVaccSkipList(idInt, virus) == 0){
                            cout << "Sorry, problem while deleting skipList node.\n";
                            delete[] arrayOfStrings;
                            delete date1;
                            continue;
                        }

                    }

                    pr->updateArrays(addressVirus, "YES", date1);

                    if((*virusHash)->insertToBloom(pr, virus) == 0){

                        cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                        delete  date1;
                        delete [] arrayOfStrings;
                        continue;

                    }

                    if((*virusHash)->insertToSkipList(pr, virus) == 0){
                        cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                        delete  date1;
                        delete [] arrayOfStrings;
                        continue;
                    }

                    delete[] arrayOfStrings;
                    continue;

                }else{
                    
                    cout << "Error, citizen with this ID already exists but with different info\n";
                    pr->printData();
                    delete [] arrayOfStrings;
                    continue;

                }

            }

        }else if( arrayOfStrings[0] == opt7 ){
            
            if(counter - 1 > 1 || counter - 1 < 1){
                cout << "Sorry, this command needs a virus name.\n";
                delete [] arrayOfStrings;
                continue;
            }

            string virus = arrayOfStrings[1];

            if((*virusHash)->search(virus) == 1){
                
                // found virus...
                (*virusHash)->printNotVaccinatedSkipList(virus);

            }else{

                cout << "Sorry, this virus does not exist.\n";
                delete[] arrayOfStrings;
                continue;

            }

        }else if(arrayOfStrings[0] == opt8){

            // exit...
            delete *citizenHash;
            
            delete *countryHash;
            
            delete *virusHash;

            *countryHash = NULL;
            *virusHash = NULL;   
             

            delete[] arrayOfStrings;
            free(personArray);
            cout << "Thank you.\n";
            return 0;

        }else{

            cout <<"\nSorry, did not get that one, please check your commands and try again.\n";
            cout <<"The commands are:\n";
            cout << opt1 << endl;
            cout << opt2 << endl;
            cout << opt3 << endl;
            cout << opt4 << endl;
            cout << opt5 << endl;
            cout << opt6 << endl;
            cout << opt7 << endl;
            cout << opt8 << endl;
            
            if(arrayOfStrings != NULL){
                delete [] arrayOfStrings;
                continue;
            }
            continue;
        }

        if(arrayOfStrings != NULL){
            delete [] arrayOfStrings;
        }
        continue;
    }
}

string toUpper(string str){
    int length = str.length();
    for(int i = 0; i < length; i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] = str[i] - 32;
        }
    }
    cout << str <<endl;
    return str;
}

string firstCap(string str){
    int length = str.length();
    int flag = 0;
    for(int i = 0; i < length; i++){

        if(i == 0 || flag == 1){
            if(str[i] >= 'a' && str[i] <= 'z'){
                str[i] = str[i] - 32;
            }
        }

        if(str[i] == '-') flag = 1;
        if(i != 0 && str[i] >= 'A' && str[i] <= 'Z'){
            if(flag != 1){
                str[i] = str[i] + 32;
            }else{
                flag = 0;
            }
        }

    }
    return str;
}

date* dateConverter(string sdate){

    string myText(sdate);
    istringstream iss(myText);
    string token;
            
    getline(iss, token, '-');
    int day = stoi(token);
            
    getline(iss, token, '-');
    int month = stoi(token);
            
    getline(iss, token, '-');
    int year = stoi(token);

    date* convDate = new date(day,month,year);
    return convDate;
}

string* readInput(int* counter){

    string wholeCommand, command;
    cout << "\nWaiting for command...\n\n";
    getline(cin, wholeCommand);
    string temp(wholeCommand);
    istringstream iss(temp);    
    string temp2;
    string* arrayOfStrings = NULL;
    string* final;

    while(iss >> temp2){    
        ++(*counter);
        if((*counter)>1){
            final = new string[*counter];
            int i;
            for(i = 0; i < *counter - 1; i++){
                final[i] = arrayOfStrings[i];
            }
            final[i] = temp2;
            delete [] arrayOfStrings;
            arrayOfStrings = NULL;
            arrayOfStrings = final;

        }else if(*counter == 1){
            arrayOfStrings = new string[1];
            arrayOfStrings[0] = temp2;
        }
    }
    return arrayOfStrings;

}


int checkDateInput(string dateGiven){

    string param = dateGiven;
    int length = param.length();
    int counterDash = 0;
    int errorFlag = 0;
                    
    for(int i = 0; i< length ; i++){
                        
        if((i == 0 || i == length - 1 ) && param[i] == '-'){
            return 1;
        }

        if(param[i] == '-') counterDash++;

        if((param[i] < '0' || param[i] > '9') && param[i] != '-'){        
           return 1;
        }
    }

    if(counterDash != 2){
        return 1;   
    }
    
    date* date1 = dateConverter(dateGiven);

    if(date1->checkDate() == 1){ 
        delete date1;
        return 2;
    }
    delete date1;
    return 0;
}

string** updateArray(string** virusArray, string* newVirus, int* length){
    string** temp;
    if(*length == 0){
        temp = new string*[1];
        temp[0] = newVirus; 
        (*length)++;
    }else{
        int i = 0;
        int exist = 0;

        for(i = 0; i < *length; i++){
            if(*newVirus == *virusArray[i]){
                // this virus already exists...
                exist = 1;
                break;
            }
        }

        if(exist == 1){
            return virusArray;
        }

        temp = new string*[*length+1];
        
        for(i = 0; i < *length; i++){
            
            temp[i] = (virusArray[i]);
        }

        temp[i] = newVirus;
        delete[] virusArray;
        (*length)++;
    }
    return temp;
}