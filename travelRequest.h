#ifndef _TRAVEL_REQ
#define _TRAVEL_REQ
#include "helpers.h"
class travelRequest{
    
    public:
        date* dateOfTrip;
        string countryTo;
        string countryFrom;
        string virus;
        int status;
        int monitor;
        travelRequest(date*, string, string, string, int, int);
        travelRequest();
        ~travelRequest();
};
#endif