
#ifndef SORTING_MYPLACE_H
#define SORTING_MYPLACE_H

#include <string.h>
#include "Place.h"

class MyPlace : public Place{
public:
    MyPlace(void *argument) : Place(argument){

    }

static const int init_ = 0;
static const int findMinTemperature_ =1;

virtual void *callMethod(int functionId, void*argument){
    switch(functionId){
        case init_ :
            return init(argument);
        case findMinTemperature_:
            return findMinTemperature();
    }
    return NULL;
};

private:
void *init(void *argument);
void *findMinTemperature();
};

#endif //SORTING_MYPLACE_H
