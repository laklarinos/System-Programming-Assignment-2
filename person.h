#ifndef PERSON_H
#define PERSON_H
#include <iostream>  
#include <string>
#include "date.h"
using namespace std;
    
    class person{

        private: 
            int id;
            string name;
            string surname;
            int age;
            int numViruses;
            string* country; // must be pointer to country
            //string* virus; // must be pointer to virus
            
        public:
            string** virusArray; // more thank 1 viruses
            date** arrayOfVaccDates; // more thank 2 dates
            string* arrayVaccinated; // more than 2 vaccinated 
            //string vaccinated;
            date* dateOfVaccination;

            // helpers...
            void initCountry(string* str);
            date* getDateOfVaccination(string virus);
            int getNumViruses();
            string** getArrayOfViruses();
            date** getVaccDateArray();
            string* getVaccArray();
            int updateArrays(string* newVirus, string vaccinated, date* vaccDate = NULL);
            void initVirus(string* str);
            int getId();
            int checkRecord();
            int getAge();
            string getName();
            string getSurname();
            string* getCountry();
            
            // printing
            person* printData();
            void printVaccDate(string str);

            //constructors
            person(int id, string& name, string& surname, int age, string vaccinated,
             string *virus=NULL,string *country=NULL,date* dateV = NULL);
            person(person*);
            ~person();
    
    };
#endif