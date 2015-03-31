#include <stdio.h>
/* 
   run in a dev cmd prompt (Visual Studio Tools)
   compile with cl scratch_notes.c
   view results with scratch_notes.exe
*/
main()
{
  int a = 0x0B; /*1011*/
  printf("a = %d\n", a);
  int b = (a & 0xE) >> 1; /*E=1110*/
  printf("b = %d\n", b);
}

