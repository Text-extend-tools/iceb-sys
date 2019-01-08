/*$Id: dirupldok3.c,v 5.3 2013/10/04 07:25:01 sasa Exp $*/
/*14.07.2015	11.04.2012	Белых А.И.	dirupldok3.c
Ввод и корректировка списка норм списания топлива для путевого листа
*/
#include        <errno.h>
#include        "buhg.h"

class dirupldok3_poi
 {
  public:
  int kod_avt;
  int kod_vod;
  float probeg;
  float probeg_spisano;
  float spis_pn;
  float spis_pf;
  class iceb_tu_str nom_dok; //номер документа в начале и изменённый
  class iceb_tu_str podr; //код подразделения в символьном виде в начале и изменённый
  short dd,md,gd;
  
  dirupldok3_poi()
   {
    kod_avt=0;
    kod_vod=0;
    probeg=0.;
    nom_dok.plus("");
    dd=md=gd=0;    
    probeg_spisano=0.;
    spis_pn=spis_pf=0.;
    podr.plus("");
   }
 };
 
void dirupldok3_vvod(short dd,short md,short gd,int kp,const char *nom_dok,const char *kod_nst,int kod_avt);
void dirupldok3_sap(class dirupldok3_poi*);
int   dirupldok3_zap(short dd,short md,short gd,int kp,const char *nom_dok,VVOD *VV,const char *kod_nst);
void	dirupldok3_ras(short dd,short md,short gd,const char *nom_dok,int kp,SQLCURSOR *cur);
int dirupldok3_uddok(short dd,short md,short gd,const char *nomdok, const char *podr);



void dirupldok3(short dd,short md,short gd,
const char *nom_dok,
int kp)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		mspz=0;
int		prc;
int             Y=6;
int             X=0;
class iceb_tu_str koment("");
class dirupldok3_poi poisk;
class iceb_tu_str kod_nst("");
class iceb_tu_str naim_nst("");
class SQLCURSOR cur,cur1;
int i=0,kom=0;

/*узнаём код автомобиля*/
sprintf(strsql,"select ka,kv,psv,psz from Upldok where god=%d and kp=%d and nomd='%s'",gd,kp,nom_dok);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s-%s:%d %s:%d %s:%s",
  __FUNCTION__,
  gettext("Не найдена шапка документа"),
  gettext("Год"),
  gd,
  gettext("Подразделение"),
  kp,
  gettext("Номер документа"),
  nom_dok);
  
  iceb_t_soob(strsql);
  return;
 }
poisk.kod_avt=atoi(row[0]);
poisk.kod_vod=atoi(row[1]);
poisk.probeg=atof(row[3])-atof(row[2]);

poisk.nom_dok.new_plus(nom_dok);
poisk.podr.new_plus(kp);
poisk.dd=dd;
poisk.md=md;
poisk.gd=gd;

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-Y+3;

sprintf(strsql,"select * from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s'",gd,md,dd,kp,nom_dok);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirupldok3_sap(&poisk);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

poisk.probeg_spisano=0.;
poisk.spis_pn=poisk.spis_pf=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_tu_srav_r(gettext("Км"),row[8],0) == 0)
    poisk.probeg_spisano+=atof(row[4]);
  poisk.spis_pn+=atof(row[6]);
  poisk.spis_pf+=atof(row[7]);
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


  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nst.new_plus(row1[0]);
        
  sprintf(strsql,"%-*s|%-*.*s|%-*s|%8.8g|%8.8g|%8.8g|%8.8g|%s",
  iceb_tu_kolbait(6,row[3]),
  row[3],
  iceb_tu_kolbait(20,naim_nst.ravno()),  
  iceb_tu_kolbait(20,naim_nst.ravno()),  
  naim_nst.ravno(),
  iceb_tu_kolbait(6,row[8]),
  row[8],
  atof(row[4]),
  atof(row[5]),
  atof(row[6]),
  atof(row[7]),
  row[9]);

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

dirupldok3_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-24);


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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),&kod_nst,1,'|');

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
     iceb_t_pdoc("upl2_3_3.txt");
    goto naz;

  case SFK1: //Просмотр и корректировка шапки документа
     vdupl1(&poisk.dd,&poisk.md,&poisk.gd,&poisk.nom_dok,&poisk.podr);   
     goto naz1;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirupldok3_vvod(poisk.dd,poisk.md,poisk.gd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),kod_nst.ravno(),poisk.kod_avt);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:

    dirupldok3_vvod(poisk.dd,poisk.md,poisk.gd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),"",poisk.kod_avt);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"delete from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and knst='%s'",gd,md,dd,poisk.podr.ravno_atoi(),poisk.nom_dok.ravno(),kod_nst.ravno());
    sql_zapis(strsql,0,0);

    goto naz1;

  case SFK3:  //Удаление документа
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz1;
     
    if(dirupldok3_uddok(dd,md,gd,poisk.nom_dok.ravno(),poisk.podr.ravno()) == 0)
      return;

    goto naz1;

  case FK5:    //Распечатать
    dirupldok3_ras(poisk.dd,poisk.md,poisk.gd,poisk.nom_dok.ravno(),poisk.podr.ravno_atoi(),&cur);
    goto naz;

  case FK6:    //Переход в режим списания топлива
    dirupld1(poisk.dd,poisk.md,poisk.gd,&poisk.nom_dok,&poisk.podr);
    goto naz;


  default:
    goto naz;
    break;
 }



}
/**************************/
/*Шапка                   */
/**************************/

