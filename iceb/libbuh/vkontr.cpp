/*$Id: vkontr.c,v 5.60 2014/07/31 07:09:28 sasa Exp $*/
/*29.04.2018    14.09.1993      Белых А.И.      vkontr.c
Ввод организаций
*/
#include        <errno.h>
#include	"buhl.h"

void vkontr_rk(const char *kontr);

extern char	*imabaz;


int vkontr(const char *sht, /*Счет */
class iceb_tu_str *kodp, //Код контрагента
int metka_kor) /*0- ввод новой записи 1-корректировка*/
{
int kt=0; /*Кто записал*/
time_t vr=0; /*Время записи*/
class spis_oth oth;
SQL_str         row;
SQLCURSOR       cur;
time_t		vrem;
class iceb_tu_str kodz("");
class iceb_tu_str naimz("");
char		strsql[4096];
int		poz,komv;
class iceb_tu_str kod("");
class iceb_tu_str naimp("");
int		maxkz; //Максимальное количество знаков в коде контрагента с лидирующими нолями
short		metkor;
int K=0,N=0;
class iceb_tu_str kodgr("");
int metka_nal=0;
int voz=0;

VVOD		DANET(1);

VVOD  VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента......."));//0
VV.VVOD_SPISOK_add_MD(gettext("Короткое наименование."));//1
VV.VVOD_SPISOK_add_MD(gettext("Адрес................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код ЕГРПОУ............"));//3
VV.VVOD_SPISOK_add_MD(gettext("Наименование банка...."));//4
VV.VVOD_SPISOK_add_MD(gettext("МФО..................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Номер счета..........."));//6
VV.VVOD_SPISOK_add_MD(gettext("Инд. нал. номер......."));//7
VV.VVOD_SPISOK_add_MD(gettext("Город (банка)........."));//8
VV.VVOD_SPISOK_add_MD(gettext("Номер св. пл. НДС....."));//9
VV.VVOD_SPISOK_add_MD(gettext("Номер телефона........"));//10
VV.VVOD_SPISOK_add_MD(gettext("Группа контрагента...."));//11
VV.VVOD_SPISOK_add_MD(gettext("Налоговый адрес......."));//12
VV.VVOD_SPISOK_add_MD(gettext("Рег. номер ФОП........"));//13
VV.VVOD_SPISOK_add_MD(gettext("Полное наименование..."));//14
VV.VVOD_SPISOK_add_MD(gettext("Населённый пункт......"));//15
VV.VVOD_SPISOK_add_MD(gettext("Система налога........"));//16

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(255);//1
VV.VVOD_SPISOK_add_data(100);//2
VV.VVOD_SPISOK_add_data(15);//3
VV.VVOD_SPISOK_add_data(50);//4
VV.VVOD_SPISOK_add_data(20);//5
VV.VVOD_SPISOK_add_data(30);//6
VV.VVOD_SPISOK_add_data(20);//7
VV.VVOD_SPISOK_add_data(50);//8
VV.VVOD_SPISOK_add_data(15);//9
VV.VVOD_SPISOK_add_data(50);//10
VV.VVOD_SPISOK_add_data(50);//11
VV.VVOD_SPISOK_add_data(255);//12
VV.VVOD_SPISOK_add_data(30);//13
VV.VVOD_SPISOK_add_data(255);//14
VV.VVOD_SPISOK_add_data(255);//15
VV.VVOD_SPISOK_add_data(64);//16

if(metka_kor == 1)
 {
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kodp->ravno());
  if(readkey(strsql,&row,&cur) != 1)
   return(1);

    VV.VVOD_SPISOK_zapis_data(0,row[0]);
    VV.VVOD_SPISOK_zapis_data(1,row[1]);
    VV.VVOD_SPISOK_zapis_data(2,row[3]);
    VV.VVOD_SPISOK_zapis_data(3,row[5]);
    VV.VVOD_SPISOK_zapis_data(4,row[2]);
    VV.VVOD_SPISOK_zapis_data(5,row[6]);
    VV.VVOD_SPISOK_zapis_data(6,row[7]);
    VV.VVOD_SPISOK_zapis_data(7,row[8]);
    VV.VVOD_SPISOK_zapis_data(8,row[4]);
    VV.VVOD_SPISOK_zapis_data(9,row[9]);
    VV.VVOD_SPISOK_zapis_data(10,row[10]);

    VV.VVOD_SPISOK_zapis_data(11,row[11]);

    VV.VVOD_SPISOK_zapis_data(12,row[14]);
    VV.VVOD_SPISOK_zapis_data(13,row[15]);
    VV.VVOD_SPISOK_zapis_data(14,row[16]);
    VV.VVOD_SPISOK_zapis_data(15,row[17]);
    metka_nal=atoi(row[18]);
        
    kt=atoi(row[12]);
    vr=atol(row[13]);
 }
