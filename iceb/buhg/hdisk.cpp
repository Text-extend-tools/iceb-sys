/*$Id: hdisk.c,v 5.9 2014/07/31 07:09:16 sasa Exp $*/
/*14.07.2015	28.08.2003	Белых А.И.	Белых А.И.
Добавление заголовка и концовки в файл который записывается на дискету для передачи в банк
*/
#include	<errno.h>
#include        "buhg.h"


void hdisk(int nomer,double itogoo,char *imafkartf)
{
struct tm *bf;
char imaftmp[64];
FILE *fftmp;
FILE *ffkartf;
char strsql[1024];
time_t  vrem;

time(&vrem);
bf=localtime(&vrem);


sprintf(imaftmp,"pib%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }

if((ffkartf = fopen(imafkartf,"r")) == NULL)
 {
  error_op_nfil(imafkartf,errno,"");
  return;
 }

fprintf(fftmp,"\
CODE: 048\n\
FILE: UOBLUAAM.F\n\
DATE: %02d/%02d/%04d\n\
CURR: 980\n\
CARD: %d\n\
AMNT: %.2f\n\
TEXT:\n\
START:\n",
bf->tm_mday,
bf->tm_mon+1,
bf->tm_year+1900,
nomer,itogoo);

while(fgets(strsql,sizeof(strsql),ffkartf) != NULL)
   fprintf(fftmp,"%s",strsql);
  
fprintf(fftmp,"END:\n");
fclose(fftmp);
fclose(ffkartf);
rename(imaftmp,imafkartf);
}
