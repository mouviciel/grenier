#include <stdlib.h>
#include <string.h>
#include "obj.h"



// Full (public+private) declaration of `obj` class
struct obj_full {
  struct obj public;
  char * description;
};

static void obj_release ( struct obj * self )
{
  struct obj_full * full = ( struct obj_full * ) self;
  if ( full->description )
    free ( full->description );
  free ( full );
}

static const char * obj_description ( struct obj * self )
{
  if ( !self ) return NULL;
  struct obj_full * full = ( struct obj_full * ) self;
  return full->description;
}

static int obj_set_description ( struct obj * self,
    const char * description )
{
  if ( !self ) return -1;
  struct obj_full * full = ( struct obj_full * ) self;
  if ( full->description )
    free ( full->description );
  full->description = strdup ( description );
  if ( !full->description ) return -1;
  return 0;
}

struct obj * obj_new ( const char * description )
{
  struct obj_full * full = malloc ( sizeof ( struct obj_full ) );
  if ( !full ) return NULL;
  full->description = NULL;
  full->public.release = obj_release;
  full->public.description = obj_description;
  full->public.set_description = obj_set_description;
  return &full->public;
}



// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
