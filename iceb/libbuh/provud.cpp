/*$Id: provud.c,v 5.52 2013/09/26 09:43:55 sasa Exp $*/
/*28.03.2018	20.02.1999	Белых А.И.	provud.c
Проверка возможности удаления нормативно-справочной информации
Если вернули 0 удалять можно
             1-нельзя
*/
#include	"buhl.h"


int provud(const char *tabl, //Имя таблицы
const char *kod, //Проверяемый код
int met) //0-без отчёта 1-с отчётом
{
char		strsql[1024];
long		kolstr;
char		bros[1024];


if(met != 0)
 {
  clear();
  printw(gettext("Проверка вожможности удаления ! (%s %s)\n"),tabl,kod);
  /*OSTANOV();*/
  GDITE();
 }
class SQLCURSOR cur;

if(SRAV(tabl,"Plansh",0) == 0)
 {
  if(met != 0)
   {
    printw("%s Upldok2a\n",gettext("Проверяем возможность удаления в таблице"));
    refresh();
   }
  sprintf(strsql,"select shet from Upldok2a where shet='%s' limit 1",kod);
   
  if(readkey(strsql) != 0)
   {
     if(met != 0)
      {
       beep();
       printw(gettext("Удалить нельзя, есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
   }    

  if(met != 0)
   {
    printw("%s Zarsdo\n",gettext("Проверяем возможность удаления в таблице"));
    refresh();
   }
  sprintf(strsql,"select sh from Zarsdo where sh='%s' limit 1",kod);
   
  if(readkey(strsql) != 0)
   {
     if(met != 0)
      {
       beep();
       printw(gettext("Удалить нельзя, есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
   }    

  if(met != 0)
   {
    printw(gettext("Проверяем сальдо\n"));
    refresh();
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"select ns from Saldo where ns='%s' limit 1",kod);
//  printw("%s\n",strsql);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
  if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Счет %s удалить невозможно, потому, что есть сальдо\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

  if(met != 0)
   {
    printw(gettext("Проверяем проводки\n"));
    refresh();
    sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh='%s' or shk='%s' limit 1",kod,kod);
   }
  else
   sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh like '%s%%' or shk like '%s%%' limit 1",kod,kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Счет %s удалить невозможно, потому, что есть проводки\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем карточки в материальном учете\n"));
     refresh();
    }
   sprintf(strsql,"select shetu from Kart where shetu='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Счет %s удалить невозможно, потому,\
 что есть карточки материалов с этим счетом\n"),kod);
       OSTANOV();
      }
     return(1);
    }

   if(met != 0)
    {
     printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Nash");
     refresh();
    }
   
   sprintf(strsql,"select shet from Nash where shet='%s' limit 1",kod);
//   printw("%s\n",strsql);
   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }


   if(met != 0)
    {
     printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uder");
     refresh();
    }
   sprintf(strsql,"select shet from Uder where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw(gettext("Проверяем записи в Учете основных средств. Таблица Uosinp\n"));
     refresh();
    }
   sprintf(strsql,"select shetu from Uosinp where shetu='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw(gettext("Проверяем записи в таблице услуг: Uslugi\n"));
     refresh();
    }
   sprintf(strsql,"select shetu from Uslugi where shetu='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw(gettext("Проверяем записи в таблице: Usldokum1\n"));
     refresh();
    }
   sprintf(strsql,"select shetu from Usldokum1 where shetu='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }
 
   if(met != 0)
    {
     printw(gettext("Проверяем записи в таблице: Usldokum2\n"));
     refresh();
    }
   sprintf(strsql,"select shetu from Usldokum2 where shetu='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Zarsoc.");
     refresh();
    }
   sprintf(strsql,"select shet from Zarsoc where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Zarsocz.");
     refresh();
    }
   sprintf(strsql,"select shet from Zarsocz where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Kas");
     refresh();
    }

   sprintf(strsql,"select shet from Kas where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Kasop1");
     refresh();
    }

   sprintf(strsql,"select shetkas from Kasop1 where shetkas='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   sprintf(strsql,"select shetkor from Kasop1 where shetkor='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Kasop2");
     refresh();
    }
   sprintf(strsql,"select shetkas from Kasop2 where shetkas='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   sprintf(strsql,"select shetkor from Kasop2 where shetkor='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Kasord");
     refresh();
    }

   sprintf(strsql,"select shetk from Kasord where shetk='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Ukrkras");
     refresh();
    }
 
   sprintf(strsql,"select shet from Ukrkras where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Ukrdok1");
     refresh();
    }
 
   sprintf(strsql,"select shet from Ukrdok1 where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

  if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Uplost");
     refresh();
    }
 
   sprintf(strsql,"select shet from Uplost where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }
/***************************
  if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Upldok2");
     refresh();
    }
 
   sprintf(strsql,"select shet from Upldok2 where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

*****************/
  if(met != 0)
    {
     printw("%s:%s\n",
     gettext("Проверяем записи в таблице"),"Opldok");
     refresh();
    }
 
   sprintf(strsql,"select shet from Opldok where shet='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
       return(1);
      }
    }
   else
    {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Счет %s удалить невозможно, потому,\
что есть записи с этим счетом\n"),kod);
         OSTANOV();
        }
       return(1);
      }
    }

   return(0);
 }

