#include "helpers.h"

// ARGC[1] == READ...
// ARGC[2] == WRITE...
pthread_mutex_t mtxCyclicBuffer;
pthread_mutex_t mtxModify;
pthread_mutex_t mtxCond;
pthread_cond_t cond;
pthread_cond_t signEmpty;
int receiveString(int readfd, char** ptr);
int sendString(int bufferSize, const char* str, int writefd);

typedef struct argsForThreads{
    person** personsArray;
    hashTable* virusHash;
    hashTable* countryHash;
    hashTable* citizenHash;
    int numLines;
    int legalLines;
    cyclicBuffer* cycBuffer;
    string*** arrayOfVirusToSend;
    int virusArrayLength;
    int createArrayOfViruses;
};

void* test(void* args){
    
    argsForThreads* ptr = (argsForThreads*) args;
    ifstream myfile;
    
    int id, age;
    string name, surname, country, virus, vaccinated, sdate, line;
    date* dateOfVaccination;
    hashTable* virusHash = ptr->virusHash;
    hashTable* countryHash = ptr->countryHash;
    person** personsArray = ptr->personsArray;
    char* temp;
    
    while(1){ 
        pthread_mutex_lock(&mtxCyclicBuffer);
        int res = ptr->cycBuffer->readFromCyclicBuffer(&temp);
        if(res == -1) {
            // empty
            pthread_cond_signal(&signEmpty);
            pthread_cond_wait(&cond, &mtxCyclicBuffer);
            pthread_mutex_unlock(&mtxCyclicBuffer);
            continue;
        }
        if(strcmp(temp,"/exit") == 0){
            free(temp);
            pthread_mutex_unlock(&mtxCyclicBuffer);
            break;
        }
        string subDir = temp;
        pthread_mutex_unlock(&mtxCyclicBuffer);
        //pthread_exit(NULL);
        
        myfile.open(subDir);
        if(!myfile.is_open()){
            perror("Problem when opening file ");
            exit(1);
        }

        if(myfile.peek() == EOF){
            pthread_exit(NULL);
        }

        person* citizen;
        myfile >> id >> name >> surname >> age >> country >> virus >> vaccinated;
        pthread_mutex_lock(&mtxModify);
        person* pr = ptr->citizenHash->search(id);
        pthread_mutex_unlock(&mtxModify);

        if(vaccinated.compare("NO") == 0){
            if(pr == NULL){
                // new citizen...
                pthread_mutex_lock(&mtxModify);
                citizen = new person(id, name, surname, age, vaccinated);
                string* countryPtr = (string*)countryHash->insertToHash(country);
                string* virusPtr = (string*)virusHash->insertToHash(virus);
                citizen->initCountry(countryPtr);
                citizen->initVirus(virusPtr);
                ptr->citizenHash->insertToHash(citizen);
                virusHash->insertToSkipList(citizen, virus);
                if(ptr->createArrayOfViruses == 1){
                    *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, virusPtr, &(ptr->virusArrayLength));
                }

                pthread_mutex_unlock(&mtxModify);
                
            }else{
                // this citizen already exists
                if( pr->getName() == name 
                    && pr->getSurname() == surname 
                    && pr->getAge() == age 
                    && *pr->getCountry() == country)
                {
                    pthread_mutex_lock(&mtxModify);
                    string* addressVirus = virusHash->insertToHash(virus);
                    if(ptr->createArrayOfViruses == 1)
                        *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, addressVirus, &ptr->virusArrayLength);
                    if(virusHash->searchNotVaccSkipList(id, ptr->citizenHash, virus) == 0){
                        if(virusHash->searchVaccSkipList(id, ptr->citizenHash, virus) == 1){
                            cout << "Sorry, duplication error. Citizen is already vaccinated for virus: "<<virus << endl;
                            //delete [] arrayOfStrings;
                            continue;
                        }
                        
                        pr->updateArrays(addressVirus, vaccinated, NULL);
                        if(virusHash->insertToSkipList(pr, virus) == 0){
                            cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                            //delete [] arrayOfStrings;
                            //continue;
                        }

                        cout << "CITIZEN NOT VACCINATED\n";
                    }else{
                        cout << "Sorry, this citizen is already not vaccined for this virus.\n";
                        //delete [] arrayOfStrings;
                        continue;
                    }
                    pthread_mutex_unlock(&mtxModify);
                }else{
                    cout << "Sorry, this ID belongs to a citizen with different info\n";
                    pr->printData();
                    delete citizen;
                }
            }
        }else{

            myfile >> sdate;
            string myText(sdate);
            istringstream iss(myText);
            string token;
            getline(iss, token, '-');
            int day = stoi(token);
            
            getline(iss, token, '-');
            int month = stoi(token);
            
            getline(iss, token, '-');
            int year = stoi(token);

            dateOfVaccination = new date(day,month,year);

            if(pr == NULL){
                // does not exist already...
                pthread_mutex_lock(&mtxModify);
                citizen = new person(id, name, surname, age, vaccinated, NULL, NULL,dateOfVaccination);
                string* countryPtr = (string*)countryHash->insertToHash(country);
                string* virusPtr = (string*)virusHash->insertToHash(virus);
                citizen->initCountry(countryPtr);
                citizen->initVirus(virusPtr);
                ptr->citizenHash->insertToHash(citizen);
                //virusHash->insertToHash(virus);
                virusHash->insertToSkipList(citizen, virus);
                virusHash->insertToBloom(citizen, virus);
                if(ptr->createArrayOfViruses == 1)
                    *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, virusPtr, &ptr->virusArrayLength);
                pthread_mutex_unlock(&mtxModify);
                //sendString(socketBufferSize, virus.c_str(), writefd);
                // wait for ack...

            }else{
                // citizen exists already...
                if( pr->getName() == name 
                    && pr->getSurname() == surname 
                    && pr->getAge() == age 
                    && *pr->getCountry() == country)
                {
                    pthread_mutex_lock(&mtxModify);
                    
                    if(virusHash->searchVaccSkipList(id, ptr->citizenHash,virus) == 1){
                        cout << "Sorry, duplication error. This citizen is already vaccined.\n";
                        delete dateOfVaccination;
                        //delete [] arrayOfStrings;
                        continue;
                    }
                    
                    if(virusHash->searchNotVaccSkipList(id,ptr->citizenHash, virus) == 1){
                        cout << "Sorry, duplication error. Citizen is already not vaccinated for virus: "<<virus << endl;
                        //cout << "In order to change its status, use /vaccineNow.\n";
                        delete dateOfVaccination;
                        //delete [] arrayOfStrings;
                        continue;
                    }
                    // if not...
                    // insert it...
                    string* addressVirus = virusHash->insertToHash(virus);
                    if(ptr->createArrayOfViruses == 1)
                        *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, addressVirus, &ptr->virusArrayLength);
                    pr->updateArrays(addressVirus, vaccinated, dateOfVaccination);
                    cout << "CITIZEN VACCINATED\n";
                    if(virusHash->insertToBloom(citizen, virus) == 0){

                        cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                        //delete  date1;
                        delete dateOfVaccination;
                        continue;
                    }

                    cout << virus << endl;
                    if(virusHash->insertToSkipList(citizen, virus) == 0){
                        cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                        //delete  date1;
                        delete dateOfVaccination;
                        continue;
                    }
                    pthread_mutex_unlock(&mtxModify);

                }else{
                    cout << "Sorry, this ID belongs to a citizen with different info\n";
                    pr->printData();
                    delete citizen;
                }

            }

        }
        
        while(getline(myfile, line)){ 
            myfile >> id >> name >> surname >> age >> country >> virus >> vaccinated;
            person* pr = ptr->citizenHash->search(id);
            
            if(vaccinated.compare("NO") == 0){
                if(pr == NULL){
                // new citizen...
                    pthread_mutex_lock(&mtxModify);
                    citizen = new person(id, name, surname, age, vaccinated);
                    string* countryPtr = (string*)countryHash->insertToHash(country);
                    string* virusPtr = (string*)virusHash->insertToHash(virus);
                    citizen->initCountry(countryPtr);
                    citizen->initVirus(virusPtr);
                    ptr->citizenHash->insertToHash(citizen);
                    virusHash->insertToHash(virus);
                    virusHash->insertToSkipList(citizen, virus);
                    if(ptr->createArrayOfViruses == 1)
                        *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, virusPtr, &ptr->virusArrayLength);
                    pthread_mutex_unlock(&mtxModify);
                }else{
                    // this citizen already exists
                    if( pr->getName() == name 
                        && pr->getSurname() == surname 
                        && pr->getAge() == age 
                        && *pr->getCountry() == country)
                    {
                        pthread_mutex_lock(&mtxModify);
                        string* addressVirus = virusHash->insertToHash(virus);
                        if(ptr->createArrayOfViruses == 1)
                            *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, addressVirus, &ptr->virusArrayLength);
                        if(virusHash->searchNotVaccSkipList(id, ptr->citizenHash, virus) == 0){
                            if(virusHash->searchVaccSkipList(id, ptr->citizenHash, virus) == 1){
                                cout << "Sorry, duplication error. Citizen is already vaccinated for virus: "<<virus << endl;
                                //delete [] arrayOfStrings;
                                //continue;
                            }
                            pr->updateArrays(addressVirus, vaccinated, NULL);
                            if(virusHash->insertToSkipList(pr, virus) == 0){
                                cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                                //delete [] arrayOfStrings;
                                continue;
                            }
                        }else{
                            cout << "Sorry, this citizen is already not vaccined for this virus.\n";
                            //delete [] arrayOfStrings;
                            continue;
                        }
                        pthread_mutex_unlock(&mtxModify);
                    }else{
                        cout << "Sorry, this ID belongs to a citizen with different info\n";
                        pr->printData();
                        delete citizen;
                    }
                }

            }else{

                myfile >> sdate;
                string myText(sdate);
                istringstream iss(myText);
                string token;

                getline(iss, token, '-');
                int day = stoi(token);
                
                getline(iss, token, '-');
                int month = stoi(token);
                
                getline(iss, token, '-');
                int year = stoi(token);

                dateOfVaccination = new date(day,month,year);
                //citizen = new person(id, name, surname, age, vaccinated, NULL, NULL,dateOfVaccination);
                
                if(pr == NULL){
                    // does not exist already...
                    pthread_mutex_lock(&mtxModify);
                    citizen = new person(id, name, surname, age, vaccinated, NULL, NULL,dateOfVaccination);
                    string* countryPtr = (string*)countryHash->insertToHash(country);
                    string* virusPtr = (string*)virusHash->insertToHash(virus);
                    citizen->initCountry(countryPtr);
                    citizen->initVirus(virusPtr);
                    ptr->citizenHash->insertToHash(citizen);
                    virusHash->insertToHash(virus);
                    virusHash->insertToSkipList(citizen, virus);
                    virusHash->insertToBloom(citizen, virus);
                    if(ptr->createArrayOfViruses == 1)
                        *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, virusPtr, &ptr->virusArrayLength);
                    pthread_mutex_unlock(&mtxModify);
                    
                }else{
                    // citizen exists already...
                    if( pr->getName() == name 
                        && pr->getSurname() == surname 
                        && pr->getAge() == age 
                        && *pr->getCountry() == country)
                    {
                        pthread_mutex_lock(&mtxModify);

                        if(virusHash->searchVaccSkipList(id, ptr->citizenHash,virus) == 1){
                            cout << "Sorry, duplication error. This citizen is already vaccined.\n";
                            delete dateOfVaccination;
                            continue;
                            //delete [] arrayOfStrings;
                            //continue;
                        }
                        
                        if(virusHash->searchNotVaccSkipList(id, ptr->citizenHash, virus) == 1){
                            cout << "Sorry, duplication error. Citizen is already not vaccinated for virus: "<<virus << endl;
                            //cout << "In order to change its status, use /vaccineNow.\n";
                            delete dateOfVaccination;
                            continue;
                            //delete [] arrayOfStrings;
                            //continue;
                        }
                        // if not...
                        // insert it...
                        string* addressVirus = virusHash->insertToHash(virus);
                        if(ptr->createArrayOfViruses == 1)
                            *ptr->arrayOfVirusToSend = updateArray(*ptr->arrayOfVirusToSend, addressVirus, &ptr->virusArrayLength);
                        
                        pr->updateArrays(addressVirus, vaccinated, dateOfVaccination);

                        if(virusHash->insertToBloom(citizen, virus) == 0){

                            cout << "Sorry, something went wrong when trying to insert person to Bloom Filter.\n";
                            //delete  date1;
                            delete dateOfVaccination;
                            continue;
                        }

                        if(virusHash->insertToSkipList(pr, virus) == 0){
                            cout << "Sorry, something went wrong when trying to insert person to Skip List.\n";
                            //delete  date1;
                            delete dateOfVaccination;
                            continue;
                        }
                        pthread_mutex_unlock(&mtxModify);

                    }else{
                        cout << "Sorry, this ID belongs to a citizen with different info\n";
                        pr->printData();
                        delete citizen;
                    }
                }
            }
            pthread_mutex_lock(&mtxModify);
            ptr->numLines++;
            pthread_mutex_unlock(&mtxModify);
        }
        free(temp);
        myfile.close();
    }
    pthread_exit(0);
}

