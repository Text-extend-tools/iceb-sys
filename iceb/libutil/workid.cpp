/*$Id: workid.c,v 5.11 2013/05/17 14:56:43 sasa Exp $*/

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utmp.h>
#include "util.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif

#define DEV_DIR_WITH_TRAILING_SLASH "/dev/"
#define DEV_DIR_LEN (sizeof (DEV_DIR_WITH_TRAILING_SLASH) - 1)

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

/*
 * Функция workid возвращает указатель на строку, идентифицирующую
 * рабочее место, на котором запущена программа, в формате:
 *
 * хост|удаленный_хост|терминал
 *   |         |            
 *   |         +--- откуда зашли telnet'ом
 *   |
 *   +--- на котором запущена программа
 *
 * либо NULL, если идентификация невозможна.
 *
 */

char *workid() {
   struct utmp *u;
   char *ttyname_b = NULL;
   char host[HOST_NAME_MAX];
//   int n = 0;
   int found = 0;
   
   host[0] = '\0';
   gethostname(host, HOST_NAME_MAX-1);
   ttyname_b = ttyname (0);
   if (!ttyname_b)
      return(NULL);
   if (strncmp (ttyname_b, DEV_DIR_WITH_TRAILING_SLASH, DEV_DIR_LEN) == 0)
      ttyname_b += DEV_DIR_LEN;	/* Discard /dev/ prefix.  */

   while ((u=getutent()))
      if (u->ut_type == USER_PROCESS ) 
         if (strcmp(u->ut_line, ttyname_b) == 0) {
            found++;
            break;
         }

   if (found) {
      found=strlen(host)+1+strlen(u->ut_host)+1+strlen(u->ut_line)+1;
      ttyname_b = (char*) malloc(found);
      sprintf(ttyname_b, "%s|%s|%s", host, u->ut_host, u->ut_line);
   }
   else ttyname_b = NULL;
   return(ttyname_b);
}
