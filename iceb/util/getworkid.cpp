/* getworkid.c - выводит на stdout строку, идентифицирующую
 *               рабочее место.
 *
 * Статус завершения: 0 - если все OK
 *                    1 - если идентификация невозможна
 */
#include <stdio.h>
#include <util.h>
#include <nl_types.h>

// 3 faked declarations to make linker happy
nl_catd		fils;
char		imabaz[40];
char		*putnansi;

int main() 
{
  char *p;
  if ((p = workid())) 
   {
    printf("%s\n", p);
    exit(0);
   }
  else 
   {
    fprintf(stderr, "Not found.\n");
    exit(1);
   }
}
