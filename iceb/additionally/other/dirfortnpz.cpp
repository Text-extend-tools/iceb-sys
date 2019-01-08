/*$Id:$*/
/*02.03.2017	02.03.2017	Белых А.И.	dirfortnpz.c
Ввод и корректирвка процентов выполнения по детали
*/
#include        <errno.h>
#include        <buhg.h>
#include "fort.h"

class dirfortnpz_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   short mn,gn;
   short mk,gk;

   class iceb_tu_str kod_ceh;
   class iceb_tu_str kod_det;
   class iceb_tu_str naim_det;
   class iceb_tu_str ei;
   class iceb_tu_str naim_ei;
   class iceb_tu_str naim_ceh;
   class iceb_tu_str koment;
   double cena_kop;
   double vrem_min;
   
   short metka_poi;

  dirfortnpz_poi()
   {
    cena_kop=0.;
    vrem_min=0.;
    
    datan.plus("");
    datak.plus("");
    mn=mk=gn=gk=0;
    metka_poi=0;
    kod_ceh.plus("");
    kod_det.plus("");
    naim_det.plus("");
    ei.plus("");
    naim_ei.plus("");
    naim_ceh.plus("");
    koment.plus("");
   }
 };
 
void dirfortnpz_vvod(unsigned int nom_zap,unsigned int nom_zap_h);
void dirfortnpz_sap(class dirfortnpz_poi*);
int dirfortnpz_zap(VVOD *VV,unsigned int nom_zap,unsigned int nom_zap_h);
void	dirfortnpz_ras(SQLCURSOR *cur,class dirfortnpz_poi *poisk);
int dirfortnpz_prov(SQL_str row,class dirfortnpz_poi *poisk);
int dirfortti_poi(class dirfortnpz_poi *poisk);



