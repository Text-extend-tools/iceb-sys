/*$Id: b_brokbiznes.c,v 5.22 2013/05/17 14:55:56 sasa Exp $*/
/*20.05.2016	21.07.2004	Белых А.И. 	b_brokbiznes.c
Формирование файла для подсистемы Клиент-банк для Врокбизнес банка.
Структура записи dbf файла

dtpd     DATE   8    Дата платёжного документа
npd      char   10   Номер платёжного документа
dmfo     int    11   Дебет МФО (ПЛАТЕЛЬШИКА)
cmfo     int    11   Кредит МФО (ПОЛУЧАТЕЛЯ)
dacc     char   17   Дебет Р/С (ПЛАТЕЛЬЩИКА)
cacc     char   17   Кредит Р/С (ПОЛУЧАТЕЛЯ)
dokpo    char   17   Дебет ОКПО (ПЛАТУЛЬЩИКА)
cokpo    char   17   Кредит ОКПО (ПОЛУЧАТУЛЯ)
amount   double 19.4 Сумма
currency short  6    Код валюты (980)
remark   char   160  Назначение платежа
dname    char   50   Название плательщика
cname    char   50   Название получателя

Телефон 520827 Андрей Александрович
Адрес - Киевская 14  


*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

void		b_brokbiznes_h(const char *imaf,long kolz);

extern class REC rec;

int	b_brokbiznes(const char *tabl)
{
char		imaf1[64];
FILE		*ff1;
class iceb_tu_str koment("");
char		strsql[1024];
short		d,m,g;
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа для передачи в банк!"));
  return(1);
 }



sprintf(imaf1,"plat_b.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
   {
    error_op_nfil(imaf1,errno,"");
    return(1);
   }

int nomstr=0;

while(cur.read_cursor(&row) != 0)
 {

  rsdat(&d,&m,&g,row[0],2);

  if(readpdok(tabl,g,row[1]) != 0)
     continue;


  /*Читаем комментарий*/
  readkom(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment);
  nomstr++;  
  fprintf(ff1," %04d%02d%02d%-*.*s%11.11s%11.11s%-17.17s%-17.17s%-17.17s%-17.17s%19.4f\
%6d%-*.*s%-*.*s%-*.*s",
  g,m,d,
  iceb_tu_kolbait(10,row[1]),
  iceb_tu_kolbait(10,row[1]),
  row[1],
  rec.mfo.ravno(),
  rec.mfo1.ravno(),
  rec.nsh.ravno(),
  rec.nsh1.ravno(),
  rec.kod.ravno(),
  rec.kod1.ravno(),
  rec.sumd,
  980,
  iceb_tu_kolbait(160,koment.ravno()),iceb_tu_kolbait(160,koment.ravno()),koment.ravno(),
  iceb_tu_kolbait(50,rec.naior.ravno()),iceb_tu_kolbait(50,rec.naior.ravno()),rec.naior.ravno(),
  iceb_tu_kolbait(50,rec.naior1.ravno()),iceb_tu_kolbait(50,rec.naior1.ravno()),rec.naior1.ravno());


 }
fputc(26, ff1);

fclose(ff1);

perecod(2,imaf1);

char imafdbf[64];

sprintf(imafdbf,"plat.txt");
b_brokbiznes_h(imafdbf,nomstr);

/*Сливаем два файла*/


iceb_t_cat(imafdbf,imaf1);

unlink(imaf1);

return(0);
}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_brokbiznes_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void b_brokbiznes_h(const char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  13
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

memset(&h,'\0',sizeof(h));

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
b_brokbiznes_f(&f[shetshik++],"dtpd", 'D', 8, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"npd", 'C', 10, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dmfo", 'N', 11, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cmfo", 'N', 11, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dacc", 'C',17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cacc", 'C',17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dokpo", 'C', 17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cokpo", 'C', 17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"amount", 'N', 19, 4,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"currency", 'N', 6, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"remark", 'C', 160, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dname", 'C', 50, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cname", 'C', 50, 0, &header_len,&rec_len);


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
