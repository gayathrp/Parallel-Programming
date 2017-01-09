//
// Created by Gayathri Palanisami on 12/12/16.
//

#ifndef SORTING_MYPLACE_H
#define SORTING_MYPLACE_H

#include <string.h>
#include "Place.h"

class MyPlace : public Place{
public:
    MyPlace(void *argument):MyPlace(argument){

    }

static const int init_ = 0;
static const int findMinTemperature_ =1;

virtual void *callMethod(int functionId, void*argument){
    switch(functionId){
        case init_ :
            return init(argument);
        case findMinTemperature_:
            return findMinTemperature(argument);
    }
    return NULL;
};

private:
void *init(void *argument);
void *findMinTemperature(void *argument);
};

#endif //SORTING_MYPLACE_H
