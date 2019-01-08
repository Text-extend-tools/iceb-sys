/*$Id: poigod.c,v 5.11 2013/09/26 09:43:55 sasa Exp $*/
/*15.11.2012	24.06.2000	Белых А.И.	poigod.c
Определение за какие годы введена в базу информация
*/
#include        "buhl.h"


void		poigod(int zapros)
/*
int		zapros; 
                          0-запрос по всем базам
			  1-главная книга
                          2-материальный учет
                          3-расчёт зарплаты
                          4-учет основных средств
                          5-платежные поручения
                          6-платежные требования
                          7-объявки в банк
                          8-учет услуг
                          */
{
long		kolstr=0,kolstr1=0;
SQL_str         row;
char		strsql[512];
short		god,d,m,g;

clear();
printw("%s.\n",gettext("Перечень годов за которые имеется информация в базе данных"));
GDITE();
SQLCURSOR cur;

if(zapros == 1 || zapros == 0)
 {
  sprintf(strsql,"select distinct datp from Prov order by datp asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s\n",gettext("База данных главной книги"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

if(zapros == 2 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Dokummat order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных материального учета"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

if(zapros == 3 || zapros == 0)
 {
  GDITE();
  sprintf(strsql,"select distinct god from Zarn order by god asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных заработной платы"));

  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    while(cur.read_cursor(&row) != 0)
     printw("%s\n",row[0]);
   }
 }

if(zapros == 4 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Uosdok order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных учета основных средств"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

if(zapros == 5 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Pltp order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных платежных поручений"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
  }

if(zapros == 6 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Pltt order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных платежных требований"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

if(zapros == 7 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Obqvka order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s\n",gettext("База данных объявок о сдаче наличных в банк"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

if(zapros == 8 || zapros == 0)
 {
  sprintf(strsql,"select distinct datd from Usldokum order by datd asc");
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }
  printw("%s.\n",gettext("База данных учета услуг"));
  
  if(kolstr == 0)
   {
    printw("%s\n",gettext("Не найдено ни одной записи !"));
   }
  else
   {
    kolstr1=god=0;
    while(cur.read_cursor(&row) != 0)
     {
      kolstr1++;
      strzag(LINES-1,0,kolstr,kolstr1);
      rsdat(&d,&m,&g,row[0],2);
      if(god != g)
       {
        printw("%d\n",g);
        god=g;
       }
     }
   }
 }

OSTANOV();

}