if(SRAV(tabl,"Grup",0) == 0)
 {
  sprintf(strsql,"select kodgr from Material where kodgr=%d limit 1",
  atoi(kod));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Группу %s удалить невозможно, \
потому, что есть материалы с этой группой\n"),kod);
        OSTANOV();
       }
      return(1);
     }    
   }
  return(0);
 } 


if(SRAV(tabl,"Material",0) == 0)
 {
  sprintf(strsql,"select kodm from Kart where kodm=%d limit 1",
  atoi(kod));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
   }
  else
   {   
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Материал %s удалить невозможно, \
потому, что есть карточки с этим материалом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
   }

  sprintf(strsql,"select kodm from Dokummat1 where kodm=%d limit 1",atoi(kod));
/*
   printw("\nstrsql=%s\n",strsql);
   OSTANOV();
*/
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
   }
   
   
  if(kolstr != 0)
   {
     if(met != 0)
      {
       beep();
       printw(gettext("Материал %s удалить невозможно, \
потому, что есть документы с этим материалом\n"),kod);
       OSTANOV();
      }
     return(1);
   }    

  sprintf(strsql,"select km from Dokummat4 where km=%d limit 1",atoi(kod));
  if(readkey(strsql) != 0)
   {
     if(met != 0)
      {
       beep();
       sprintf(strsql,"%s Dokummat4",gettext("Удалить нельзя так как есть записи с этим кодом в таблице"));
       iceb_t_soob(strsql);
      }
     return(1);
   }    
  sprintf(strsql,"select kd from Dokummat4 where kd=%d limit 1",atoi(kod));
  if(readkey(strsql) != 0)
   {
     if(met != 0)
      {
       beep();
       sprintf(strsql,"%s Dokummat4",gettext("Удалить нельзя так как есть записи с этим кодом в таблице"));
       iceb_t_soob(strsql);
      }
     return(1);
   } 



  sprintf(strsql,"select kodi from Specif where kodi=%s or kodd=%s and kz=0 limit 1",kod,kod);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Материал %s удалить невозможно, \
потому, что есть материалы\nс этим кодом в таблице спецификаций\n"),kod);
        OSTANOV();
       }
      return(1);
     }    
   }

  sprintf(strsql,"select metka,kodzap from Usldokum1 where \
metka=0 and kodzap=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
       msql_error(&bd,gettext("Ошибка"),strsql);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Материал %s удалить невозможно, \
потому, что есть записи\nс этим кодом в документах услуг\n"),kod);
        OSTANOV();
       }
      return(1);
     }    
   }

   sprintf(strsql,"select kodm from Roznica where kodm='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     msql_error(&bd,gettext("Ошибка"),strsql);
    }
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       printw("%s Roznica ",gettext("Таблица"));
       printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Uplmt");
    refresh();
   }

  sprintf(strsql,"select kodm from Uplmt where kodm=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw("%s Uplmt ",gettext("Таблица"));
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }


  sprintf(strsql,"select mz from Restdok1 where mz=0 and kodz=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
       msql_error(&bd,gettext("Ошибка"),strsql);
    }
   
  if(kolstr > 0)
   {
     if(met != 0)
      {
       printw("%s Restdok1",gettext("Таблица"));
       printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
   }

  return(0);

 } 


