/* $Id: dirtabelo.c,v 5.47 2013/05/17 14:55:59 sasa Exp $ */
/*28.03.2018	31.07.2000	Белых А.И.	dirtabelo.c
Ввод и корректировка отработанного времени
*/
#include        <errno.h>
#include        "buhg.h"

class dirtabelo_poi
 {
  public:
  class iceb_tu_str kodpoi;
  class iceb_tu_str fiopoi;
  class iceb_tu_str podpoi;
  class iceb_tu_str tabpoi;
  class iceb_tu_str koment;
  int		metpoi; //0-без проверки 1-с проверкой

  dirtabelo_poi()
   {
    kodpoi.new_plus("");
    fiopoi.new_plus("");
    podpoi.new_plus("");
    tabpoi.new_plus("");
    koment.new_plus("");
    metpoi=0;
   }
  
 };

void saptab(short mp,short gp,class dirtabelo_poi *poi,int dlinna_fio);
int		provztab(SQL_str,class dirtabelo_poi*,class iceb_tu_str *fio);
void		rasztab(SQLCURSOR *cur,short mp,short gp,class dirtabelo_poi *poi,int kolstr);
void            impotvr(short,short);
void		voodtvsem(short,short);
void dirtabelo_uz(SQLCURSOR *cur,int kolstr,class dirtabelo_poi *poi);
void dirtabelo_zkt(SQLCURSOR *cur,class dirtabelo_poi *poi,int kolstr);

void vvodztab(int metka,short mp,short gp,int tabn,int kod_tab,int nom_zap);

extern float	dnei; /*Количество рабочих дней в месяце*/

void dirtabelo(short mp,short gp)
{
class dirtabelo_poi poi;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		prc;
int		K,kom,kom1;
int		i;
class iceb_tu_str fio("");
int		tabnn;
class iceb_tu_str tabn("");
class iceb_tu_str naim("");
class iceb_tu_str fiov("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kod("");
int		pod;
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str kodv("");
SQLCURSOR cur1;
K=pozz=pozz1=kom=0;
int dlinna_nazv=0;
int max_dlinna_nazv=0;


//Создаем класс меню
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

naz1:;
clear();
GDITE();
short           kls=LINES-5;
short		nomzap[kls];

sprintf(strsql,"select * from Ztab where god=%d and mes=%d \
order by tabn asc ",gp,mp);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  

if(kolstr == 0)
 {

lll:;
  saptab(mp,gp,&poi,20);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == FK3 || K == FK5)
     goto lll;
  if(K == KEY_RESIZE)
    goto naz1;
  goto vper;

 }
max_dlinna_nazv=0;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  else
   fio.new_plus("");
   
  if(iceb_tu_strlen(fio.ravno()) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(fio.ravno());

 }

naz:;
GDITE();
if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();

dlinna_nazv=COLS-58;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;

for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;


  if(provztab(row,&poi,&fio) != 0)
   continue;
  nomzap[i]=atoi(row[14]);
  
  sprintf(strsql,"%-5s|%-*.*s|%-2s|%-2.3g|%-6.6g|%-2.5g|%-2.5g|%-2.5g|%-*.*s",
  row[2],
  iceb_tu_kolbait(dlinna_nazv,fio.ravno()),iceb_tu_kolbait(dlinna_nazv,fio.ravno()),fio.ravno(),
  row[3],atof(row[4]),atof(row[5]),atof(row[6]),
  atof(row[11]),atof(row[12]),
  iceb_tu_kolbait(5,row[13]),
  iceb_tu_kolbait(5,row[13]),
  row[13]);

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;
  
 }

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
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

saptab(mp,gp,&poi,dlinna_nazv);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,2,0,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    if(provztab(row,&poi,&fio) != 0)
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

    if(provztab(row,&poi,&fio) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  if(pozz + kls >= kolstr)
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
    if(provztab(row,&poi,&fio) != 0)
     continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
  polen(spis_menu.spis_str.ravno(kom),&tabn,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&kod,3,'|');
 }

