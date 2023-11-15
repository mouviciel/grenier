#include <stdlib.h>
#include <string.h>
#include "obj.h"



// Return a string representation of an `obj` instance
static const char * static_obj_string ( struct obj const * self )
{
  return "<obj>";
}

// Destructor (with deallocation) of an `obj` instance
static void static_obj_destroy ( struct obj * self )
{
  self->vtable = NULL;
  free ( self );
}



// Constructor (with allocation) of an `obj` instance
struct obj * obj_create ( void )
{
  static struct obj_vtable const vtable =
  {
    .string = static_obj_string,
    .destroy = static_obj_destroy,
  };
  struct obj * self = malloc ( sizeof ( struct obj ) );
  if ( self )
  {
    self->vtable = &vtable;
  }
  return self;
}

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
