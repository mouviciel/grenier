#ifndef NUMBER_H
#define NUMBER_H



#include "Obj.h"
#include <math.h> // Contains the definition of nan(), used below



// BRIEF //////////////////////////////////////////////////////////////////////



// The 'Number' class
typedef struct Number * Number;

// Constructor (with allocation) of an instance of 'Number'
Number Number_create ( void );



// DETAILS ////////////////////////////////////////////////////////////////////



// The 'Number_interface' interface
typedef const struct Number_interface * Number_interface;

// The 'Number_storage' data storage
typedef struct Number_storage * Number_storage;

// Definition of the 'Number' class
struct Number
{
   struct Obj super;
   Number_interface vtable;
   Number_storage data;
};

// Definition of the 'Number_interface' interface
struct Number_interface
{
   double ( * value ) ( Number self );
   void ( * set_value ) ( Number self, double value );
};

// Get the value of an instance of 'Number'
static inline double Number_value ( Number self )
{
   if ( self && self->vtable && self->vtable->value )
      return self->vtable->value ( self );
   return nan ( "0" );
}

// Set the value of an instance of 'Number'
static inline void Number_set_value ( Number self, double value )
{
   if ( self && self->vtable && self->vtable->set_value )
      self->vtable->set_value ( self, value );
}



#endif // NUMBER_H
