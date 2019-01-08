/*12.12.2016	27.11.2016	Белых А.И.	fort_anz.c
Работа с нарядом
*/

#include <buhl.h>
#include "fort.h"

void fort_anz_h(short mn,short gn,const char *nn,int podr,double otr_vrem,double sumapn);
int fort_anz_vvod(int metka_r,short mn,short gn,const char *nn,int podr,int nz);
int fort_anz_zap(int metka_r,short mn,short gn,const char *nn,int nz,int podr,class VVOD *VV);
void fort_anz_ras(SQLCURSOR *cur,short mn,short gn,const char *nn,int podr);

void fort_anz(short mn,short gn,const char *nn,int podr)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row;
class SQLCURSOR cur;

short mes_nar=mn;
short god_nar=gn;
class iceb_tu_str nom_nar(nn);

int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;

double norma_vremeni=0.;
double suma=0.;
double otr_vrem=0.;
double sumapn=0.;

class VVOD DANET(1);


naz1:;

short           kls=LINES-6;
int nom_zap[kls];

sprintf(strsql,"select nz,hz,ts,nv,rc,ko,kol,vrem,ktoz from Fortzn where datan = '%04d-%02d-01' and nn='%s' and pd=%d order by nz desc",
god_nar,mes_nar,nom_nar.ravno(),podr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }



if(kolstr == 0)
 {
lll:;


  fort_anz_h(mes_nar,god_nar,nom_nar.ravno(),podr,0.,0.);
  
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }
 else
  {
   otr_vrem=sumapn=0.;
   /*вычисляем суммы по наряду*/
   while(cur.read_cursor(&row) != 0)
    {
     otr_vrem+=atof(row[3])*atof(row[6]);
     sumapn+=atof(row[4])*atof(row[6]);
    }
  }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  
  nom_zap[i]=atoi(row[0]);
  
//  if(fort_an_prov(row,&rp) != 0)
//   continue;
  norma_vremeni=atof(row[3])*atof(row[6]);
  suma=atof(row[4])*atof(row[6]);
  sprintf(strsql,"%-8s|%-5s|%6s|%10s|%-5s|%10s|%10.3f|%10.2f",
  row[1],
  row[2],
  row[3],
  row[4],
  row[5],
  row[6],
  norma_vremeni,
  suma);
    

  spis_menu.spis_str.plus(strsql);
  i++;
  
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


fort_anz_h(mes_nar,god_nar,nom_nar.ravno(),podr,otr_vrem,sumapn);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
//    if(fort_an_prov(row,&rp) != 0)
//     continue;
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
//    if(fort_an_prov(row,&rp) != 0)
//     continue;
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
//    if(fort_an_prov(row,&rp) != 0)
//     continue;
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

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case SFK1:  /*работать с шапкой документа*/
   
    fort_vhn(&mes_nar,&god_nar,&nom_nar,&podr);
    goto naz1;

  case ENTER:
  case SFK2:   /*Корректировать*/
    fort_anz_vvod(1,mes_nar,god_nar,nom_nar.ravno(),podr,nom_zap[kom]);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    fort_anz_vvod(0,mes_nar,god_nar,nom_nar.ravno(),podr,nom_zap[kom]);
    goto naz1;


  case FK3: /*удалить запись*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Fortzn where nz=%d",nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3: /*удалить запись*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить наряд ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Fortzn where datan='%04d-%02d-01' and nn='%s' and pd=%d",god_nar,mes_nar,nom_nar.ravno(),podr);
      if(sql_zapis(strsql,0,0) == 0)
       {
        
        sprintf(strsql,"delete from Forthn where datan='%04d-%02d-01' and nn='%s' and pd=%d",god_nar,mes_nar,nom_nar.ravno(),podr);
        if(sql_zapis(strsql,0,0) == 0)
          return;
       }
     }
    goto naz;
 
  case FK4:       /*Поиск*/

     clear();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    fort_anz_ras(&cur,mes_nar,god_nar,nom_nar.ravno(),podr);
    goto naz;


  default:
    goto naz;
    break;
 }


}
/********************************/
/*шапка*/
/**********************/
void fort_anz_h(short mn,short gn,const char *nn,int podr,double otr_vrem,double sumapn)
{
short		y=2;
short           x=COLS-20;
clear();

printw("%s:%02d.%d %s:%d %s:%s\n%s:%.3f %s %s:%.2f грн.\n",
gettext("Дата"),
mn,gn,
gettext("Подразделение"),
podr,
gettext("Наряд"),
nn,
gettext("Норма времени"),
otr_vrem,
gettext("мин."),
gettext("Сумма"),
sumapn/100.);


mvprintw(2,0,"     1       2      3        4       5       6          7        8");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("шифр затрат"));
mvprintw(y++,x,"2-%s",gettext("тарифная сетка"));
mvprintw(y++,x,"3-%s",gettext("норма времени"));
mvprintw(y++,x,"4-%s",gettext("расценка"));
mvprintw(y++,x,"5-%s",gettext("код операции"));
mvprintw(y++,x,"6-%s",gettext("количество"));
mvprintw(y++,x,"7-%s",gettext("норма времени"));
mvprintw(y++,x,"8-%s",gettext("сумма"));


