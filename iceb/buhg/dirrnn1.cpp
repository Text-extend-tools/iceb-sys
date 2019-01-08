/*$Id: dirrnn1.c,v 5.59 2014/08/31 06:18:18 sasa Exp $*/
/*29.03.2018	28.08.2005	Белых А.И.	dirrnn1.c
Работа с реестром полученных налоговых накладных
*/
#include <ctype.h>
#include <errno.h>
#include "buhg.h"
#include "xrnn_poi.h"
 
void sdirrnn1(short,short,short,class xrnn_poi *poi);
int dirrnn_vvod1(int nom_zap);
int dirrnn1_poi1(class xrnn_poi *poi);
int dirrnn1_poi_prov1(SQL_str row,class xrnn_poi *poi);
void dirrnn_udzap1(SQLCURSOR *cur,int kolstr,class xrnn_poi *poi);
int dirrnn1_pnnn(int nz,const char *datavnn,const char *nomernn,const char *inn);
void dirrnn1_zapros(class SQLCURSOR *cur,int kolstr,class xrnn_poi *poi);
void dirrnn1_ras(class SQLCURSOR *cur,int kolstr,class xrnn_poi *poi);

extern class iceb_rnfxml_data rek_zag_nn;

void dirrnn1(short dnp,short mnp,short gnp)
{
class spis_oth oth;
short dnz=dnp,mnz=mnp,gnz=gnp; 
short dkz=0,mkz=0,gkz=0; 
short d,m,g;
short dd=0,md=0,gd=0;
char strsql[1024];
int kolstr=0;
int K=0;
int kom=0;
class iceb_t_mstr spis_menu(0);
int pozz=0,pozz1=0;
int i=0;
int		prc;
short          x=0,y=2;
static class xrnn_poi poi;
double sumad=0.;
double sumands=0.;
short dv,mv,gv; /*Дата выписки налоговой накладной*/
SQL_str row,row1;
short		mspz; /*метка соответствия первой записи*/
short metka_numer=0; /*метка нумерации записей*/
class iceb_tu_str dat_zap;
poi.metka_poi=0;
poi.nomzap_v.free_class();
short metka_sort_dat=0; /*0-в порядке возростания дат 1-в порядке убывания дат*/
poi.dn=dnz;
poi.mn=mnz;
poi.gn=gnz;

poi.dk=dkz;
poi.mk=mkz;
poi.gk=gkz;

//Создаем класс меню
VVOD SOOB(1);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

naz1:;

clear();
GDITE();
short           kls=LINES-5;
int nom_zap[kls];


sprintf(strsql,"select * from Reenn1 where datd >= '%04d-%02d-%02d'",gnz,mnz,dnz);


if(dnz != 0 && dkz != 0)
  sprintf(strsql,"select * from Reenn1 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gnz,mnz,dnz,gkz,mkz,dkz);

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc,nnn asc");
else
 strcat(strsql," order by datd desc,nnn asc");

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  
poi.suma[0]=poi.suma[1]=0.;
if(kolstr == 0)
 {

lll:;

  sdirrnn1(dnp,mnp,gnp,&poi);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
  if(K == FK5 || K == SFK2 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
     
  goto vper;
 }
else
 {
  while(cur.read_cursor(&row) != 0)
   {
    if(dirrnn1_poi_prov1(row,&poi) != 0)
     continue;
    poi.suma[0]+=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);
    poi.suma[1]+=atof(row[4])+atof(row[6])+atof(row[8])+atof(row[10]);
   }  
  cur.poz_cursor(0);
 }
naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();

char data_vnn[64];

for(i=0; i<kls ; )
 {
  if(cur.read_cursor(&row) == 0)
    break;
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;

  if(dirrnn1_poi_prov1(row,&poi) != 0)
   continue;

  rsdat(&d,&m,&g,row[1],2);  
  rsdat(&dv,&mv,&gv,row[21],2);  
  memset(data_vnn,'\0',sizeof(data_vnn));

  if(dv != 0)
   sprintf(data_vnn,"%02d.%02d.%d",dv,mv,gv);
  else
   sprintf(data_vnn,"%10s","");
        
  nom_zap[i]=atoi(row[0]);
   
  sumad=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);
  sumands=atof(row[4])+atof(row[6])+atof(row[8])+atof(row[10]);

  if(poi.nomzap_v.find(nom_zap[i]) >= 0)
   dat_zap.new_plus("*");
  else
   dat_zap.new_plus("");

  if(metka_numer == 0)
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
  else
    sprintf(strsql,"%d %02d.%02d.%d",pozz+i+1,d,m,g);
  dat_zap.plus(strsql);
    
  if(COLS > 90)  
    sprintf(strsql,"%s|%-*s|%-*s|%8.2f|%7.2f|%s|%-*s|%s|%s",
    dat_zap.ravno(),
    iceb_tu_kolbait(8,row[2]),row[2],
    iceb_tu_kolbait(3,row[22]),row[22],
    sumad,sumands,row[13],
    iceb_tu_kolbait(3,row[20]),row[20],
    data_vnn,
    row[12]);
  else
    sprintf(strsql,"%s|%-*s|%-*s|%8.2f|%7.2f|%s|%-*s|%s",
    dat_zap.ravno(),
    iceb_tu_kolbait(8,row[2]),row[2],
    iceb_tu_kolbait(3,row[22]),row[22],
    sumad,sumands,row[13],
    iceb_tu_kolbait(3,row[20]),row[20],
    row[12]);

  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }

sdirrnn1(dnp,mnp,gnp,&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-25);

if(K == INS) //отметка записи
 {
  int nomer_v_sp=0;
  if((nomer_v_sp=poi.nomzap_v.find(nom_zap[kom])) >= 0)
   poi.nomzap_v.new_plus(0,nomer_v_sp);
  else
   poi.nomzap_v.plus(nom_zap[kom],-1);
  kom++;
  goto naz1;
 }

if(kom == -7) //Изменение размера экрана
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


    if(dirrnn1_poi_prov1(row,&poi) != 0)
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


    if(dirrnn1_poi_prov1(row,&poi) != 0)
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
  GDITE();
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

    if(dirrnn1_poi_prov1(row,&poi) != 0)
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

 }

