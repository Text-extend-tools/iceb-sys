/* $Id: dirtabel.c,v 5.42 2013/09/26 09:43:31 sasa Exp $ */
/*31.05.2016    15.05.1995      Белых А.И.      dirtabel.c
Просмотр ввод и корректировка отработанного времени для конкретного 
табельного номера
*/
#include        "buhg.h"

int             zapktabel(short,short,int,const char*,int nom_zap,const char*,int,float,float);
void		shzr6(short,short,long,const char*,float,float,float,double,float,float,int dlinna_nai);


void dirtabel(short mp,short gp, //Дата
long tabn, //Табельный номер
const char *fam) //Фамилия
{
char            bros[1024];
class iceb_tu_str kod(""); /*Код */
short           x=0,y=4;
int             K;
short           kom;
short           i;
class iceb_tu_str n("");
class iceb_tu_str n1("");
float		kd,kh,kkd;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		prc;
class iceb_tu_str nait("");
float		dneii=0.,has=0.,kdnei=0.;
short		dn,mn,gn;
short		dk,mk,gk;
int		poz,komv;
double		okl=0.; /*Найденный оклад*/
//float		has1=0.;   /*Количество часов в одном рабочем дне*/
//float		kof=0.;   /*Коэффициент оплаты ставки*/
int metka_sp=0;
float		kolrd=0.; /*Количество рабочих дней в месяце*/
class iceb_t_mstr spis_menu(0);
float		kolrh=0.; /*Количество рабочих часов в смене*/
char strmenu[1024];
class iceb_tu_str shet_do("");
SQLCURSOR cur1;
SQLCURSOR cur;

clear();
dn=mn=gn=dk=mk=gk=K=pozz=pozz1=kom=0;

okl=oklad(tabn,&metka_sp);

//Создаем класс меню
class VVOD DANET(1);


naz1:;

clear();
short           kls=LINES-5;
short		nomzap[kls];
int dlinna_nazv=0;
int max_dlinna_nazv=0;
kd=kh=kkd=0.;


sprintf(strsql,"select kodt,dnei,has,kdnei,datn,datk,kolrd,kolrh,nomz,kom \
from Ztab where god=%d and mes=%d and tabn=%ld order by kodt asc",
gp,mp,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
 
if(kolstr == 0)
 {

lll:;

  shzr6(mp,gp,tabn,fam,kd,kh,kkd,okl,0.,0.,20);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K != FK2 && K != FK10 && K != ESC && K != PLU)
      goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
 }

max_dlinna_nazv=0;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   nait.new_plus(row1[0]);
  else
   nait.new_plus("");
   
  if(iceb_tu_strlen(nait.ravno()) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(nait.ravno());

 }

naz:;
if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();

dlinna_nazv=COLS-55;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;

for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;


  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
  
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден код табеля"),row[0]);
    OSTANOV();
    nait.new_plus("");
   }
  else
    nait.new_plus(row1[0]);
  
  dneii=atof(row[1]);
  has=atof(row[2]);
  kdnei=atof(row[3]);
  memset(bros,'\0',sizeof(bros));
  memset(strsql,'\0',sizeof(strsql));
  rsdat(&dn,&mn,&gn,row[4],2);    
  rsdat(&dk,&mk,&gk,row[5],2);    
  kolrd=atof(row[6]);
  kolrh=atof(row[7]);

  nomzap[i]=atoi(row[8]);

  if(dn != 0)
   sprintf(bros,"%d.%d.%d",dn,mn,gn);
  if(dk != 0)
    sprintf(strsql,"%d.%d.%d",dk,mk,gk);

  sprintf(strmenu,"%-2s|%-*.*s|%-3.5g|%-4.5g|%-3.5g|%-2.5g|%-2.5g|%-10s|%-10s|%s",
  row[0],
  iceb_tu_kolbait(dlinna_nazv,nait.ravno()),iceb_tu_kolbait(dlinna_nazv,nait.ravno()),nait.ravno(),
  dneii,has,kdnei,kolrd,kolrh,bros,strsql,row[9]);

  kd+=dneii;
  kh+=has;
  kkd+=kdnei;

  spis_menu.spis_str.plus(strmenu);
  i++;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shzr6(mp,gp,tabn,fam,kd,kh,kkd,okl,0.,0.,dlinna_nazv);


prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-26);


if(kom == -7) //Изменение размеров экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i < kls)
   {
    beep();
    goto naz;
   }
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  if(pozz == 0)
   {
    beep();
    goto naz;
   }
  if(pozz != 0)
    pozz--;
  goto naz;
 }

