#include "travelRequest.h"

travelRequest:: travelRequest(date* dateOfTrip, string countryFrom, string countryTo, string virus, int status, int monitor){
    this->dateOfTrip = new date(dateOfTrip);
    this->countryFrom = countryFrom;
    this->countryTo = countryTo;
    this->virus = virus;
    this->status = status;
}

travelRequest:: travelRequest(){
    this->dateOfTrip = NULL;
    this->countryFrom = "";
    this->countryTo = "";
    this->virus = "";
    this->status = -1;
}

travelRequest:: ~travelRequest(){
    delete this->dateOfTrip;
}