vper:;
switch(K)
 {  

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("rnn1.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case PLU:  //Ввод новой записи
  case FK2:  //Ввод новой записи
     dirrnn_vvod1(0);
     goto naz1;

  case ENTER:
  case SFK2:  //Корректировка записи
    dirrnn_vvod1(nom_zap[kom]);
    goto naz1;

  case FK3:  //Удалить запись
    //читаем запись которую надо корректировать
    sprintf(strsql,"select datd from Reenn1 where nz=%d",nom_zap[kom]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      iceb_t_soob(gettext("Не найдена запись для проверки даты!"));
      goto naz;
     }
    rsdat(&dd,&md,&gd,row1[0],2);
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    sprintf(strsql,"delete from Reenn1 where nz=%d",nom_zap[kom]);
    sql_zapis(strsql,0,0);
    goto naz1;

  case SFK3:  //Удалить все записи в соответствии с условиями поиска
    dirrnn_udzap1(&cur,kolstr,&poi);
    goto naz1;


  case FK4:  //Поиск
    if(dirrnn1_poi1(&poi) == 0)
     {
      dnz=poi.dn;
      mnz=poi.mn;
      gnz=poi.gn;

      dkz=poi.dk;
      mkz=poi.mk;
      gkz=poi.gk;
      
     }
    else
     {
      dnz=dnp;
      mnz=mnp;
      gnz=gnp;

      dkz=0;
      mkz=0;
      gkz=0;
     }
    pozz=0;
    goto naz1;

  case FK5: //распечатка
    poi.pr_ras.new_plus("+");
    oth.clear();

    /*Для налоговых накладных нужно определить тип периода для этого нужны две даты*/
    if(poi.dn != 0 && poi.dk == 0)
     {
      poi.dk=31;
      poi.mk=poi.mn;
      poi.gk=poi.gn;
      dpm(&poi.dk,&poi.mk,&poi.gk,5); //установить правильно последний день месяца
     }

    dirrnn1_ras(&cur,kolstr,&poi);
    goto naz;

  case FK6: //импорт документов

    ree_imp(1);
    goto naz1;

  case FK7: //Включить/выключить нумерацию строк
    metka_numer++;
    if(metka_numer > 1)
     metka_numer=0;
    goto naz1;

  case TAB: /*Включить/выключить сортировку в порядке убывания дат*/
    metka_sort_dat++;
    if(metka_sort_dat >= 2)
     metka_sort_dat=0;
    goto naz1;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }


}
/****************************/
/*Шапка */
/*****************************/

void sdirrnn1(short dp,short mp,short gp, //Дата начала просмотра реестра
class xrnn_poi *poi)
{
int		i1;
int		X;

clear();
printw("%s",gettext("Реестр полученных налоговых накладных"));

if(dp != 0)
 printw(" %02d.%02d.%d",dp,mp,gp);
if(COLS > 90) 
 mvprintw(1,0,"       1          2     3       4      5   6  7        8     9");
else                                               
 mvprintw(1,0,"       1          2     3       4      5   6  7   8");
mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));
mvprintw(++i1,X,"1 %s",gettext("дата документа"));
mvprintw(++i1,X,"2 %s",gettext("номер документа"));
mvprintw(++i1,X,"3 %s",gettext("вид документа"));
mvprintw(++i1,X,"4 %s",gettext("сумма по док."));
mvprintw(++i1,X,"5 %s",gettext("сумма НДС"));
mvprintw(++i1,X,"6 %s",gettext("метка"));
mvprintw(++i1,X,"7 %s",gettext("код оп-ции"));
if(COLS > 90)
 {
  mvprintw(++i1,X,"8 %s",gettext("дата в.н.н."));
  mvprintw(++i1,X,"9 %s",gettext("контрагент"));
 }
else
  mvprintw(++i1,X,"8 %s",gettext("контрагент"));

mvprintw(i1+=2,X=COLS-21,"%10.2f",poi->suma[1]);
mvprintw(++i1,X,"%10.2f",poi->suma[0]-poi->suma[1]);
mvprintw(++i1,X,"%10.2f",poi->suma[0]);


