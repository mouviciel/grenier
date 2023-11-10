#ifndef OBJ_H
#define OBJ_H



// `obj` class
struct obj;

// Class methods of `obj`
struct obj * obj_new ( const char * description );

// Public methods of an `obj` instance
struct obj {
  void ( * release ) ( struct obj * );
  const char * ( * description ) ( struct obj * );
  int ( * set_description ) ( struct obj *, const char * );
};



#endif // OBJ_H

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
