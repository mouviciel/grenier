#include <stdlib.h>
#include "obj.h"



// Full (public+private) declaration of `obj` class
struct obj_full {
  struct obj public;
  char * description;
};

static void obj_release ( struct obj * self )
{
  free ( self );
}

struct obj * obj_new ( const char * description )
{
  struct obj_full * self = malloc ( sizeof ( struct obj_full ) );
  if ( !self ) return NULL;
  self->public.release = obj_release;
  return &self->public;
}



// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
