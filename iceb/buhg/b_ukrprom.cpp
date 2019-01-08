/*$Id: b_ukrprom.c,v 5.13 2013-01-02 10:38:36 sasa Exp $*/
/*17.03.2012	27.10.2005	Белых А.И.	b_ukrprom.c
Формирование файла для импорта платёжек в подсистему Укрпромбанка, Биг-Энергия
Формат для автоматизированной системы банков "Мебиус-банк"
Имя файла
INxnnnnn.kmm
 где
  IN    - тип файла
  x     - номер дня недели
  nnnnn - номер клиента
  k     - символ "k"
  mm    - номер сеанса

Структура записи

char MfoD[9]         МФО плательщика
char AccD[14]        Лицевой счёт плательшика
char OkpoD[10]       ОКПО плательщика
char MfoK[9]         МФО получателя
char AccK[14]        Лицевой счёт получателя
char OkpoK[10]       ОКПО получателя
char DocN[10]        Номер документа
char Amt[19]         Сумма (в коп.) платежа
char Opr[2]          01-платёжное поручение
char NamD[38]        Наименование плательщика
char NamK[38]        Наименование получателя
char NameP[160]      Назначение платежа
char day[2]          Ден
char day[2]          Месяц
char year[2]         Год
char sym[1]
char rezerv[40]
char eol[2]=0x0a0d   Признак конца строки




*/



#include        <errno.h>
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

int b_ukrprom(const char *tabl)
{
char		imaf1[32];
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
sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }
int suma_int=0;
double suma_double=0.;
double bb=0.;
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
/************
  polen(stt,strsql,sizeof(strsql),1,'|');
  rsdat(&d,&m,&g,strsql,1);
  polen(stt,nomerdok,sizeof(nomerdok),2,'|');
*************/
  if(readpdok(tabl,g,row[1]) != 0)
     continue;

  //Округляем, чтобы не пропадали копейки
  rec.sumd=okrug(rec.sumd,0.01);

  bb=rec.sumd*100.;
 
  modf(bb,&suma_double);
  suma_int=(int)suma_double;

  /*Читаем комментарий*/
  readkom(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment);

  fprintf(ff1,"%9.9s%14.14s%10.10s%9.9s%14.14s%10.10s%-*.*s%19d\
%2.2s%-*.*s%-*.*s%-*.*s%02d%02d%02d%1.1s%-40.40s\n",
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  iceb_tu_kolbait(10,row[1]),
  iceb_tu_kolbait(10,row[1]),
  row[1], 
  suma_int,
  "01",
  iceb_tu_kolbait(38,rec.naior.ravno()),iceb_tu_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  iceb_tu_kolbait(38,rec.naior1.ravno()),iceb_tu_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  iceb_tu_kolbait(160,koment.ravno()),iceb_tu_kolbait(160,koment.ravno()),koment.ravno(),
  d,m,g-2000,
  " ",
  " ");
  
  
 }
memset(strsql,'\0',sizeof(strsql));
memset(strsql,'0',380);
fprintf(ff1,"%s\n",strsql);

fclose(ff1);
perecod(2,imaf1);
return(0);
}
