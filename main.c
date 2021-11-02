#include "grenier.h"
#include <stdio.h>


int main (int argc, char * argv[])
{
  printf ("%s \n", isPicture (argv[1]) ? "yes" : "no");
  return 0;
}
