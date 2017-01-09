#include "MASS.h"
#include "Timer.h"	//Timer
#include <stdlib.h> // atoi
#include <unistd.h>
#include <vector>
#include "MyPlace.h"
const int iterPerTurn = 9;							//Partitioned movespaces are 9 squares large
const bool printOutput = true;

Timer timer;

int main( int argc, char *args[] ) {

    // check that all arguments are present
    // exit with an error if they are not
    if ( argc != 8 ) {
        cerr << "usage: ./main username password machinefile port nProc nThreads size" << endl;
        return -1;
    }

    // get the arguments passed in
    char *arguments[4];
    arguments[0] = args[1]; // username
    arguments[1] = args[2]; // password
    arguments[2] = args[3]; // machinefile
    arguments[3] = args[4]; // port
    int nProc = atoi( args[5] ); // number of processes
    int nThr = atoi( args[6] );  // number of threads

    const int size = atoi( args[7] );
    // initialize MASS with the machine information,
    // number of processes, and number of threads
    MASS::init( arguments, nProc, nThr );
    // prepare a message for the places (this is argument below)
    char *msg = (char *)("hello\0"); // should not be char msg[]

    /*  THIS SECTION OF CODE DEALS ONLY WITH PLACES  */
    
// Create the places.
    // Arguments are, in order:
    //    handle, className, boundary_width, argument, argument_size, dim, ...
    Places *myplaces = new Places( 1, "MyPlace",  msg, 6, 1, size );



//    myplaces->callAll(MyPlace::init_);				//initialize Lands

    timer.start();


    myplaces->callAll(MyPlace::findMinTemperature_);	// display the current population
    long elaspedTime_END =  timer.lap();
    printf( "\nEnd of simulation. Elasped time using MASS framework with %i processes and %i thread :: %ld \n",nProc,nThr, elaspedTime_END);

    MASS::finish( );
}

