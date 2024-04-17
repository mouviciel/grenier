#ifndef OBJ_H
#define OBJ_H



#include <stdlib.h> // Contains the definition of NULL, used below



// BRIEF //////////////////////////////////////////////////////////////////////



// The 'Obj' class
typedef struct Obj * Obj;

// Constructor (with allocation) of an instance of 'Obj'
Obj Obj_create ( void );



// DETAILS ////////////////////////////////////////////////////////////////////



// The 'Obj_interface' function interface
typedef const struct Obj_interface * Obj_interface;

// The 'Obj_storage' data storage
typedef struct Obj_storage * Obj_storage;

// Definition of the 'Obj' class
struct Obj
{
   Obj_interface vtable;
   Obj_storage data;
};

// Definition of the 'Obj_interface' interface
struct Obj_interface
{
   void ( * destroy ) ( Obj self );
   const char * ( * string ) ( Obj self );
};

// Destructor (with release) of an instance of 'Obj'
static inline void Obj_destroy ( Obj self )
{
   if ( self && self->vtable && self->vtable->destroy )
      self->vtable->destroy ( self );
}

// String representation of an instance of 'Obj'
static inline const char * Obj_string ( Obj self )
{
   if ( self && self->vtable && self->vtable->string )
      return self->vtable->string ( self );
   return NULL;
}



#endif // OBJ_H