else
  VV.VVOD_SPISOK_zapis_data(0,kodp->ravno()); /*обязательно*/


if(metka_kor == 1)
 {

  kodz.new_plus(kodp->ravno());

 }
naimz.new_plus(VV.VVOD_SPISOK_return_data(1));


VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка реквизитов контрагента"));

kzvz(kt,vr,&VV);


naz:;
clear();

if(metka_nal == 0)
 VV.data_plus(16,gettext("Общая система"));
else
 VV.data_plus(16,gettext("Единый налог"));

//Если введена группа то читаем её наименование
if(VV.data_ravno(11)[0] != '\0')
 {
  if(polen(VV.data_ravno(11),&kodgr,1,' ') != 0)
       kodgr.new_plus(VV.data_ravno(11));
    
  sprintf(strsql,"select naik from Gkont where kod=%d",kodgr.ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   {
    sprintf(strsql,"%s %.40s",kodgr.ravno(),row[0]);
    VV.data_plus(11,strsql);
   }
 }
/************
par=0;
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
 par=1;
***************/
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("гр.кон."),
"F4",gettext("найти код"),
"F5",gettext("печать"),
"F6",gettext("дополн. информ."),
"F7",gettext("договора"),
"F10",gettext("выход"),NULL);





VV.vramka(-1,-1,0);

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    if(N == 16) //система налогообложения
     {
      class VVOD MENU(3);

      MENU.VVOD_SPISOK_add_MD(gettext("Общая система"));
      MENU.VVOD_SPISOK_add_MD(gettext("Единый налог"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      
      int kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 2 :
        case -1:
          goto naz;
      
        case 0 :
          metka_nal=0;
          break;

        case 1 :
          metka_nal=1;
          break;
       }
      goto naz;
     }

    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0)
     {

     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    voz=1;
    break;
   }

  if(K == FK1) /*Помошь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    iceb_t_pdoc("buhg4_2_2.txt");
    clear();
    goto naz;

   }
   
  if(K == FK2 || K == PLU) /*Записываем*/
   {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён код контрагента !"));
      goto naz;
     }

    if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введено наименование контрагента !"));
      goto naz;
     }
    kodgr.new_plus("");
    if(VV.VVOD_SPISOK_return_data(11)[0] != '\0')
     {
      if(polen(VV.data_ravno(11),&kodgr,1,' ') != 0)
       kodgr.new_plus(VV.data_ravno(11));
      /*Проверяем группу контрагента*/
      sprintf(strsql,"select naik from Gkont where kod=%d",kodgr.ravno_atoi());
      if(sql_readkey(&bd,strsql) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Нет найден код группы контрагента"),kodgr.ravno());
        iceb_t_soob(strsql);
        goto naz;
       }

     }

    /*Проверяем может код контрагента уже введен*/
    if(SRAV(kodz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      VV.VVOD_SPISOK_return_data(0));
      if(sql_readkey(&bd,strsql) >= 1)
       {
        sprintf(strsql,gettext("Код контрагента %s уже введён !"),VV.VVOD_SPISOK_return_data(0));
        iceb_t_soob(strsql);
        goto naz;
       }
     }
    metkor=0;
    if(metka_kor == 1 && SRAV(kodz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код контрагента ? Вы уверены ?"));
      if(danet(&DANET,2,stdscr) == 2)
        goto naz;
      metkor=1;
     }

    /*Проверяем может такое наименование уже введено*/
    if(SRAV(naimz.ravno(),VV.VVOD_SPISOK_return_data(1),0) != 0)
     {
      sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",VV.data_ravno_filtr(1));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        sprintf(strsql,gettext("Такое наименование контрагента уже введено под кодом %s !"),row[0]);
        iceb_t_soob(strsql);
        goto naz;
       }
     }

    time(&vrem);

    /*Проверка в счете такого кода контрагента*/

    if(sht[0] != '\0')
     {

      if(SRAV(VV.VVOD_SPISOK_return_data(0),kodp->ravno(),0) != 0)
       {
        sprintf(strsql,"select ns from Skontr where ns='%s' and \
  kodkon='%s'",sht,VV.VVOD_SPISOK_return_data(0));
        if(sql_readkey(&bd,strsql) == 1)
         {
          sprintf(strsql,gettext("Код контрагента %s уже введён !"),VV.VVOD_SPISOK_return_data(0));
          iceb_t_soob(strsql);
          goto naz;
         }
       }

      sprintf(strsql,"delete from Skontr where ns='%s' and \
kodkon='%s'",sht,kodz.ravno());
//      printw("\nstrsql=%s\n",strsql);
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
         goto naz;
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }      
      sprintf(strsql,"insert into Skontr \
values ('%s','%s',%d,%ld)",sht,VV.VVOD_SPISOK_return_data(0),iceb_t_getuid(),vrem);
      
