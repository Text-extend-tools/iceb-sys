/*$Id:$*/
/*16.04.2017	19.04.2015	Белых А.И.	dirf1dfvs.c
Военный сбор
*/
#include        "buhg.h"
class dirf1dfvs_poi
 {
  public:
   class iceb_tu_str fio;  
   class iceb_tu_str inn;
   short metkap;

   class iceb_tu_str nomd;
   int kvrt;
   short god;
   double nah_dohod;
   double vip_dohod;
   double nah_vs;
   double vip_vs;
   
  dirf1dfvs_poi()
   {
    fio.plus("");
    inn.plus("");
    metkap=0;
   }
 };

void dirf1dfvs_sap(class dirf1dfvs_poi *poi,int dlina_fio);
int dirf1dfvs_poi(class dirf1dfvs_poi *poi);
int dirf1df_pup(const char *inn,const char *fiot,class iceb_tu_str *fio,class dirf1dfvs_poi *poi);
void dirf1dfvs_rs(class SQLCURSOR *cur,class dirf1dfvs_poi *poi);
int dirf1dfvs_v(const char *nomd,const char *inn);

void dirf1dfvs(const char *nomd)
{
int K=0;
int mspz=0;
int		prc;
int             Y=4;
int             X=0;
int kom=0;
class iceb_tu_str fio("");
class iceb_tu_str inn("");
int i=0;
SQL_str row;
class iceb_t_mstr spis_menu(0);
int pozz=0,pozz1=0;
class dirf1dfvs_poi poi;
int kolstr=0;
class SQLCURSOR cur;
char strsql[1024];
int max_dlina_fio=0;
int dlina_fio=0;
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

/*Читаем шапку документа*/
sprintf(strsql,"select * from F8dr where nomd='%s'",nomd);
if(sql_readkey(&bd,strsql,&row,&cur) < 1)
 {
  iceb_t_soob(gettext("Не найден документ !"));
  return;
 } 

poi.god=atoi(row[0]);
poi.kvrt=atoi(row[1]);
poi.nomd.new_plus(nomd);

naz1:;

clear();

short           kls=LINES-7;

sprintf(strsql,"select * from F1dfvs where nomd='%s' order by inn asc",nomd);



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirf1dfvs_sap(&poi,30);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

poi.nah_dohod=poi.nah_vs=poi.vip_dohod=poi.vip_vs=0.;
max_dlina_fio=0;
while(cur.read_cursor(&row) != 0)
 {
  poi.nah_dohod+=atof(row[3]);
  poi.nah_vs+=atof(row[4]);
  poi.vip_dohod+=atof(row[5]);
  poi.vip_vs=atof(row[6]);    

  if(row[2][0] == '\0')
   {
    class SQLCURSOR cur1;
    SQL_str row1;        
    /*Читаем фамилию*/

    sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
    if(readkey(strsql,&row1,&cur1) < 1)
     {
//      sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),row[1]);
//      iceb_t_soob(strsql);
     fio.new_plus("");
     }
    else
      fio.new_plus(row1[0]);
   }
  else
     fio.new_plus(row[2]);
  if(max_dlina_fio < iceb_tu_strlen(fio.ravno()))
   max_dlina_fio=iceb_tu_strlen(fio.ravno());
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;



cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
dlina_fio=COLS-70;
if(max_dlina_fio < dlina_fio)
 dlina_fio=max_dlina_fio;
 
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  if(dirf1df_pup(row[1],row[2],&fio,&poi) != 0)
   continue;

  sprintf(strsql,"%s|%-*.*s|%10.2f|%8.2f|%10.2f|%8.2f",
  row[1],
  iceb_tu_kolbait(dlina_fio,fio.ravno()),  
  iceb_tu_kolbait(dlina_fio,fio.ravno()),  
  fio.ravno(),
  atof(row[3]),    
  atof(row[4]),    
  atof(row[5]),    
  atof(row[6]));
  
  i++;
  spis_menu.spis_str.plus(strsql);
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirf1dfvs_sap(&poi,dlina_fio);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-24);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(dirf1df_pup(row[1],row[2],&fio,&poi) != 0)
     continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);
    if(dirf1df_pup(row[1],row[2],&fio,&poi) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(dirf1df_pup(row[1],row[2],&fio,&poi) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&inn,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
  //  iceb_t_pdoc("uos5_10.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirf1dfvs_v(nomd,inn.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirf1dfvs_v(nomd,"");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from F1dfvs where nomd='%s' and inn='%s'",nomd,inn.ravno());
      sql_zapis(strsql,0,0);
     }
    goto naz1;

  case FK4:       /*Поиск*/
     dirf1dfvs_poi(&poi);
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    dirf1dfvs_rs(&cur,&poi);
    goto naz;


  default:
    goto naz;
    break;
 }

