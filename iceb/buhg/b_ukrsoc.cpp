/*$Id: b_ukrsoc.c,v 5.12 2013-01-02 10:38:36 sasa Exp $*/
/*19.03.2012	09.06.2006	Белых А.И.	b_ukrsoc.c
Экспорт платёжки для "Укрсоц" банка.

Формат файлов: текстовый; Кодировка:DOS(CP-866); Разделитель полей: "|" (пайп);

Имя файла "c_epd.txt"
1 Номер строки в файле (Начиная с 1)
2 Статус документа (всегда 0)
3 Дебет МФО
4 Дебет счёт
5 Дебет ОКПО
6 Кредит МФО
7 Кредит счёт
8 Кредит МФО
9 Дата документа (ДД.ММ.ГГГГ) (разделитель "точка" ASCII 46)
10 Номер документа
11 Название плательщика (до 38 символов)
12 Название получателя  (до 38 символов)
13 Сумма документа (формат например 210.00)
14 Дата акцепта (не заполняется)
15 Время акчепта (не заполняется)
16 Назначение платежа (до 160 символов)
17 Валюта (по умолчанию 980, если не заполнено)
18 Информационный (значение:F,T. По умолчанию F, если не заполнено)
19 Код назначения платежа НБУ (по умолчанию 0, если не заполнено)
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

int b_ukrsoc(const char *tabl)
{
char		imaf1[64];
FILE	*ff1;
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
int nom_str=0;
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
/************
  polen(stt,strsql,sizeof(strsql),1,'|');
  rsdat(&d,&m,&g,strsql,1);
  polen(stt,nomerdok,sizeof(nomerdok),2,'|');
***************/
  if(readpdok(tabl,g,row[1]) != 0)
     continue;

  /*Читаем комментарий*/
  readkom(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment);


  fprintf(ff1,"%d|0|%s|%s|%s|%s|%s|%s|%02d.%02d.%04d|%s|%.*s|%.*s|%.2f|||%s|980|\n",
  ++nom_str,
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  d,m,g,
  row[1], 
  iceb_tu_kolbait(38,rec.naior.ravno()),
  rec.naior.ravno(),
  iceb_tu_kolbait(38,rec.naior1.ravno()),
  rec.naior1.ravno(),
  rec.sumd,
  koment.ravno());
  
  
 }

fclose(ff1);
perecod(2,imaf1);
return(0);
}