//        printw("\nstrsql=%s\n",strsql);
//        OSTANOV();
      
       if(sql_zap(&bd,strsql) != 0)
        {
         msql_error(&bd,gettext("Ошибка ввода записи !"),strsql);
         goto naz;
        }
     }


   /*Новая запись*/

    if(metka_kor == 0)
     {   
      sprintf(strsql,"insert into Kontragent \
values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',\
%d,%ld,'%s','%s','%s','%s',%d)",
      VV.VVOD_SPISOK_return_data(0),
      VV.data_ravno_filtr(1),
      VV.data_ravno_filtr(4),
      VV.data_ravno_filtr(2),
      VV.data_ravno_filtr(8),
      VV.VVOD_SPISOK_return_data(3),
      VV.VVOD_SPISOK_return_data(5),
      VV.VVOD_SPISOK_return_data(6),
      VV.VVOD_SPISOK_return_data(7),
      VV.VVOD_SPISOK_return_data(9),
      VV.VVOD_SPISOK_return_data(10),
      kodgr.ravno(),
      iceb_t_getuid(),vrem,
      VV.data_ravno_filtr(12),
      VV.VVOD_SPISOK_return_data(13),
      VV.VVOD_SPISOK_return_data_filtr(14),
      VV.data_ravno_filtr(15),
      metka_nal);
     } 
   else/*Корректировка записи*/
     {   
      memset(strsql,'\0',sizeof(strsql));
            
      sprintf(strsql,"update Kontragent \
set \
kodkon='%s',\
naikon='%s',\
naiban='%s',\
adres='%s',\
adresb='%s',\
kod='%s',\
mfo='%s',\
nomsh='%s',\
innn='%s',\
nspnds='%s',\
telef='%s',\
grup='%s',\
ktoz=%d,\
vrem=%ld,\
na='%s',\
regnom='%s',\
pnaim='%s',\
gk='%s',\
en=%d \
where kodkon='%s'",
      VV.VVOD_SPISOK_return_data(0),
      VV.data_ravno_filtr(1),
      VV.data_ravno_filtr(4),
      VV.data_ravno_filtr(2),
      VV.data_ravno_filtr(8),
      VV.VVOD_SPISOK_return_data(3),
      VV.VVOD_SPISOK_return_data(5),
      VV.VVOD_SPISOK_return_data(6),
      VV.VVOD_SPISOK_return_data(7),
      VV.VVOD_SPISOK_return_data(9),
      VV.VVOD_SPISOK_return_data(10),
      kodgr.ravno(),
      iceb_t_getuid(),vrem,
      VV.data_ravno_filtr(12),
      VV.VVOD_SPISOK_return_data(13),
      VV.VVOD_SPISOK_return_data_filtr(14),
      VV.data_ravno_filtr(15),
      metka_nal,
      kodz.ravno());
     }

