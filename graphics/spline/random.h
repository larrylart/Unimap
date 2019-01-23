/****************************************
 * 
 * By Bill Perone (billperone@yahoo.com)
 * Original: 09-12-2003  
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 * 
 * Special Thanks to Robert Aldridge and Kenneth H. Rosen
 *
 * Notes:
 *   modulus_m = 2^31 - 1 = 2147483647                                       
 *   multiplier_a = 7^5 = 16807                                              
 *   increment_c = 0                                                         
 *   seed_x0 = x(0) = seed by client
 *   rand() = ( x(n+1) = ( multiplier_a * x(n) + increment_c ) % modulus_m ) 
 ****************************************/


#pragma once


#define RANDOMMAX        2147483646
#define RANDOMMULTIPLIER 16807 


class random
{
private:

   #if defined( _MSC_VER )
      typedef unsigned __int64 uint64;
   #else
      typedef unsigned long long uint64;
   #endif

   unsigned int seed;

public:

	random(unsigned int newSeed) : seed(newSeed) {}   

	// returns the maximum value possible of this generator
    unsigned int max() const 
    {  return RANDOMMAX;  }

	// updates the seed value and returns
    unsigned int get()
    {  return seed= (unsigned int)( ((random::uint64) RANDOMMULTIPLIER*(random::uint64)seed) % (random::uint64)(RANDOMMAX+1) );  }

    // returns current random number
    unsigned int current() const
    {  return seed;  }	
    
    // returns current random number, as a double in the range [0,1]
    double currentReal() const
    {  return (double)seed/RANDOMMAX;  }
};