int sendBloom(int writefd, uint8_t* bloom, int bloomSize, int buffSize);

int main(int argv , char* argc[]){

    //char* pipeRead = argc[1];
    //char* pipeWrite = argc[2];
    
    int readfd, writefd;
    int bloomSize = atoi(argc[10]);
    int socketBufferSize = atoi(argc[6]);
    int cyclicBufferSize = atoi(argc[8]);
    int numThreads = atoi(argc[4]);
    int numOfDirs=0;
    pthread_t tarray[numThreads];

    string* arrayOfSubDirs = NULL; // to keep track of all the sud dirs, array of strings
    int red;
    char *tempBuff;
    char *totalBuff;
    char* dirName;
    int initialParams = 11;

    numOfDirs = argv - initialParams; 
                        // 11 is the number of initial args 
                        // -p port (2)
                        // -t numThreads (2) 
                        // -b socketBufferSize (2) 
                        // -c cyclicBufferSize  (2)
                        // -s sizeOfBloom, (2)
                        // NULL (1), total 11. 
                        // so the number of paths is argv - 11

    int port = atoi(argc[2]), sock, newsock;
    unsigned int serverlen, clientlen;

    char host[HOST_NAME_MAX];
    char* IP;
    struct hostent *rem;
            
    if(gethostname(host,HOST_NAME_MAX) == -1){
        perror("GetHostName");
        exit(1);
    }

    if ((rem = gethostbyname(host)) == NULL) {	
        perror("gethostbyname");
        exit(1);
    }
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct sockaddr *clientptr = (struct sockaddr*) &client;

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Create socket ");
        exit(1);
    }


    server.sin_family = AF_INET;       /* Internet domain */
    IP = inet_ntoa(*((struct in_addr*) rem->h_addr_list[0]));
    if(inet_pton(AF_INET,IP, &server.sin_addr) != 1){
        perror("Inet_Pton ");
        exit(1);
    }

    IP = inet_ntoa(server.sin_addr);

    server.sin_port = htons(port);
    serverlen = sizeof(server);
    if (bind(sock, serverptr, serverlen) < 0){
        perror("bind");
        exit(1);
    }

    if (listen(sock, 5) < 0){
        perror("Listen ");
        exit(1);
    }

    printf("Listening for connections to port %d\n", port);
    clientlen = sizeof(client);
    /* accept connection */
    if ((newsock = accept(sock, clientptr, &clientlen)) < 0){
        perror("Accept ");
        exit(1);
    }

    writefd = newsock;
    readfd = newsock;
    /* Find client's address */
    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
        herror("gethostbyaddr"); 
        exit(1);
    }
    printf("Accepted connection from %s\n", rem->h_name);
    printf("Accepted connection\n");

    //close(newsock);
    //exit(1);

    sendString(socketBufferSize, "Hello World", newsock);
    //exit(1);

    char** arrayOfCharSubDirs = (char**) malloc(sizeof(char**) * numOfDirs);
    arrayOfSubDirs = new string[numOfDirs];

    for(int i = 0; i < numOfDirs; i++){
        arrayOfCharSubDirs[i] = argc[initialParams + i];
    }

    int buffSize = 0;
    int numBytes = 0;
    int numBytesDir = 0;
    int numDirsPerCountry = 0;

    string line;
    struct dirent **fileListTemp;
    int noOfFiles;

    if((noOfFiles = scandir(arrayOfCharSubDirs[0], &fileListTemp, NULL, alphasort)) < 0){
        perror("problem when scandir");
        exit(1);
    }

    int byteC;
    for(int c = 0; c < strlen(arrayOfCharSubDirs[0]); c++){
        if(arrayOfCharSubDirs[0][c] == '/'){
            byteC = c;
            break;
        }
    }

    for(int i = 0; i < noOfFiles; i++){
        free(fileListTemp[i]);
    }
    free(fileListTemp);

    for(int i= 0; i < numOfDirs; i++){
        arrayOfSubDirs[i] = &arrayOfCharSubDirs[i][byteC+1];
    }
    numDirsPerCountry = noOfFiles-2; // . and ..
    int* arrayOfDirsPerCountry = new int[numOfDirs];

    for(int i = 0; i<numOfDirs; i++){
        arrayOfDirsPerCountry[i] = numDirsPerCountry;
    }
    
    // we have to read the respective subdirs...
    int id;
    int age;

    string name;
    string surname;
    string country;
    string virus;
    string vaccinated;
    string sdate;
    date* dateOfVaccination;

    string dirNameString;
    string** virusArrayToSend = NULL;

    hashTable* countryHash = new hashTable(COUNTRYARRAY);
    hashTable* virusHash = new hashTable(VIRUSARRAY);
    hashTable* citizenHash;

    
    //int legalLines;
    
    person** personsArray;
    //person*** arrayOfPersonsArray = (person***) malloc(sizeof(person***)*numOfDirs); 

    int numLines = 0;
    int legalLines = 0;
    ifstream myfile;

    string subDir;
    string *arrayOfStringSubDirs = new string[numDirsPerCountry*numOfDirs];
    char* arrayOfCharDirs = new char[numDirsPerCountry*numOfDirs];
    int counterOfSubDirs = 0;

    for(int num = 0; num < numOfDirs; num++){
        for(int dirsPerCountryCounter = 0; dirsPerCountryCounter < numDirsPerCountry; dirsPerCountryCounter++){
            subDir = (string)arrayOfCharSubDirs[num];
            string numToString = to_string(dirsPerCountryCounter+1);
            subDir = subDir +"/"+ (string)arrayOfSubDirs[num]+"-"+numToString+".txt";
            arrayOfStringSubDirs[counterOfSubDirs] = subDir;
            
            counterOfSubDirs++;
            
            myfile.open(subDir);
            //cout << subDir << endl;
            if(!myfile.is_open()){
                perror("Problem when opening file");
                exit(1);
            }
            
            while(getline(myfile, line)){
                if(myfile.eof()) break;
                numLines++;
            }

            numLines++;
            myfile.close();
        }
    }
    citizenHash = new hashTable(numLines);

    pthread_mutex_init(&mtxCyclicBuffer, NULL);
    pthread_mutex_init(&mtxModify, NULL);
    pthread_mutex_init(&mtxCond, NULL);
    pthread_cond_init(&cond,NULL);
    pthread_cond_init(&signEmpty, NULL);

    pthread_mutex_lock(&mtxCond);

    cyclicBuffer* cycBuffer = new cyclicBuffer(cyclicBufferSize);

    personsArray = (person**) malloc(numLines * sizeof(person**));

    for(int i = 0; i < numLines; i++){
        personsArray[i] = NULL;
    }

    argsForThreads* struc = new argsForThreads;
    struc->countryHash = countryHash;
    struc->virusHash = virusHash;
    struc->numLines = numLines;
    struc->legalLines = legalLines;
    struc->citizenHash = citizenHash;
    pthread_t tArray[numThreads];
    struc->cycBuffer = cycBuffer;
    struc->personsArray = personsArray;
    struc->arrayOfVirusToSend = &virusArrayToSend;
    struc->virusArrayLength = 0;
    struc->createArrayOfViruses = 0;
    int br = 0;
    int numNum = 0;
    int numDirsPerCountryCounter = 0;
    
    for(int num = 0; num < numOfDirs; num++){
        for(int dirsPerCountryCounter = 0; dirsPerCountryCounter < numDirsPerCountry; dirsPerCountryCounter++){
            subDir = arrayOfStringSubDirs[numDirsPerCountry*num + dirsPerCountryCounter];
            if(cycBuffer->insertToCyclicBuffer((char*)subDir.c_str()) == 0){
                // buffer is full...
                if(br == 0){
                    for(int i = 0; i < numThreads; i++){
                        pthread_create(&tArray[i], NULL, test, struc);
                    }
                }

                pthread_cond_broadcast(&cond);
                br = 1;
                //for(int i = 0; i < numThreads; i++)
                    pthread_cond_wait(&signEmpty, &mtxCond);
                cycBuffer->insertToCyclicBuffer((char*)subDir.c_str());
            }
        }
    }
    pthread_cond_wait(&signEmpty, &mtxCond);

    legalLines = struc->legalLines;
    numLines = struc->numLines;

    if(legalLines < numLines){
        person** updatedPerson = (person**)(malloc((legalLines)*sizeof(person*)));
        for(int i = 0; i < legalLines; i++){
            updatedPerson[i] = personsArray[i];
        }

        free(personsArray);
        personsArray = NULL;
        personsArray = updatedPerson;
        struc->personsArray = personsArray;
    }

    //citizenHash = new hashTable(legalLines);   
    
    // int updateBuckets = countryHash->checkHash();
    // hashTable* updateCountryHash = NULL;

    // updateBuckets = virusHash->checkHash();
    // hashTable* updateVirusHash = NULL;

    int i = 0;

    // for(int k = 0; k < legalLines; k++){
    //     if(personsArray[k] != NULL){
    //         if(citizenHash->insertToHash(personsArray[k]) == 0){ 
    //             delete personsArray[k];
    //             personsArray[k] = NULL;
    //         }
    //     }
    // }
    
    //cout << "\n****************************************************\n";

    //citizenHash->initSkipListBloomFilter(bloomSize,virusHash);

    int numOfBloomFilters;

    linkedListNode** virusHashArray = struc->virusHash->getArray();

    int counter = 0;
    
    for(int virusCounter = 0; virusCounter < virusHash->getNumBuckets(); virusCounter++){
        // check the whole array...
        linkedListNode* temp = virusHashArray[virusCounter];
        while(temp!=NULL){
            string* str = (string*)temp->getThing();
            counter++;
            temp = temp->next;
        }
    }

    // now I should return the bloom filters...
    // i do that using sockets...
    // I have the port so i just need to listen to it for connections...
    // and also send the bloom filters...

    numOfBloomFilters = counter;    
    
    if((red = write(writefd,&numOfBloomFilters,sizeof(int))) == -1){
        perror("Problem when writing to fifo monitor: ");
        exit(1);
    }
    
    // send bloom filters to parent...
    char** arrayOfViruses = (char**)malloc(sizeof(char**)*numOfBloomFilters);
    int counterVirus = 0;
    bloomFilter** arrayOfBloomFilters = new bloomFilter*[numOfBloomFilters];

    // create array with viruses to send...
    for(int virusCounter = 0; virusCounter < virusHash->getNumBuckets(); virusCounter++){
        // check the whole array...
        linkedListNode* temp = virusHashArray[virusCounter];
        while(temp!=NULL){
            string* str = (string*)temp->getThing();
            int len = (*str).length() + 1;
            arrayOfViruses[counterVirus] = (char*)malloc(sizeof(char*)*(len));
            strcpy(arrayOfViruses[counterVirus], (char*)str->c_str());
            arrayOfBloomFilters[counterVirus] = virusHash->getBFOfVirus(*str);
            counterVirus++;
            temp = temp->next;
        }
    }

    for(int cVirus = 0; cVirus < counterVirus; cVirus++){

        int counterBytes = 0;
        int bytesToWrite = 0;
        char* tempChar;
        int len = strlen(arrayOfViruses[cVirus])+1;
        int ready;
        int OK = 0;
        char* ack;
        sendString(socketBufferSize, arrayOfViruses[cVirus], writefd);
        receiveString(writefd, &ack);
        free(ack);
    
        // send bloom filter of virus...
        uint8_t* tempArray = arrayOfBloomFilters[cVirus]->getArray();
        sendBloom(writefd, tempArray, bloomSize, socketBufferSize);
        receiveString(writefd, &ack);
        free(ack);
    }
    //done sending the blooms...

    while(1){

        char* command;

        receiveString(readfd, &command);
        cout << "\n";
        if(strcmp(command,"/travelRequest") == 0){
            // receive virus...
            char* virus; 
            
            receiveString(readfd, &virus);
            // I should receive an id...
            if(read(readfd, &id, sizeof(int)) == -1){
                perror("Problem when reading ");
                exit(1);
            }

            // parent should ask us we we have a specific user...
            // I should check weather I have this user in my system....

            int vacced = virusHash->searchVaccSkipList(id, citizenHash, virus);
            
            if(vacced == 1){

                char* answer = "YES";
                // should send "YES" and Date ...
                
                if(sendString(socketBufferSize, answer, writefd) != 1){
                    cout << "Problem when sending the string\n";
                }

                //sleep(1);
                // should retrieve the date...
                date* dateGiven = citizenHash->getDateOfVaccination(id, virus);
                int day = dateGiven->getDay();
                int month = dateGiven->getMonth();
                int year = dateGiven->getYear();

                if(write(writefd, &day, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }

                if(write(writefd, &month, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }

                if(write(writefd, &year, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                
            }else if(vacced == 0){
                // should send NO ...
                char* answer = "NO";
                if(sendString(socketBufferSize, answer, writefd) != 1){
                    cout << "Problem when sending the string\n";
                }
            }
            free(command);
        }else if(strcmp(command,"/searchVaccinationStatus") == 0){
            // I will get an ID...

            int id;
            if(read(readfd, &id, sizeof(int)) == -1){
                perror("Problem when reading ");
                exit(1);
            }

            // check if you have him...
            person* pr = citizenHash->search(id);
            if(pr != NULL){
                char* answer = "YES";
                sendString(socketBufferSize, answer, writefd);
                char* ack;
                receiveString(readfd, &ack);
                free(ack);
                // should retrieve all all information...
                // lets send identity information

                string name = pr->getName();
                string surname = pr->getSurname();
                string* country = pr->getCountry();
                int age = pr->getAge();

                sendString(socketBufferSize,name.c_str(),writefd);
                receiveString(readfd, &ack);
                free(ack);

                sendString(socketBufferSize,surname.c_str(),writefd);
                receiveString(readfd, &ack);
                free(ack);

                sendString(socketBufferSize,country->c_str(),writefd);
                receiveString(readfd, &ack);
                free(ack);

                if(write(writefd, &age, sizeof(int)) == -1){
                    perror("Problem when reading ");
                    exit(1);
                }

                string** citizenViruses = pr->getArrayOfViruses();
                
                for(int i = 0; i < pr->getNumViruses(); i++){
                    // send named of virus and either a date of vaccination
                    // or a NO -> to indicate lack of vaccination...
                    sendString(socketBufferSize, citizenViruses[i]->c_str(), writefd);
                    receiveString(readfd, &ack);
                    free(ack);
                    // check if he is vaccinated... 
                    // if yes... send "YES" and the date...
                    if(virusHash->searchVaccSkipList(id, citizenHash, *(citizenViruses[i])) == 1){
                        sendString(socketBufferSize, "YES", writefd);
                        receiveString(readfd, &ack);
                        free(ack);

                        date* d = citizenHash->getDateOfVaccination(id, *(citizenViruses[i]));
                        int day = d->getDay();
                        int month = d->getMonth();
                        int year = d->getYear();

                        if(write(writefd, &day, sizeof(int)) == -1){
                            perror("Problem when reading ");
                            exit(1);
                        }

                        if(write(writefd, &month, sizeof(int)) == -1){
                            perror("Problem when reading ");
                            exit(1);
                        }

                        if(write(writefd, &year, sizeof(int)) == -1){
                            perror("Problem when reading ");
                            exit(1);
                        }

                        receiveString(readfd, &ack);
                        free(ack);

                    }else{
                        sendString(socketBufferSize, "NO", writefd);
                        receiveString(readfd, &ack);
                        free(ack);
                    }
                    // if no... send "NO"...
                }

                sendString(socketBufferSize,"DONE", writefd);
                receiveString(readfd, &ack);
                free(ack);
            }else{
                char* answer = "NO";
                sendString(socketBufferSize, answer, writefd);
                free(command);
                continue;
            }
            free(command);
        }else if(strcmp(command,"/addVaccinationRecords") == 0){

            // as I don't have the name of the country
            // to which new data were inserted 
            // I need check all the countries and find
            // the one that got updated... 
            // I do that
            struc->createArrayOfViruses = 1;
            char* country;
            receiveString(readfd, &country);
            struct dirent **fileListTemp;
            int noOfFiles;
            string newDir;
            string countryUpdated;
            int oldNumberOfDirs = 0; // so to know which files are already checked...
            int found = -1;
            int countryNum = -1;
            char* temp = "";

            for(int i = 0; i<numOfDirs; i++){
                for(int k = 0; k < strlen(arrayOfCharSubDirs[i]); k++){
                    if(arrayOfCharSubDirs[i][k]=='/'){
                        temp = &arrayOfCharSubDirs[i][k+1];
                        if(strcmp(temp, country) == 0){
                            //the country I am looking for...
                            noOfFiles = scandir(arrayOfCharSubDirs[i], &fileListTemp, NULL, alphasort);
                            if(noOfFiles == -1){
                                perror("Problem when scandir ");
                                exit(1);
                            }
                            noOfFiles -= 2; //. , ..
                            //for(int k = 2; k < noOfFiles; k++){
                            
                            if(noOfFiles > arrayOfDirsPerCountry[i]){
                                countryUpdated = newDir;
                                countryNum = i;
                                oldNumberOfDirs = arrayOfDirsPerCountry[i];
                                arrayOfDirsPerCountry[i] = noOfFiles;
                                found = 1;   
                                break;
                            }   
                        }
                    }
                }
            }
             
            int virusArrayLength = 0;

            if(found != -1){
                sendString(socketBufferSize, "YES", writefd);
                newDir = arrayOfCharSubDirs[countryNum];

                for(int i = oldNumberOfDirs; i < arrayOfDirsPerCountry[countryNum]; i++){
                    subDir = fileListTemp[i+2]->d_name;
                    string tempDir = newDir+"/"+subDir;
                    cycBuffer->insertToCyclicBuffer((char*)tempDir.c_str());
                }

                // send the viruses name and the bloom filters...
                pthread_cond_broadcast(&cond);
                for(int i = 0; i < numThreads; i++)
                    pthread_cond_wait(&signEmpty, &mtxCond);

                char* ack;
                virusArrayLength = struc->virusArrayLength;
                virusArrayToSend = *struc->arrayOfVirusToSend;
                for(int i = 0; i < virusArrayLength; i++){
                    sendString(socketBufferSize, (*virusArrayToSend[i]).c_str(),writefd);
                    receiveString(readfd, &ack);
                    free(ack);
                    bloomFilter* bl = virusHash->getBFOfVirus((*virusArrayToSend[i]).c_str());
                    sendBloom(writefd, bl->getArray(), bloomSize, socketBufferSize);
                    receiveString(readfd, &ack);
                    free(ack);
                }
                
                delete[] *struc->arrayOfVirusToSend;
                struc->virusArrayLength = 0;
                *struc->arrayOfVirusToSend = NULL;
                virusArrayToSend = NULL;
                struc->virusArrayLength = 0;
                
                char* done = "DONE";
                sendString(socketBufferSize, done, writefd);
                for(int i = 0; i < noOfFiles+2; i++){
                    free(fileListTemp[i]);
                }
                free(fileListTemp);

                free(country);
                free(command);
                continue;
            }else{
                sendString(socketBufferSize, "NO", writefd);
                for(int i = 0; i < noOfFiles+2; i++){
                    free(fileListTemp[i]);
                }
                free(country);
                free(fileListTemp);
                free(command);
                continue;
            }

        }else if(strcmp(command,"/exit") == 0){

            // should delete everything...
            cout << "EXIT\n";
            int counter;
            int accepted;
            int rejected;
            sendString(socketBufferSize, "OK", writefd);

            if(read(readfd, &counter, sizeof(int)) == -1){
                perror("Problem when reading ");
                exit(1);
            }

            if(read(readfd, &accepted, sizeof(int)) == -1){
                perror("Problem when reading ");
                exit(1);
            }

            if(read(readfd, &rejected, sizeof(int)) == -1){
                perror("Problem when reading ");
                exit(1);
            }

            ofstream outfile ("logfile."+to_string(getpid()));
            
            for(int i = 0; i < numOfDirs; i++){
                outfile << arrayOfCharSubDirs[i] << endl;
            }
            outfile <<"TOTAL REQUESTS: " << counter << endl;
            outfile <<"ACCEPTED REQUESTS: " << accepted << endl;
            outfile <<"REJECTED REQUESTS: "<< rejected << endl;

            delete struc->citizenHash;
            delete struc->countryHash;
            delete struc->virusHash;
            char* path;

            while(struc->cycBuffer->empty() != 1){
                struc->cycBuffer->readFromCyclicBuffer(&path);
                free(path);
            }

            for(int i = 0; i < numThreads; i++){
                struc->cycBuffer->insertToCyclicBuffer("/exit");
            }
            pthread_cond_broadcast(&cond);

            for(int i = 0; i < numThreads; i++){
                pthread_join(tArray[i], NULL);
            }

            for(int i = 0; i < counterVirus; i++){
                free(arrayOfViruses[i]);
            }

            free(arrayOfViruses);
            free(arrayOfCharSubDirs);
            delete[] arrayOfBloomFilters;
            
            delete[] arrayOfSubDirs;
            delete[] arrayOfStringSubDirs;
            delete[] arrayOfDirsPerCountry;
            delete[] arrayOfCharDirs;
            delete cycBuffer;
            
            free(struc->personsArray);
            delete struc;      
            free(command);
            break;
        }else{
            cout << "ALL OTHERS\n";
        }
    }
    sendString(socketBufferSize, "OK", writefd);
    close(newsock);
    close(sock);
    myfile.close();
    return 0;
    
}

int receiveString(int readfd, char** ptr){
    // receive total number Of Bytes...
    int c = 0;
    int numOfBytes;
    int bytesToReceive;
    char* command;
    while(1){
        if(read(readfd, &numOfBytes, sizeof(int)) == -1){
            perror("Problem when reading 1");
            exit(1);
        }
        break;
    }
    *ptr = (char*) malloc(sizeof(char*) * numOfBytes);
    while(1){
        if(read(readfd, &bytesToReceive, sizeof(int)) == -1){
            perror("Problem when reading 2");
            exit(1);
        }

        if(read(readfd, &(*ptr)[c], sizeof(char)*bytesToReceive) == -1){
            perror("Problem when reading 3");
            exit(1);
        }
        c+=bytesToReceive;
        
        if(c == numOfBytes) break;
    }
    return numOfBytes;
}

int sendString(int bufferSize, const char* str, int writefd){
    int len = strlen(str) + 1;
    if(write(writefd, &len, sizeof(int)) == -1){
        perror("Problem when writing ");
        exit(1);
    }
    if(len <= bufferSize ){
        
        if(write(writefd, &len, sizeof(int)) == -1){
            perror("Problem when writing ");
            exit(1);
        }
        
        if(write(writefd, str, sizeof(char)*len) == -1){
            perror("Problem when writing ");
            exit(1);
        }
    }else{
        int bytesWritten = 0;
        int bytesLeft;
        while(bytesWritten < len){
            bytesLeft = len - bytesWritten;
            if(bytesLeft <= bufferSize){
                if(write(writefd, &bytesLeft, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                if(write(writefd, &str[bytesWritten], sizeof(char)*bytesLeft) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                bytesWritten += bytesLeft;
            }else{
                if(write(writefd, &bufferSize, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                if(write(writefd, &str[bytesWritten], sizeof(char)*bufferSize) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                bytesWritten += bufferSize;
            }
        }
    }
    return 1;
}

int sendBloom(int writefd, uint8_t* bloom, int bloomSize, int buffSize){

        int bytesWritten = 0;
        int bytesLeft = 0; 
        int bytesToWrite = 0;        
        uint8_t* ogarray = bloom;
        uint8_t* tempArray = bloom;
        
        while(bytesWritten < bloomSize){
            bytesLeft = bloomSize - bytesWritten;
            tempArray = &ogarray[bytesWritten];
            
            if(bytesLeft < buffSize){
                // send how many bytes you will write...
                
                if(write(writefd, &bytesLeft, sizeof(int)) < 0){
                    perror("Problem when writing ... ");
                    exit(1);
                }
                bytesWritten += bytesLeft;
                bytesToWrite = bytesLeft;

            }else{
                
                if(write(writefd, &buffSize, sizeof(int)) < 0){
                    perror("Problem when writing ... ");
                    exit(1);
                }
                bytesWritten += buffSize;
                bytesToWrite = buffSize;
            }

            //cout << bytesToWrite << endl;
            //send the array...
            if(write(writefd, tempArray, sizeof(uint8_t)*bytesToWrite) < 0){
                perror("Problem when writing ... ");
                exit(1);
            }
        }
}