if(kom == -5) /*Страница вниз*/
 {
  if(i == kls)
    pozz+=kls;
  else
    beep();
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  pozz-=kls;
  if(pozz < 0)
   {
    beep();
    pozz=0;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_out.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(str_out.ravno(),&kod,1,'|');
  polen(str_out.ravno(),&n1,2,'|');
  polen(str_out.ravno(),&dneii,3,'|');
  polen(str_out.ravno(),&has,4,'|');
  polen(str_out.ravno(),&kdnei,5,'|');

  polen(str_out.ravno(),&kolrd,6,'|');

  polen(str_out.ravno(),&kolrh,7,'|');

  polen(str_out.ravno(),bros,sizeof(bros),8,'|');
  rsdat(&dn,&mn,&gn,bros,1);
  polen(str_out.ravno(),bros,sizeof(bros),9,'|');
  rsdat(&dk,&mk,&gk,bros,1);
 }

vper:;

switch (K)
 {
  case ESC : /*Выход*/
  case FK10 : /*Выход*/
     break;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_4.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;



  case FK2:   /*Добавить запись*/
  case PLU:   /*Добавить запись*/

  n.new_plus("");
  n1.new_plus("");
  poz=komv=0;

  if(dirmat1("Tabel",&n,&n1,0,-1,-1,2,&poz,&komv,"","\0") == 0)
    zapktabel(mp,gp,tabn,n.ravno(),0,n1.ravno(),0,0.,0.);

  goto naz1;
  break;

  case SFK2:   /*Корректировать запись*/
  case ENTER:

  zapktabel(mp,gp,tabn,kod.ravno(),nomzap[kom],n1.ravno(),1,0.,0.);
  goto naz1;
  break;

  case FK3:   /*Удалить*/

    if(iceb_t_pbpds(mp,gp) != 0)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Игнорировать блокировку ?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz1;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Ztab where god=%d and mes=%d and \
tabn=%ld and kodt=%s and nomz=%d",
      gp,mp,tabn,kod.ravno(),nomzap[kom]);

      sql_zapis(strsql,1,0);

      clear();
      goto naz1;
     }
    clear(); /*Очистить экран и закрасить фоновым цветом*/
    goto naz;

  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
refresh();

}

/******************************/
/*Ввод и корректировка записей*/
/******************************/
/*Если вернули 0 запись сделана
               1 нет
*/
int zapktabel(short mp,short gp,int tabn,const char *kod_tab,int nom_zap,
const char *naim_tab,int mz, /*0-ввод новой 1-корректировка*/
float has1,float kof)
{
short		dn,mn,gn;
short		dk,mk,gk;
short		d,m,g;
int		K,N;
double		bb;
short		maxkkd; /*Максимальное количество календарных дней*/
char		bros[512];
class iceb_tu_str snomz("");
char		strsql[1024];
short		dnp=0,mnp=0,gnp=0;
short		dkp=0,mkp=0,gkp=0;
float dnei=0.,hasov=0.;
SQL_str row;
class SQLCURSOR cur;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(6);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(40);
VV.VVOD_SPISOK_add_data(3);

redkal(mp,gp,&dnei,&hasov);


dn=mn=gn=dk=mk=gk=maxkkd=K=N=0;
m=mp; g=gp;
snomz.new_plus(nom_zap);
dpm(&maxkkd,&m,&g,5);
sprintf(bros,"%s %s",kod_tab,naim_tab);
VV.VVOD_SPISOK_add_ZAG(bros);

if(mz == 1)
 {

  sprintf(strsql,"select dnei,has,kdnei,datn,datk,ktoz,vrem,kolrd,kolrh,kom from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
  tabn,gp,mp,atoi(kod_tab),nom_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }

  if(atof(row[0]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[0]));
    VV.data_plus(0,strsql);
   }
  if(atof(row[1]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[1]));
    VV.data_plus(1,strsql);
   }

  if(atof(row[2]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[2]));
    VV.data_plus(2,strsql);
   }

  if(atof(row[7]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[7]));
    VV.data_plus(3,strsql);
   }
  if(atof(row[8]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[8]));
    VV.data_plus(4,strsql);
   }

  if(row[3][0] != '0')
    VV.data_plus(5,iceb_tu_datzap(row[3]));

  if(row[4][0] != '0')
    VV.data_plus(6,iceb_tu_datzap(row[4]));

  VV.data_plus(7,row[9]);
  VV.data_plus(8,nom_zap);

  kzvz(row[5],row[6],&VV);

 }



VV.VVOD_SPISOK_add_MD(gettext("Количество дней.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Количество часов (ч:м)......."));//1
VV.VVOD_SPISOK_add_MD(gettext("Количество календарных дней.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Колич. раб. дней в месяце...."));//3
VV.VVOD_SPISOK_add_MD(gettext("Колич. часов в рабочем дне..."));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..................."));//7
VV.VVOD_SPISOK_add_MD(gettext("Номер записи................."));//8

rsdat(&dnp,&mnp,&gnp,VV.VVOD_SPISOK_return_data(5),1);
rsdat(&dkp,&mkp,&gkp,VV.VVOD_SPISOK_return_data(6),1);

