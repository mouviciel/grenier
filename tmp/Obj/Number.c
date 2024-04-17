#include <Obj.h>
#include <Number.h>
#include <stdlib.h>
#include <math.h>



// Definition of the 'Number_storage' data storage
struct Number_storage
{
   double value;
};



// Get the value of an instance of 'Number'
static double static_Number_value ( Number self )
{
   return self->data->value;
}



// Set the value of an instance of 'Number'
static void static_Number_set_value ( Number self, double value )
{
   self->data->value = value;
}



// String representation of an instance of 'Number'
static const char * static_Number_string ( Obj self )
{
   return "<Number>";
}



// Destructor (with release) of an instance of 'Obj'
static void static_Number_destroy ( Obj obj )
{
   Number self = ( Number ) obj;

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

      // Clear the vtable of the instance of the parent class
      self->super.vtable = NULL;

      // Release the object itself
      free ( self );
   }
}



// Constructor (with allocation) of an instance of 'Number'
Number Number_create ( void )
{
   // Allocate a new instance
   Number self = malloc ( sizeof ( struct Number ) );
   if ( ! self ) return NULL;

   // Create an instance of the parent class
   struct Obj * super = Obj_create ();
   if ( ! super )
   {
      free ( self );
      return NULL;
   }
   self->super = * super;
   Obj_destroy ( super );

   // Overwrite the vtable of the instance of the parent class
   static const struct Obj_interface super_vtable =
   {
      .destroy = static_Number_destroy,
      .string = static_Number_string,
   };
   self->super.vtable = & super_vtable;

   // Allocate a new storage space
   self->data = malloc ( sizeof ( struct Number_storage ) );
   if ( ! self->data )
   {
      free ( self );
      return NULL;
   }
   self->data->value = nan ( "0" );

   // Instantiate the interface
   static const struct Number_interface vtable =
   {
      .value = static_Number_value,
      .set_value = static_Number_set_value,
   };
   self->vtable = & vtable;

   // 'Number' instance is ready
   return self;
}
