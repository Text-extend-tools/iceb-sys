/*$Id: zvb_privatbank_end.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*20.05.2016	17.10.2014	Белых А.И.	zvb_privatbank_end.c
Концовки файлов
Описание формата файла dbf:

BRANCH     CHAR     3     карточный бранч             обязательно      (в банке должны вам его сообщить)
ZPКOD      CHAR     3     код зарплатного проэкта     обязательно      (в банке дожны вам его сообщить)
CARD_NO    CHAR     16    номер карты                 не обязательно
LSTBL      CHAR     10    Табельный номер сотрудника  обязательно
FAM        CHAR     50    фамилия                     не обязательно
NAME       CHAR     50    имя                         не обязательно
OT         CHAR     50    отчество                    не обязательно
RLSUM      NUM      15.2  сумма                       обязательно
INN        CHAR     20    инн сотрудника              не обязательно
RLKOD      CHAR     30    назначение зачисления       не обязательно

*/
#include <errno.h>
#include "buhg.h"

void privatbank_h(const char *imaf,long kolz);

void zvb_privatbank_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp)
{
class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;

iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);

fputc(26,ff_dbf_tmp);



fprintf(ffr,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(80,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
%-*s_____________________ %s\n\n\
%-*s_____________________ %s\n",
iceb_tu_kolbait(20,gettext("Руководитель")),
gettext("Руководитель"),
ruk.fio.ravno(),
iceb_tu_kolbait(20,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"),
glbuh.fio.ravno());

podpis(ffr); 

fclose(ffr);

fclose(ff_dbf_tmp);

perecod(2,imaf_dbf_tmp); /*Перекодировка*/

privatbank_h(imaf_dbf,kolzap);
iceb_t_cat(imaf_dbf,imaf_dbf_tmp);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void privatbank_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void privatbank_h(const char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 10 //Количество колонок (полей) в dbf файле
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
/**************
BRANCH     CHAR     3     карточный бранч             обязательно      (в банке должны вам его сообщить)
ZPКOD      CHAR     3     код зарплатного проэкта     обязательно      (в банке дожны вам его сообщить)
CARD_NO    CHAR     16    номер карты                 не обязательно
LSTBL      CHAR     10    Табельный номер сотрудника  обязательно
FAM        CHAR     50    фамилия                     не обязательно
NAME       CHAR     50    имя                         не обязательно
OT         CHAR     50    отчество                    не обязательно
RLSUM      NUM      15.2  сумма                       обязательно
INN        CHAR     20    инн сотрудника              не обязательно
RLKOD      CHAR     30    назначение зачисления       не обязательно
************/

privatbank_f(&f[shetshik++],"BRANCH", 'C', 3, 0,&header_len,&rec_len);//1
privatbank_f(&f[shetshik++],"ZPKOD", 'C', 3, 0,&header_len,&rec_len);//2
privatbank_f(&f[shetshik++],"CARD_NO", 'C', 16, 0,&header_len,&rec_len);//3
privatbank_f(&f[shetshik++],"LSTBL", 'C', 10, 0,&header_len,&rec_len);//4
privatbank_f(&f[shetshik++],"FAM",'C',50,0,&header_len,&rec_len);//5
privatbank_f(&f[shetshik++],"NAME",'C',50,0,&header_len,&rec_len);//6
privatbank_f(&f[shetshik++],"OT",'C',50,0,&header_len,&rec_len);//7
privatbank_f(&f[shetshik++],"RLSUM",'N',15,2,&header_len,&rec_len);//8
privatbank_f(&f[shetshik++],"INN",'C',20,0,&header_len,&rec_len);//9
privatbank_f(&f[shetshik++],"RLCOD",'C',30,0,&header_len,&rec_len);//10


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
