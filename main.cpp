#include "helpers.h"

using namespace std; 


void commands(uint8_t*** bloomsArray, int* arrayOfSock, int sizeOfBloom, 
            int* bloomsPerMonitor, int numMonitors, char*** arrayOfVirusesPerMonitor, 
            int bufferSize, int* numOfDirsPerMonitor, string* arrayOfCountries, int numOfCountries, int* arrayOfPid);

int receiveString(int readfd, char** ptr);
int sendString(int bufferSize, const char* str, int writefd);

int main(int argc, char *argv[]){    
    char host[HOST_NAME_MAX];
    char* IP;

    int sock;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;

    int numMonitors = 0;
    int bufferSize = 0;
    int numOfSubDirsPerCountry = 0;
    char* fileName = "";
    char* dirName = "";
    int bloomSize = 0;
    int cyclicBufferSize = 0;
    //char* buffer;
    int numThreads = 0;
    DIR *dir_ptr;

    if(argc != 13) {
        cout << "Sorry, not enough parametres (13 needed).\n";
        exit(1);
    }


    if(strcmp(argv[1], "-m") == 0){
        
        // first command param is -m
        // I expect a valid number
        char c = argv[2][0];
        int counter = 1;
        while(c != '\0'){
            if(c < '0' || c > '9'){
                cout << "-m parameter should be a legal number.\n";
                exit(1);
            }
            c = argv[2][counter];
            counter++;
        }
        numMonitors = atoi(argv[2]);

        // -b should follow...
        if(strcmp(argv[3], "-b") == 0){

            char c = argv[4][0];
            int counter = 1;
            while(c != '\0'){
                if(c < '0' || c > '9'){
                    cout << "-b parameter should be a legal number.\n";
                    exit(1);
                }
                c = argv[4][counter];
                counter++;
            }
            bufferSize = atoi(argv[4]);

            if(strcmp(argv[5], "-c") == 0) {

                // i expect a number...
                char c = argv[6][0];
                int counter = 1;
                while(c != '\0'){
                    if(c < '0' || c > '9'){
                        cout << "-c parameter should be a legal number.\n";
                        exit(1);
                    }
                    c = argv[6][counter];
                    counter++;
                }
                
                cyclicBufferSize = atoi(argv[6]);

                if(strcmp(argv[7],"-s") == 0){
                    char c = argv[8][0];
                    int counter = 1;
                    while(c != '\0'){
                        if(c < '0' || c > '9'){
                            cout << "-s parameter should be a legal number.\n";
                            exit(1);
                        }
                        c = argv[8][counter];
                        counter++;
                    }
                    
                    bloomSize = atoi(argv[8]);

                    if(strcmp(argv[9], "-i") == 0) {
                        //directory... 
                        struct stat sb;
                        
                        if ( ( dir_ptr = opendir ( argv[10] ) ) == NULL ){
                            fprintf ( stderr , " cannot open %s \n " , argv[10] ) ;
                            exit(1);
                        }
                        
                        dirName = argv[10];

                        if(strcmp(argv[11],"-t") == 0){
                            char c = argv[12][0];
                            int counter = 1;
                            while(c != '\0'){
                                if(c < '0' || c > '9'){
                                    cout << "-t parameter should be a legal number.\n";
                                    exit(1);
                                }
                                c = argv[12][counter];
                                counter++;
                            }
                            numThreads = atoi(argv[12]);
                        }
                        closedir(dir_ptr);
                    }
                }
            }
        } 
    }

    cout << "numMonitors: "<<numMonitors<<endl;
    cout << "socketBufferSize: "<<bufferSize<<endl;
    cout << "cyclicBufferSize: "<<cyclicBufferSize<<endl;
    cout << "bloomSize: "<<bloomSize<<endl;
    cout << "dirName: "<<dirName<<endl;
    cout << "numThreads: "<<numThreads<<endl;

    char pipeName[] = "/tmp/namedPipe";

    int length = strlen(pipeName);
    char* newPipe;

    DIR *dr = opendir(dirName);
    struct dirent *de;


    string* arrayOfSubDirectories;

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        perror("Could not open current directory ");
        return 0;
    }
    int file_count = 0;

    while ((de = readdir(dr)) != NULL){
        file_count++;
    }

    file_count-=2; // . , ..
    closedir(dr);

    arrayOfSubDirectories = new string[file_count]; 
    int numOfCountries = file_count;
    int counter=0;

    struct dirent* de2;

    int* numOfDirsPerMonitor = new int[numMonitors];
    for(int i = 0; i < numMonitors; i++){
        numOfDirsPerMonitor[i] = 0;
    }

    struct dirent **fileListTemp;
    struct dirent **fileListTemp2;
    int noOfFiles;
    noOfFiles = scandir(dirName, &fileListTemp, NULL, alphasort);
    //while ((de = readdir(dr)) != NULL){
    for(int i = 0; i < noOfFiles; i++){
        if(counter == 2){
            // i need it just once...
            string subDirName = dirName;
            subDirName += "/";
            subDirName += fileListTemp[i]->d_name;
            char subDirChar[subDirName.length()+1];
            strcpy(subDirChar, subDirName.c_str());
            // find number of subDirs in a specific country...
            numOfSubDirsPerCountry = scandir(subDirChar, &fileListTemp2, NULL, alphasort);
        }

        if(counter >= 2){
            char* subDirName = fileListTemp[i]->d_name;
            arrayOfSubDirectories[counter-2] = string(subDirName);
        }
        counter++;
    }

    for(int i = 0; i < noOfFiles; i++){
        free(fileListTemp[i]);
    }
    free(fileListTemp);

    for(int i = 0; i < numOfSubDirsPerCountry; i++){
        free(fileListTemp2[i]);
    }
    free(fileListTemp2);
    numOfSubDirsPerCountry = numOfSubDirsPerCountry - 2;

    int status;
    int arrayOfWriteFd[numMonitors];
    int arrayOfReadFd[numMonitors];
    int bloomsPerMonitor[numMonitors]; // how many bloom filter does each monitor have...
    char** arrayOfViruses;

    // parent generate n children...
    int charLengthOfDir = 0;
    charLengthOfDir = strlen(dirName)+1;
    int writefd, readfd;

    uint8_t*** arrayOfArrayOfBloomFilters = (uint8_t***)malloc(sizeof(uint8_t***)*numMonitors);
    char*** arrayOfVirusesPerMonitor = (char***) malloc(sizeof(char***)*numMonitors);
    int* arrayOfPid = new int[numMonitors];

    // 
    string temp = to_string(numThreads);
    char* numThreadsChar = strdup(temp.c_str());

    temp = to_string(bufferSize);
    char* socketBufferSizeChar = strdup(temp.c_str());

    temp = to_string(cyclicBufferSize);
    char* cyclicBufferSizeChar = strdup(temp.c_str());

    temp = to_string(bloomSize);
    char* bloomSizeChar = strdup(temp.c_str());
    //exit(1);

    srand( time(NULL) );

            // generate a port between 49152–65535...
    int portInt[numMonitors];
    for(int i = 0; i < numMonitors; i++){
        portInt[i] = rand() % (65535 +1 - (49152+i)) + 49152;
    }

    int arrayOfSock[numMonitors];

    for(int i = 0; i < numMonitors; i++){
        
        int id2 = fork();
        
        if(id2 < 0 ){
            perror("Fork Error: ");
            exit(1);
        }

        char* pipe1, *pipe2;
        if(id2 == 0){
            // child

            // should create an array of char* 
            // with the paths for the countries 
            // that each monitor will use...
            //cout << dirName << endl;
            int lengthOfSubDirName = 0;
            int counterForSubDirs = i;
            string subDir;
            char** arrayOfCharSubDirs = NULL;
            int numSubDirs = 0;
            while(counterForSubDirs < file_count){

                numOfDirsPerMonitor[i]++;
                subDir = dirName + (string)"/" +arrayOfSubDirectories[counterForSubDirs];
                lengthOfSubDirName = subDir.length();
                char subDirNameChar[lengthOfSubDirName+1];
                strcpy(subDirNameChar, subDir.c_str());
                
                if(numSubDirs > 0){
                    
                    char** temp = (char**) malloc(sizeof(char**)*(numSubDirs+1));
                    int length;
                    for(int k = 0; k < numSubDirs; k++){
                        length = strlen(arrayOfCharSubDirs[k]);
                        temp[k] = (char*) malloc(sizeof(char*)*(length+1));
                        strcpy(temp[k], arrayOfCharSubDirs[k]);
                    }
                    length = strlen(subDirNameChar);
                    temp[numSubDirs] = (char*) malloc(sizeof(char*)*(length+1));
                    strcpy(temp[numSubDirs], subDirNameChar);

                    for(int i = 0; i < numSubDirs - 1; i++){
                        free(arrayOfCharSubDirs[i]);
                    }
                    free(arrayOfCharSubDirs);

                    arrayOfCharSubDirs = temp;
                    numSubDirs ++;

                }else if (numSubDirs == 0){
                    numSubDirs ++;
                    arrayOfCharSubDirs = (char**) malloc(sizeof(char**) * numSubDirs);
                    arrayOfCharSubDirs[0] = (char*) malloc(sizeof(char*)*(lengthOfSubDirName+1));
                    strcpy(arrayOfCharSubDirs[0], subDirNameChar);
                }
                counterForSubDirs += numMonitors;
            }

            char numberOfArgs[4]; //4 bytes for an integer, 
            sprintf(numberOfArgs, "%d", 12+numSubDirs);
            
            
            char port[4];
            sprintf(port,"%d", portInt[i]);

            char* args[numSubDirs+12]={
                // number of paths + 12 (number of other parametres(11) + 1 (NULL))
                "./monitorServer",
                "-p",
                port,
                "-t",
                numThreadsChar,
                "-b",
                socketBufferSizeChar,
                "-c",
                cyclicBufferSizeChar,
                "-s",
                bloomSizeChar,
            };

            for(int i = 0; i < numSubDirs; i++){
                args[i+11] = arrayOfCharSubDirs[i];
            }

            args[numSubDirs+11] = NULL;

            int r = execv(
                args[0],
                args
            );

            if(r == -1){
                perror("Execv problem: ");
                exit(1);
            }
            break;

        }else{
            //exit(1);
            // parent...
            arrayOfPid[i] = id2;

            

            if(gethostname(host,HOST_NAME_MAX) == -1){
                perror("GetHostName");
                exit(1);
            }

            if ((rem = gethostbyname(host)) == NULL) {	
                perror("gethostbyname");
                exit(1);
            }

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                perror("Socket");
                exit(1);
            }

            arrayOfSock[i] = sock;
            server.sin_family = AF_INET;       /* Internet domain */
            memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
            server.sin_port = htons(portInt[i]);         /* Server port */
            int connectStatus;
            do{
                connectStatus = connect(sock, serverptr, sizeof(server));
            }while(connectStatus < 0);
                

            printf("Connecting to %s port %d\n", rem->h_name, portInt[i]);

            IP = inet_ntoa(*((struct in_addr*) rem->h_addr_list[0]));
            printf("%s \n", IP);

            char* received;
            receiveString(sock, &received);
            printf("%s\n", received);

            readfd = sock;
            free(received);
            // // we wait for the bloom filters...

            // // how many bloom filters will I receive ?
            int r;
            int numOfBloomFilters;

            if((r = read(readfd, &numOfBloomFilters, sizeof(int))) == -1){
                perror("Problem when reading 1... ");
                exit(1);
            }

            bloomsPerMonitor[i] = numOfBloomFilters;

            // I need to save both the array of the bloom filter and the virus it 
            // represents...

            // Create an array of arrays and an array of char*. The 2 arrays should
            // be equal in size and a specific array's virus should be found in the respective
            // index in array of chars*

            char* virus;
            arrayOfViruses = (char**) malloc(numOfBloomFilters*sizeof(char**));
            arrayOfVirusesPerMonitor[i] = arrayOfViruses;
            arrayOfArrayOfBloomFilters[i] = (uint8_t**) malloc(sizeof(uint8_t**)*numOfBloomFilters);

            for(int bloomsCounter = 0; bloomsCounter < numOfBloomFilters; bloomsCounter++){
                
                arrayOfArrayOfBloomFilters[i][bloomsCounter] = (uint8_t*) malloc(sizeof(uint8_t*)*bloomSize);
                int numOfBytes;
                int total = 0;
                int diff = bufferSize;
                char* tempBuff;
                int OK;

                //arrayOfViruses[bloomsCounter] = (char*) malloc(sizeof(char*)*numOfBytes);
                receiveString(sock, &arrayOfViruses[bloomsCounter]);
                sendString(bufferSize,"OK", sock);
                
                // receive bloom filter...
                int counter = 0;
                int c = 0;
                uint8_t* temp = arrayOfArrayOfBloomFilters[i][bloomsCounter];
                
                while(1){
                    //temp = &;
                    if(read(sock, &counter, sizeof(int)) == -1){
                        perror("Problem when reading... ");
                        exit(1);
                    }
                    
                    if(read(sock, &arrayOfArrayOfBloomFilters[i][bloomsCounter][c], sizeof(uint8_t)*counter) == -1){
                        perror("Problem when reading... ");
                        exit(1);
                    }
                    c+=counter;
                    if(c == bloomSize) break;
                }
                sendString(bufferSize, "OK", sock);
            }
        }
    }

    commands(arrayOfArrayOfBloomFilters,arrayOfSock,bloomSize,
             bloomsPerMonitor , numMonitors, arrayOfVirusesPerMonitor, 
             bufferSize, numOfDirsPerMonitor, arrayOfSubDirectories, numOfCountries, arrayOfPid);
    close(sock);
    // time for cleaning
    free(bloomSizeChar);
    free(cyclicBufferSizeChar);
    free(numThreadsChar);
    free(socketBufferSizeChar);

    for(int i = 0; i < numMonitors; i++){
        for(int j = 0; j < bloomsPerMonitor[i]; j++){
            free(arrayOfArrayOfBloomFilters[i][j]);
        }
        free(arrayOfArrayOfBloomFilters[i]);
    }
    free(arrayOfArrayOfBloomFilters);

    for(int i = 0; i < numMonitors; i++){
        for(int j = 0; j < bloomsPerMonitor[i]; j++){
            free(arrayOfVirusesPerMonitor[i][j]);
        }
        free(arrayOfVirusesPerMonitor[i]);
    }
    free(arrayOfVirusesPerMonitor);

    delete[] arrayOfSubDirectories;
    delete[] numOfDirsPerMonitor;
    delete[] arrayOfPid;

    while(wait(NULL) > 0);
    return 0;
}