void	dirupldok3_sap(class dirupldok3_poi *poisk)
{
short		y=5;
short           x=COLS-20;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;


clear();
printw(gettext("Расчёт списания топлива по нормам"));

mvprintw(1,0,"%s:%d.%d.%d %s:%s %s:%d %s:%.10g/%.10g",
gettext("Дата"),
poisk->dd,poisk->md,poisk->gd,
gettext("Номер"),
poisk->nom_dok.ravno(),
gettext("Подразделение"),
poisk->podr.ravno_atoi(),
gettext("Пробег"),
poisk->probeg,
poisk->probeg_spisano);

class iceb_tu_str nom_znak("");
class iceb_tu_str naim("");
sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",poisk->kod_avt);

if(readkey(strsql,&row,&cur) == 1)
 {
  nom_znak.new_plus(row[0]);
  naim.new_plus(row[1]);
 }
mvprintw(2,0,"%s:%d %s %s\n",gettext("Автомобиль"),poisk->kod_avt,nom_znak.ravno(),naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naik from Uplouot where kod=%d and mt=1",poisk->kod_vod);

if(readkey(strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);
 }

mvprintw(3,0,"%s:%d %s\n",gettext("Водитель"),poisk->kod_vod,naim.ravno());
mvprintw(4,0,"%s:%.3f %s:%.3f",gettext("По норме"),poisk->spis_pn,gettext("По факту"),poisk->spis_pf);                       




mvprintw(y,0,"   1           2                3        4         5        6      7      8");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("един.измерения"));
mvprintw(y++,x,"4-%s",gettext("количество"));
mvprintw(y++,x,"5-%s",gettext("норма"));
mvprintw(y++,x,"6-%s",gettext("затраты по норме"));
mvprintw(y++,x,"7-%s",gettext("затраты по факту"));
mvprintw(y++,x,"8-%s",gettext("коментарий"));


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("топливо"),
"F10",gettext("выход"),NULL);


}

/**********************/
/*Ввод и корректировки*/
/**********************/