vper:;

switch (K)
 {
  case ESC : /*Выход*/
  case FK10 : /*Выход*/
     break;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_6.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;



  case FK2:   /*Добавить запись*/
  case PLU:   /*Добавить запись*/
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz1;
     

    vvodztab(0,mp,gp,0,0,0);
    goto naz1;

  case SFK2:   /*Корректировать запись*/
  case ENTER:
    if(iceb_t_pbpds(mp,gp) != 0)
     goto naz1;

    vvodztab(1,mp,gp,tabn.ravno_atoi(),kod.ravno_atoi(),nomzap[kom]);
    goto naz1;

  case FK3:   /*Удалить запись*/
    if(iceb_t_pbpds(mp,gp) != 0)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Игнорировать блокировку?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz1;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Ztab where god=%d and mes=%d and \
tabn=%s and kodt=%s and nomz=%d" ,
      gp,mp,tabn.ravno(),kod.ravno(),nomzap[kom]);
/*
      printw("\nstrsql=%s\n",strsql);
      OSTANOV();
*/
      if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

      clear();
      goto naz1;
     }
    clear(); /*Очистить экран и закрасить фоновым цветом*/
    goto naz;

  case SFK3:   //Удалить все записи
    dirtabelo_uz(&cur,kolstr,&poi);
    goto naz1;

  case FK4:  /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_MD(gettext("Фамилия.........."));
    VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.."));
    VV.VVOD_SPISOK_add_MD(gettext("Код табеля......."));
    VV.VVOD_SPISOK_add_MD(gettext("Подразделение...."));
    VV.VVOD_SPISOK_add_MD(gettext("Коментарий......."));

    VV.VVOD_SPISOK_add_data(poi.fiopoi.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.tabpoi.ravno(),32);
    VV.VVOD_SPISOK_add_data(poi.kodpoi.ravno(),32);
    VV.VVOD_SPISOK_add_data(poi.podpoi.ravno(),32);
    VV.VVOD_SPISOK_add_data(poi.koment.ravno(),128);

fk4:;
    
    poi.metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);

    poi.fiopoi.new_plus(VV.VVOD_SPISOK_return_data(0));
    poi.tabpoi.new_plus(VV.VVOD_SPISOK_return_data(1));
    poi.kodpoi.new_plus(VV.VVOD_SPISOK_return_data(2));
    poi.podpoi.new_plus(VV.VVOD_SPISOK_return_data(3));
    poi.koment.new_plus(VV.VVOD_SPISOK_return_data(4));

    switch(kom1)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
        poi.metpoi=1;
        goto naz1;

      case FK3:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов табеля"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        kom1=0;        
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

        switch (kom1)
         {
          case 3 :
          case -1:
           goto fk4;

          case 0 :

            pod=0;
            if(dirtab(&tabnn,&fiov,pod,1) == 0)
             {
              poi.tabpoi.new_plus(tabnn);

              VV.VVOD_SPISOK_zapis_data(1,poi.tabpoi.ravno());
             }
            clear();
            goto fk4;

          case 1 :
            kodv.new_plus("");
            naim.new_plus("");
            if(dirzarpodr(2,&kodv,&naim) == 0)
             VV.data_plus(3,kodv.ravno());
            goto fk4;

          case 2 :
            kodv.new_plus("");
            if(vibrek("Tabel",&kodv) == 0)
              VV.VVOD_SPISOK_zapis_data(2,kodv.ravno());
            goto fk4;
         }    
      case FK4:
        VV.VVOD_clear_data();
        goto fk4;
        
      default:
        goto fk4;
     }

    goto naz1;

  case FK5:
    rasztab(&cur,mp,gp,&poi,kolstr);
    goto naz;

  case FK6: //записать всем одинаково
    voodtvsem(mp,gp);
    goto naz1;

  case FK7: //Импорт данных из файла
    impotvr(mp,gp);
    goto naz1;
    
  case FK8: /*Замена кода табеля*/
    dirtabelo_zkt(&cur,&poi,kolstr);
    goto naz1;

  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }

}


