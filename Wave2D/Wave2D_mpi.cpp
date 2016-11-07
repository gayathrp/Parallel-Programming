#include "mpi.h"
#include <iostream>
#include "Timer.h"
#include <stdlib.h>   // atoi
#include <omp.h>
int default_size = 100;  // the default system size
int defaultCellWidth = 8;
double c = 1.0;      // wave speed
double dt = 0.1;     // time quantum
double dd = 2.0;     // change in system
using namespace std;

int tMinusOne(int tm)
{
        int a;
        if(tm == 0) a = 2;
        if(tm == 1) a = 0;
        if(tm == 2) a = 1;
        return a;
}
int tMinusTwo(int tm)
{
        int b;
        if(tm == 0) b = 1;
        if(tm == 1) b = 2;
        if(tm == 2) b = 0;
        return b;
}
int main( int argc, char *argv[] ) {
  // verify arguments
  if ( argc != 5 ) {
    cerr << "usage: Wave2D size max_time interval" << endl;
    return -1;
  }
  int size = atoi( argv[1] );
  int max_time = atoi( argv[2] );
  int interval  = atoi( argv[3] );
  int thread = atoi( argv[4] );
  if ( size < 100 || max_time < 3 || interval < 0 ) {
    cerr << "usage: Wave2D size max_time interval" << endl;
    cerr << "       where size >= 100 && time >= 3 && interval >= 0" << endl;
    return -1;
  }
  omp_set_num_threads(thread);
  // create a simulation space
  double z[3][size][size];
  for ( int p = 0; p < 3; p++ ) 
    for ( int i = 0; i < size; i++ )
      for ( int j = 0; j < size; j++ )
	z[p][i][j] = 0.0; // no wave

  // start a timer
  Timer time;
  time.start( );

  // time = 0;
  // initialize the simulation space: calculate z[0][][]
  int weight = size / default_size;
for( int i = 0; i < size; i++ ) {
    for( int j = 0; j < size; j++ ) {
      if( i > 40 * weight && i < 60 * weight  &&
	  j > 40 * weight && j < 60 * weight ) {
	z[0][i][j] = 20.0;
      } else {
	z[0][i][j] = 0.0;
      }
    }
  }

  // time = 1
  // calculate z[1][][] 
  // cells not on edge
  for(int i = 0 ; i < size ; i++)
  {
	for(int j = 0 ; j < size ; j++)
	{
		if(i == 0 || i == size-1 || j == 0 || j == size-1){
			z[0][i][j] = 0.0;
		}		
		else {
			z[1][i][j] = z[0][i][j]+((c*c)/2)*0.0025*(z[0][i+1][j]+z[0][i-1][j]+z[0][i][j+1]+z[0][i][j-1]-4.0*z[0][i][j]);
     		}		
	}
  }

	int my_rank = 0;
	int mpi_size = 1;

	MPI_Init( &argc, &argv ); // start MPI
	MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
	MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );

int stripe = size / mpi_size;     // partitioned stripe

// simulate wave diffusion from time = 2 
for ( int t = 2; t < max_time; t++ ) {
  int tm = t%3;

	int a;
	a = tMinusOne(tm); // t-1
  	int b;
	b = tMinusTwo(tm); // t-2

if(interval!=0)
{
if(my_rank == 0)
{
        if(!(t%interval))
        {
                cout<< t <<endl;
                for(int i = 0 ; i < size ; i++)
                {
                        for(int j = 0 ; j < size ; j++)
                        {

                                cout<<z[tm][j][i] << " ";
                        }
                        cout << endl;
                }
        }
}
}
	
// Transferring boundary information to previous and next ranks
	MPI_Status status;
	if(my_rank == 0 && mpi_size > 1)
	{
  		MPI_Send(z[a][stripe-1],  size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  		MPI_Recv(z[a][stripe],  size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
	}
	else if(my_rank > 0 && my_rank < mpi_size-1)
	{
  		MPI_Recv(z[a][(my_rank*stripe)-1], size, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &status);
  		MPI_Send(z[a][my_rank*stripe], size, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD);
  		MPI_Send(z[a][(my_rank*stripe)+stripe-1], size, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD);
  		MPI_Recv(z[a][(my_rank*stripe)+stripe], size, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, &status);
	}
	else if(my_rank == mpi_size-1)
	{
  		MPI_Send(z[a][my_rank*stripe], size, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD);
  		MPI_Recv(z[a][(my_rank*stripe)-1], size, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &status);
	}
	for(int i = my_rank * stripe ; i < (my_rank * stripe)+stripe ; i++)
	{
        #pragma omp parallel for private(my_rank, stripe)
	for(int j = 0 ; j < size ; j++)
        {			
				// Compute Schroedinger's Wave Equation
				if(i != 0 && i != size-1 && j != 0 && j != size-1){ 
					z[tm][i][j] = 2.0*z[a][i][j]-z[b][i][j]+(c*c)*0.0025*(z[a][i+1][j]+z[a][i-1][j]+z[a][i][j+1]+z[a][i][j-1]-4.0*z[a][i][j]);
                        	}
				else
				{
					z[tm][i][j] = 0.0;
				}
        }
        }
	if(interval!=0)
	{
	if(my_rank == 0)
	{
		for(int i = 1 ; i < mpi_size ; i++)
		{
		MPI_Status status;
		MPI_Recv(z[tm][stripe * i], stripe * size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
        	}
	}
	else
	{
        	MPI_Send(z[tm][stripe * my_rank], stripe * size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	}
} // end of simulation
  MPI_Finalize();
  // finish the timer
  cerr << "Rank "<<my_rank<<" range is: "<<stripe*my_rank<<" ~ "<<(stripe*my_rank)+stripe-1<<endl;	
  if(my_rank == 0) {
  cerr << "Elapsed time = " << time.lap( ) << endl;
  }
  return 0;
}
