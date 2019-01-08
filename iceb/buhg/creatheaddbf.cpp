/*$Id: creatheaddbf.c,v 5.4 2013/05/17 14:55:57 sasa Exp $*/
/*20.05.2016	02.08.2011	Белых А.И.	creatheaddbf.c
Создание dbf файла для формы 1ДФ
Создаем заголовок файла dbf
*/

#include <errno.h>
#include "buhg.h"

void create_field(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void		creatheaddbf(char imaf[],long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 15 //Количество колонок (полей) в dbf файле
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
/******************
printw("размер заголовка=%d\n",sizeof(h));
printw("размер подзаголовка=%d\n",sizeof(DBASE_FIELD));
printw("размер записи=%d\n",sizeof(f));

OSTANOV();
************************/
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
create_field(&f[shetshik++],"NP", 'N', 5, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"PERIOD", 'N', 1, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"RIK", 'N', 4, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"KOD", 'C', 10, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"TYP", 'N', 1, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"TIN", 'C', 10, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"S_NAR", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_DOX", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_TAXN", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_TAXP", 'N', 12, 2, &header_len,&rec_len);
/*********До 1.1.2011
create_field(&f[shetshik++],"OZN_DOX", 'N', 2, 0, &header_len,&rec_len);
************/
create_field(&f[shetshik++],"OZN_DOX", 'N', 3, 0, &header_len,&rec_len);

create_field(&f[shetshik++],"D_PRIYN", 'D', 8, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"D_ZVILN", 'D', 8, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"OZN_PILG", 'N', 2, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"OZNAKA", 'N', 1, 0, &header_len,&rec_len);

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