/************/
/*Шапка меню*/
/************/

void saptab(short mp,short gp,class dirtabelo_poi *poi,int dlinna_fio)
{
int		X,Y;
move(0,0);
printw("%s. %s:%d.%d%s",
gettext("Ввод и корректировка отработанного времени"),
gettext("Дата"),mp,gp,
gettext("г."));

move(1,0);
//printw("  1              2          3   4  5    6  7  8");
printw("    1     %*s2%*s3   4    5    6  7  8  9",dlinna_fio/2-1,"",dlinna_fio/2-1,"");

move(Y=2,X=COLS-20);
printw("%s:",gettext("Расшифровка полей"));

move(++Y,X);
printw(gettext("1-таб.номер"));

move(++Y,X);
printw(gettext("2-фамилия"));

move(++Y,X);
printw(gettext("3-код табеля"));

move(++Y,X);
printw(gettext("4-отработано дней"));

move(++Y,X);
printw(gettext("5-отработано часов"));

move(++Y,X);
printw(gettext("6-календарных дней"));

move(++Y,X);
printw(gettext("7-кол.рабочих дней"));
move(++Y,X);
printw(gettext("8-кол.час.в смене"));
move(++Y,X);
printw("9-%s",gettext("комментарий"));

if(poi->metpoi == 1)
 {
  Y++;
//  attron(A_BLINK);
  mvprintw(++Y,X,gettext("Поиск:"));
  if(poi->fiopoi.getdlinna() > 1)
    mvprintw(++Y,X,"%s",poi->fiopoi.ravno());
  if(poi->podpoi.getdlinna() > 1)
    mvprintw(++Y,X,"%s",poi->podpoi.ravno());
  if(poi->kodpoi.getdlinna() > 1)
    mvprintw(++Y,X,"%s",poi->kodpoi.ravno());
  if(poi->tabpoi.getdlinna() > 1)
    mvprintw(++Y,X,"%s",poi->tabpoi.ravno());
  if(poi->koment.getdlinna() > 1)
    mvprintw(++Y,X,"%s",poi->koment.ravno());
//  attroff(A_BLINK);
 }


helstr(LINES-1,0,"F1",gettext("допомога"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("запись"),
"F7",gettext("импорт"),
"F8",gettext("заменить"),
"F10",gettext("выход"),NULL);
}
/******************/
/*Проверка записей*/
/*****************/
int		provztab(SQL_str row,
class dirtabelo_poi *poi,
class iceb_tu_str *fio1)
{
SQL_str 	row1;
char		strsql[512];
class iceb_tu_str podr1("");
SQLCURSOR cur1;

fio1->new_plus("");
/*Читаем фамилию*/
sprintf(strsql,"select fio,podr from Kartb where tabn=%s",row[2]);
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  beep();
  printw("\nНе найдена фамилия %s !\n",row[2]);
  OSTANOV();
 }
else
 {
  fio1->new_plus(row1[0]);
  podr1.new_plus(row1[1]);
 }
if(poi->metpoi == 0)
 return(0);

if(poi->fiopoi.getdlinna() > 1)
  if(strstrm(fio1->ravno(),poi->fiopoi.ravno()) == 0)
    return(1);

if(poi->kodpoi.getdlinna() > 1)
 if(SRAV(poi->kodpoi.ravno(),row[3],0) != 0)
    return(2);

if(poi->podpoi.getdlinna() > 1)
 if(SRAV(poi->podpoi.ravno(),podr1.ravno(),0) != 0)
   return(3);

if(poi->tabpoi.getdlinna() > 1)
 if(SRAV(poi->tabpoi.ravno(),row[2],0) != 0)
   return(3);

if(poi->koment.getdlinna() > 1)
  if(strstrm(row[13],poi->koment.ravno()) == 0)
   return(3);

return(0);
}

