/*$Id: razuz.c,v 5.16 2013-04-07 10:22:44 sasa Exp $*/
/*19.03.2013	18.01.2000	Белых А.И.	razuz.c
Разузлование - определение какие узлы и в каком количестве
входят в изделие
Возвращает количество входящих узлов

Определяем все узлы изделия первого уровня.
Если они есть создаем массивы и заполняем их.
Дальше работаем в цикле.

Берем первый узел и определяем есть ли в нем входящие узлы.

Если нет проверяем следующий узел.
Если есть проверяем есть ли они в списке узлов
Если есть увеличиваем количество
Если нет добавляем в список узлов
Берем для проверки следующий узел


*/
#include        "buhg.h"

int razuz(class iceb_t_razuz_data *data)
{
SQL_str         row;
char		strsql[1024];
int		koluz=0;
int		kodviz=0;
int		kolstr,kolstr1;
int		zsuz,i;
int		kodizo;
double          kolviz=1.;

/*
printw("\nrazuz-Код изделия: %d\n",data->kod_iz);
refresh();
*/

zsuz=koluz=0;
kodizo=data->kod_izd;
SQLCURSOR cur;

for(;;)
 {
  sprintf(strsql,"select kodd,kolih from Specif where kodi=%d",kodizo);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(0);
   }

  if(kolstr == 0 )
   {
    /*В изделии нет ни одной записи*/
    printw("\n%s-%s !!!\n%s\n%d\n",__FUNCTION__,gettext("Нет ни одной записи в изделии"),strsql,kodizo);
    OSTANOV();
    return(0);
   }

  while(cur.read_cursor(&row) != 0)
   {

  //  printw("%s %s %d %d\n",row[0],row[1],zsuz,koluz);
  //  OSTANOV();

    /*Проверяем не является ли входящий материал изделием*/
    sprintf(strsql,"select distinct kodi from Specif where kodi=%s",row[0]);
    SQLCURSOR cur1;
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,"",strsql);

    kodviz=0;
    if(kolstr1 != 0)
      kodviz=atoi(row[0]);
    else
      continue;

    if(kodviz == data->kod_izd)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD("Ошибка в спецификации !");
      sprintf(strsql,"Изделие входит само в себя в узле %d. Это не возможно !",kodizo);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
     }

    double kkk=kolviz*atof(row[1]);

    /*Проверяем есть ли он в массиве*/
    if((i=data->kod_uz.find(kodviz)) != -1)
     {
      data->kolih_uz.plus(kkk,i);
     }
    else
     {
      koluz++;

  //    printw("Новый узел %d\n",kodviz);

      data->kod_uz.plus(kodviz,-1);
      data->kolih_uz.plus(kkk,-1);

     }


   }

   
  /*Обрабатываем узлы*/
  if(zsuz < koluz)
   {
    kodizo=data->kod_uz.ravno(zsuz);
    kolviz=data->kolih_uz.ravno(zsuz);
    zsuz++;
   }
  else
   break;
 }
return(koluz);

}