return;

}
/*******************************************/
/*шапка*/
/***************************/
void dirf1dfvs_sap(class dirf1dfvs_poi *poi,int dlina_fio)
{
short		y=2;
short           x=COLS-20;

clear();

printw("%s %s:%s %s:%d %s:%d\n",
gettext("Военный сбор"),
gettext("Документ"),
poi->nomd.ravno(),
gettext("Год"),
poi->god,
gettext("Квартал"),
poi->kvrt);
printw("%s:%.2f %.2f\n",gettext("Начислено"),poi->nah_dohod,poi->nah_vs);
printw("%s:%.2f %.2f\n",gettext("Выплачено"),poi->vip_dohod,poi->vip_vs);
char stroka[1024];
int razmer=dlina_fio/2;
sprintf(stroka,"      1     %*s2%*s      3          4          5        6",razmer," ",razmer," ");

//mvprintw(3,0,"      1                   2                     3           4          5        6");

mvprintw(3,0,"%s",stroka);

mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("идент.код"));
mvprintw(y++,x,"2-%s",gettext("фамилия"));
mvprintw(y++,x,"3-%s",gettext("нач.доход"));
mvprintw(y++,x,"4-%s",gettext("нач.сбор"));
mvprintw(y++,x,"5-%s",gettext("выпл.доход"));
mvprintw(y++,x,"6-%s",gettext("выпл.сбор"));

if(poi->metkap != 0)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  if(poi->inn.getdlinna() > 1)
    mvprintw(y++,x,"%s:%s",gettext("Иден.к."),poi->inn.ravno());
  if(poi->fio.getdlinna() > 1)
    mvprintw(y++,x,"%s:%s",gettext("Фамилия"),poi->fio.ravno());
  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

}
/*****************************************/
/*поиск*/
/****************************************/

int dirf1dfvs_poi(class dirf1dfvs_poi *poi)
{
int kom=0;
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Идентификационній код..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Фамлилия...............(,,)."));

VV.VVOD_SPISOK_add_data(poi->inn.ravno(),255);
VV.VVOD_SPISOK_add_data(poi->fio.ravno(),255);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi->inn.new_plus(VV.VVOD_SPISOK_return_data(0));
poi->fio.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   poi->metkap=0;
   return(1);

  case FK2:
  case PLU:
     poi->metkap=1;
     return(0);
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

}
/**************************************/
/*проверка условий поиска*/
/*************************************/
int dirf1df_pup(const char *inn,const char *fiot,class iceb_tu_str *fio,class dirf1dfvs_poi *poi)
{
fio->new_plus("");
if(fiot[0] == '\0')
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[1024];
  /*Читаем фамилию*/

  sprintf(strsql,"select fio from Kartb where inn='%s'",inn);
  if(sql_readkey(&bd,strsql,&row1,&cur1) < 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),inn);
    iceb_t_soob(strsql);
   }
  else
    fio->new_plus(row1[0]);
 }
else
   fio->new_plus(fiot);

if(poi->metkap == 0)
 return(0);
if(proverka(poi->inn.ravno(),inn,4,0) != 0)
  return(1);
if(proverka(poi->fio.ravno(),fio->ravno(),4,0) != 0)
  return(1);