helstr(LINES-1,0,"Enter",gettext("корректировка"),
"F2/+",gettext("новая запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}
/**************************************/
/*ввод и корректировка записи*/
/**************************************/
int fort_anz_vvod(int metka_r,/*0-ввод новой записи 1-корректировка уже введённой*/
short mn,short gn,const char *nn,int podr,int nz)
{
int N=0;
int K=0;
char strsql[1024];
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(8);//0
VV.VVOD_SPISOK_add_data(5);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(10);//4
VV.VVOD_SPISOK_add_data(10);//5



if(metka_r != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  
  sprintf(strsql,"select hz,ts,nv,rc,ko,kol,ktoz,vrem from Fortzn where nz=%d",nz);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    printw("\n%s\n",strsql);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);
  VV.VVOD_SPISOK_zapis_data(4,row[4]);
  VV.VVOD_SPISOK_zapis_data(5,row[5]);



  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(atoi(row[6]),atol(row[7]),&VV);
     
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 }



VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Тарифная сетка.........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Норма времени (минуты).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Расценка (копейки)......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код операции............"));//4
VV.VVOD_SPISOK_add_MD(gettext("Количество (штуки)......"));//5


naz:;


helstr(LINES-1,0,
//"F1","допомога"),
"F2/+",gettext("запись"),
//"F3",gettext("реквизиты"),
"F10",gettext("виход"),NULL);

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
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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
    N=-1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(fort_anz_zap(metka_r,mn,gn,nn,nz,podr,&VV) != 0)
     goto naz;
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(0);


return(0);
}
/**************************/
/*Запись*/
/*****************************/
int fort_anz_zap(int metka_r,/*0-ввод новой записи 1-корректировка уже введённой*/
short mn,short gn,const char *nn,int nz,int podr,class VVOD *VV)
{
char strsql[1024];

if(metka_r == 1) /*корректировка записи*/
 {
  sprintf(strsql,"update Fortzn set \
hz='%s',\
ts=%d,\
nv=%.3f,\
rc=%.3f,\
ko=%d,\
kol=%.2f,\
ktoz=%d,\
vrem=%ld \
where nz=%d",
  VV->data_ravno_filtr(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(3),
  VV->data_ravno_atoi(4),
  VV->data_ravno_atof(5),
  iceb_t_getuid(),
  time(NULL),
  nz);
 }
else /*ввод новой записи*/
 {
  sprintf(strsql,"insert into Fortzn (datan,nn,pd,hz,ts,nv,rc,ko,kol,ktoz,vrem) values('%04d-%02d-01',%s,%d,'%s',%d,%.3f,%.3f,%d,%.2f,%d,%ld)",
  gn,mn,
  nn,
  podr,
  VV->data_ravno_filtr(0),
  VV->data_ravno_atoi(1),
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(3),
  VV->data_ravno_atoi(4),
  VV->data_ravno_atof(5),
  iceb_t_getuid(),
  time(NULL));

 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);
}

/***********************************/
/*распечатка*/
/**********************************/
void fort_anz_ras(SQLCURSOR *cur,short mn,short gn,const char *nn,int podr)
{
char	imaf[64];
SQL_str row;
class SQLCURSOR cur1;
char strsql[1024];

class SPISOK hz;      /*шифры затрат*/
class masiv_din_double vrem; /*время по затратам*/
class masiv_din_double isum; /*сумма по затратам*/

class SPISOK ts_ko; /*список тарифных сеток с кодом операции*/
class masiv_din_double ts_ko_v; 
class masiv_din_double ts_ko_s;

class iceb_tu_str zak("");
class iceb_tu_str isp("");
class iceb_tu_str kom("");

/*читаем шапку документа*/
sprintf(strsql,"select zak,isp,kom from Forthn where datan='%04d-%02d-01' and nn='%s' and pd=%d",gn,mn,nn,podr);
if(readkey(strsql,&row,&cur1) == 1)
 {
  zak.new_plus(row[0]);
  isp.new_plus(row[1]);
  kom.new_plus(row[2]);
 }






sprintf(imaf,"nar%d.lst",getpid());
class iceb_fopen nar;
if(nar.start(imaf,"w") != 0)
 return;


sprintf(strsql,"%s:%02d.%d %s %s:%d %s %s",gettext("Дата"),mn,gn,
gettext("г."),
gettext("Подразделение"),
podr,
gettext("Наряд"),
nn);
iceb_t_zagolov(strsql,nar.ff);

if(zak.getdlinna() > 1)
 fprintf(nar.ff,"%s:%s\n",gettext("Заказ"),zak.ravno());
if(isp.getdlinna() > 1)
 fprintf(nar.ff,"%s:%s\n",gettext("Исполнитель"),isp.ravno());
if(kom.getdlinna() > 1)
 fprintf(nar.ff,"%s:%s\n",gettext("Коментарий"),kom.ravno()); 

fprintf(nar.ff,"\
---------------------------------------------------------------------\n");
fprintf(nar.ff,"\
 Шифр |Тариф.| Норма |Розцінка|Код  |Кількість|  Всього  |   Сума   |\n\
витрат|сітка | часу  |копійки |опер.|  штуки  |норма часу|  гривні  |\n\
      |      |хвилини|        |     |         |  хвилини |          |\n");
/*************
123456 123456 1234567 12345678 12345 123456789 1234567890 1234567890
  
***************/
fprintf(nar.ff,"\
---------------------------------------------------------------------\n");

cur->poz_cursor(0);
double norma_vremeni=0.;
double suma=0.;
double itogo[2];
memset(itogo,'\0',sizeof(itogo));
int nom_hz=0;
while(cur->read_cursor(&row) != 0)
 {
  itogo[0]+=norma_vremeni=atof(row[3])*atof(row[6]);
  itogo[1]+=suma=atof(row[4])*atof(row[6])/100.;
  
  fprintf(nar.ff,"%-6s %6s %7s %8s %5s %9s %10.2f %10.2f\n",
  row[1],
  row[2],
  row[3],
  row[4],
  row[5],
  row[6],
  norma_vremeni,
  suma);

  if((nom_hz=hz.find(row[1])) < 0)
    hz.plus(row[1]);
  vrem.plus(norma_vremeni,nom_hz);
  isum.plus(suma,nom_hz);
  
  
  sprintf(strsql,"%s|%s",row[2],row[5]);
  if((nom_hz=ts_ko.find(strsql)) < 0)
   ts_ko.plus(strsql);
  ts_ko_v.plus(norma_vremeni);
  ts_ko_s.plus(suma);
 }
fprintf(nar.ff,"\
---------------------------------------------------------------------\n");
fprintf(nar.ff,"%*s %10.2f %10.2f\n",
iceb_tu_kolbait(46,gettext("Итого")),
gettext("Итого"),
itogo[0],
itogo[1]);

fprintf(nar.ff,"\n\nПідрозділ %d Наряд %s Дата %02d.%d\n",podr,nn,mn,gn);

fprintf(nar.ff,"\
-----------------------------\n");
fprintf(nar.ff,"\
 Шифр |Норма часу|   Сума   |\n\
витрат|  години  |  гривні  |\n");
/******
123456 1234567890 1234567890
*********/
fprintf(nar.ff,"\
-----------------------------\n");
for(int nom=0; nom < hz.kolih(); nom++)
 {
  fprintf(nar.ff,"%6s %10.3f %10.2f\n",
  hz.ravno(nom),
  vrem.ravno(nom)/60.,
  isum.ravno(nom)/100.);
 }
fprintf(nar.ff,"\
-----------------------------\n");
fprintf(nar.ff,"%6s %10.3f %10.2f\n","",itogo[0]/60.,itogo[1]);

fprintf(nar.ff,"\n\nПідрозділ %d Наряд %s Дата %02d.%d\n",podr,nn,mn,gn);
fprintf(nar.ff,"\
--------------------------------------\n");
fprintf(nar.ff,"\
Тариф.|  Код   |Норма часу|   Сума   |\n\
сітка |операції|  години  |  гривні  |\n");
 
fprintf(nar.ff,"\
--------------------------------------\n");
class iceb_tu_str ts("");
class iceb_tu_str ko("");

for(int nom=0; nom < ts_ko.kolih(); nom++)
 {
  polen(ts_ko.ravno(nom),&ts,1,'|');
  polen(ts_ko.ravno(nom),&ko,2,'|');
    
  fprintf(nar.ff,"%6s %8s %10.3f %10.2f\n",
  ts.ravno(),
  ko.ravno(),
  ts_ko_v.ravno(nom)/60.,
  ts_ko_s.ravno(nom)/100.);      
 }
fprintf(nar.ff,"\
--------------------------------------\n");
fprintf(nar.ff,"%15s %10.3f %10.2f\n\n","",itogo[0]/60.,itogo[1]);
 
podpis(nar.ff);
nar.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка наряда"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
