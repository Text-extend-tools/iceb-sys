/* $Id: nastrmariq.c,v 5.25 2013/05/17 14:56:06 sasa Exp $ */
/*16.04.2017	29.01.2000	Белых А.И.	nastrmariq.c
Чтение настроек для работы с кассовым регистратором
*/
#include        <errno.h>
#include	<ctype.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        <pwd.h>
#include	"buhg.h"
#ifdef	HAVE_UNISTD_H
#include        <unistd.h>
#endif


#define MAXKASN	16
int	nastrmariq
   /* входные параметры 	*/
   (int autoid,		// 1 - автовыбор по ид. раб. места; 0 - меню
   /* выходные параметры 	*/
    KASSA *kasr)
{
char	bros[512];
char	buf[512], buf1[512];
int 	kom;
KASSA	kassa[MAXKASN];
int i, j, ret;
char *whost=NULL;
struct passwd *ktor;
char	*p;
int 	kassano;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

if ((p = getenv("KASSANO")) != NULL) 
  kassano = atoi(p);
else 
  kassano = -1;

ktor = getpwuid(getuid());
kasr->prodavec = ktor->pw_name;
//sprintf(buf,"%s/kassa.alx",confpath);
sprintf(buf,"kassa.alx");
const char *imafil={"kassa.alx"};

if (autoid == 0) 
  clear();

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imafil);
  iceb_t_soob(strsql);
  return(1);
 }


VVOD MENU(3);
if (autoid != 0) 
 {	/* автоматический выбор кассы 	*/
  if ((kassano == -1) && ((whost=workid()) == NULL)) 
   {
    return(1);

   }
 } 
else 
 {
  MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите кассовый регистратор:"));
  MENU.VVOD_SPISOK_add_ZAG("-------------+----------------------+------------");
  MENU.VVOD_SPISOK_add_ZAG(gettext(" склад-номер |         хост         |    порт    "));
 }


i = 0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if (i >= MAXKASN) 
    break;
//  printw("%s",row_alx[0]);
  if((row_alx[0][0] == '#') || (strlen(row_alx[0]) == 0) || isspace(row_alx[0][0]))
        continue;

  if(polen(row_alx[0],buf1,sizeof(buf1),2,'#') != 0)
    continue;

  polen(row_alx[0],bros,sizeof(bros),1,'#');
  
  polen(bros,buf,sizeof(buf),1,'|');
  kassa[i].sklad = atoi(buf);
  polen(bros,buf,sizeof(buf),2,'|');
  kassa[i].nomer = atoi(buf);
  if (autoid != 0)
   {
    if (kassano != -1) 
     {
      if (kassa[i].nomer != kassano) 
        continue;
     }
    else 
      if (SRAV(whost,buf1,1) != 0) 
        continue;
   }
  POLE(bros,buf,3,'|');
  kassa[i].host = (char*) malloc(strlen(buf)+1);
  strcpy(kassa[i].host, buf);
  
  POLE(bros,buf,4,'|');
  if ((j = strlen(buf)) != 0) 
   {
    kassa[i].port = (char*) malloc(strlen(buf)+1);
    strcpy(kassa[i].port, buf);
   }
  else kassa[i].port = NULL;
  
  POLE(bros,buf,5,'|');
  kassa[i].parol = (char*) malloc(strlen(buf)+1);
  strcpy(kassa[i].parol, buf);
  if (autoid == 0) 
   {
     //            | склад-номер | хост                 | порт       |
     sprintf(buf, "    %2d-%2d    | %20s | %10s ", kassa[i].sklad, kassa[i].nomer, kassa[i].host, kassa[i].port);
     MENU.VVOD_SPISOK_add_MD(buf);
   }
  i++;
 }


if(i == 0)
 return(1);
 
i--;

if (autoid) 
  ret = kom = 0;
else 
 {
  menu3(&MENU,&kom,1,0,0);
  ret = (kom == -1) ? 1 : 0;
 }

while (i>=0) 
 {
  if ((i == kom) && (ret == 0)) 
   {
    kasr->host = kassa[i].host;
    kasr->port = kassa[i].port;
    kasr->parol = kassa[i].parol;
    kasr->sklad = kassa[i].sklad;
    kasr->nomer = kassa[i].nomer;
   }
  else 
   {
    free(kassa[i].host);
    free(kassa[i].port);
    free(kassa[i].parol);
   }
  i--;
 }
return(ret);
}