/******************************/
/*Распечатка введенних табелей*/
/******************************/

void rasztab(SQLCURSOR *cur,short mp,short gp,
class dirtabelo_poi *poi,
int kolstr)
{
FILE	*ff;
char	imaf[64];
SQL_str row;
int	kolstr1;
class iceb_tu_str fio("");
short	dn,mn,gn,dk,mk,gk;
time_t	tmm;
struct  tm      *bf;

sprintf(imaf,"tabel%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
bf=localtime(&tmm);
fprintf(ff,"%s\n\
%s.\n\
%s %d.%d%s\n\
%s %d.%d.%d  %s: %d.%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка отработанного времени"),
gettext("Дата"),
mp,gp,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),bf->tm_hour,bf->tm_min);

if(poi->metpoi == 1)
 {
  fprintf(ff,"%s\n",gettext("Поиск:"));
  if(poi->fiopoi.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Фамилия"),poi->fiopoi.ravno());
  if(poi->kodpoi.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Код табеля"),poi->kodpoi.ravno());
  if(poi->podpoi.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Код подразделения"),poi->podpoi.ravno());
  if(poi->tabpoi.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Табельный номер"),poi->tabpoi.ravno());
  if(poi->koment.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Коментарий"),poi->koment.ravno());
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |           Фамилия                      |Код|Отработано|Отработано|Календарных|Рабочих дней|Количество часов|Дата начала|Дата конца|\n");
fprintf(ff,"\
     |                                        |   |  дней    |   часов  |   дней    |в месяце    | в смене        |           |          |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");

kolstr1=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  
  if(provztab(row,poi,&fio) != 0)
   continue;
  dn=mn=gn=dk=mk=gk=0;
  rsdat(&dn,&mn,&gn,row[7],2);
  rsdat(&dk,&mk,&gk,row[8],2);

  fprintf(ff,"\
%-5s %-*.*s %3s %10s %10s %11s %12s %16s %02d.%02d.%04d  %02d.%02d.%04d\n",
  row[2],
  iceb_tu_kolbait(40,fio.ravno()),iceb_tu_kolbait(40,fio.ravno()),fio.ravno(),
  row[3],row[4],row[5],row[6],row[11],row[12],dn,mn,gn,dk,mk,gk);
 }

podpis(ff);

fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка видов табеля"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
iceb_t_rabfil(&oth,"");

}
/*****************************/
/*Ввод вида табеля сразу всем*/
/*****************************/

void	voodtvsem(short mp,short gp)
{
float		dnei=0.,hasov=0.;
short		maxkkd=0; /*Максимальное количество календарных дней*/
float		hassm=8;
class iceb_tu_str naim("");
char		strsql[512];
int		kolstr=0;
SQL_str		row;
int		K=0,N=0;
double		bb=0.;
short		dn,mn,gn;
short		dk,mk,gk;
short		d;
int		kom1;
class iceb_tu_str kod("");
class iceb_tu_str naik("");
SQLCURSOR 	cur1;
VVOD VV(0);
VVOD MENU(3);

dpm(&maxkkd,&mp,&gp,5);
redkal(mp,gp,&dnei,&hasov);

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(6);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(3);

if(dnei != 0.)
 {
  sprintf(strsql,"%.5g",dnei);
  VV.VVOD_SPISOK_zapis_data(2,strsql);
 }

if(hasov != 0.)
 {
  sprintf(strsql,"%.5g",hasov);
  VV.VVOD_SPISOK_zapis_data(3,strsql);
 } 
if(maxkkd != 0)
 {
  sprintf(strsql,"%d",maxkkd);
  VV.VVOD_SPISOK_zapis_data(4,strsql);
 }
sprintf(strsql,"select naik from Tabel where kod=1");
if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
 {
  naim.new_plus(row[0]);
  VV.VVOD_SPISOK_zapis_data(1,"1");
 }

sprintf(strsql,"%s:%d.%d%s %s:%.f %s:%.f %s:%d",
gettext("Дата"),mp,gp,gettext("г."),
gettext("Дней"),dnei,
gettext("Часов"),hasov,
gettext("Кал-рных дней"),maxkkd);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Подразделение.....(,,)......."));
VV.VVOD_SPISOK_add_MD(gettext("Код табеля..................."));
VV.VVOD_SPISOK_add_MD(gettext("Количество дней.............."));
VV.VVOD_SPISOK_add_MD(gettext("Количество часов (ч:м)......."));
VV.VVOD_SPISOK_add_MD(gettext("Количество календарных дней.."));
VV.VVOD_SPISOK_add_MD(gettext("Колич. раб. дней в месяце...."));
VV.VVOD_SPISOK_add_MD(gettext("Колич. часов в рабочем дне..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.................."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..................."));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.................."));
VV.VVOD_SPISOK_add_MD(gettext("Номер записи................"));



naz:;
clear();
if(naim.getdlinna() > 1)
 mvprintw(1,0,"%s",naim.ravno());

//Вывод списка табелей на экран

sprintf(strsql,"select kod,naik from Tabel order by kod asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }
int X=COLS-30;
int Y=0;

while(cur.read_cursor(&row) != 0)
 {
  mvprintw(Y++,X,"%2s %.27s",row[0],row[1]);
  if(Y >= LINES)
   break;
 }  


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
  refresh();
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

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Подразделение
     {
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Код табеля*/
     {
        sprintf(strsql,"select naik from Tabel where kod=%s",VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
          naim.new_plus(row[0]);
        else
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код табеля"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          goto naz;
         }
        N++;
        goto naz;
     }
    
    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Количество отработанных дней*/
     {
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N))*8;

      bb=okrug(bb,0.01);
      VV.data_plus(3,bb);

      if(ATOFM(VV.VVOD_SPISOK_return_data(N)) != dnei && ATOFM(VV.VVOD_SPISOK_return_data(4)) == maxkkd)
         VV.data_plus(4,"");
                
      N++;
      goto naz;
     }
 
    if(N == 3) /*Количество отработанных часов*/
     {

      hasov=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(polen(VV.VVOD_SPISOK_return_data(N),strsql,sizeof(strsql),2,':') == 0)
       {
        bb=(hasov*60+ATOFM(strsql))/60;
       }
      if(hasov != 0.)
       VV.data_plus(N,hasov);
      N++;
      goto naz;
     }

    if(N == 4) /*Количество отработанных календарных дней*/
     {
      d=(short)ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(d > maxkkd)
       {
        sprintf(strsql,gettext("В этом месяце календарных дней не может быть больше %d !"),maxkkd);
        iceb_t_soob(strsql);
        goto naz;
       }
     }
 
    if(N == 6 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Количество часов в смене*/
     {
      hassm=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(hassm != 0. && ATOFM(VV.VVOD_SPISOK_return_data(2)) != 0.)
       {
        hasov=hassm*ATOFM(VV.VVOD_SPISOK_return_data(2));
        VV.data_plus(3,hasov);
       }
      goto naz;
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

  attron(VV.VVOD_return_cs(iceb_CFS));
  VV.vprintw(N,stdscr);

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    return;
   }
  if(K == FK1) /*Просмотр подсказки*/
   {  
    GDITE();
    iceb_t_pdoc("zarp2_6_1.txt");
    clear();
    goto naz;
   }
  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    
    if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введены все обязательные реквизиты"));
      goto naz;
     }
    dn=mn=gn=dk=mk=gk=0;
    if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
     if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(7),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата начала !"));
       goto naz;
      }

    if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
     if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(8),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата конца !"));
       goto naz;
      }
 
    sprintf(strsql,"select tabn,podr from Kartb where datk = '0000-00-00' \
or datk >= '%d-%d-01'",gp,mp);
    SQLCURSOR cur;

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,"",strsql);


    while(cur.read_cursor(&row) != 0)
     {
      if(proverka(VV.VVOD_SPISOK_return_data(0),row[1],0,0) != 0)
         continue;
      zaptabel(0,mp,gp,atol(row[0]),atoi(VV.VVOD_SPISOK_return_data(1)),atof(VV.VVOD_SPISOK_return_data(2)),\
      atof(VV.VVOD_SPISOK_return_data(3)),atof(VV.VVOD_SPISOK_return_data(4)),dn,mn,gn,dk,mk,gk,atof(VV.VVOD_SPISOK_return_data(5)),\
      atof(VV.VVOD_SPISOK_return_data(6)),VV.data_ravno_filtr(9),atoi(VV.VVOD_SPISOK_return_data(10)),0);
     }
    return;
   }

  if(K == FK3) /*Просмотр списка видов табелей*/
   {  
    attroff(VV.VVOD_return_cs(iceb_CFS));
    MENU.VVOD_delete();
    
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов табеля"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        kod.new_plus("");
        if(vibrek("Tabel",&kod) == 0)
         {
          VV.data_plus(1,kod.ravno());
          goto naz;
         }
        goto naz;

      case 1 :
        if(dirzarpodr(2,&kod,&naik) == 0)
         {
          VV.data_plus(0,kod.ravno());
          N++;
         }
        else
         {
          VV.data_plus(0,"");
         }
        goto naz;
     }
   }
  if(K == FK1) /*Просмотр подсказки*/
   {  
   }
 }

}
/******************************/
/*Удаление всех записей которые подходят по условиям поиска*/
/********************************/

