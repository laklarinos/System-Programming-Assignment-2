
#include "person.h"
#include <iostream>  
#include <string>
using namespace std;

        void person::initCountry(string* str){
            this->country = str;

        }
        void person::initVirus(string* str){
            //this->virus = str;
            this->virusArray = new string*[1];
            this->virusArray[0] = str;
            this->numViruses = 1;
        }

        date* person:: getDateOfVaccination(string virus){
            for(int i = 0; i < this->numViruses; i++){                
                if(*(this->virusArray[i]) == virus){
                    return this->arrayOfVaccDates[i];
                }
            }
        }

        int person::updateArrays(string* newVirus, string vaccinated, date* vaccDate){
            
            // this method is used to initialize 
            int prevNumViruses = this->numViruses;
            this->numViruses++;
            string** newArray = new string*[this->numViruses];
            int i = 0; 

            if(this->numViruses > 1){
                for(i = 0; i < this->numViruses - 1; i++){
                    if(*newVirus == *(this->virusArray[i])){           
                        if(this->arrayVaccinated[i] == "NO"){
                            this->numViruses--;
                            this->arrayVaccinated[i] = vaccinated;
                            delete this->arrayOfVaccDates[i];
                            this->arrayOfVaccDates[i] = vaccDate;
                            delete[] newArray;
                            return 1;
                        }
                    }
                    newArray[i] = this->virusArray[i];
                }
            }

            newArray[i] = newVirus;
            delete [] this->virusArray;
            this->virusArray = NULL;
            this->virusArray = newArray;

            string* newVaccArray = new string[this->numViruses];
            for(i = 0; i < prevNumViruses; i++){
                newVaccArray[i] = this->arrayVaccinated[i];
            }
            newVaccArray[i] = vaccinated;
            
            delete [] this->arrayVaccinated;
            this->arrayVaccinated = NULL;
            this->arrayVaccinated = newVaccArray;

            date** newVaccDate = new date*[this->numViruses];

            for(i = 0; i < prevNumViruses; i++){
                newVaccDate[i] = this->arrayOfVaccDates[i];
            }

            if(vaccDate == NULL){
                date temp(0,0,0000);
                vaccDate = new date(temp);
                newVaccDate[i] = vaccDate;
            }else{
                date* d = new date(vaccDate);
                newVaccDate[i] = d; 
            }

            delete [] this->arrayOfVaccDates;
            this->arrayOfVaccDates = NULL;
            this->arrayOfVaccDates = newVaccDate;

            return 1;

        }

        person:: person(person* pr){

            this->id = pr->id;
            this->name = pr->name;
            this->surname = pr->surname;
            this->country = pr->country;
            this->age = pr->age;
            this->numViruses = pr->numViruses;
            this->virusArray = new string*[pr->numViruses];
            this->arrayVaccinated = new string[pr->numViruses];
            this->arrayOfVaccDates = new date*[pr->numViruses];
            
            if(pr->dateOfVaccination != NULL){
                this->dateOfVaccination = new date(pr->dateOfVaccination);
            }else{
                this->dateOfVaccination = NULL;
            }
            
            for(int i = 0; i < this->numViruses; i++){
                this->virusArray[i] = pr->virusArray[i];
                this->arrayVaccinated[i] = pr->arrayVaccinated[i];
                this->arrayOfVaccDates[i] = new date(pr->arrayOfVaccDates[i]);
            }

        }

        person::person(int id, string& name, string& surname, int age, string vaccinated,
             string *virus, string *country, date* dateOfVaccination){
             
             this->id = id;
             this->name = name;
             this->age = age;
             this->surname = surname;

             if(country!=NULL && virus!=NULL){
                this->country = country;
                //this->virus = virus;
                this->virusArray = new string*[1];
                this->virusArray[0] = virus;
                this->numViruses = 1;
             }

             //this->vaccinated = vaccinated;
             this->arrayVaccinated = new string[1];
             this->arrayVaccinated[0] = vaccinated;

             if(dateOfVaccination != NULL){
                // in other words, if dateOfVaccination is initialized...
                this->dateOfVaccination = new date(*dateOfVaccination);
                this->arrayOfVaccDates = new date*[1];
                this->arrayOfVaccDates[0] = dateOfVaccination;

             }else{

                this->dateOfVaccination = NULL;
                date temp(0,0,0000);
                date* tempDate = new date(temp);
                this->arrayOfVaccDates = new date*[1];
                this->arrayOfVaccDates[0] = tempDate;
                 
             }

        } 

        date** person:: getVaccDateArray(){
            return this->arrayOfVaccDates;
        }
        string* person:: getVaccArray(){
            return this->arrayVaccinated;
        }

        string** person:: getArrayOfViruses(){
            return this->virusArray;
        }

        int person:: getNumViruses(){
            return this->numViruses;
        }

        int person::checkRecord(){
            
            if (this->id < 1 || (this->arrayVaccinated[0] == "NO" && this->dateOfVaccination != NULL) ){
                return 1;
            }
            return 0;
            
        }

        int person:: getId(){
            return this->id;
        }

        int person:: getAge(){
            return this->age;
        }

        string person:: getName(){
            return this->name;
        }

        string person:: getSurname(){
            return this->surname;
        }

        string* person:: getCountry(){
            return this->country;
        }

        person* person:: printData(){
            cout<<"\n------- CITIZEN ID: " << this->id <<" INFO -------"<<endl;
            cout<<"NAME: "<<this->name<<endl;
            cout<<"SURNAME: "<<this->surname<<endl;
            cout<<"AGE: "<<this->age<<endl;
            if(this->country != NULL ) cout<<"COUNTRY: "<<*(this->country)<<endl;
                for(int i = 0; i < this->numViruses; i++){
                    if(i>0)cout <<"---------------\n";
                    cout <<"VIRUS #"<<i<<" :"<<*(this->virusArray[i])<<endl;
                    cout <<"VACCINATED: "<<this->arrayVaccinated[i]<<endl;
                    if(this->arrayVaccinated[i] == "YES"){
                        this->arrayOfVaccDates[i]->printDate();
                        cout <<endl;
                    }
                }
            //} 
            cout << endl;
        }

        void person::printVaccDate(string str){
            
            if(this->arrayOfVaccDates == NULL){ 
                cout << "Sorry, citizen: "<<this->id
                <<" does not have a vacc date for virus: "
                <<str<<endl;
                return;
            }
            for(int i = 0; i < this->numViruses; i++){
                if(*(this->virusArray[i]) == str){
                    this->arrayOfVaccDates[i]->printDate();
                    return;
                }
            }
        }   

        person::~person(){
            if(this->virusArray != NULL){

                delete [] this->virusArray;
                this->virusArray = NULL;
                for(int i = 0; i < numViruses; i++){
                    delete this->arrayOfVaccDates[i];
                }
                
                delete [] this->arrayOfVaccDates;
                this->arrayOfVaccDates == NULL;
                delete [] this->arrayVaccinated;
                this->arrayVaccinated = NULL;
            }

            if(this->dateOfVaccination != NULL){
                delete this->dateOfVaccination;
            }
            
        }