if(poi->metka_poi == 1)
 {
  move(i1+=2,X=COLS-21);
//  attron(A_BLINK);
  printw(gettext("Поиск:"));

  if(poi->dn != 0)
    mvprintw(++i1,X,"%s:%d.%d.%d",gettext("Дата н."),poi->dn,poi->mn,poi->gn);
  if(poi->dk != 0)
    mvprintw(++i1,X,"%s:%d.%d.%d",gettext("Дата к."),poi->dk,poi->mk,poi->gk);
  if(poi->nnn.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Номер н.н."),poi->nnn.ravno());
  if(poi->nomdok.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Номер док."),poi->nomdok.ravno());
  if(poi->inn.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Инд.нал.ном."),poi->inn.ravno());
  if(poi->naim_kontr.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Наи.к-та"),poi->naim_kontr.ravno());
  if(poi->koment.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Комент."),poi->koment.ravno());
  if(poi->kodgr.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Код гр."),poi->kodgr.ravno());
  if(poi->podsistema.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Подсис."),poi->podsistema.ravno());
  if(poi->kodop.ravno()[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Код оп."),poi->kodop.ravno());
  if(poi->dvnn != 0)
    mvprintw(++i1,X,"%s:%d.%d.%d",gettext("дата в.н.н."),poi->dvnn,poi->mvnn,poi->gvnn);
  if(poi->dvnk != 0)
    mvprintw(++i1,X,"%s:%d.%d.%d",gettext("дата в.н.н."),poi->dvnk,poi->mvnk,poi->gvnk);
  if(poi->viddok.getdlinna() > 1)
    mvprintw(++i1,X,"%s:%s",gettext("Вид док."),poi->viddok.ravno());

  if(poi->metka_ins == 1)
    mvprintw(++i1,X,"%s:%s",gettext("Отметка"),gettext("отмеченные"));
  if(poi->metka_ins == 2)
    mvprintw(++i1,X,"%s:%s",gettext("Отметка"),gettext("не отмеченные"));
      
//  attroff(A_BLINK);
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("импорт"),
"F7",gettext("номер"),
"F10",gettext("вых."),NULL);

}
/******************************************/
/*Ввод и коррректировка*/
/*******************************************/

int dirrnn_vvod1(int nom_zap)
{
int N=0;
int K=0;
class iceb_tu_str nomdi("");
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
short dd,md,gd;
short ddp=0,mdp=0,gdp=0;
int metka_pods=0;
short mz=0,gz=0;
char datdi[11];
short dv,mv,gv;
strcpy(datdi,"0000-00-00");
double nds7proc=7.;
double   nds1=20; /*Н.Д.С.*/



VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(20);//5
VV.VVOD_SPISOK_add_data(20);//6
VV.VVOD_SPISOK_add_data(20);//7
VV.VVOD_SPISOK_add_data(20);//8
VV.VVOD_SPISOK_add_data(20);//9
VV.VVOD_SPISOK_add_data(20);//10
VV.VVOD_SPISOK_add_data(124);//11
VV.VVOD_SPISOK_add_data(100);//12
VV.VVOD_SPISOK_add_data(10);//13
VV.VVOD_SPISOK_add_data(10);//14
VV.VVOD_SPISOK_add_data(11);//15
VV.VVOD_SPISOK_add_data(8);//16

if(nom_zap != 0)
 {

  //читаем запись которую надо корректировать
  sprintf(strsql,"select * from Reenn1 where nz=%d",nom_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки !"));
    return(1);
   }

  VV.data_plus(0,row[2]);
  
  rsdat(&dd,&md,&gd,row[1],2);
  mz=md;
  gz=gd;
  
  sprintf(strsql,"%d.%d.%d",dd,md,gd);
  VV.data_plus(1,strsql);

  VV.data_plus(2,row[3]);
  VV.data_plus(3,row[4]);

  VV.data_plus(4,row[5]);
  VV.data_plus(5,row[6]);

  VV.data_plus(6,row[7]);
  VV.data_plus(7,row[8]);

  VV.data_plus(8,row[9]);
  VV.data_plus(9,row[10]);


  VV.data_plus(10,row[11]);
  VV.data_plus(11,row[12]);

  metka_pods=atoi(row[13]);

  VV.data_plus(12,row[16]);
  VV.data_plus(13,row[17]);
  VV.data_plus(14,row[20]);
  VV.data_plus(16,row[22]);
  
  strcpy(datdi,row[14]);
  rsdat(&ddp,&mdp,&gdp,row[14],2);

  nomdi.new_plus(row[15]);

  kzvz(atoi(row[18]),atol(row[19]),&VV);

  rsdat(&dv,&mv,&gv,row[21],2);

  if(dv != 0)
   {
    sprintf(strsql,"%d.%d.%d",dv,mv,gv);
    VV.data_plus(15,strsql);
   }
  
 }

VV.VVOD_SPISOK_add_MD( gettext("Номер налоговой накладной...................."));//0
VV.VVOD_SPISOK_add_MD( gettext("Дата получения налоговой накладной..(д.м.г).."));//1

sprintf(strsql,        gettext("Облагается %4.1f%%, 0%%...(сумма по документу).."),nds1);//2
VV.VVOD_SPISOK_add_MD(strsql);
sprintf(strsql,        gettext("Облагается %4.1f%%, 0%%..(сумма НДС)............"),nds1);//3
VV.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,        gettext("Облагается 7%%(сумма по документу)............"));//4
VV.VVOD_SPISOK_add_MD(strsql);
sprintf(strsql,        gettext("Облагается 7%%(сумма НДС)....................."));//5
VV.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,        gettext("Облагается 0%% (сумма по документу)..........."));//6
VV.VVOD_SPISOK_add_MD(strsql);
sprintf(strsql,        gettext("Облагается 0%% (сумма НДС)...................."));//7
VV.VVOD_SPISOK_add_MD(strsql);



sprintf(strsql,        gettext("Освобождено от нал. об.(сумма по документу).."));//8
VV.VVOD_SPISOK_add_MD(strsql);
sprintf(strsql,        gettext("Освобождено от нал. об.(сумма НДС)..........."));//9
VV.VVOD_SPISOK_add_MD(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер..............."));//10
VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента....................."));//11
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..................................."));//12
VV.VVOD_SPISOK_add_MD(gettext("Код группы документа........................."));//13
VV.VVOD_SPISOK_add_MD(gettext("Код операции................................."));//14
VV.VVOD_SPISOK_add_MD(gettext("Дата выписки налоговой накладной..(д.м.г)...."));//15
VV.VVOD_SPISOK_add_MD(gettext("Вид документа................................"));//16

naz:;

double suma1=VV.data_ravno_atof(2)+VV.data_ravno_atof(4)+VV.data_ravno_atof(6)+VV.data_ravno_atof(8);
double suma2=VV.data_ravno_atof(3)+VV.data_ravno_atof(5)+VV.data_ravno_atof(7)+VV.data_ravno_atof(9);

VV.VVOD_delete_ZAG();
if(nom_zap == 0)
 {
  sprintf(strsql,"%s %.2f/%.2f",gettext("Ввод новой записи"),suma1,suma2);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }
else
 {
  sprintf(strsql,"%s %.2f/%.2f",gettext("Корректировка записи"),suma1,suma2);
  VV.VVOD_SPISOK_add_ZAG(strsql);
  if(metka_pods != 0)      
   {
    if(metka_pods == 1)
      VV.VVOD_SPISOK_add_ZAG(gettext("Материальный учёт"));
    if(metka_pods == 2)
      VV.VVOD_SPISOK_add_ZAG(gettext("Учёт услуг"));
    if(metka_pods == 3)
      VV.VVOD_SPISOK_add_ZAG(gettext("Учёт основных средств"));
    if(metka_pods == 4)
      VV.VVOD_SPISOK_add_ZAG(gettext("Учёт командировочных расходов"));
    if(metka_pods == 5)
      VV.VVOD_SPISOK_add_ZAG(gettext("Гланая книга"));
    
    sprintf(strsql,"%d.%d.%d %s",ddp,mdp,gdp,nomdi.ravno());
    
    VV.VVOD_SPISOK_add_ZAG(strsql);

    if(VV.data_ravno(14)[0] != '\0') //код операции в документе        
     {
      if(metka_pods == 1)
       sprintf(strsql,"select naik from Prihod where kod='%s'",VV.data_ravno_filtr(14));
      if(metka_pods == 2)
       sprintf(strsql,"select naik from Usloper1 where kod='%s'",VV.data_ravno_filtr(14));
      if(metka_pods == 3)
       sprintf(strsql,"select naik from Uospri where kod='%s'",VV.data_ravno_filtr(14));
      if(metka_pods == 4)
       sprintf(strsql,"select naik from Ukrvkr where kod='%s'",VV.data_ravno_filtr(14));

      SQL_str row1;
      class SQLCURSOR cur1;

      if(readkey(strsql,&row1,&cur1) == 1)
       {
        sprintf(strsql,"%s:%s %s",gettext("Код операции"),row[20],row1[0]);
        VV.VVOD_SPISOK_add_ZAG(strsql);
        
       }
     }
     
   }
 }

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("расчёт"),
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
   {
    goto naz;
   }

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

    if(N == 0) /*Номер налоговой накладной*/
     {
      dirrnn1_pnnn(nom_zap,VV.data_ravno(15),VV.data_ravno(0),VV.data_ravno(10));
     }

    if(N == 2 || N == 3 || N == 4 || N == 5 || N == 6 || N == 7 || N == 8 || N == 9) /*обновляем сумму в заголовке*/
     {
      N++;
      goto naz;
     }     
    if(N == 11 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //наименование контрагента
     {
      if(isdigit(VV.VVOD_SPISOK_return_data(N)[0]) != 0)  /*Значит символ число*/
       {
        sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%d'",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          if(row[2][0] == '\0')
            VV.data_plus(N,row[0]);
          else
           {
            VV.data_plus(N,row[2]);
           }
          VV.data_plus(10,row[1]);
          
          goto naz;
         }
       }
     }
    if(N == 10) /*индивидуальный налоговый номер*/
     {
      dirrnn1_pnnn(nom_zap,VV.data_ravno(15),VV.data_ravno(0),VV.data_ravno(10));
     }
    if(N == 15) /*Дата выписки налоговой накладной*/
     {
      dirrnn1_pnnn(nom_zap,VV.data_ravno(15),VV.data_ravno(0),VV.data_ravno(10));
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
    if(rsdat(&dd,&md,&gd,VV.data_ravno(1),1) != 0)
     {
      sprintf(strsql,gettext("Не правильно введена дата получения налоговой накладной !"));
      iceb_t_soob(strsql);
      goto naz;
     }
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;
    if(nom_zap != 0)
     if(iceb_t_pbpds(mz,gz) != 0)
      goto naz;

    dv=mv=gv=0;
    if(VV.data_ravno(15)[0] != '\0')
    if(rsdat(&dv,&mv,&gv,VV.data_ravno(15),1) != 0)
     {
      sprintf(strsql,gettext("Неправильно введена дата выписки!"));
      iceb_t_soob(strsql);
      goto naz;

     }
   
    char kodgr[VV.data_dlinna(13)*2+1];
    strcpy(kodgr,VV.data_ravno(13));
    sqlfiltr(kodgr,sizeof(kodgr));

    if(kodgr[0] != '\0')
     {
      sprintf(strsql,"select kod from Reegrup1 where kod='%s'",kodgr);
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код группы документа"),kodgr);
        iceb_t_soob(strsql);
        goto naz;
       }
     }
    time_t vrem;
    time(&vrem);
    
    if(nom_zap == 0) //Ввод новой записи
     {
      /*Проверяем может для контрагента в этом году уже был такой номер*/
      if(VV.data_ravno_atoi(10) != 0)
        sprintf(strsql,"select datd from Reenn1 where datd >= '%04d-01-01' and datd <= '%04d-12-01' and nnn='%s' and inn='%s'",
        gd,gd,VV.data_ravno(0),VV.data_ravno(10));    
      else
        sprintf(strsql,"select datd from Reenn1 where datd >= '%04d-01-01' and datd <= '%04d-12-01' and nnn='%s' and nkontr='%s'",
        gd,gd,VV.data_ravno(0),VV.data_ravno(11));    
        
      if(readkey(strsql,&row,&cur) > 0)    
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s!",gettext("Внимание"));
        SOOB.VVOD_SPISOK_add_MD(strsql);

        SOOB.VVOD_SPISOK_add_MD(gettext("Уже есть такой номер налоговой накладной в этом году"));
                
        short d,m,g;
        rsdat(&d,&m,&g,row[0],2);
        sprintf(strsql,"%d.%d.%d",d,m,g);
        SOOB.VVOD_SPISOK_add_MD(strsql);
    
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        
       }


      //фильтрование символьных переменных внутри функции
      if(zapvree1(dd,md,gd,VV.data_ravno(0),
       VV.data_ravno_atof(2),
       VV.data_ravno_atof(3),
       VV.data_ravno_atof(4),
       VV.data_ravno_atof(5),
       VV.data_ravno_atof(6),
       VV.data_ravno_atof(7),
       VV.data_ravno_atof(8),
       VV.data_ravno_atof(9),
       VV.data_ravno(10),
       VV.data_ravno(11),
       metka_pods,
       datdi,
       nomdi.ravno(),
       VV.data_ravno(12),
       VV.data_ravno(13),
       VV.data_ravno(14),
       dv,mv,gv,VV.data_ravno(16)) != 0)
         goto naz;
     }
    else
     {

      char naim_kontr[VV.data_dlinna(11)*2+1];
      strcpy(naim_kontr,VV.data_ravno(11));
      sqlfiltr(naim_kontr,sizeof(naim_kontr));

      char koment[VV.data_dlinna(12)*2+1];
      strcpy(koment,VV.data_ravno(12));
      sqlfiltr(koment,sizeof(koment));

      sprintf(strsql,"update Reenn1 set \
datd='%04d-%02d-%02d',\
nnn='%s',\
sumd1=%.2f,\
snds1=%.2f,\
sumd2=%.2f,\
snds2=%.2f,\
sumd3=%.2f,\
snds3=%.2f,\
sumd4=%.2f,\
snds4=%.2f,\
inn='%s',\
nkontr='%s',\
datdi='%04d-%02d-%02d',\
nomdi='%s',\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
gr='%s',\
ko='%s',\
dvd='%04d-%02d-%02d',\
vd='%s' \
where nz=%d",
      gd,md,dd,
      VV.data_ravno(0),
      VV.data_ravno_atof(2),
      VV.data_ravno_atof(3),
      VV.data_ravno_atof(4),
      VV.data_ravno_atof(5),
      VV.data_ravno_atof(6),
      VV.data_ravno_atof(7),
      VV.data_ravno_atof(8),
      VV.data_ravno_atof(9),
      VV.data_ravno(10),
      naim_kontr,
      gdp,mdp,ddp,
      nomdi.ravno(),
      koment,
      iceb_t_getuid(),
      vrem,
      kodgr,
      VV.data_ravno(14),
      gv,mv,dv,
      VV.data_ravno(16),
      nom_zap);      

      if(sql_zapis(strsql,0,0) != 0)
       goto naz;

     }
//    printw("\nstrsql=%s\n",strsql);
//    OSTANOV();
    break;
   }

  if(K == FK3)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    class iceb_tu_str kod("");
    class iceb_tu_str kodgr("");
    class iceb_tu_str naim("");


    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр контрагентов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп документов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Материальный учёт\""));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт основных средств\""));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт услуг\""));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;


      case 0 :
        if(dirkontr(&kod,&naim,1) == 0)
         {
          VV.data_plus(11,iceb_t_get_pnk(kod.ravno(),1));
          //Читаем индивидуальный налоговый номер
          sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno());
          if(readkey(strsql,&row,&cur) == 1)
           VV.data_plus(10,row[0]);
         }
        goto naz;

      case 1 :
       vibrek("Reegrup1",&kodgr,&naim);

        VV.VVOD_SPISOK_zapis_data(13,kodgr.ravno());
        goto naz;

      case 2 :

 //       vibrek("Prihod",&kod,&naim);
        if(dirprihod(1,&kod,&naim) == 0) 
          VV.data_z_plus(14,kod.ravno());
        break;

      case 3 :
        vibrek("Uospri",&kod,&naim);
        VV.data_plus(14,kod.ravno());
        break;

      case 4 :
        if(diruslop1(1,&kod,&naim) == 0)
          VV.data_plus(14,kod.ravno());
        break;

      case 5 :
        vibrek("Ukrvkr",&kod,&naim);
        VV.data_plus(14,kod.ravno());
        break;
     }





    goto naz;
   }

  if(K == FK4)
   {
    if(VV.data_ravno_atof(4) != 0.)
     goto naz;
    
    double suma_dok=VV.data_ravno_atof(2);
    double nds_dok=VV.data_ravno_atof(3);
    double suma_r=suma_dok*nds7proc/100.;
    double nds_r=nds_dok*nds7proc/100.;

    suma_r=okrug(suma_r,0.01);
    nds_r=okrug(nds_r,0.01);

    VV.data_plus(2,suma_dok-suma_r);
    VV.data_plus(3,nds_dok-nds_r);

    VV.data_plus(4,suma_r);
    VV.data_plus(5,nds_r);
    goto naz;
   }


 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(0);
}
/************************/
/*ввод реквизитов поиска*/
/************************/

