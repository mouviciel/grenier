#include "Obj.h"
#include <stdlib.h>



// Definition of the 'Obj_storage' data storage
struct Obj_storage
{
   const struct dummy * dummy;
};



// Destructor (with release) of an instance of 'Obj'
static void static_Obj_destroy ( Obj self )
{
   if ( self )
   {
      // Clear the vtable
      self->vtable = NULL;

      // Release the storage space
      if ( self->data )
      {
         free ( self->data );
         self->data = NULL;
      }

      // Release the object itself
      free ( self );
   }
}



// String representation of an instance of 'Obj'
static const char * static_Obj_string ( Obj self )
{
   return "<Obj>";
}



// Constructor (with allocation) of an instance of 'Obj'
Obj Obj_create ( void )
{
   // Allocate a new instance
   Obj self = malloc ( sizeof ( struct Obj ) );
   if ( ! self ) return NULL;

   // Allocate a new storage space
   self->data = malloc ( sizeof ( struct Obj_storage ) );
   if ( ! self->data )
   {
      free ( self );
      return NULL;
   }
   self->data->dummy = NULL;

   // Instantiate the interface
   static const struct Obj_interface vtable =
   {
      .destroy = static_Obj_destroy,
      .string = static_Obj_string,
   };
   self->vtable = & vtable;

   // 'Obj' instance is ready
   return self;
}