int dirfortnpz(unsigned int nom_zap_h)
{
class iceb_tu_str kod_ceh("");
class iceb_tu_str kod_det("");
char bros[1024];
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=9;
int             X=0;
class iceb_tu_str koment("");
class dirfortnpz_poi poisk;
class iceb_tu_str naim_det("");
class VVOD DANET(1);
class VVOD VVOD1(2);
class SQLCURSOR cur;
double suma=0.;

sprintf(strsql,"select * from Fortnp where nz=%d",nom_zap_h);
if(readkey(strsql,&row,&cur) == 1)
 {
  poisk.kod_ceh.new_plus(row[1]);
  poisk.kod_det.new_plus(row[2]);
  poisk.ei.new_plus(row[3]);
  poisk.koment.plus(row[4]);  
 }

sprintf(strsql,"select naik from Fortsc where kod='%s'",poisk.kod_ceh.ravno_filtr());
if(readkey(strsql,&row,&cur) == 1)
 poisk.naim_ceh.new_plus(row[0]);

sprintf(strsql,"select naik from Edizmer where kod='%s'",poisk.ei.ravno_filtr());
if(readkey(strsql,&row,&cur) == 1)
 poisk.naim_ei.new_plus(row[0]);

sprintf(strsql,"select naimat from Material where kodm=%d",poisk.kod_det.ravno_atoi());
if(readkey(strsql,&row,&cur) == 1)
 poisk.naim_det.new_plus(row[0]);

sprintf(strsql,"select min,kop from Fortti where ceh='%s' and kd=%d",poisk.kod_ceh.ravno(),poisk.kod_det.ravno_atoi());
if(readkey(strsql,&row,&cur) == 1)
 {
  poisk.cena_kop=atof(row[1]);
  poisk.vrem_min=atof(row[0]);
 }

naz1:;

short           kls=LINES-6;
unsigned int nom_zap[kls];

if(poisk.metka_poi == 0)
 sprintf(strsql,"select nz,data,kol,pr,kom from Fortnpz where nzh=%d order by data desc",nom_zap_h);
else
 {
  sprintf(strsql,"select nz,data,kol,pr,kom from Fortnpz where nzh=%d ",nom_zap_h);
  if(poisk.mn != 0)
   {
    sprintf(bros," and data >= '%04d-%02d-01'",poisk.gn,poisk.mn);
    strcat(strsql,bros);
   }
  if(poisk.mk != 0)
   {
    sprintf(bros," and data <= '%04d-%02d-01'",poisk.gk,poisk.mk);
    strcat(strsql,bros);
   }
  strcat(strsql," order by data desc");

 }

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirfortnpz_sap(&poisk);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
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

  if(dirfortnpz_prov(row,&poisk) != 0)
   continue;


  nom_zap[i]=atoi(row[0]);  

  suma=0.;

  if(poisk.cena_kop != 0.)
    suma=poisk.cena_kop*atof(row[2])*atof(row[3])/100.;

  sprintf(strsql,"%s|%10.10g|%8.8g|%10.2f|%s",
  iceb_tu_datzap_mg(row[1]),
  atof(row[2]),
  atof(row[3]),
  suma,
  row[4]);
    
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

dirfortnpz_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


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
    if(dirfortnpz_prov(row,&poisk) != 0)
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
    if(dirfortnpz_prov(row,&poisk) != 0)
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
/*  
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
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
    if(dirfortnpz_prov(row,&poisk) != 0)
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

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
    goto naz1;
          
  case SFK2:   /*Корректировать*/
    dirfortnpz_vvod(nom_zap[kom],nom_zap_h);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirfortnpz_vvod(0,nom_zap_h);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Fortnpz where nz=%d",nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

    

  case FK4:       /*Поиск*/
     
     dirfortti_poi(&poisk);

     clear();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    dirfortnpz_ras(&cur,&poisk);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirfortnpz_vvod(unsigned int nom_zap,unsigned int nom_zap_h)
{
char		strsql[2048];
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
SQL_str row;
class SQLCURSOR cur;
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(8);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(60);//3



if(nom_zap != 0)
 {

  sprintf(strsql,"select * from Fortnpz where nz=%d",nom_zap);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,iceb_tu_datzap_mg(row[2]));
  VV.VVOD_SPISOK_zapis_data(1,row[3]);
  VV.VVOD_SPISOK_zapis_data(2,row[4]);
  VV.VVOD_SPISOK_zapis_data(3,row[5]);
  ktozap=atoi(row[6]);
  vremz=atol(row[7]);

     
 }



VV.VVOD_SPISOK_add_MD(gettext("Дата (м.г).........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Процент выполнения.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.........."));//3


naz:;

VV.VVOD_delete_ZAG();

if(nom_zap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktozap,vremz,&VV);
   
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
//"F3",gettext("реквизиты"),
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
    VV.vprintw_clear(N,stdscr);

    if(N == 2)
     {
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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
    if(dirfortnpz_zap(&VV,nom_zap,nom_zap_h) != 0)
     goto naz;
    break;
   }


 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int dirfortnpz_zap(VVOD *VV,unsigned int nom_zap,unsigned int nom_zap_h)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       
short m=0,g=0;
if(rsdat1(&m,&g,VV->data_ravno(0)) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата!"));
  return(1);
 }

if(nom_zap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Fortnpz (nzh,data,kol,pr,kom,ktoz,vrem) values(%d,'%04d-%02d-01',%.6f,%.6f,'%s',%d,%ld)",
  nom_zap_h,
  g,m,
  VV->data_ravno_atof(1),  
  VV->data_ravno_atof(2),  
  VV->data_ravno_filtr(3),  
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Fortnpz set \
data='%04d-%02d-01',\
kol=%.6f,\
pr=%.6f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",  
  g,m,
  VV->data_ravno_atof(1),  
  VV->data_ravno_atof(2),  
  VV->data_ravno_filtr(3),  
  iceb_t_getuid(),
  time(NULL),
  nom_zap);

if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirfortnpz_sap(class dirfortnpz_poi *poisk)
{
short		y=2;
short           x=COLS-20;



clear();
printw("%s\n",gettext("Ввод и корректировка процента выполнения"));

printw("%s:%s %s\n",gettext("Склад"),poisk->kod_ceh.ravno(),poisk->naim_ceh.ravno());
printw("%s:%s %s\n",gettext("Деталь"),poisk->kod_det.ravno(),poisk->naim_det.ravno());
printw("%s:%s %s\n",gettext("Единица измерения"),poisk->ei.ravno(),poisk->naim_ei.ravno());
printw("%s:%s\n",gettext("Коментарий"),poisk->koment.ravno());
printw("%s:%.6g\n",gettext("Трудоёмкость в копейках"),poisk->cena_kop);
printw("%s:%.6g\n",gettext("Трудоёмкость в минутах"),poisk->vrem_min);
y+=6;
mvprintw(y++,0,"    1          2         3          4     5");
y=2;
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("количество"));
mvprintw(y++,x,"3-%s",gettext("процент"));
mvprintw(y++,x,"4-%s",gettext("сумма"));
mvprintw(y++,x,"5-%s",gettext("коментарий"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);

  mvprintw(y++,x,"%s:",gettext("Поиск"));
 
  if(poisk->datan.getdlinna() > 1)  
   mvprintw(y++,x,"%s:%s",gettext("Дата н."),poisk->datan.ravno());
  if(poisk->datak.getdlinna() > 1)  
   mvprintw(y++,x,"%s:%s",gettext("Дата к."),poisk->datak.ravno());
 
  attroff(A_BLINK);
  
 }
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
}