naz:;

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
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

    if(N == 0) /*Количество отработанных дней*/
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
         {
          if(has1 != 0.)
            bb=ATOFM(VV.VVOD_SPISOK_return_data(N))*has1*kof;
          else
           {
            if(ATOFM(VV.VVOD_SPISOK_return_data(N)) == dnei && hasov > 0.)
             bb=hasov;
            else
             bb=ATOFM(VV.VVOD_SPISOK_return_data(N))*8;
           }
          bb=okrug(bb,0.01);
          VV.data_plus(1,bb);
         }
        if(VV.VVOD_SPISOK_return_data(2)[0] == '\0' && dnei == ATOFM(VV.VVOD_SPISOK_return_data(0)))
         {
          VV.data_plus(2,maxkkd);
         }
        N++;
        goto naz;
       }
     }
    if(N == 1) /*Количество отработанных часов*/
     {

      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(polen(VV.VVOD_SPISOK_return_data(N),bros,sizeof(bros),2,':') == 0)
       {
        bb=(bb*60+ATOFM(bros))/60;
       }
      if(bb != 0.)
       VV.data_plus(N,bb);
      N++;
      goto naz;
     }

    if(N == 2) /*Количество отработанных календарных дней*/
     {
      d=(short)ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(d > maxkkd)
       {
        sprintf(strsql,gettext("В этом месяце календарных дней не может быть больше %d !"),maxkkd);
        iceb_t_soob(strsql);
        goto naz;
       }
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата начала периода
     {
      if(rsdat(&dnp,&mnp,&gnp,VV.VVOD_SPISOK_return_data(N),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала !"));
        goto naz;
       }
     }
    if(N == 6 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата конца периода
     {
      if(rsdat(&dkp,&mkp,&gkp,VV.VVOD_SPISOK_return_data(N),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата конца !"));
        goto naz;
       }
      if(dnp != 0 && dkp != 0)
       {
        sprintf(strsql,"%d",period(dnp,mnp,gnp,dkp,mkp,gkp,0));
        VV.VVOD_SPISOK_zapis_data(2,strsql);
        sprintf(strsql,"%d",krdvp(dnp,mnp,gnp,dkp,mkp,gkp));
        VV.VVOD_SPISOK_zapis_data(0,strsql);
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
  attron(VV.VVOD_return_cs(iceb_CFS));
  VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     return(1);
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

     if(iceb_t_pbpds(mp,gp) != 0)
      {
       class VVOD DANET(1);
       DANET.VVOD_SPISOK_add_MD(gettext("Игнорировать блокировку?"));
       if(danet(&DANET,2,stdscr) == 2)
         goto naz;
      }
     if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
     if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(5),1) != 0)
      {
       iceb_t_soob(gettext("Не верно ведена дата начала !"));
       goto naz;
      }
     if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
     if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(6),1) != 0)
      {
       iceb_t_soob(gettext("Не верно ведена дата конца !"));
       goto naz;
      }

     if(zaptabel(mz,mp,gp,tabn,atoi(kod_tab),atof(VV.VVOD_SPISOK_return_data(0)),atof(VV.VVOD_SPISOK_return_data(1)),
     atof(VV.VVOD_SPISOK_return_data(2)),dn,mn,gn,dk,mk,gk,atof(VV.VVOD_SPISOK_return_data(3)),atof(VV.VVOD_SPISOK_return_data(4)),VV.data_ravno_filtr(7),
     atoi(VV.VVOD_SPISOK_return_data(8)),snomz.ravno_atoi()) != 0)
       goto naz;

     return(0);
    }
 }
}


/********/
/*Шапка */
/*********/
void shzr6(short mp,short gp,long tn,const char *fam,float kd,
float kh,float kkd,
double oklad, //Найденный оклад
float has1,   //Количество часов в одном рабочем дне
float kof, //Коэффициент оплаты ставки
int dlinna_nai)
{

move(0,0);

printw(gettext("Ввод и корректировка табеля.\n"));
printw("%s:%d.%d%s %d %s\n",gettext("Дата"),mp,gp,
gettext("г."),tn,fam);

printw("%s %.6g %s %.6g %s %.6g",
gettext("Дней"),kd,
gettext("Часов"),kh,
gettext("Кал-рных дней"),kkd);
if(oklad != 0.)
 printw(" %s:%.2f/%.2f/%.2f",gettext("Оклад"),oklad,has1,kof);
move(3,0);

short		y=2;
short           x=COLS-20;

mvprintw(3,0,"   1   %*s2%*s  3    4   5   6  7     8          9      10",dlinna_nai/2-1,"",dlinna_nai/2-1,"");


mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x," 1-%s",gettext("код табеля"));
mvprintw(y++,x," 2-%s",gettext("наименование"));
mvprintw(y++,x," 3-%s",gettext("отработано дней"));
mvprintw(y++,x," 4-%s",gettext("отработано часов"));
mvprintw(y++,x," 5-%s",gettext("календарных дней"));
mvprintw(y++,x," 6-%s",gettext("кол.раб.дней"));
mvprintw(y++,x," 7-%s",gettext("кол.раб.часов"));
mvprintw(y++,x," 8-%s",gettext("дата начала"));
mvprintw(y++,x," 9-%s",gettext("дата конца"));
mvprintw(y++,x,"10-%s",gettext("коментарий"));

helstr(LINES-1,0,"F1",gettext("допомога"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F10",gettext("выход"),NULL);

}