void commands(uint8_t*** bloomsArray, int* arrayOfSock, 
        int sizeOfBloom ,int* bloomsPerMonitor, int numMonitors,
        char*** arrayOfVirusesPerMonitor, int bufferSize, int* numOfDirsPerMonitor, 
        string* arrayOfCountries, int numOfCountries, int* arrayOfPid){
    
    int counter;
    string* arrayOfStrings = NULL;
    string command1 = "/travelRequest";
    string command2 = "/travelStats";
    string command3 = "/addVaccinationRecords";
    string command4 = "/searchVaccinationStatus";
    string command5 = "/exit";
    travelRequest** arrayOfRequests = NULL;

    int requests = 0;

    while(1){
        counter = 0;
        arrayOfStrings = readInput(&counter);
        
        if(arrayOfStrings == NULL){
            continue;
        }else if(arrayOfStrings[0] == command1){

            // arrayOfStrings[1] == citizenId
            // arrayOfStrings[2] == date
            // arrayOfStrings[3] == countryFrom
            // arrayOfStrings[4] == countryTo
            //arrayOfStrings[5] == virusName...            
            if(counter != 6){
                cout << "Sorry, \"/travelRequest\" requires 6 parametres (citizenID, Date, CountryFrom, CountryTo, VirusName).\n";
                delete[] arrayOfStrings;
                continue;
            }
            int leave = 0;
            uint8_t* respectiveBloom = NULL;
            int monitor = -1;
            int error = 0;
        
            for(int i = 0; i < arrayOfStrings[1].length(); i++){
                if(arrayOfStrings[1][i] < '0' || arrayOfStrings[1][i] > '9'){
                    cout << "Sorry, Id should be integer\n";
                    error = 1;
                    break;
                }
            }

            if(error){
                delete[] arrayOfStrings;
                continue;
            }

            int id = stoi(arrayOfStrings[1]);
            string dString = arrayOfStrings[2];

            if(checkDateInput(dString) == 1){
                cout << "\nSorry, this is not a valid date.\n";
                delete[] arrayOfStrings;
                continue;
            }

            // date Of trip
            date* d = dateConverter(dString);

            // get today's date...
            time_t t = std::time(0);  
            tm* now = std::localtime(&t);

            int yearNow = now->tm_year + 1900;
            int monthNow = now->tm_mon + 1;
            int dayNow = now->tm_mday;

            int yearOfTrip = d->getYear();
            int monthOfTrip = d->getMonth();
            int dayOfTrip = d->getDay();

            if(yearOfTrip < yearNow){
                cout << "\nSorry, The Date Of the trip must be some time in the future 1.\n";
                delete d;
                delete[] arrayOfStrings;
                continue;
            }else if(yearOfTrip == yearNow){
                if(monthOfTrip < monthNow){
                    cout << "\nSorry, The Date Of the trip must be some time in the future 2.\n";
                    delete d;
                    delete[] arrayOfStrings;
                    continue;
                }else if(monthOfTrip == monthNow){
                    if(dayOfTrip < dayNow){
                        cout << "\nSorry, The Date Of the trip must be some time in the future 3.\n";
                        delete d;
                        delete[] arrayOfStrings;
                        continue;
                    }
                }
            }

            string countryFrom = firstCap(arrayOfStrings[3]);
            string countryTo = firstCap(arrayOfStrings[4]);
            string virus = toUpper(arrayOfStrings[5]);

            int keep = -1;
            for(int i = 0; i < numOfCountries; i++){
                if(arrayOfCountries[i] == countryFrom){
                    keep = i;
                }
            }

            if(keep == -1){
                cout << "Sorry, \"CountryFrom\" does not exist in system. Try another one.\n";
                delete d;
                delete[] arrayOfCountries;
                continue;
            }
            // determine which monitor contains this country...
            if(keep <= numMonitors){
                monitor = keep;
            }else{
                // keep > numMonitors
                monitor = keep % numMonitors;
            }
            
            for(int i = 0; i < bloomsPerMonitor[monitor]; i++){                
                if(strcmp(arrayOfVirusesPerMonitor[monitor][i], arrayOfStrings[5].c_str())==0){
                    respectiveBloom = bloomsArray[monitor][i];
                    break;
                }
            }
            
            if(respectiveBloom == NULL){
                cout << "--------------------------------------------------------\n";
                cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED FOR THIS VIRUS\n";
                cout << "(did not find this virus in this monitor)\n";
                cout << "--------------------------------------------------------\n";
                delete d;
                delete[] arrayOfStrings;
                continue;
            }

            bloomFilter* bl = new bloomFilter(respectiveBloom, sizeOfBloom);
            requests++;

            
            if(bl->search(arrayOfStrings[1]) == 1){ 
                
                if(sendString(bufferSize, arrayOfStrings[0].c_str(), arrayOfSock[monitor]) == 0){
                    cout << "Sorry, something went wrong";
                    delete d;
                    delete bl;
                    delete[] arrayOfStrings;
                    //free(comm);
                    //continue;
                    exit(1);
                }

                char* ack;
                //send the virus...
                if(sendString(bufferSize, arrayOfStrings[5].c_str(), arrayOfSock[monitor]) == 0){
                    cout << "Sorry, something went wrong";
                    delete bl;
                    delete[] arrayOfStrings;
                    delete d;
                    //free(comm);
                    //continue;
                    exit(1);
                }

                if(write(arrayOfSock[monitor], &id, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
        
                char* answer;
                receiveString(arrayOfSock[monitor], &answer);
                
                if(strcmp(answer, "YES") == 0){
                    // should receive the date...
                    // To make my life easy, I just receive the 3 int intead of 
                    // passing the whole class...

                    int dayOfVacc, monthOfVacc, yearOfVacc;

                    if(read(arrayOfSock[monitor], &dayOfVacc, sizeof(int)) == -1){
                        perror("Problem when trying to read");
                        exit(1);
                    }
                    if(read(arrayOfSock[monitor], &monthOfVacc, sizeof(int)) == -1){
                        perror("Problem when trying to read");
                        exit(1);
                    }

                    if(read(arrayOfSock[monitor], &yearOfVacc, sizeof(int)) == -1){
                        perror("Problem when trying to read");
                        exit(1);
                    }
        
                    if(yearOfTrip > yearOfVacc){
                        cout << "----------------------------------------------------\n";
                        cout << "REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION\n";
                        cout << "----------------------------------------------------\n";

                        if(arrayOfRequests == NULL){

                            // this is the first request...
                            arrayOfRequests = new travelRequest*[1];
                            arrayOfRequests[0] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);

                        }else{
                            // we should add another request...
                            travelRequest** temp = new travelRequest*[requests];
                            int i;
                            for(i = 0; i < requests-1; i++){
                                temp[i] = arrayOfRequests[i];
                            }
                            temp[i] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                            delete[] arrayOfRequests;

                            arrayOfRequests = temp;
                        }

                        delete[] arrayOfStrings;
                        delete d;
                        delete bl;
                        free(answer);
                        //free();
                        continue;
                    }else if(yearOfTrip == yearOfVacc){
                        if(monthOfTrip - 6 > monthOfVacc){

                            if(arrayOfRequests == NULL){
                            // this is the first request...
                                arrayOfRequests = new travelRequest*[1];
                                arrayOfRequests[0] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                            }else{
                                // we should add another request...
                                travelRequest** temp = new travelRequest*[requests];
                                int i;
                                for(i = 0; i < requests-1; i++){
                                    temp[i] = arrayOfRequests[i];
                                }
                                temp[i] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                                delete[] arrayOfRequests;

                                arrayOfRequests = temp;
                            }

                            cout << "----------------------------------------------------\n";
                            cout << "REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION\n";
                            cout << "----------------------------------------------------\n";
                            delete[] arrayOfStrings;
                            delete bl;
                            delete d;
                            free(answer);
                            //free(comm);
                            continue;

                        }else{

                            if(arrayOfRequests == NULL){
                            // this is the first request...
                                arrayOfRequests = new travelRequest*[1];
                                arrayOfRequests[0] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                            }else{
                                // we should add another request...
                                travelRequest** temp = new travelRequest*[requests];
                                int i;
                                for(i = 0; i < requests-1; i++){
                                    temp[i] = arrayOfRequests[i];
                                }
                                temp[i] = new travelRequest(d, countryFrom, countryTo, virus, 1, monitor);
                                delete[] arrayOfRequests;

                                arrayOfRequests = temp;
                            }

                            cout << "--------------------------------\n";
                            cout << "REQUEST ACCEPTED - HAPPY TRAVELS\n";
                            cout << "--------------------------------\n";
                            delete[] arrayOfStrings;
                            delete bl;
                            delete d;
                            free(answer);
                            //free(comm);
                            continue;
                        }
                    }
                    
                }else if(strcmp(answer, "NO") == 0){
                    if(arrayOfRequests == NULL){
                    // this is the first request...
                        arrayOfRequests = new travelRequest*[1];
                        arrayOfRequests[0] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                    }else{
                        // we should add another request...
                        travelRequest** temp = new travelRequest*[requests];
                        int i;
                        for(i = 0; i < requests-1; i++){
                            temp[i] = arrayOfRequests[i];
                        }
                        temp[i] = new travelRequest(d, countryFrom, countryTo, virus, 1, monitor);
                        delete[] arrayOfRequests;

                        arrayOfRequests = temp;
                    }
                    cout << "-----------------------------------------\n";
                    cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED\n";
                    cout << "-----------------------------------------\n";
                    delete[] arrayOfStrings;
                    delete bl;
                    delete d;
                    free(answer);
                    //free(comm);
                    continue;
                }
                
            }else{
                
                if(arrayOfRequests == NULL){
                    // this is the first request...
                    arrayOfRequests = new travelRequest*[1];
                    arrayOfRequests[0] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                }else{

                    // we should add another request...
                    travelRequest** temp = new travelRequest*[requests];
                    int i;
                    for(i = 0; i < requests-1; i++){
                        temp[i] = arrayOfRequests[i];
                    }
                    temp[i] = new travelRequest(d, countryFrom, countryTo, virus, 0, monitor);
                    delete[] arrayOfRequests;

                    arrayOfRequests = temp;
                }

                cout << "-----------------------------------------\n";
                cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED\n";
                cout << "-----------------------------------------\n\n";
                delete[] arrayOfStrings;
                delete d;
                delete bl;
                continue;

            }

        }else if(arrayOfStrings[0] == command2){
            
            // arrayOfStrings[1] == virusName
            // arrayOfStrings[2] == date1
            // arrayOfStrings[3] == date2
            // arrayOfStrings[4] == country --> optional

            if(counter > 5 || counter < 4){
                cout << "Sorry, \"Travel Stats\" command requires at least 3 params (virusName, Date 1, Date 2) and 1 optional (country)\n";
                delete[] arrayOfStrings;
                continue;
            }

            // check the input...
            string virus = arrayOfStrings[1];
            string date1 = arrayOfStrings[2];
            string date2 = arrayOfStrings[3];
            
            if(checkDateInput(date1) == 1){
                cout << "Sorry, Date 1 is not a valid date\n";
                delete[] arrayOfStrings;
                continue;
            }else if (checkDateInput(date2) == 1){
                cout << "Sorry, Date 2 is not a valid date\n";
                delete[] arrayOfStrings;
                continue;
            }

            date* d1 = dateConverter(date1);
            date* d2 = dateConverter(date2);

            if(d1->greaterEqThan(d2)){
                cout << "Sorry, Date 1 should be less than Date 2 in chronological order\n";
                delete[] arrayOfStrings;
                delete d1;
                delete d2;
                continue;
            }

            int totalRequests = 0;
            int rejectedRequests = 0;
            int acceptedRequests = 0;

            if(counter == 4){
                // no country
                for(int i = 0; i < requests; i++){
                    if(arrayOfRequests[i]->dateOfTrip->greaterEqThan(d1) && arrayOfRequests[i]->dateOfTrip->lessEqThan(d2)){
                    //if((arrayOfRequests[i]->dateOfTrip) >= d1 && (arrayOfRequests[i]->dateOfTrip) <= d2){
                        totalRequests++;
                        if(arrayOfRequests[i]->status == 1){
                            acceptedRequests++;
                        }else if(arrayOfRequests[i]->status == 0){
                            rejectedRequests++;
                        }
                    }
                }
                cout << "TOTAL REQUESTS: "<<totalRequests<<endl;
                cout << "ACCEPTED REQUESTS: "<<acceptedRequests<<endl;
                cout << "REJECTED REQUESTS: "<<acceptedRequests<<endl;

                delete d1;
                delete d2;
                delete[] arrayOfStrings;
                continue;
            }else if(counter == 5){
                string country = arrayOfStrings[4];
                for(int i = 0; i < requests; i++){
                    if(arrayOfRequests[i]->countryTo == country){
                        if(arrayOfRequests[i]->dateOfTrip->greaterEqThan(d1) && arrayOfRequests[i]->dateOfTrip->lessEqThan(d2)){
                            totalRequests++;
                            if(arrayOfRequests[i]->status == 1){
                                acceptedRequests++;
                            }else if(arrayOfRequests[i]->status == 0){
                                rejectedRequests++;
                            }
                        }
                    }
                }
                cout << "TOTAL REQUESTS: "<<totalRequests<<endl;
                cout << "ACCEPTED REQUESTS: "<<acceptedRequests<<endl;
                cout << "REJECTED REQUESTS: "<<acceptedRequests<<endl;

                delete d1;
                delete d2;
                delete[] arrayOfStrings;
                continue;
            }

            delete d1;
            delete d2;
            delete[] arrayOfStrings;
            continue;
        }else if(arrayOfStrings[0] == command4){

            // searchVaccinationStatus...
            if(counter != 2) {
                cout << "Sorry, \"SearchVaccinationStatus\" requires 1 parameter(citizenID)\n";
                delete[] arrayOfStrings;
                continue;
            }

            int error = 0;
            string idString = arrayOfStrings[1];
            for(int i = 0; i < idString.length(); i++){
                if(idString[i] < '0' || idString[i] > '9'){
                    error = 1;
                    break;
                }
            }
            
            int id = stoi(idString);
            if(error){
                cout << "Sorry, citizenID must be valid integer\n";
                delete[] arrayOfStrings;
                continue;
            }
            
            int monitor = -1;
            for(int i = 0; i < numMonitors; i++){
                // send command
                sendString(bufferSize, arrayOfStrings[0].c_str(), arrayOfSock[i]);

                //send the ID to all monitors...
                if(write(arrayOfSock[i], &id, sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }

                // read an answer
                char* answer;
                int numBytes = receiveString(arrayOfSock[i], &answer);
            
                if(strcmp(answer,"YES") == 0){
                    monitor = i;
                    free(answer);
                    break;
                }
                free(answer);
            }

            if(monitor == -1){
                cout << "Sorry, this citizen ID does not exist\n";
                delete[] arrayOfStrings;
                continue;
            }

            // I should now receive everything about citizen with ID
            // I expect a name, a surname, a country, an age...
            
            char* name;
            char* surname;
            char* country;
            int age;

            char* ack = "OK";
            sendString(bufferSize, ack, arrayOfSock[monitor]);
            int lengthName = receiveString(arrayOfSock[monitor], &name);

            sendString(bufferSize, ack, arrayOfSock[monitor]);
            int lengthSurname = receiveString(arrayOfSock[monitor], &surname);

            sendString(bufferSize, ack, arrayOfSock[monitor]);
            int lengthCountry = receiveString(arrayOfSock[monitor], &country);

            sendString(bufferSize, ack, arrayOfSock[monitor]);

            
            if(read(arrayOfSock[monitor], &age, sizeof(int)) == -1){
                perror("Problem when reading");
                exit(1);
            }
            char* virus1;
            char* vacced;
            printf("%d %s %s %s \n", id, name, surname, country);
            cout <<"AGE "<<age << endl<<endl;
            while(1){
                // name Of virus
                receiveString(arrayOfSock[monitor], &virus1);
                if(strcmp(virus1, "DONE") == 0) break;
                sendString(bufferSize, ack, arrayOfSock[monitor]);
                printf("%s ",virus1);
                // vaccination status...
                receiveString(arrayOfSock[monitor], &vacced);
                if(strcmp(vacced,"YES") == 0){
                    // vaccinated...
                    // 3 ints for date
                    sendString(bufferSize, ack, arrayOfSock[monitor]);
                    int day, month, year;
                    if(read(arrayOfSock[monitor], &day, sizeof(int)) == -1){
                        perror("Problem when reading");
                        exit(1);
                    }

                    if(read(arrayOfSock[monitor], &month, sizeof(int)) == -1){
                        perror("Problem when reading");
                        exit(1);
                    }

                    if(read(arrayOfSock[monitor], &year, sizeof(int)) == -1){
                        perror("Problem when reading");
                        exit(1);
                    }
                    cout << "VACCINATED ON ";
                    cout <<day<<"-"<<month<<"-"<<year<<endl;
                    
                    sendString(bufferSize, ack, arrayOfSock[monitor]);

                }else if(strcmp(vacced,"NO") == 0){
                    // not vaccinated ...
                    cout << "NOT YET VACCINATED\n";
                    sendString(bufferSize, ack, arrayOfSock[monitor]);
                }
                free(virus1);
                free(vacced);
            }
            free(virus1);
            sendString(bufferSize, ack, arrayOfSock[monitor]);
            free(country);
            free(name);
            free(surname);
            delete[] arrayOfStrings;

        }else if(command5 == arrayOfStrings[0]){
            // exit...

            // write a log file 

            // ofstream outfile ("logfile."+to_string(getpid()));

            // for(int i = 0; i < numOfCountries; i++){
            //     outfile << arrayOfCountries[i] << endl;
            // }

            int counter = 0;
            int accepted = 0;
            int rejected = 0;

            int counterArray[numMonitors];
            int acceptedArray[numMonitors];
            int rejectedArray[numMonitors];
            for(int i = 0; i < numMonitors; i++){
                counterArray[i] = 0;
                acceptedArray[i] = 0;
                rejectedArray[i] = 0;
            }

            for(int i = 0; i < requests; i++){
                counterArray[arrayOfRequests[i]->monitor]++;
                if(arrayOfRequests[i]->status == 1){
                    acceptedArray[arrayOfRequests[i]->monitor]++;
                }else if(arrayOfRequests[i]->status == 0){
                    rejectedArray[arrayOfRequests[i]->monitor]++;
                }
            }

            for(int i = 0; i < numMonitors; i++){
                sendString(bufferSize,arrayOfStrings[0].c_str(), arrayOfSock[i]);
                char* ack;
                receiveString(arrayOfSock[i], &ack);
                free(ack);
                if(write(arrayOfSock[i], &counterArray[i], sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }

                if(write(arrayOfSock[i], &acceptedArray[i], sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }

                if(write(arrayOfSock[i], &rejectedArray[i], sizeof(int)) == -1){
                    perror("Problem when writing ");
                    exit(1);
                }
                receiveString(arrayOfSock[i], &ack);
                free(ack);
            }
            
            if(requests > 0){
                for(int i = 0; i < requests; i++){
                    delete arrayOfRequests[i];
                }
                delete[] arrayOfRequests;
            } 

            delete[] arrayOfStrings;
            break;

        }else if(command3 == arrayOfStrings[0]){
            
            //addVaccinationRecords...

            if(counter != 2){
                cout << "Sorry, \"addVaccinationRecords\" requires 1 parameter (country)\n";
                delete[] arrayOfStrings;
                continue;
            }

            // user provided country... I need to find which monitor has this country
            // so I can send a SIGUSR1 signal...
            
            string countryFrom = arrayOfStrings[1];
            int keep = -1;
            int monitor;
            for(int i = 0; i < numOfCountries; i++){
                if(arrayOfCountries[i] == countryFrom){
                    keep = i;
                }
            }

            if(keep == -1){
                cout << "Sorry, \"CountryFrom\" does not exist in system. Try another one.\n";
                delete[] arrayOfStrings;
                continue;
            }

            // determine which monitor contains this country...
            if(keep <= numMonitors){
                monitor = keep;
            }else{
                // keep > numMonitors
                monitor = keep % numMonitors;
            }

            //send command
            sendString(bufferSize, arrayOfStrings[0].c_str(), arrayOfSock[monitor]);

            // send the country...
            sendString(bufferSize, arrayOfStrings[1].c_str(), arrayOfSock[monitor]);
            
            // meanwhile, the monitor checks the catalog for newly inserted files...
            // it returns a YES or NO depending on weather it found new records or not...
            char* proceed;
            receiveString(arrayOfSock[monitor], &proceed);

            if(strcmp(proceed, "NO") == 0){
                cout << "No new records were inserted\n";
                free(proceed);
                delete[] arrayOfStrings;
                continue;
            }
            free(proceed);
        
            // after monitor has updated the bloom filters...
            // i should receive them...

            int cc = 0;
            while(1){
                char* nameOfVirus;
                char* ack = "ACK";
                
                // nameOfVirus... 
                     
                receiveString(arrayOfSock[monitor], &nameOfVirus);
                if(strcmp(nameOfVirus, "DONE") == 0){
                    free(nameOfVirus);
                    break;
                }

                // find the bloomfilter of this virus if there exists one...
                bloomFilter** respectiveBloom;
                int found = 0;
                for(int i = 0; i < bloomsPerMonitor[monitor]; i++){

                    if(strcmp(arrayOfVirusesPerMonitor[monitor][i], nameOfVirus)==0){
                        // found the respective bloom filter...
                        found = 1;
                        free(bloomsArray[monitor][i]);
                        bloomsArray[monitor][i] = (uint8_t*) malloc(sizeof(uint8_t*)*sizeOfBloom);

                        // lets receive the bloom...
                        int counter = 0;
                        int c = 0;
                        
                        // send ack so monitor can begin sending you the bloom...
                        sendString(bufferSize, ack, arrayOfSock[monitor]);

                        // receive bloom
                        while(1){
                            if(read(arrayOfSock[monitor], &counter, sizeof(int)) == -1){
                                perror("Problem when reading... ");
                                exit(1);
                            }
                            
                            if(read(arrayOfSock[monitor], &bloomsArray[monitor][i][c], sizeof(uint8_t)*counter) == -1){
                                perror("Problem when reading... ");
                                exit(1);
                            }
                            c+=counter;
                            if(c == sizeOfBloom) break;
                        }

                        // done receiving the monitor...
                        cc++;
                        break;
                    }
                }

                if(found){
                    sendString(bufferSize, ack, arrayOfSock[monitor]);
                    free(nameOfVirus);
                    continue;
                }
                // if this virus does not exist...
                
                // new virus...
                // should increase the bloomFilters array by one...
                // also the virus array by one...

                char** temp = (char**)malloc(sizeof(char**)*(bloomsPerMonitor[monitor]+1));
                int i;
                for(i = 0; i < bloomsPerMonitor[monitor]; i++){
                    // bloomsPerMonitor has the number of blooms..
                    // each monitor has -> the number of viruses...
                    temp[i] = arrayOfVirusesPerMonitor[monitor][i]; 
                }

                temp[i] = (char*) malloc(sizeof(char*)*(strlen(nameOfVirus)+1));
                strcpy(temp[i], nameOfVirus);

                free(arrayOfVirusesPerMonitor[monitor]);
                arrayOfVirusesPerMonitor[monitor] = temp;

                uint8_t** tempUint = (uint8_t**)malloc(sizeof(uint8_t**)*(bloomsPerMonitor[monitor]+1));
                for(i = 0; i < bloomsPerMonitor[monitor]; i++){
                    // bloomsPerMonitor has the number of blooms each monitor has -> the number of viruses...
                    tempUint[i] = bloomsArray[monitor][i]; 
                }
                bloomsPerMonitor[monitor]++;

                // send ack so monitor can start sending...
                int c = 0;
                int counter =0;
                tempUint[i] = (uint8_t*) malloc(sizeof(uint8_t*)*sizeOfBloom);
                
                sendString(bufferSize, "ACK", arrayOfSock[monitor]);
                while(1){
                    if(read(arrayOfSock[monitor], &counter, sizeof(int)) == -1){
                        perror("Problem when reading ");
                        exit(1);
                    }
                    
                    if(read(arrayOfSock[monitor], &tempUint[i][c], sizeof(uint8_t)*counter) == -1){
                        perror("Problem when reading");
                        exit(1);
                    }
                    c+=counter;
                    if(c == sizeOfBloom) break;
                }

                free(bloomsArray[monitor]);
                bloomsArray[monitor] = tempUint;
                // send ack...
                sendString(bufferSize, ack, arrayOfSock[monitor]);
                free(nameOfVirus);
            }
            delete[] arrayOfStrings;
            continue;
        }else{
            cout << "Did not quite catch that, try again\n";
            delete[] arrayOfStrings;
            continue;
        }
    }
}

int receiveString(int readfd, char** ptr){
    // receive total number Of Bytes...
    int c = 0;
    int numOfBytes;
    int bytesToReceive;
    char* command;

    if(read(readfd, &numOfBytes, sizeof(int)) == -1){
        perror("Problem when reading a");
        exit(1);
    }

    *ptr = (char*) malloc(sizeof(char*) * numOfBytes);
    
    while(1){
        if(read(readfd, &bytesToReceive, sizeof(int)) == -1){
            perror("Problem when reading b");
            exit(1);
        }
        if(read(readfd, &(*ptr)[c], sizeof(char)*bytesToReceive) == -1){
            perror("Problem when reading c");
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
                if(write(writefd, &(str[bytesWritten]), sizeof(char)*bytesLeft) == -1){
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