/********************/
/*Распечатать       */
/********************/

void	dirfortnpz_ras(SQLCURSOR *cur,class dirfortnpz_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row;


sprintf(imaf,"pv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Процент выполнения"),ff);

fprintf(ff,"%s:%s %s\n",gettext("Склад"),poisk->kod_ceh.ravno(),poisk->naim_ceh.ravno());
fprintf(ff,"%s:%s %s\n",gettext("Деталь"),poisk->kod_det.ravno(),poisk->naim_det.ravno());
fprintf(ff,"%s:%s %s\n",gettext("Единица измерения"),poisk->ei.ravno(),poisk->naim_ei.ravno());
fprintf(ff,"%s:%s\n",gettext("Коментарий"),poisk->koment.ravno());
fprintf(ff,"%s:%.6g\n",gettext("Трудоёмкость в копейках"),poisk->cena_kop);
fprintf(ff,"%s:%.6g\n",gettext("Трудоёмкость в минутах"),poisk->vrem_min);

if(poisk->datan.getdlinna() > 1)  
 fprintf(ff,"%s:%s\n",gettext("Дата нaчала"),poisk->datan.ravno());
if(poisk->datak.getdlinna() > 1)  
 fprintf(ff,"%s:%s\n",gettext("Дата конца"),poisk->datak.ravno());

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата |Количество|Процент|Сумма коп.|Время мин.|Коментарий\n"));
/*
1234567 1234567890 1234567 1234567890 1234567890 
*/
fprintf(ff,"\
-------------------------------------------------------------\n");

cur->poz_cursor(0);
double suma_kop=0.;
double suma_min=0.;
double i_suma_kop=0.;
double i_suma_min=0.;

while(cur->read_cursor(&row) != 0)
 {

  if(dirfortnpz_prov(row,poisk) != 0)
   continue;

  if(poisk->cena_kop != 0.)
    suma_kop=poisk->cena_kop*atof(row[2])*atof(row[3])/100.;

  if(poisk->vrem_min != 0.)
    suma_min=poisk->vrem_min*atof(row[2])*atof(row[3])/100.;
  i_suma_kop+=suma_kop;
  i_suma_min+=suma_min;
  
  fprintf(ff,"%s|%10.10g|%7.7g|%10.2f|%10.2f|%s\n",
  iceb_tu_datzap_mg(row[1]),
  atof(row[2]),
  atof(row[3]),
  suma_kop,
  suma_min,
  row[4]);

 }
fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(26,gettext("Итого")),
gettext("Итого"),
i_suma_kop,
i_suma_min);


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Процент выполнения"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}

/*********************************/
/*ввод реквизитов поиска*/
/******************************/
int dirfortti_poi(class dirfortnpz_poi *poisk)
{
class VVOD MENU(3);
class VVOD VV(0);
int kl=0;
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала (м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца  (м.г).."));//1

VV.VVOD_SPISOK_add_data(poisk->datan.ravno(),8);
VV.VVOD_SPISOK_add_data(poisk->datak.ravno(),8);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kl=VV.vvod(0,1,1,-1,-1);

poisk->datan.new_plus(VV.data_ravno(0));
poisk->datak.new_plus(VV.data_ravno(1));

switch(kl)
 {
  case FK10:
  case ESC:
    poisk->metka_poi=0; 
    break;

  case FK2:
    if(rsdat1(&poisk->mn,&poisk->gn,poisk->datan.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата начала!"));
      goto naz;
     }

    if(rsdat1(&poisk->mk,&poisk->gk,poisk->datak.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата конца!"));
      goto naz;
     }
    poisk->metka_poi=1; 
    break;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
               
  default:
    goto naz;
 }

return(poisk->metka_poi);
}
/******************************/
/*проверка реквизитов поиска*/
/******************************/
int dirfortnpz_prov(SQL_str row, //Строка для проверки 
class dirfortnpz_poi *poisk)
{
if(poisk->metka_poi == 0)
 return(0);

return(0);
}