//    printw("\nstrsql=%s\n",strsql);
//    OSTANOV();


    GDITE();
    if(sql_zap(&bd,strsql) != 0)
     {
     if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
      {
       iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
       goto naz;
      }
     else
      {
       msql_error(&bd,gettext("Ошибка ввода записи !"),strsql);
       goto naz;
      }
     }

    if(metkor == 1)
     {
      VVOD SOOBN(1);
      SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода контрагента в таблицах !"));
      soobsh(&SOOBN,-1,-1,1);

      GDITE();
      smenkodk(kodz.ravno(),VV.VVOD_SPISOK_return_data(0));
     }
    kodp->new_plus(VV.VVOD_SPISOK_return_data(0));

     break;
   }
  if(K == FK3) /*Просмотр списка групп контрагентов*/
   {
     clear();
     naimp.new_plus("");
     kod.new_plus("");
     if(dirmat1("Gkont",&kod,&naimp,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
      {
       VV.VVOD_SPISOK_zapis_data(11,kod.ravno());
      }
     goto naz;
   }  
  if(K ==  FK4)  /*Взять новый код*/
   {
    GDITE();
    iceb_t_poldan("Максимальное количество знаков в коде контрагента с лидирующими нолями",&maxkz,"nastrb.alx");

    if(maxkz == 0)
     {
      sprintf(strsql,"%d",iceb_t_getnkontr(1));
      VV.VVOD_SPISOK_zapis_data(0,strsql);
     }
    else
     { 
      sprintf(strsql,"%0*d",maxkz,iceb_t_getnkontr(1));
      VV.VVOD_SPISOK_zapis_data(0,strsql);
     }
    goto naz;
   }  
  if(K == FK5) /*Распечатка реквизитов*/
   {
    vkontr_rk(VV.data_ravno(0));
    goto naz;
   }
  if(K == FK6) /*Запись дополнительной информации*/
   {
    GDITE();
    //Установить флаг работы с карточкой
    sprintf(strsql,"k%s%s",imabaz,VV.VVOD_SPISOK_return_data(0));

    if(sql_flag(&bd,strsql,0,0) != 0)
     {
      iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
      goto naz;
     }


    sprintf(strsql,"kontr%d.tmp",getpid());
    dikont('-',strsql,VV.VVOD_SPISOK_return_data(0),"Kontragent1",VV.VVOD_SPISOK_return_data(13));
    vizred(strsql);
    GDITE();
    dikont('+',strsql,VV.VVOD_SPISOK_return_data(0),"Kontragent1",VV.VVOD_SPISOK_return_data(13));
    unlink(strsql);

    /*Снять флаг работы с карточкой*/
    sprintf(strsql,"k%s%s",imabaz,VV.VVOD_SPISOK_return_data(0));
    sql_flag(&bd,strsql,0,1);
    goto naz;
   }
  if(K == FK7) /*ввод списка договоров*/
   {
    iceb_t_dirdog(0,VV.data_ravno(0));
    goto naz;
   }

  if(K == FK10 || K ==  ESC)
   {
    kodp[0]='\0'; /*Чтобы не было поиска по этому коду после выхода*/
    
    break;
   }



 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(voz);
}
/********************************/
/*Корректировка кода контрагента*/
/********************************/

void smenkodk(const char *skod, //Старый код
const char *nkod)  //Новый код
{
char		strsql[1024];
class iceb_tu_str bros("");
SQL_str		row;
time_t          vrem;

time(&vrem);


sprintf(strsql,"update Skontr set kodkon='%s',ktoi=%d,vrem=%ld where kodkon='%s'",nkod,
iceb_t_getuid(),
vrem,
skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return;
  }
 else
  if(sql_nerror(&bd) != ER_DUP_ENTRY) //Запись уже есть
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
 }
 
sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Saldo set kodkon='%s',ktoi=%d,vrem=%ld where kodkon='%s'",
nkod,
iceb_t_getuid(),
vrem,
skod);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_DUP_ENTRY) //Запись уже есть
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
 }


sprintf(strsql,"update Dokummat set kontr='%s' where kontr='%s'",nkod,skod);

sql_zapis(strsql,0,0);

sprintf(strsql,"update Dokummat set k00='%s' where k00='%s'",nkod,skod);

sql_zapis(strsql,0,0);

sprintf(strsql,"update Gnali set kontr='%s' where kontr='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Uosdok set kontr='%s' where kontr='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Usldokum set kontr='%s' where kontr='%s'",nkod,skod);

