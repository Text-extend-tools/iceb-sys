/*$Id: platp.c,v 1.54 2014/06/30 06:36:34 sasa Exp $*/
/*15.07.2015	29.09.2001	Белых А.И.	platp.c
Печать платежного поручения в соответствии с постановлением
правления национального банка Украины от 29 марта 2001г. N135
Зарегистрировано в министерстве юстиции Украины 25 апреля 2001г
за N368/5559
Официальный весник Украины N18 часть 2 за 2001 год страница 848
В разделе 2 пункт 6 оговорена нумерация платежек

Если вернули 0 документ сформирован
             1 нет
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

extern short	kolstrdok; //Количество строк между документами
extern class REC rec;
extern short	ots; /*Отступ от левого края*/
extern short	kor; /*Корректор межстрочного растояния*/
extern short    mvs;       /*1-с разбивкой суммы по счетам 0-без*/

int platp(const char *imaf1,
short kp, //Количество копий
short nomk, //Номер печатаемой копии
FILE *ff,
int viddok) //0-Платежное поручение 1- платежное требование
{
class iceb_tu_str stroka("");
char		stroka1[512];
char		stroka2[512];
FILE		*ff1;
class iceb_tu_str bank_plat("");
class iceb_tu_str bank_poluh("");
char		imaf[112];
short		kolstrok=0;
char		bros[512];
char dt2[128]; //Дата платежа
double		bb=0.;
short dt,mt,gt;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
poltekdat(&dt,&mt,&gt);

class iceb_tu_str mescc("");
mesc(rec.md,1,&mescc);
sprintf(dt2,"%02d %s %d%s",rec.dd,mescc.ravno(),rec.gd,gettext("г."));

if(sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) <= 0)
 {
  if(viddok == 0)
    sprintf(imaf,"%s","pp03072009.alx");
  if(viddok == 1)
    sprintf(imaf,"%s","pt03072009.alx");
 }
else
 {
  if(viddok == 0)
    sprintf(imaf,"%s","platp.alx");
  if(viddok == 1)
    sprintf(imaf,"%s","platt.alx");
 }

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_t_soob(strsql);
  return(1);
 }