if(SRAV(tabl,"Kontragent",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем сальдо\n"));
    refresh();
   }
   sprintf(strsql,"select kodkon from Saldo where kodkon='%s' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     msql_error(&bd,gettext("Ошибка"),strsql);
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалит невозможно, \
потому, что есть сальдо\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем проводки\n"));
     refresh();
    }
   sprintf(strsql,"select kodkon from Prov where kodkon='%s' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     msql_error(&bd,gettext("Ошибка"),strsql);
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалит невозможно, \
потому, что есть проводки\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем документы в материальном учете\n"));
     refresh();
    }
   
   sprintf(strsql,"select kontr from Dokummat where kontr='%s' limit 1",kod);

   kolstr=readkey(strsql);
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   sprintf(strsql,"select kontr from Dokummat where k00='%s' limit 1",kod);

   kolstr=readkey(strsql);
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем документы в Учете основных средств\n"));
     refresh();
    }

   if(met != 0)
    {
     printw("%s %s\n",gettext("Проверяем записи в таблице"),"Usldokum");
     refresh();
    }
   
   sprintf(strsql,"select kontr from Usldokum where kontr='%s' limit 1",kod);

   kolstr=readkey(strsql);
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   sprintf(strsql,"select kontr from Usldokum where k00='%s' limit 1",kod);

   kolstr=readkey(strsql);
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем документы в Учете основных средств\n"));
     refresh();
    }

   sprintf(strsql,"select kontr from Uosdok where kontr='%s' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
       OSTANOV();
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw(gettext("Проверяем платежные поручения,таблица:Pltp\n"));
     refresh();
    }

   sprintf(strsql,"select polu from Pltp where polu like '%s#%%' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw(gettext("Проверяем платежные требования,таблица:Pltt\n"));
     refresh();
    }
   sprintf(strsql,"select polu from Pltt where polu like '%s#%%' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw(gettext("Проверяем типовые платежные требования,таблица:Tpltt\n"));
     refresh();
    }
   sprintf(strsql,"select polu from Tpltt where polu like '%s#%%' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw(gettext("Проверяем типовые платежные поручения,таблица:Tpltp\n"));
     refresh();
    }
   sprintf(strsql,"select polu from Tpltp where polu like '%s#%%' limit 1",
   kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         beep();
         printw(gettext("Код котрагента %s удалить невозможно, \
потому, что есть документы\nс этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }


   if(met != 0)
    {
     printw("%s: %s\n",gettext("Проверяем записи в таблице"),"Kasord1");
     refresh();
    }

   memset(strsql,'\0',sizeof(strsql));
   sprintf(strsql,"select kontr from Kasord1 where kontr='%s' limit 1",
   kod);
  
   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         printw(gettext("Код %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw("%s: %s\n",gettext("Проверяем записи в таблице"),"Ukrdok");
     refresh();
    }

   memset(strsql,'\0',sizeof(strsql));
   sprintf(strsql,"select kont from Ukrdok where kont='%s' limit 1",
   kod);
  
   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         printw(gettext("Код %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }

   if(met != 0)
    {
     printw("%s: %s\n",gettext("Проверяем записи в таблице"),"Uplouot");
     refresh();
    }
 
   memset(strsql,'\0',sizeof(strsql));
   sprintf(strsql,"select kontr from Uplouot where kontr='%s' limit 1",
   kod);
  
   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      {
       msql_error(&bd,gettext("Ошибка"),strsql);
      }
    }
   else   
    {
     
     if(kolstr != 0)
      {
       if(met != 0)
        {
         printw(gettext("Код %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
         OSTANOV();
        }
       return(1);
      }    
    }
 
   if(met != 0)
    {
     printw("%s: %s\n",gettext("Проверяем записи в таблице"),"Ukrdok1");
     refresh();
    }
 
   memset(strsql,'\0',sizeof(strsql));
   sprintf(strsql,"select kontr from Ukrdok1 where kontr='%s' or kdrnn='%s' limit 1",
   kod,kod);
  
   if(readkey(strsql) > 0)
    {
     if(met != 0)
      {
       printw(gettext("Код %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }
 
 
   return(0);
 }

if(SRAV(tabl,"Skontr",1) == 0)
 {
  POLE(tabl,bros,2,'#');
  
  if(met != 0)
   {
    printw(gettext("Проверяем сальдо\n"));
    refresh();
   }
  sprintf(strsql,"select kodkon from Saldo where ns='%s' and kodkon='%s' limit 1",
  bros,kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалит невозможно, \
потому, что есть сальдо\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем проводки\n"));
     refresh();
    }
   sprintf(strsql,"select kodkon from Prov where kodkon='%s' and sh='%s' limit 1",kod,bros);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Код котрагента %s удалит невозможно, \
потому, что есть проводки\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   return(0);
 }

if(SRAV(tabl,"Foroplat",0) == 0)
 {
  sprintf(strsql,"select nomerz,sodz from Dokummat2 where nomerz=8 and sodz='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  
  
  if(kolstr != 0)
   {
    if(met != 0)
     {
      beep();
      printw(gettext("Код формы оплаты %s удалить невозможно, \
потому, что есть документы \nв материальном учете с этим кодом\n"),kod);
      OSTANOV();
     }
    return(1);
   }    

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Uosdok");
    refresh();
   }

  sprintf(strsql,"select forop from Uosdok where forop='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      msql_error(&bd,gettext("Ошибка"),strsql);
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Usldokum");
    refresh();
   }

  sprintf(strsql,"select forop from Usldokum where forop='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
      msql_error(&bd,gettext("Ошибка"),strsql);
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  return(0);
  
 }


 
if(SRAV(tabl,"Tabel",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Ztab");
    refresh();
   }
   sprintf(strsql,"select kodt from Ztab where kodt=%s limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Табель %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);
 }

if(SRAV(tabl,"Gnali",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Kartb");
    refresh();
   }
   sprintf(strsql,"select kodg from Kartb where kodg=%s limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Город %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);
 }

if(SRAV(tabl,"Kateg",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Kartb");
    refresh();
   }
   sprintf(strsql,"select kateg from Kartb where kateg=%s limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Категорию %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);
 }

if(SRAV(tabl,"Opltp",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Pltp");
    refresh();
   }
   sprintf(strsql,"select oper from Pltp where oper='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Операцию %s удалить невозможно, \
потому, что есть платежные поручения с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Tpltp");
     refresh();
    }
   sprintf(strsql,"select oper from Tpltp where oper='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Операцию %s удалит невозможно, \
потому, что есть типовые платежные поручения\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);
 }

if(SRAV(tabl,"Opltt",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Pltt");
    refresh();
   }
   sprintf(strsql,"select oper from Pltt where oper='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
     printw(gettext("Операцию %s удалить невозможно, \
потому, что есть платежные требования с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    

   if(met != 0)
    {
     printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Tpltt");
     refresh();
    }
   sprintf(strsql,"select oper from Tpltt where oper='%s' limit 1",kod);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Операцию %s удалить невозможно, \
потому, что есть типовые платежные требования\nс этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);
 }

/*Не проверяем потому что записывается не код а сама фамилия*/
if(SRAV(tabl,"Ksn",0) == 0)
 {
  return(0);
 }



if(SRAV(tabl,"Oznvb",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Obqvka");
    refresh();
   }
  sprintf(strsql,"select kodop from Obqvka where kodop='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     printw("**%s\n",sql_error(&bd));
     OSTANOV();
    }
   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       beep();
       printw(gettext("Операцию %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
    }    
   return(0);

 }


if(SRAV(tabl,"Uospod",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosdok");
    refresh();
   }
  sprintf(strsql,"select podr from Uosdok where podr=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Подразделение %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 

  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosamor");
    refresh();
   }
  sprintf(strsql,"select podr from Uosamor where podr=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Подразделение %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }
if(SRAV(tabl,"Uosgrup",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosinp");
    refresh();
   }
  sprintf(strsql,"select hna from Uosinp where hna='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Группу %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 

  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosamor");
    refresh();
   }
  sprintf(strsql,"select hna from Uosamor where hna='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Группу %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }

if(SRAV(tabl,"Uosgrup1",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosinp");
    refresh();
   }
  sprintf(strsql,"select hnaby from Uosinp where hnaby='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Группу %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 

  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosamor1");
    refresh();
   }
  sprintf(strsql,"select hna from Uosamor1 where hna='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Группу %s удалить невозможно, \
потому, что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }

if(SRAV(tabl,"Uoshau",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosinp");
    refresh();
   }
  sprintf(strsql,"select hau from Uosinp where hau='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Шифр алитического учета %s удалить невозможно, \
потому,\nчто есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }
if(SRAV(tabl,"Uoshz",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosinp");
    refresh();
   }
  sprintf(strsql,"select hzt from Uosinp where hzt='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Шифр затрат %s удалить невозможно, \
потому,\nчто есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 

  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosamor");
    refresh();
   }
  sprintf(strsql,"select hzt from Uosamor where hzt='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Шифр затрат %s удалить невозможно, \
потому,\nчто есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }

if(SRAV(tabl,"Uosol",0) == 0)
 {
  if(met != 0)
   {
    printw(gettext("Проверяем возможность удаления в таблице %s\n"),"Uosdok");
    refresh();
   }
  sprintf(strsql,"select kodol from Uosdok where kodol=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код ответственного %s удалить невозможно, \
потому,\nчто есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   } 
  return(0);
 }

if(SRAV(tabl,"Uslgrup",0) == 0)
 {

  if(met != 0)
   {
    printw(gettext("Проверяем записи в таблице услуг:Uslugi\n"));
    refresh();
   }
  sprintf(strsql,"select kodgr from Uslugi where kodgr=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("Ошибка:%d %s\n",sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код группы услуги %s удалить невозможно, потому,\n\
что есть записи с этой группой\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }
  return(0);
 }



if(SRAV(tabl,"Uslugi",0) == 0)
 {

  if(met != 0)
   {
    printw(gettext("Проверяем записи в таблице:Usldokum1\n"));
    refresh();
   }
  sprintf(strsql,"select metka,kodzap from Usldokum1 where metka=1 \
and kodzap=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("%s\nОшибка:%d %s\n",strsql,sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код услуги %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }
  if(met != 0)
   {
    printw(gettext("Проверяем записи в таблице:Usldokum2\n"));
    refresh();
   }
  sprintf(strsql,"select metka,kodzap from Usldokum2 where metka=1 \
and kodzap=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("%s\nОшибка:%d %s\n",strsql,sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код услуги %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

 if(met != 0)
  {
   printw("%s:%s\n",
   gettext("Проверяем записи в таблице"),"Specif");
   refresh();
  }

  sprintf(strsql,"select kodi from Specif where kodi=%s or kodd=%s and kz=1 limit 1",kod,kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("%s\nОшибка:%d %s\n",strsql,sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Restdok1");
    refresh();
   }
  
  sprintf(strsql,"select mz from Restdok1 where mz=1 and kodz=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
       msql_error(&bd,gettext("Ошибка"),strsql);
    }
   
  if(kolstr > 0)
   {
     if(met != 0)
      {
       printw("%s Restdok1",gettext("Таблица"));
       printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
       OSTANOV();
      }
     return(1);
   }

 
  return(0);
 }


if(SRAV(tabl,"Gkont",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Kontragent");
    refresh();
   }
  sprintf(strsql,"select kodkon from Kontragent where grup='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      printw("%s\nОшибка:%d %s\n",strsql,sql_nerror(&bd),sql_error(&bd));
      OSTANOV();
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        beep();
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }
  return(0);
 }



if(SRAV(tabl,"Ukrvkr",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Ukrdok");
    refresh();
   }

  sprintf(strsql,"select vkom from Ukrdok where vkom='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }
  return(0);
 }

if(SRAV(tabl,"Ukrgrup",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Ukrkras");
    refresh();
   }

  sprintf(strsql,"select kgr from Ukrkras where kgr=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }
  return(0);
 }

if(SRAV(tabl,"Uplpodr",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Upldok");
    refresh();
   }

  sprintf(strsql,"select kp from Upldok where kp=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Upldok1");
    refresh();
   }

  sprintf(strsql,"select kp from Upldok1 where kp=%s limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  return(0);
 }



if(SRAV(tabl,"Zargrupp",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",gettext("Проверяем записи в таблице"),"Podr");
    refresh();
   }

  sprintf(strsql,"select  kod from Podr where grup='%s' limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  return(0);
 }

if(SRAV(tabl,"Reegrup1",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Reenn");
    refresh();
   }

  sprintf(strsql,"select gr from Reenn where gr='%s' and tp=1 limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  return(0);
 }
if(SRAV(tabl,"Reegrup2",0) == 0)
 {

  if(met != 0)
   {
    printw("%s:%s\n",
    gettext("Проверяем записи в таблице"),"Reenn");
    refresh();
   }

  sprintf(strsql,"select gr from Reenn where gr='%s' and tp=2 limit 1",kod);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     {
      msql_error(&bd,gettext("Ошибка"),strsql);
      return(1);
     }
   }
  else
   {   
    
    if(kolstr != 0)
     {
      if(met != 0)
       {
        printw(gettext("Код %s удалить невозможно, потому,\n\
что есть записи с этим кодом\n"),kod);
        OSTANOV();
       }
      return(1);
     }
   }

  return(0);
 }


sprintf(strsql,"%s-%s %s !",__FUNCTION__,gettext("Для проверки задана неизвестная таблица"),tabl);
iceb_t_soob(strsql);

return(1);

}