sql_zapis(strsql,0,0);

sprintf(strsql,"update Usldokum set k00='%s' where k00='%s'",nkod,skod);

sql_zapis(strsql,0,0);


sprintf(strsql,"update Kasord1 set kontr='%s' where kontr='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kontragent2 set kodkon='%s' where kodkon='%s'",nkod,skod);
sql_zapis(strsql,0,0);

sprintf(strsql,"update Kontragent1 set kodkon='%s' where kodkon='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Kontragent2 set kodkon='%s' where kodkon='%s'",nkod,skod);
sql_zapis(strsql,0,0);

SQLCURSOR cur;

sprintf(strsql,"select distinct polu from Pltp where polu like '%s#%%'",
skod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_tu_str rek(nkod);
   polen(row[0],&bros,1,'#');
   rek.plus(iceb_tu_adrsimv(iceb_tu_strlen(bros.ravno()),row[0]));

   sprintf(strsql,"update Pltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   sql_zapis(strsql,1,0);

  }

sprintf(strsql,"select distinct polu from Pltt where polu like '%s#%%'",
skod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_tu_str rek(nkod);
   polen(row[0],&bros,1,'#');
   rek.plus(iceb_tu_adrsimv(iceb_tu_strlen(bros.ravno()),row[0]));

   sprintf(strsql,"update Pltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   sql_zapis(strsql,1,0);

  }


sprintf(strsql,"select distinct polu from Tpltt where polu like '%s#%%'",
skod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_tu_str rek(nkod);
   polen(row[0],&bros,1,'#');
   rek.plus(iceb_tu_adrsimv(iceb_tu_strlen(bros.ravno()),row[0]));

   sprintf(strsql,"update Tpltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   sql_zapis(strsql,1,0);

  }
sprintf(strsql,"select distinct polu from Tpltp where polu like '%s#%%'",skod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
   class iceb_tu_str rek(nkod);
   polen(row[0],&bros,1,'#');
   rek.plus(iceb_tu_adrsimv(iceb_tu_strlen(bros.ravno()),row[0]));

   sprintf(strsql,"update Tpltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   sql_zapis(strsql,1,0);

  }


sprintf(strsql,"update Ukrdok1 set kontr='%s' where kontr='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);


sprintf(strsql,"update Ukrdok1 set kdrnn='%s' where kdrnn='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

}
/******************************/
/*распечатка реквизитов контрагента*/
/*********************************/
void vkontr_rk(const char *kontr)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff;
char imaf[64];


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr);
if(readkey(strsql,&row,&cur) <= 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден контрагент"),kontr);
  
  iceb_t_soob(strsql);
  return;
 }


sprintf(imaf,"kontr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Реквизиты контрагента"),0,0,0,0,0,0,ff);
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kontr,row[1]);
fprintf(ff,"---------------------------------------------------------\n");

fprintf(ff,"%s%s\n",gettext("Адрес..............."),row[3]);
fprintf(ff,"%s%s\n",gettext("Код ЕГРПОУ.........."),row[5]);
fprintf(ff,"%s%s\n",gettext("Наименование банка.."),row[2]);
fprintf(ff,"%s%s\n",gettext("МФО................."),row[6]);
fprintf(ff,"%s%s\n",gettext("Номер счета........."),row[7]);
fprintf(ff,"%s%s\n",gettext("Инд. нал. номер....."),row[8]);
fprintf(ff,"%s%s\n",gettext("Город (банка)......."),row[4]);
fprintf(ff,"%s%s\n",gettext("Номер св. пл. НДС..."),row[9]);
fprintf(ff,"%s%s\n",gettext("Номер телефона......"),row[10]);
fprintf(ff,"%s%s\n",gettext("Группа контрагента.."),row[11]);
fprintf(ff,"%s%s\n",gettext("Налоговый адрес....."),row[14]);
fprintf(ff,"%s%s\n",gettext("Рег. номер Ч.П. ...."),row[15]);
fprintf(ff,"%s%s\n",gettext("Полное наименование."),row[16]);


sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' order by nomz asc",kontr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"-----------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
   fprintf(ff,"%s\n",row[0]);
 }
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Реквизиты контрагента"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