void dirupldok3_vvod(short dd,short md,short gd,
int kp,
const char *nom_dok,
const char *kod_nst,
int kod_avt)
{
char		strsql[2048];
SQL_str		row;
int		N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naimkod("");

SQLCURSOR cur;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(16);//0
VV.VVOD_SPISOK_add_data(16);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(20);//5
VV.VVOD_SPISOK_add_data(255);//6



if(kod_nst[0] != '\0')
 {

  sprintf(strsql,"select * from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and knst='%s'",gd,md,dd,kp,nom_dok,kod_nst);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[3]);
  VV.VVOD_SPISOK_zapis_data(1,row[8]);
  VV.VVOD_SPISOK_zapis_data(2,row[5]);
  VV.VVOD_SPISOK_zapis_data(3,row[4]);
  VV.VVOD_SPISOK_zapis_data(4,row[6]);
  VV.VVOD_SPISOK_zapis_data(5,row[7]);
  VV.VVOD_SPISOK_zapis_data(6,row[9]);

  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка вида нормы списания топлива"));
  kzvz(row[10],row[11],&VV);     
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод нового вида нормы списания топлива"));



VV.VVOD_SPISOK_add_MD(gettext("Код нормы списания.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Норма списания......"));//2
VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));//3
VV.VVOD_SPISOK_add_MD(gettext("Затраты по норме...."));//4
VV.VVOD_SPISOK_add_MD(gettext("Затраты по факту...."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.........."));//6


naz:;



helstr(LINES-1,0,
//"F1",gettext("помощь"),
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

    if(N == 0)
     {
      if(diruplanst(2,kod_avt,&kod) == 0)
       {
        VV.data_plus(0,kod.ravno());
        sprintf(strsql,"select nst,ei from Uplavt1 where kod=%d and knst='%s'",kod_avt,kod.ravno());
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(2,row[0]);
          VV.data_plus(1,row[1]);
         }
        N++;
       }
      goto naz;
     }

    if(N == 1) //единица измерения
     {
      kod.new_plus("");
      naimkod.new_plus("");
      if(iceb_tl_ei(2,&kod,&naimkod) == 0)
       {
        VV.data_plus(1,kod.ravno());
        N++;        
       }
      goto naz;
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 3 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //количество
     {
      sprintf(strsql,"%.10g",VV.data_ravno_atof(2)*VV.data_ravno_atof(N)/100.);
      VV.data_plus(4,strsql);
      if(VV.data_ravno(5)[0] == '\0')
       VV.data_plus(5,strsql);
      goto naz;
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
    if(dirupldok3_zap(dd,md,gd,kp,nom_dok,&VV,kod_nst) != 0)
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

int   dirupldok3_zap(short dd,short md,short gd,
int kp,
const char *nom_dok,
VVOD *VV,
const char *kod_nst)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(iceb_t_pbpds(md,gd) != 0)
 return(1);


if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(3)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(4)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(5)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

/*проверяем код в списке*/
sprintf(strsql,"select kod from Uplnst where kod='%s'",VV->data_ravno(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не существует кода нормы списания топлива"),VV->data_ravno(0));
  iceb_t_soob(strsql);
  return(1);
 }
/*провераяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",VV->data_ravno(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не существует единицы измерения"),VV->data_ravno(1));
  iceb_t_soob(strsql);
  return(1);
 }

if(SRAV(kod_nst,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может код уже введён
  sprintf(strsql,"select knst from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and knst='%s'",gd,md,dd,kp,nom_dok,VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код нормы списани топлива уже ввели !"));
    return(1);
   }

  if(kod_nst[0] != '\0')
   {
    
    iceb_t_soob(gettext("Корректировка кода нормы списания топлива невозможна!"));
    return(1);
   }

 }
 

if(kod_nst[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Upldok3 values('%04d-%02d-%02d',%d,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','%s',%d,%ld)",
  gd,md,dd,
  kp,
  nom_dok,
  VV->data_ravno_filtr(0),
  VV->data_ravno_atof(3),
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(4),
  VV->data_ravno_atof(5),
  VV->data_ravno_filtr(1),
  VV->data_ravno_filtr(6),
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Upldok3 set \
knst='%s',\
kl=%.3f,\
nst=%.3f,\
zn=%.3f,\
zf=%.3f,\
ei='%s',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and knst='%s'",
  VV->data_ravno_filtr(0),
  VV->data_ravno_atof(3),
  VV->data_ravno_atof(2),  
  VV->data_ravno_atof(4),
  VV->data_ravno_atof(5),
  VV->data_ravno_filtr(1),
  VV->data_ravno_filtr(6),
  iceb_t_getuid(),
  time(NULL),
  gd,md,dd,kp,nom_dok,kod_nst);
  
if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}



/********************/
/*Распечатать       */
/********************/

void	dirupldok3_ras(short dd,short md,short gd,
const char *nom_dok,
int kp,SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
double itogo_pn=0.;
double itogo_pf=0.;
sprintf(imaf,"nsta%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Расчёт списания топлива к путевому листу"),ff);
fprintf(ff,"%s:%02d.%02d.%04d\n",gettext("Дата путевого листа"),dd,md,gd);
fprintf(ff,"%s:%s\n",gettext("Номер документа"),nom_dok);
fprintf(ff,"%s:%d\n",gettext("Подразделение"),kp);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  | Наименование  вида списания топлива        |Един.измер.|Норма списания|Количество| По норме | По факту |Коментарий\n");
/*
123456 12345678901234567890123456789012345678901234 12345678901 12345678901234 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);
class iceb_tu_str naim_nst("");
char strsql[512];
while(cur->read_cursor(&row) != 0)
 {
  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_nst.new_plus(row1[0]);
   

  fprintf(ff,"%-*s|%-*.*s|%-*s|%14.14g|%10.10g|%10.10g|%10.10g|%s\n",
  iceb_tu_kolbait(6,row[3]),
  row[3],
  iceb_tu_kolbait(44,naim_nst.ravno()),  
  iceb_tu_kolbait(44,naim_nst.ravno()),  
  naim_nst.ravno(),
  iceb_tu_kolbait(11,row[8]),
  row[8],
  atof(row[5]),
  atof(row[4]),
  atof(row[6]),
  atof(row[7]),
  row[9]);
  itogo_pn+=atof(row[6]);
  itogo_pf+=atof(row[7]);
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.10g %10.10g\n",
iceb_tu_kolbait(89,gettext("Итого")),
gettext("Итого"),
itogo_pn,itogo_pf);

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Расчёт списания топлива к путевому листу"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/***********************/
/*Удаление документа   */
/***********************/
// 0-удалили 1-нет

int dirupldok3_uddok(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
const char *podr) //Подразделение
{
char strsql[512];

sprintf(strsql,"delete from Upldok2a where datd='%d-%d-%d' and kp=%s and nomd='%s'",
gd,md,dd,podr,nomd);

if(sql_zapis(strsql,0,0) != 0)
  return(1);

sprintf(strsql,"delete from Upldok1 where datd='%d-%d-%d' and kp=%s and nomd='%s'",
gd,md,dd,podr,nomd);

sql_zapis(strsql,0,0);

sprintf(strsql,"delete from Upldok3 where datd='%d-%d-%d' and kp=%s and nomd='%s'",
gd,md,dd,podr,nomd);

sql_zapis(strsql,0,0);

sprintf(strsql,"delete from Upldok where god=%d and kp=%s and nomd='%s'",
gd,podr,nomd);

sql_zapis(strsql,0,0);

return(0);

}
