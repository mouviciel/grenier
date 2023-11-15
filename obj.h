#ifndef OBJ_H
#define OBJ_H



// Constructor (with allocation) of an `obj` instance
struct obj * obj_create ( void );

// `obj` class
struct obj
{
  struct obj_vtable
  {
    const char * ( * string )  ( struct obj const * self );
    void         ( * destroy ) ( struct obj * self );
  } const * vtable;
};

// Methods of `obj` class

// Destructor (with deallocation) of an `obj` instance
static inline void obj_destroy ( struct obj * self )
{
  if ( self && self->vtable && self->vtable->destroy )
    self->vtable->destroy ( self );
}

// Return a string representation of an `obj` instance
static inline const char * obj_string ( struct obj const * self )
{
  if ( self && self->vtable && self->vtable->string )
    return self->vtable->string ( self );
  return NULL;
}



#endif // OBJ_H

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
