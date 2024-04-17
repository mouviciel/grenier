#include <stdio.h>
#include <stdlib.h>



#include <Obj.h>
#include <Number.h>



int main ( int argc, char * argv[] )
{
   // Obj
   Obj myObj = Obj_create ();
   printf ( "myObj: %s\n", Obj_string ( (Obj)myObj ) );
   Obj_destroy ( (Obj)myObj );

   // Number
   Number myNumber = Number_create ();
   printf ( "myNumber: %s:%19.12E\n", Obj_string ( (Obj)myNumber ), Number_value ( myNumber ) );
   Number_set_value ( myNumber, 3.14 );
   printf ( "myNumber: %s:%19.12E\n", Obj_string ( (Obj)myNumber ), Number_value ( myNumber ) );
   Obj_destroy ( (Obj)myNumber );

   return EXIT_SUCCESS;
}
