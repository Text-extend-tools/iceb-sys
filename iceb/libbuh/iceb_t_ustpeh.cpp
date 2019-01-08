/*$Id: iceb_t_ustpeh.c,v 5.11 2013/05/17 14:56:29 sasa Exp $*/
/*19.01.2015	05.09.2009	Белых А.И.	iceb_t_ustpeh.c
Определение максимально длинной строки в файле
и установка команд для печати файла с нужным ужатием и ориентацией
Возвращает какая ориентация действительно получилась
 0-ориентация портрет
 1-ориентация ландшафт
-1-ошибка при открытии файла 

*/
#include <ctype.h>
#include <errno.h>
#include "buhl.h"

#define MAX_RAZMER_PO_GOR 197.2 /*Максимальный размер по горизонтали на листе А4 в милиметрах*/
#define MAX_RAZMER_PO_VER 271.7 /*Максимальный размер по вертикали на листе А4 в милиметрах*/

int iceb_t_ustpeh_kus(const char *stroka,double *rmsi);

int iceb_t_ustpeh(const char *imaf,int orient, /*0-любая 1-только портрет 2-только ландшафт 3-сначала портрет если не подходит ландшафт*/
int *kolstr) /*количество строк на листе с учётом выбранной ориетации и межстрочного расстояния*/
{
class iceb_tu_str stroka("");
class iceb_tu_str imaf_tmp(imaf);
imaf_tmp.plus(".tmp");
int voz=0; /*0-ориентация портрет 1-ландшафт -1 ошибка*/

*kolstr=0;

FILE *ff;
FILE *ff_tmp;

if((ff=fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,__FUNCTION__);
  return(-1);
 }
int max_dlina_str=0;
int dlina_str=0;
double rmsi=25.4/6; /*стандартный размер межстрочного интервала в милиметрах*/
while(iceb_tu_fgets(&stroka,ff) == 0)
 {

  dlina_str=iceb_tu_strlen(stroka.ravno());
  dlina_str-=iceb_t_ustpeh_kus(stroka.ravno(),&rmsi);
  if(max_dlina_str < dlina_str)
    max_dlina_str=dlina_str;
 }

if((ff_tmp=fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  error_op_nfil(imaf_tmp.ravno(),errno,__FUNCTION__);
  return(-1);
 }

startfil(ff_tmp);

voz=iceb_tu_ustpeh(max_dlina_str,orient,ff_tmp);

rewind(ff);
while(iceb_tu_fgets(&stroka,ff) == 0)
 fprintf(ff_tmp,"%s",stroka.ravno());
double razmer=MAX_RAZMER_PO_VER;
if(voz == 1)
 razmer=MAX_RAZMER_PO_GOR;

*kolstr=(int)(razmer/rmsi);
//printw("\n%s-kolstr=%d razmer=%f rmsi=%f\n",__FUNCTION__,*kolstr,razmer,rmsi);
//OSTANOV();

fclose(ff);
fclose(ff_tmp);

rename(imaf_tmp.ravno(),imaf);
return(voz);
}
/************************************/
/*оперделяем количество управляющих символов в строке и концевых пробелов*/
/*****************************************************/
int iceb_t_ustpeh_kus(const char *stroka,double *rmsi) /*размер межстрочного интервала в милиметрах*/
{
int kolih_simv=0;
int kolih_simv_v_str=strlen(stroka);

/*проверяем может в конце строки есть пробелы*/
for(int nom=kolih_simv_v_str-1; nom >= 0 ; nom--)
 {
  if(stroka[nom] == '\n' || stroka[nom] == '\r') /*учитываем в следующей проверке*/
   continue;
  if(stroka[nom] == ' ')
   kolih_simv++;     
  else
   break;
 }

/* ищем управляющие символы в строке*/
int kod_simv=0;
for(int nom=0; nom < kolih_simv_v_str; nom++)
 {
  if(iscntrl(kod_simv=stroka[nom])) //Проверка на управляющий символ
  {
   switch(kod_simv)
    {
    /*
     *   ESC-последовательности
     */
     case 27: //Esc

      kolih_simv++;
      nom++;
      switch (stroka[nom])
       {
         case 15: /*Включение уплотнённого режима печати*/
           kolih_simv++;
           continue;           

         case '-': //Включить/выключить режим подчеркивания
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что следующий елемент строки продолжение команды для принтера
           continue;

         case '0':		/* межстрочный интервал 1/8 inch */
           kolih_simv++;
            continue;

         case '2': //Межстрочный интервал 1/6 дюйма
           kolih_simv++;
           *rmsi=25.4/6;
           continue;

         case '3': //Межстрочный интервал n/216 дюйма
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           *rmsi=25.4*stroka[nom]/216;
           continue;

         case '[': //Установка размера шрифта
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           continue;

         case 'A': //Межстрочный интервал n/72 дюйма
           kolih_simv++;
           nom++;  //Увеличиваем счётчик потому, что нужен следующий элемент строки
           *rmsi=25.4*stroka[nom]/72;
           continue;

         case 'G': //Двуударный режим печати
         case 'E': //Выделенный шрифт
           kolih_simv++;
           continue;

         case 'H': //Отмена двуударного режима печати
         case 'F': //Отмена выделенного шрифта
           kolih_simv++;
           continue;

         case 'P': //Десять знаков на дюйм
           kolih_simv++;
           continue;

         case 'M': //Двенадцать знаков на дюйм
           kolih_simv++;
           continue;

         case 'g': //Пятнадцать знаков на дюйм (применяестся не на всех принтерах)
           kolih_simv++;
           continue;

                
         case 'l': //Установка левого поля
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           continue;
                
         /* Нереализованные трехсимвольные команды*/
         case 'U': //Включение/выключение однонаправленного режима печати
         case 'x': //Включение/выключение высококачественного режима печати
         case 'S': //Выбор режима печати индексов
         case 'W': //Включение/выключение  режима печати расширенными символами
         case 'a': //Выравнивание текста по правую или левую границу печати
         case 'J': //Перевод бумаги в прямом направлении на заданное количество елементрарных шагов 
         case 'j': //Перевод бумаги в обратном направлении на заданное количество елементрарных шагов
           kolih_simv++;
           nom++;  //Трех-символьная команда игнорируем третий символ
           continue;
                
         case '@': //Инициализация принтера
           kolih_simv++;
            //счётчик не увеличиваем-двубайтная последовательность
            continue;

                
         default: //Игнорируются неизвестные двубайтные последовательности
           kolih_simv++;
           continue;
       }
  
    /*
     *   1-байтные управляющие символы
     */
    case 14: //^N - Двойная ширина символа для одной строки
      kolih_simv++;
      continue;


    case 15: //^O - Включить уплотненный режим печати
      kolih_simv++;
      continue;
  
    case 18: //^R - Выключить уплотненный режим печати
      kolih_simv++;
      continue;

    case 20: //^T - Отмена двойной ширины символа для одной строки
      kolih_simv++;
      continue;
  
    case '\n':  //Перевод строки. 
      kolih_simv++;
      continue;

    case '\r':  /*возврат каретки игнорируем*/
      kolih_simv++;
      continue;

    case '\f':  //Перевод формата
      kolih_simv++;
      continue;

    case '\t':  //Табулятор
      kolih_simv++;
      continue;

    default:
      kolih_simv++;
      /* остальные управляющие символы пропускаем */
      continue;
      
    }
  } 
 }

return(kolih_simv);



}
/***********************************************/
int iceb_t_ustpeh(const char *imaf,int orient) /*0-любая 1-только портрет 2-только ландшафт 3-сначала портрет если не подходит ландшафт*/
{
int kolstr=0;
return(iceb_t_ustpeh(imaf,orient,&kolstr));

}