void dirtabelo_uz(SQLCURSOR *cur,int kolstr,class dirtabelo_poi *poi)
{
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return;

char strsql[512];
SQL_str row;
class iceb_tu_str fio("");
int kolstr1=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  
  if(provztab(row,poi,&fio) != 0)
   continue;

  sprintf(strsql,"delete from Ztab where god=%s and mes=%s and \
tabn=%s and kodt=%s and nomz=%s" ,
  row[0],row[1],row[2],row[3],row[14]);

  sql_zapis(strsql,0,0);
 }
}

/************************/
/*Заменить один код табеля на другой*/
/*************************************/
void dirtabelo_zkt(SQLCURSOR *cur,
class dirtabelo_poi *poi,
int kolstr)
{
class iceb_tu_str skod("");
class iceb_tu_str nkod("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Замена во всех записях кода табеля"));

VV.VVOD_SPISOK_add_MD(gettext("Код табеля который надо заменить......."));
VV.VVOD_SPISOK_add_MD(gettext("Код табеля на котороый нужно заменить.."));

VV.VVOD_SPISOK_add_data(skod.ravno(),10);
VV.VVOD_SPISOK_add_data(nkod.ravno(),10);

naz:;

clear();
 
helstr(LINES-1,0,\
"F2/+",gettext("заменить"),
//"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

skod.new_plus(VV.VVOD_SPISOK_return_data(0));
nkod.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
char strsql[512];

sprintf(strsql,"select kod from Tabel where kod=%d",skod.ravno_atoi());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код табеля"),skod.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select kod from Tabel where kod=%d",nkod.ravno_atoi());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код табеля"),nkod.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }
 

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Заменить ? Вы уверены ?"));
if(danet(&DANET,2,stdscr) == 2)
 return;
 
class iceb_tu_str fio("");
int kolstr1=0;
SQL_str row;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  

  if(provztab(row,poi,&fio) != 0)
   continue;

  if(proverka(skod.ravno(),row[3],0,0) != 0)
   continue;

  sprintf(strsql,"update Ztab set kodt=%d where tabn=%s and god=%s and mes=%s and kodt=%s and nomz=%s",
  nkod.ravno_atoi(),row[2],row[0],row[1],row[3],row[14]);

  sql_zapis(strsql,0,0);
  
 }

}