return(0);

}
/**********************************************/
/*Распечатка*/
/***********************************************/
void dirf1dfvs_rs(class SQLCURSOR *cur,class dirf1dfvs_poi *poi)
{
SQL_str row;
char imaf[64];
class iceb_tu_str fio("");
sprintf(imaf,"vs%d.lst",getpid());
double itogo[4];
class iceb_fopen fil;

if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Военный сбор"),fil.ff);

fprintf(fil.ff,"%s:%s %s:%d %s:%d\n",
gettext("Документ"),
poi->nomd.ravno(),
gettext("Год"),
poi->god,
gettext("Квартал"),
poi->kvrt);


fprintf(fil.ff,"----------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%s\n",gettext("Идент.код |       Фамилия                |Нач.доход |Нач.сбор|Вып.доход |Вып.сбор|"));

fprintf(fil.ff,"----------------------------------------------------------------------------------\n");

cur->poz_cursor(0);
memset(itogo,'\0',sizeof(itogo));

while(cur->read_cursor(&row) != 0)
 {
  if(dirf1df_pup(row[1],row[2],&fio,poi) != 0)
   continue;
  
  fprintf(fil.ff,"%-10s|%-*.*s|%10.2f|%8.2f|%10.2f|%8.2f|\n",
  row[1],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  atof(row[3]),
  atof(row[4]),
  atof(row[5]),
  atof(row[6]));
  
  itogo[0]+=atof(row[3]);
  itogo[1]+=atof(row[4]);
  itogo[2]+=atof(row[5]);
  itogo[3]+=atof(row[6]);
  
 }

fprintf(fil.ff,"----------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%46s:%10.2f %8.2f %10.2f %8.2f\n",gettext("Итого"),itogo[0],itogo[1],itogo[2],itogo[3]);
podpis(fil.ff);
fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Военный сбор"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/**********************************************/
/*Ввод по табельному номеру*/
/***********************************/
int dirf1dfvs_v(const char *nomd,const char *inn)
{
char strsql[1024];
class VVOD VV(0);
class SQLCURSOR cur;
SQL_str row;
int N=0,K=0;
class iceb_tu_str fio("");

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(60);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(20);//5

if(inn[0] != '\0')
 {
  
  sprintf(strsql,"select * from F1dfvs where nomd='%s' and inn='%s'",nomd,inn);
  if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден идентификационный номер"),inn);
    iceb_t_soob(strsql);     
    return(1);
   }  

  VV.data_plus(0,inn);
  VV.data_plus(1,row[2]);
  VV.data_plus(2,row[3]);
  VV.data_plus(3,row[4]);
  VV.data_plus(4,row[5]);
  VV.data_plus(5,row[6]);
 
  kzvz(row[7],row[8],&VV);

  if(row[2][0] == '\0')
   {
    SQL_str row1;
    class SQLCURSOR cur1;
    /*Читаем фамилию*/

    sprintf(strsql,"select fio from Kartb where inn='%s'",inn);
    if(sql_readkey(&bd,strsql,&row1,&cur1) < 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),inn);
      iceb_t_soob(strsql);
     }
    else
      VV.data_plus(1,row1[0]);
   }
  

 }

VV.VVOD_SPISOK_add_MD(gettext("Идентификационный номер...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Фамилия...................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма начисленного дохода.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Сумма начисленного сбора..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Сумма выплаченного дохода.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Сумма выплаченного сбора..."));//5

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
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

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*идентификационный код*/
     {
      if(strlen(VV.data_ravno(N)) < 8)
       {
        sprintf(strsql,"select fio,inn from Kartb where tabn=%d",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(0,row[1]);
          VV.data_plus(1,row[0]);
          N+=2;
          goto naz;
         }        
       }
      sprintf(strsql,"select fio from Kartb where inn='%s'",VV.data_ravno(N));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        VV.data_plus(1,row[0]);
        N++;
        goto naz;
       }
     }
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
     N=-1;
     break;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

     GDITE();

     if(VV.data_ravno(0)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введён идентификационный код !"));
       goto naz;
      }

     if(SRAV(inn,VV.data_ravno(0),0) != 0)
      {
       sprintf(strsql,"select inn from F1dfvs where nomd='%s' and inn='%s'",nomd,VV.data_ravno(0));
       if(readkey(strsql) >= 1)
        {
         iceb_t_soob(gettext("Такая запись уже есть!"));
         goto naz;
        }
      }




     sprintf(strsql,"select fio from Kartb where inn='%s'",VV.data_ravno(0));
     if(sql_readkey(&bd,strsql,&row,&cur) < 1)
        fio.new_plus(VV.data_ravno(1));
     else
        fio.new_plus("");
             



     if(inn[0] == '\0')
      {
       sprintf(strsql,"insert into F1dfvs \
values ('%s','%s','%s',%.2f,%.2f,%.2f,%.2f,%d,%ld)",
       nomd,
       VV.data_ravno(0),
       fio.ravno_filtr(),
       ATOFM(VV.data_ravno(2)),
       ATOFM(VV.data_ravno(3)),
       ATOFM(VV.data_ravno(4)),
       ATOFM(VV.data_ravno(5)),
       iceb_t_getuid(),
       time(NULL));
     }
    else            
      sprintf(strsql,"update F1dfvs set \
inn='%s',\
fio='%s',\
sfr=%.2f,\
svs=%.2f,\
sfrv=%.2f,\
svsv=%.2f,\
ktoz=%d,\
vrem=%ld \
where nomd='%s' and inn='%s'",
      VV.data_ravno(0),
      fio.ravno_filtr(),
      ATOFM(VV.data_ravno(2)),
      ATOFM(VV.data_ravno(3)),
      ATOFM(VV.data_ravno(4)),
      ATOFM(VV.data_ravno(5)),
      iceb_t_getuid(),
      time(NULL),
      nomd,
      inn);
     
     if(sql_zapis(strsql,1,0) != 0)
       goto naz;

     break;
    }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

return(0);

}
