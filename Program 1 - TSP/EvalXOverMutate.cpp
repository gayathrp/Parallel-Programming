//////////Parallelizing Travelling Salesman with Open MP//////////////

#include <cstring>
#include <iostream>
#include "Trip.h"
#include <cmath>
#include <stdlib.h> 
#include <string.h>
using namespace std;

// Compare two distances of trips and returns value for qsort method

int compareMyType (const void * T1, const void * T2)
{
  if ( ((Trip*)T1)->fitness <  ((Trip*)T2)->fitness ) return -1;
  if ( ((Trip*)T1)->fitness == ((Trip*)T2)->fitness ) return 0;
  if ( ((Trip*)T1)->fitness >  ((Trip*)T2)->fitness ) return 1;
}

//////////////////////////////EVALUATE////////////////////////////////
// This method calculates distance of each trip and sorts 

void evaluate( Trip trip[CHROMOSOMES], int coordinates[CITIES][2] )
{
#pragma omp parallel for
for(int i = 0 ; i < CHROMOSOMES ; i++ )
{

int indexA = ( trip[i].itinerary[0] >= 'A') ? trip[i].itinerary[0] - 'A' : trip[i].itinerary[0] - '0' + 26;
int distancex0 = (coordinates[indexA][0] - 0) * (coordinates[indexA][0] - 0);
int distancey0 = (coordinates[indexA][1] - 0) * (coordinates[indexA][1] - 0);
double fitness =  sqrt(distancex0 + distancey0);   
for(int j = 1 ; j < CITIES ; j++)
{
int index = ( trip[i].itinerary[j-1] >= 'A') ? trip[i].itinerary[j-1] - 'A' : trip[i].itinerary[j-1] - '0' + 26;
int index1 = ( trip[i].itinerary[j] >= 'A' ) ? trip[i].itinerary[j] - 'A' : trip[i].itinerary[j] - '0' + 26;
int distancex = (coordinates[index1][0] - coordinates[index][0]) * (coordinates[index1][0] - coordinates[index][0]);
int distancey = (coordinates[index1][1] - coordinates[index][1]) * (coordinates[index1][1] - coordinates[index][1]);
fitness = fitness + sqrt(distancex + distancey);
}
trip[i].fitness = fitness;
}
qsort (trip, CHROMOSOMES, sizeof(trip[0]), compareMyType);
}

///////////////////////////////CROSSOVER//////////////////////////////
// getNextCity() method determines the next city of a given trip

char getNextCity( char Parents[CITIES], char Offspring)
{
for(int i = 0 ; i < CITIES-1; i++)
{
if(Parents[i] == Offspring)
{
return Parents[i+1];
}
}
return Parents[0];
}

// Generates random city

char generateRandomCity( ) {
  char city = rand( ) % CITIES;
  if ( city < 26 )
    city += 'A';
  else
    city = city - 26 + '0';
  return city;
}

// Compliments offsprings

void complement(char offsprings[CITIES+1], char offspringsc[CITIES+1])
{
        std::string comString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        int length = comString.length();
	for(int i = 0 ; i < length ; i++)
	{
        for(int j = 0 ; j < CITIES ; j++)
        {
                if(offsprings[j] == comString.at(i))
                {
                        offspringsc[j] = comString.at(length-i-1);
                }

        }
	}

}

// This method creates offsprings using parents[i] and parents[i+1] and complements the offsprings

void crossover( Trip parents[TOP_X], Trip offsprings[TOP_X], int coordinates[CITIES][2] )
{

// Parallelizes using Open MP

#pragma omp parallel for
for(int i = 0 ; i < TOP_X ; i+=2)
{
memset(offsprings[i].itinerary,0,CITIES);
offsprings[i].itinerary[0] = parents[i].itinerary[0];
for(int j = 1 ; j < CITIES ; j++)
{
char nextCity1 = getNextCity(parents[i].itinerary, offsprings[i].itinerary[j-1]);
char nextCity2 = getNextCity(parents[i+1].itinerary, offsprings[i].itinerary[j-1]);
int index = ( parents[i].itinerary[j-1] >= 'A' ) ? parents[i].itinerary[j-1] - 'A' : parents[i].itinerary[j-1] - '0' + 26;
int index1 = ( nextCity1  >= 'A' ) ?  nextCity1 - 'A' :  nextCity1 - '0' + 26;
int index2 = ( nextCity2 >= 'A') ?  nextCity2 - 'A' :  nextCity2 - '0' + 26;
int distancex1 = (coordinates[index1][0] - coordinates[index][0]) * (coordinates[index1][0] - coordinates[index][0]);
int distancey1 = (coordinates[index1][1] - coordinates[index][1]) * (coordinates[index1][1] - coordinates[index][1]);
double distance1 = sqrt(distancex1 + distancey1);
int distancex2 = (coordinates[index2][0] - coordinates[index][0]) * (coordinates[index2][0] - coordinates[index][0]);
int distancey2 = (coordinates[index2][1] - coordinates[index][1]) * (coordinates[index2][1] - coordinates[index][1]);
double distance2 = sqrt(distancex2 + distancey2);

// if next city of parents i and i+1 already available in offspring

if(strchr(offsprings[i].itinerary, nextCity1) && strchr(offsprings[i].itinerary, nextCity2))
{
      char randomCity = generateRandomCity();
        while(strchr(offsprings[i].itinerary, randomCity))
        {
		randomCity = generateRandomCity();               
        }
offsprings[i].itinerary[j] = randomCity;
}


// if distance from parent i and its next city is smaller, insert parent i’s next city 

else if(distance1<distance2)
{
  if(strchr(offsprings[i].itinerary, nextCity1))
  {
    offsprings[i].itinerary[j] = nextCity2;
  }
  else
  {
    offsprings[i].itinerary[j] = nextCity1;
  }
}


// if distance from parent i+1 and its next city is smaller, insert parent i+1’s next city 

else
{
  if(strchr(offsprings[i].itinerary, nextCity2))
  {
    offsprings[i].itinerary[j] = nextCity1;
  }
  else
  {
    offsprings[i].itinerary[j] = nextCity2;
  }
}
}
complement(offsprings[i].itinerary,offsprings[i+1].itinerary);
}
}

///////////////////////////////MUTATE/////////////////////////////////
// Swaps two random cities in a trip

void mutate( Trip offsprings[TOP_X] )
{
for(int i = 0; i < TOP_X ; i++)
{
if(rand() % 100 < MUTATE_RATE)
{
	int index1 = rand()%CITIES;
	int index2 = rand()%CITIES;
	char temp = offsprings[i].itinerary[index1];
	offsprings[i].itinerary[index1] = offsprings[i].itinerary[index2];
	offsprings[i].itinerary[index2] = temp;	
}
}
}
