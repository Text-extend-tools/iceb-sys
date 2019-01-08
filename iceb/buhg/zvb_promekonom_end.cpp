/*$Id: zvb_promekonom_end.c,v 5.4 2013/05/17 14:56:19 sasa Exp $*/
/*20.05.2016	10.12.2009	Белых А.И.	zvb_promekonom_end.c
Концовки файлов
*/
#include <errno.h>
#include "buhg.h"

void promekonom_h(const char *imaf,long kolz);

void zvb_promekonom_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp)
{
class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;

iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);

fputc(26,ff_dbf_tmp);


fprintf(ffr,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(80,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());

podpis(ffr); 

fclose(ffr);

fclose(ff_dbf_tmp);

perecod(1,imaf_dbf_tmp); /*Перекодировка*/

promekonom_h(imaf_dbf,kolzap);
iceb_tu_cat(imaf_dbf,imaf_dbf_tmp);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void promekonom_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}

/***********************************************/
void promekonom_h(const char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 3 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
promekonom_f(&f[shetshik++],"NKS", 'C',40, 0,&header_len,&rec_len);
promekonom_f(&f[shetshik++],"FIO", 'C', 60, 0,&header_len,&rec_len);
promekonom_f(&f[shetshik++],"SUMA", 'C', 10, 0,&header_len,&rec_len);


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

if(write(fd, &h, sizeof(h)) < 0)
 {
  printw("\n%s-%s\n",__FUNCTION__,strerror(errno));
  OSTANOV();
 }

for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) < 0)
   {
    printw("\n%s-%s\n",__FUNCTION__,strerror(errno));
    OSTANOV();
   }

 }
fputc('\r', ff);

fclose(ff);

}