int dirrnn1_poi1(class xrnn_poi *poi)
{
char strsql[1024];
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int N=0,K=0;


int kom1=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)................"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.....(д.м.г)............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента..........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Номер документа..............(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код группы документа....(,,)......."));//7
VV.VVOD_SPISOK_add_MD(gettext("Подсистема..............(,,)......."));//8
VV.VVOD_SPISOK_add_MD(gettext("Код операции............(,,)......."));//9
VV.VVOD_SPISOK_add_MD(gettext("Дата вып.нал.накладной нач.(д.м.г)."));//10
VV.VVOD_SPISOK_add_MD(gettext("Дата вып.нал.накладной кон.(д.м.г)."));//11
VV.VVOD_SPISOK_add_MD(gettext("Вид документа............(,,)......"));//12
VV.VVOD_SPISOK_add_MD(gettext("Отметка............................"));//13

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(11);//1
if(poi->dn != 0)
 {
  sprintf(strsql,"%d.%d.%d",poi->dn,poi->mn,poi->gn);
  VV.data_plus(0,strsql);
 }
if(poi->dk != 0)
 {
  sprintf(strsql,"%d.%d.%d",poi->dk,poi->mk,poi->gk);
  VV.data_plus(1,strsql);
 }
VV.VVOD_SPISOK_add_data(poi->nnn.ravno(),128);//2
VV.VVOD_SPISOK_add_data(poi->inn.ravno(),128); //3
VV.VVOD_SPISOK_add_data(poi->naim_kontr.ravno(),128);//4
VV.VVOD_SPISOK_add_data(poi->nomdok.ravno(),128);//5
VV.VVOD_SPISOK_add_data(poi->koment.ravno(),128);//6
VV.VVOD_SPISOK_add_data(poi->kodgr.ravno(),128);//7
VV.VVOD_SPISOK_add_data(poi->podsistema.ravno(),128);//8
VV.VVOD_SPISOK_add_data(poi->kodop.ravno(),128);//9