if((ff1 = fopen(imaf1,"r")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }


fprintf(ff,"\x12"); //Выключить уплотненный режим
fprintf(ff,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
fprintf(ff,"\x1B\x78%c",1); //Включить черновой режим печати

if(sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) > 0)
 if(kp > 1)
  fprintf(ff,"%70s N%d\n",gettext("Экземпляр"),nomk+1);

//fprintf(ff,"\x1B\x55\x31"); /*Включение однонаправленного режима*/
fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//fprintf(ff,"\x1B\x41%c",7-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
fprintf(ff,"\x1b\x45"); //Режим выделенного шрифта
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
  kolstrok++;
  switch(kolstrok)
   {

    case 2: //Номер платежного поручения
     fprintf(ff,"\x1B\x33%c",16-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//     fprintf(ff,"\x1B\x41%c",5-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
     iceb_tu_vstav(&stroka,rec.nomdk.ravno(),30,49,1);
     break;

    case 3: 
      fprintf(ff,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//      fprintf(ff,"\x1B\x41%c",10-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
      break;

    case 4: //Дата документа
     iceb_tu_vstav(&stroka,dt2,16,44,1);
     break;

    case 6: //Год
     iceb_tu_vstav(&stroka,gt,64,68,1);
     break;

    case 7: //Плательщик
     if(viddok == 0)
       iceb_tu_vstav(&stroka,rec.naior.ravno(),8,60,1);
     if(viddok == 1)
       iceb_tu_vstav(&stroka,rec.naior1.ravno(),8,60,1);

     break;

    case 8: //Наименование плательщика, которое не поместилось в первой строке
     
     if(viddok == 0)
      if(iceb_tu_strlen(rec.naior.ravno()) > 53)
       iceb_tu_vstav(&stroka,iceb_tu_adrsimv(53,rec.naior.ravno()),0,69,1);
     if(viddok == 1)
      if(iceb_tu_strlen(rec.naior1.ravno()) > 53)
       iceb_tu_vstav(&stroka,iceb_tu_adrsimv(53,rec.naior1.ravno()),0,69,1);

/***************** дата валютирования теперь не печатаестся
     if(rec.dv != 0)
      {
       sprintf(bros,"%02d",rec.dv);
       iceb_tu_vstav(stroka,bros,53,55,1,sizeof(stroka));

       sprintf(bros,"%02d",rec.mv);
       iceb_tu_vstav(stroka,bros,59,61,1,sizeof(stroka));

       sprintf(bros,"%d",rec.gv);
       iceb_tu_vstav(stroka,bros,64,68,1,sizeof(stroka));
      }
     else
      {
       sprintf(bros,"%d",gt);
       iceb_tu_vstav(stroka,bros,64,68,1,sizeof(stroka));
      }    
********************/
     break;

    case 10: //Код СДЗПОУ
     if(viddok == 0)
       iceb_tu_vstav(&stroka,rec.kod.ravno(),11,25,1);
     if(viddok == 1)
       iceb_tu_vstav(&stroka,rec.kod1.ravno(),11,25,1);
     break;

    case 12: //Банк плательщика
      if(viddok == 0)
       {
        //sprintf(bank_plat,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
        bank_plat.new_plus(rec.naiban.ravno());
        bank_plat.plus(" в ",rec.gorod.ravno());
       }
      if(viddok == 1)
       {
        //sprintf(bank_plat,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
        bank_plat.new_plus(rec.naiban1.ravno());
        bank_plat.plus(" в ",rec.gorod1.ravno());
       }
      iceb_tu_vstav(&stroka,bank_plat.ravno(),14,31,1);
     break;
   
    case 13: //Банк плательщика, код банка, счёт дебета, сумма платежа
     if(iceb_tu_strlen(bank_plat.ravno()) > 18)
        iceb_tu_vstav(&stroka,iceb_tu_adrsimv(18,bank_plat.ravno()),0,31,1);
     if(viddok == 0)
      {
       iceb_tu_vstav(&stroka,rec.mfo.ravno(),37,45,1);
       iceb_tu_vstav(&stroka,rec.nsh.ravno(),57,73,1);
      }
     if(viddok == 1)
      {
       iceb_tu_vstav(&stroka,rec.mfo1.ravno(),37,45,1);
       iceb_tu_vstav(&stroka,rec.nsh1.ravno(),57,73,1);
      }
     bb=rec.sumd+rec.uslb;

     prnb1(bb,bros);

     iceb_tu_vstav(&stroka,bros,79,87,1);

     break;

    case 15: //Получатель 1 строка
     if(viddok == 0)
      iceb_tu_vstav(&stroka,rec.naior1.ravno(),10,61,1);
     if(viddok == 1)
      iceb_tu_vstav(&stroka,rec.naior.ravno(),10,61,1);
     break;

    case 16: //Получатель 2 строка
     
     if(viddok == 0)
      {
       if(iceb_tu_strlen(rec.naior1.ravno()) > 52)
         iceb_tu_vstav(&stroka,iceb_tu_adrsimv(52,rec.naior1.ravno()),0,61,1);
      }
     if(viddok == 1)
      {
       if(iceb_tu_strlen(rec.naior.ravno()) > 52)
         iceb_tu_vstav(&stroka,iceb_tu_adrsimv(52,rec.naior.ravno()),0,61,1);
      }
     break;

    case 18: //Код, номер счета за услуги, сумма за услуги
     iceb_tu_vstav(&stroka,rec.kod1.ravno(),11,25,1);
     break;

    case 19: //Кредит счёт N
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
        bb=rec.sumd;
        prnb1(bb,bros);

       }
     if(viddok == 0)
        iceb_tu_vstav(&stroka,rec.nsh1.ravno(),53,69,1);
     if(viddok == 1)
        iceb_tu_vstav(&stroka,rec.nsh.ravno(),53,69,1);
      iceb_tu_vstav(&stroka,bros,75,83,1);
      break;

    case 20: /*Банк получателя*/
      if(viddok == 0)
       {
        bank_poluh.new_plus(rec.naiban1.ravno());
        bank_poluh.plus(" в ",rec.gorod1.ravno());
       }
      if(viddok == 1)
       {
        bank_poluh.new_plus(rec.naiban.ravno());
        bank_poluh.plus(" в ",rec.gorod.ravno());
       }
      iceb_tu_vstav(&stroka,bank_poluh.ravno(),16,31,1);
      break;
        
    case 21: //Банк получателя, МФО, счет
      if(iceb_tu_strlen(bank_poluh.ravno()) > 16)
        iceb_tu_vstav(&stroka,iceb_tu_adrsimv(16,bank_poluh.ravno()),0,31,1);

      if(viddok == 0)
       {
        iceb_tu_vstav(&stroka,rec.mfo1.ravno(),37,45,1);
       }
      if(viddok == 1)
       {
        iceb_tu_vstav(&stroka,rec.mfo.ravno(),37,45,1);
       }
      iceb_tu_vstav(&stroka,rec.shbzu.ravno(),57,73,1);
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
         prnb1(rec.uslb,bros);

       }

      iceb_tu_vstav(&stroka,bros,79,87,1);
       
     break;

    case 24: //Сумма словами
      bb=rec.sumd+rec.uslb;

      memset(bros,'\0',sizeof(bros));
      preobr(bb,bros,0);
      iceb_tu_vstav(&stroka,bros,0,101,1);

      break;

    case 25: //Код нерезидента
      iceb_tu_vstav(&stroka,rec.kodnr.ravno(),63,70,1);
      break;

    case 27: //Назначение платежа
    case 28: //Назначение платежа
    case 29: //Назначение платежа
    case 30: //Назначение платежа
    
      memset(stroka1,'\0',sizeof(stroka1));
      if(FGETSS(stroka1,sizeof(stroka1),ff1) != 0)
       {
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%s\x1B\x50",stroka1);
        iceb_tu_vstav(&stroka,stroka2,0,299,1);
       }
      break;

    default:
     break;
   }
  fprintf(ff,"%s",stroka.ravno());
 }

if(nomk == 0 || nomk == 2)
if(kp > 1)
  for(int i=0; i< kolstrdok ; i++)
     fprintf(ff,"\n");

fclose(ff1);

return(0);
}