VV.VVOD_SPISOK_add_data(11);//10

if(poi->dvnn != 0)
 {
  sprintf(strsql,"%d.%d.%d",poi->dvnn,poi->mvnn,poi->gvnn);
  VV.data_plus(10,strsql);
 }
VV.VVOD_SPISOK_add_data(11);//11
if(poi->dvnk != 0)
 {
  sprintf(strsql,"%d.%d.%d",poi->dvnk,poi->mvnk,poi->gvnk);
  VV.data_plus(11,strsql);
 }
VV.VVOD_SPISOK_add_data(poi->viddok.ravno(),128);//12

VV.VVOD_SPISOK_add_data(128);//13


naz:;

if(poi->metka_ins == 0)
 strcpy(strsql,gettext("Все документы"));
if(poi->metka_ins == 1)
 strcpy(strsql,gettext("Только отмеченные документы"));
if(poi->metka_ins == 2)
 strcpy(strsql,gettext("Только не отмеченные документы"));

VV.data_plus(13,strsql);//13

 
helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F3",gettext("реквизиты"),
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

    if(N == 13)
     {
      class VVOD MENU(3);
      MENU.VVOD_SPISOK_add_MD(gettext("Все документы"));
      MENU.VVOD_SPISOK_add_MD(gettext("Только отмеченные документы"));
      MENU.VVOD_SPISOK_add_MD(gettext("Только не отмеченные документы"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 3 :
        case -1:
          goto naz;
      
        case 0 :
          poi->metka_ins=0;
          break;

        case 1 :
          poi->metka_ins=1;
          break;
 
        case 2 :
          poi->metka_ins=2;
          break;
       }
      goto naz;
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) 
     {
     }
    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //наименование контрагента
     {
      if(isdigit(VV.VVOD_SPISOK_return_data(N)[0]) != 0)  /*Значит символ число*/
       {
        VV.data_plus(N,iceb_t_get_pnk(VV.data_ravno(N),1));
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
    poi->metka_poi=0;
    N=-1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    poi->metka_poi=1;

    poi->dn=poi->mn=poi->gn=0;
    poi->dk=poi->mk=poi->gk=0;
    
    if(VV.data_ravno(0)[0] != '\0')
     if(rsdat(&poi->dn,&poi->mn,&poi->gn,VV.data_ravno(0),1) != 0)
      {
       iceb_t_soob(gettext("Не правильно введена дата начала !"));
       goto naz;
      }
    
    if(VV.data_ravno(1)[0] != '\0')
     if(rsdat(&poi->dk,&poi->mk,&poi->gk,VV.data_ravno(1),1) != 0)
      {
       iceb_t_soob(gettext("Не правильно введена дата конца !"));
       goto naz;
      }

    if(VV.data_ravno(10)[0] != '\0')
     if(rsdat(&poi->dvnn,&poi->mvnn,&poi->gvnn,VV.data_ravno(10),1) != 0)
      {
       iceb_t_soob(gettext("Не правильно введена дата выписки!"));
       goto naz;
      }

    if(VV.data_ravno(11)[0] != '\0')
     {
       if(rsdat(&poi->dvnk,&poi->mvnk,&poi->gvnk,VV.data_ravno(11),1) != 0)
        {
         iceb_t_soob(gettext("Не правильно введена дата выписки!"));
         goto naz;
        }
     }
    else
     {
      if(VV.data_ravno(10)[0] != '\0')
       {
        poi->dvnk=poi->dvnn;
        poi->mvnk=poi->mvnn;
        poi->gvnk=poi->gvnn;
        dpm(&poi->dvnk,&poi->mvnk,&poi->gvnk,5); 
       }
     } 
   
    if(poi->dn != 0 && poi->dk == 0)
     {
      poi->dk=31;
      poi->mk=poi->mn;
      poi->gk=poi->gn;
      dpm(&poi->dk,&poi->mk,&poi->gk,5); //Установить правильно последний день месяца
     }

    poi->nnn.new_plus(VV.VVOD_SPISOK_return_data(2));
    poi->inn.new_plus(VV.VVOD_SPISOK_return_data(3));
    poi->naim_kontr.new_plus(VV.VVOD_SPISOK_return_data(4));
    poi->nomdok.new_plus(VV.VVOD_SPISOK_return_data(5));
    poi->koment.new_plus(VV.VVOD_SPISOK_return_data(6));
    poi->kodgr.new_plus(VV.VVOD_SPISOK_return_data(7));
    poi->podsistema.new_plus(VV.VVOD_SPISOK_return_data(8));
    poi->kodop.new_plus(VV.VVOD_SPISOK_return_data(9));
    poi->viddok.new_plus(VV.data_ravno(12));
    return(0);
   }

  if(K == FK3) //Реквизиты
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп документов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подсистем"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Материальный учёт\""));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт основных средств\""));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт услуг\""));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7 :
      case -1:
       goto naz;


      case 0 :
         if(vibrek("Reegrup2",&poi->kodgr) == 0)
           VV.data_plus(7,poi->kodgr.ravno());
         goto naz;

      case 1 :

        MENU.VVOD_delete();

        sprintf(strsql,"%-3s %s",ICEB_MP_MATU,gettext("Материальный учет"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        sprintf(strsql,"%-3s %s",ICEB_MP_USLUGI,gettext("Учет услуг"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        sprintf(strsql,"%-3s %s",ICEB_MP_UOS,gettext("Учет основных средств"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        sprintf(strsql,"%-3s %s",ICEB_MP_UKR,gettext("Учет командировочных расходов"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        sprintf(strsql,"%-3s %s",gettext("ГК"),gettext("Главная книга"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        
        kom1=0;
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
        switch (kom1)
         {
          case 5 :
          case -1:
           goto naz;

          case 0 :
             VV.data_z_plus(8,ICEB_MP_MATU);
             break;
          case 1 :
             VV.data_z_plus(8,ICEB_MP_USLUGI);
             break;
          case 2 :
             VV.data_z_plus(8,ICEB_MP_UOS);
             break;
          case 3 :
             VV.data_z_plus(8,ICEB_MP_UKR);
             break;

          case 4 :
             VV.data_z_plus(8,gettext("ГК"));
             break;
         }
        break;

      case 2 :
//        vibrek("Prihod",&poi->kodop);
        if(dirprihod(1,&kod,&naim) == 0) 
          VV.data_z_plus(9,kod.ravno());
        break;

      case 3 :
        vibrek("Uospri",&poi->kodop);
        VV.data_plus(9,poi->kodop.ravno());
        break;

      case 4 :

        naim.new_plus("");
        if(diruslop1(1,&kod,&naim) == 0)
         VV.data_plus(9,kod.ravno());
        break;

      case 5 :
        vibrek("Ukrvkr",&poi->kodop);
        VV.data_plus(9,poi->kodop.ravno());
        break;
         
      case 6 :
      
        if(vibrek("Kontragent",&kod) == 0)
         {
          VV.data_plus(4,iceb_t_get_pnk(kod.ravno(),1));
         }

        break;
     }         
    goto naz;
   }

  if(K == FK4) //Очистить
   {
    VV.VVOD_clear_data();  
    poi->clear_data();
    goto naz;
   }
  

 }

return(1);
}
/*************************/
/*Проверка на соответствие условиям поиска*/
/******************************************/
int dirrnn1_poi_prov1(SQL_str row,class xrnn_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);

if(poi->podsistema.ravno()[0] != '\0')
 {
  class iceb_tu_str metka_pod("");

  if(row[13][0] == '1')
   metka_pod.new_plus(ICEB_MP_MATU);
  if(row[13][0] == '2')
   metka_pod.new_plus(ICEB_MP_USLUGI);
  if(row[13][0] == '3')
   metka_pod.new_plus(ICEB_MP_UOS);
  if(row[13][0] == '4')
   metka_pod.new_plus(ICEB_MP_UKR);
  if(row[13][0] == '5')
   metka_pod.new_plus(gettext("ГК"));

  if(proverka(poi->podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
   return(1); 
 }
 
if(proverka(poi->nnn.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(poi->nomdok.ravno(),row[15],0,0) != 0)
 return(1);

if(proverka(poi->viddok.ravno(),row[22],0,0) != 0)
 return(1);

if(proverka(poi->kodgr.ravno(),row[17],0,0) != 0)
 return(1);

if(proverka(poi->kodop.ravno(),row[20],0,0) != 0)
 return(1);

if(proverka(poi->inn.ravno(),row[11],4,0) != 0)
 return(1);
if(proverka(poi->koment.ravno(),row[16],4,0) != 0)
 return(1);
 
if(proverka(poi->naim_kontr.ravno(),row[12],4,0) != 0)
 return(1);

if(poi->dvnn != 0)
 {
  short d,m,g;
  rsdat(&d,&m,&g,row[21],2);
  if(sravmydat(poi->dvnn,poi->mvnn,poi->gvnn,d,m,g) > 0)
   return(1);
 
 }

if(poi->dvnk != 0)
 {
  short d,m,g;
  rsdat(&d,&m,&g,row[21],2);
  if(sravmydat(poi->dvnk,poi->mvnk,poi->gvnk,d,m,g) < 0)
   return(1);
 
 }

if(poi->metka_ins == 1) //*только отмеченные
 if(poi->nomzap_v.find(atoi(row[0])) < 0)
 return(1);

if(poi->metka_ins == 2) //*только не отмеченные
 if(poi->nomzap_v.find(atoi(row[0])) >= 0)
 return(1);
 
return(0);
}
/**************************/
/*Удаление записей с учётом условий поиска*/
/******************************************/

void dirrnn_udzap1(SQLCURSOR *cur,int kolstr,class xrnn_poi *poi)
{
SQL_str row;
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return;
short dt,mt,gt;

poltekdat(&dt,&mt,&gt);


VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
if(parolv(&PAR,dt,mt,gt,0) != 0)
 return;

int kolstr1=0;
char strsql[512];
cur->poz_cursor(0);
short d=0,m=0,g=0;
short mz=0,gz=0;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirrnn1_poi_prov1(row,poi) != 0)
   continue;
  
  rsdat(&d,&m,&g,row[1],2);
  
  if(mz != m || gz != g)
   {
    if(iceb_t_pbpds(m,g) != 0)
     return;
    mz=m;
    gz=g;
   }  
  
  sprintf(strsql,"delete from Reenn1 where nz=%s",row[0]);
  sql_zapis(strsql,1,0);

 } 
}
/***********************************************/
/*проверка номера налоговой накладной на повторение*/
/****************************************************/
int dirrnn1_pnnn(int nz,const char *datavnn,const char *nomernn,const char *inn)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

if(datavnn[0] == '\0')
 return(0);
if(nomernn[0] == '\0')
 return(0);
 
if(inn[0] == '\0')
 return(0);


sprintf(strsql,"select datd from Reenn1 where dvd='%s' and nnn='%s' and inn='%s' and nz != %d limit 1",
iceb_tu_tosqldat(datavnn),nomernn,inn,nz);

if(readkey(strsql,&row,&cur) >= 1)
 {
  sprintf(strsql,"%s\n%s:%s",
  gettext("Такой номер налоговой накладной уже введён!"),
  gettext("Дата получения налоговой накладной"),
  iceb_tu_datzap(row[0]));
  iceb_t_soob(strsql);
  return(1);
 }

return(0);

}
/*************************/
/*меню для запроса*/
/***************************/
int dirrnn1_zap_menu(class iceb_tu_str *nomer,int *metka) /*0-не обрезать 1-обрезать*/
{
int K=0,N=0;
static class iceb_tu_str danet(gettext("Да"));
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Запрос на получение сведений из Единого реестра налоговых накладных"));

VV.VVOD_SPISOK_add_MD(gettext("Номер запроса............................"));//0
VV.VVOD_SPISOK_add_MD(gettext("Обрезать вид документа до 2-х символов..."));//1

VV.VVOD_SPISOK_add_data(nomer->ravno(),20);//0
VV.VVOD_SPISOK_add_data(danet.ravno(),20);//1



naz:;

clear();


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

  if(K == KEY_RESIZE)
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);


    if(N == 1) /*Количество*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFS)); 
      class VVOD MENU(3);
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

      MENU.VVOD_SPISOK_add_MD(gettext("Да"));
      MENU.VVOD_SPISOK_add_MD(gettext("Нет"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      
      int kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 2 :
        case -1:
         goto naz;

        case 0:
          VV.data_plus(1,gettext("Да"));
          break;       
        case 1:
          VV.data_plus(1,gettext("Нет"));
          break;       
       }
      goto naz;
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));



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
    return(1);
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }
    break;
   }
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

danet.new_plus(VV.data_ravno(1));

if(SRAV(danet.ravno(),gettext("Да"),0) == 0)
 *metka=1;
else
 *metka=0;
 
return(0);
}











/*************************************/
/*формирование Запроса о получении сведений с Единого реестра налоговых накладных*/
/**********************************************************************************/
void dirrnn1_zapros(class SQLCURSOR *cur,int kolstr,class xrnn_poi *poi)
{
class SQLCURSOR cur1;
SQL_str row;
if(kolstr == 0)
 return;

static class iceb_tu_str nom_zapr("1");
int metka=0; /*0-без обрезания вида документа 1-с обрезанием до 2-х символов*/

if(dirrnn1_zap_menu(&nom_zapr,&metka) != 0)
  return;

char imaf[64];

FILE *ff;
sprintf(imaf,"zps%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

int period_type=0;
char imaf_xml[64];
FILE *ff_xml;
iceb_t_rnfxml(&rek_zag_nn); /*чтение данных для xml файлов*/
if(iceb_t_openxml(poi->dn,poi->mn,poi->gn,poi->dk,poi->mk,poi->gk,imaf_xml,"J13","001","01",1,&period_type,&rek_zag_nn,&ff_xml) != 0)
 return;

//Заголовок файла
iceb_t_rnn_sap_xml(poi->mn,poi->gn,poi->mk,"J13","001",1,1,"J1300101.XSD",period_type,&rek_zag_nn,ff_xml);

class iceb_tu_str edrpou("");
class iceb_tu_str inn("");
if(readkey("select kod,innn from Kontragent where kodkon='00'",&row,&cur1) == 1)
 {
  edrpou.new_plus(row[0]);
  inn.new_plus(row[1]);
 }

fprintf(ff_xml," <DECLARBODY>\n");
if(nom_zapr.getdlinna() > 1)
  fprintf(ff_xml,"   <HNUM>%s</HNUM>\n",nom_zapr.ravno());
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(iceb_t_get_pnk("00",1)));
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",edrpou.ravno());
fprintf(ff_xml,"   <HNPDV>%s</HNPDV>\n",inn.ravno());
fprintf(ff_xml,"   <HKSTI>%s</HKSTI>\n",rek_zag_nn.kod_dpi.ravno());
/*название органа гос. нал. службы Украины*/
//fprintf(ff_xml,"   <HSTI>%s</HSTI>\n", );
/*електронный адрес*/
//fprintf(ff_xml,"   <HEMAIL>%s</HEMAIL>\n", );
 
int kolstr1=0;
class iceb_tu_str stroka("");
class iceb_tu_str nom_dok("");
class iceb_tu_str nom_dok1("");
class iceb_tu_str nom_dok2("");
char bros[1024];
short dv,mv,gv;
iceb_t_zagolov(gettext("Запрос на получение сведений из Единого реестра налоговых накладных"),poi->dn,poi->mn,poi->gn,poi->dk,poi->mk,poi->gk,ff);
fprintf(ff,"%s:%s\n",gettext("Номер запроса"),nom_zapr.ravno());
fprintf(ff,"\
---------------------------------------------------------------------\n");
fprintf(ff,"\
  N   |Номер документа|Дата выписки|Вид док.|Индивидуальный нал.ном.|\n");
/*******
123456 123456789012345 123456789012 12345678   
***********/
fprintf(ff,"\
---------------------------------------------------------------------\n");


int nomer_str=0;
cur->poz_cursor(0);
class iceb_tu_str vid_dok("");
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirrnn1_poi_prov1(row,poi) != 0)
   continue;

  nom_dok.new_plus("");
  nom_dok1.new_plus("");
  nom_dok2.new_plus("");
    
  if(polen(row[2],&nom_dok,1,'/') != 0)
   nom_dok.new_plus(row[2]);
  else
   {
    polen(row[2],&stroka,2,'/');
    /*Если первый символ после косой не ноль то это код вида деятельности*/
    sprintf(bros,"%*.*s",iceb_tu_kolbait(1,stroka.ravno()),iceb_tu_kolbait(1,stroka.ravno()),stroka.ravno());
    if(atoi(bros) > 0)
     {
      nom_dok1.new_plus(bros);
      nom_dok2.new_plus(iceb_tu_adrsimv(1,stroka.ravno()));
     }    
    else
     nom_dok2.new_plus(stroka.ravno());
   }
  rsdat(&dv,&mv,&gv,row[21],2);
  if(metka == 0)
    vid_dok.new_plus(row[22]);
  else 
    vid_dok.new_plus(row[22],2);


  nomer_str++;
      
  fprintf(ff_xml,"   <T1RXXXXG21 ROWNUM=\"%d\">%s</T1RXXXXG21>\n",nomer_str,nom_dok.ravno()); 
  if(nom_dok1.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG22 ROWNUM=\"%d\">%s</T1RXXXXG22>\n",nomer_str,nom_dok1.ravno()); 
  if(nom_dok2.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG23 ROWNUM=\"%d\">%s</T1RXXXXG23>\n",nomer_str,nom_dok2.ravno()); 
 
  fprintf(ff_xml,"   <T1RXXXXG3D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG3D>\n",nomer_str,dv,mv,gv); 
 
  fprintf(ff_xml,"   <T1RXXXXG4S ROWNUM=\"%d\">%s</T1RXXXXG4S>\n",nomer_str,vid_dok.ravno()); 
 
  fprintf(ff_xml,"   <T1RXXXXG5 ROWNUM=\"%d\">%s</T1RXXXXG5>\n",nomer_str,row[11]); 
   
  fprintf(ff,"%6d %-*s %02d.%02d.%04d   %-*s %-*s %s\n",
  nomer_str,
  iceb_tu_kolbait(15,row[2]),row[2],
  dv,mv,gv,
  iceb_tu_kolbait(8,vid_dok.ravno()),vid_dok.ravno(),
  iceb_tu_kolbait(12,row[11]),row[11],
  row[12]);
  
 } 

fprintf(ff,"\
---------------------------------------------------------------------\n");


class iceb_t_fioruk_rk rukov;
iceb_t_fioruk(1,&rukov);
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

fclose(ff_xml);
podpis(ff);
fclose(ff);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Запрос на получение сведений из Единого реестра налоговых накладных"));

iceb_t_ustpeh(imaf,3);

oth.spis_imaf.plus(imaf_xml);
oth.spis_naim.plus(gettext("Запрос на получение сведений из Единого реестра налоговых накладных в формате xml"));

iceb_t_rabfil(&oth,"");

}
/******************************************************************/
/*распечатка реестра с объединением сумм в разных колонках в одну колонку*/
/***********************************************************************/
void dirrnn1_svod(class SQLCURSOR *cur,int kolstr,class xrnn_poi *poi)
{
SQL_str row;
if(kolstr == 0)
 return;

char imaf[64];
sprintf(imaf,"rnnsvod%d",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Реестр полученных налоговых накладных для проверки"),poi->dn,poi->mn,poi->gn,poi->dk,poi->mk,poi->gk,fil.ff);

cur->poz_cursor(0);
class iceb_tu_str vid_dok("");
int kolstr1=0;
int nomer_str=0;
double itogo[3];
memset(itogo,'\0',sizeof(itogo));

fprintf(fil.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirrnn1_poi_prov1(row,poi) != 0)
   continue;
  class iceb_tu_str data_pol(iceb_tu_datzap(row[1]));
  class iceb_tu_str data_vip(iceb_tu_datzap(row[21]));
  
  double suma_podok=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);
  double suma_beznds=suma_podok-atof(row[4])-atof(row[6])-atof(row[8])-atof(row[10]);
  double suma_nds=atof(row[4])+atof(row[6])+atof(row[8])+atof(row[10]);
  itogo[0]+=suma_podok;
  itogo[1]+=suma_beznds;
  itogo[2]+=suma_nds;
  
  fprintf(fil.ff,"|%-5d|%10s|%10s|%-*s|%-*s|%-*.*s|%-*s|%10.2f|%10.2f|%10.2f|\n",
  ++nomer_str,
  data_pol.ravno(),
  data_vip.ravno(),
  iceb_tu_kolbait(10,row[2]),
  row[2],
  iceb_tu_kolbait(6,row[22]),
  row[22],
  iceb_tu_kolbait(30,row[12]),
  iceb_tu_kolbait(30,row[12]),
  row[12],
  iceb_tu_kolbait(12,row[11]),
  row[11],
  suma_podok,
  suma_beznds,
  suma_nds);

 }
fprintf(fil.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s|%10.2f|%10.2f|%10.2f|\n",
iceb_tu_kolbait(90,gettext("Итого")),
gettext("Итого"),
itogo[0],
itogo[1],
itogo[2]);


podpis(fil.ff);

fil.end();  

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Реестр полученных налоговых накладных для проверки"));

iceb_t_ustpeh(imaf,3);


iceb_t_rabfil(&oth,"");

}

/*************/
/*распечатки*/
/***************/
void dirrnn1_ras(class SQLCURSOR *cur,int kolstr,class xrnn_poi *poi)
{



class spis_oth oth;
int		kom;

VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Реестр полученных налоговых накладных"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать запрос на получение сведений из Единого реестра налоговых накладных"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Реестр для проверки"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0 :
    poi->pr_ras.new_plus("+");
    oth.clear();
    if(rasrnn_r(poi,&oth) == 0)
     iceb_t_rabfil(&oth,"");
    return;

  case 1:
   dirrnn1_zapros(cur,kolstr,poi);
   return;

  case 2:
   dirrnn1_svod(cur,kolstr,poi);
   return;
       
  default:
    kom=0;
    goto naz;
 }
}
