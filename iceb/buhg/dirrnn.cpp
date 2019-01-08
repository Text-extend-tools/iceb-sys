/*$Id: dirrnn.c,v 5.65 2014/07/31 07:09:16 sasa Exp $*/
/*12.02.2016	28.08.2005	Белых А.И.	dirrnn.c
Работа с реестром выданных налоговых накладных
*/
#include <ctype.h>
#include "buhg.h"
#include "xrnn_poi.h"

void sdirrnn(short,short,short,class xrnn_poi *poi);
int dirrnn_vvod(int nom_zap);
int dirrnn_poi(class xrnn_poi *poi);
int dirrnn_poi_prov(SQL_str row,class xrnn_poi *poi);
void dirrnn_udzap(SQLCURSOR *cur,int kolstr,class xrnn_poi *poi);
void dirrnn_pz(int unnomzap,class xrnn_poi *poi);
void dirrrn_xml(int nom_zap);
int dirrnn_provnnn(int metka_num_nn,short md,short gd,const char *nomnn);

extern iceb_tu_str kat_for_nal_nak; //Каталог для налоговых накладных
extern class iceb_rnfxml_data rek_zag_nn;

void dirrnn(short dnp,short mnp,short gnp)
{
class spis_oth oth;
short dnz=dnp,mnz=mnp,gnz=gnp; 
short dkz=0,mkz=0,gkz=0; 
short d,m,g;
short dd,md,gd;
char strsql[1024];
class iceb_t_mstr spis_menu(0);
int kolstr=0;
int K=0;
int kom=0;
int pozz=0,pozz1=0;
int i=0;
int		prc;
short           x=0,y=2;
static class xrnn_poi poi;
class iceb_tu_str sort("");
short metka_sort_dat=0; /*0-в порядке возростания дат 1-в порядке убывания дат*/
class iceb_tu_str dat_zap;
SQL_str row;
short		mspz; /*метка соответствия первой записи*/
poi.metka_sort=0;
poi.metka_poi=0;
poi.nomzap_v.free_class();

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

if(poi.metka_sort == 0)
 {
  if(metka_sort_dat == 0)
   sort.new_plus(" order by datd asc,nnni asc");
  else
   sort.new_plus(" order by datd desc,nnni asc");
 }
if(poi.metka_sort == 1)
 {
  if(metka_sort_dat == 0)
   sort.new_plus(" order by datd asc,dnz asc");
  else
   sort.new_plus(" order by datd desc,dnz asc");
  
 } 
sprintf(strsql,"select * from Reenn where datd >= '%04d-%02d-%02d'",gnz,mnz,dnz);


if(dnz != 0 && dkz != 0)
  sprintf(strsql,"select * from Reenn where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gnz,mnz,dnz,gkz,mkz,dkz);

strcat(strsql,sort.ravno());

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

  sdirrnn(dnp,mnp,gnp,&poi);

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
    if(dirrnn_poi_prov(row,&poi) != 0)
     continue;
    poi.suma[0]+=atof(row[3]);
    poi.suma[1]+=atof(row[4]);
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


for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(dirrnn_poi_prov(row,&poi) != 0)
   continue;

  rsdat(&d,&m,&g,row[1],2);  
  
  nom_zap[i]=atoi(row[0]);

  if(poi.nomzap_v.find(nom_zap[i]) >= 0)
   dat_zap.new_plus("*");
  else
   dat_zap.new_plus("");

  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  dat_zap.plus(iceb_tu_datzap(row[1]));
  
  if(poi.metka_sort == 0)
   sprintf(strsql,"%s|%-*s|%-*s|%8.2f|%7.2f|%s|%-*s|%s",
   dat_zap.ravno(),
   iceb_tu_kolbait(8,row[2]),row[2],
   iceb_tu_kolbait(3,row[16]),row[16],
   atof(row[3]),
   atof(row[4]),
   row[8],
   iceb_tu_kolbait(3,row[15]),row[15],
   row[7]);
  else
   sprintf(strsql,"%s|%-3s|%-*s|%-*s|%8.2f|%7.2f|%s|%-*s|%s",
   dat_zap.ravno(),
   row[18],
   iceb_tu_kolbait(8,row[2]),row[2],
   iceb_tu_kolbait(3,row[16]),row[16],
   atof(row[3]),
   atof(row[4]),
   row[8],
   iceb_tu_kolbait(3,row[15]),row[15],
   row[7]);
  
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

sdirrnn(dnp,mnp,gnp,&poi);

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


    if(dirrnn_poi_prov(row,&poi) != 0)
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


    if(dirrnn_poi_prov(row,&poi) != 0)
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

    if(dirrnn_poi_prov(row,&poi) != 0)
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

  polen(spis_menu.spis_str.ravno(kom),strsql,sizeof(strsql),1,'|');
  rsdat(&dd,&md,&gd,strsql,1);
 }

vper:;

switch(K)
 {  

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("rnn1_1.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case PLU:  //Ввод новой записи
  case FK2:  //Ввод новой записи
     dirrnn_vvod(0);
     goto naz1;

  case ENTER:
  case SFK2:  //Корректировка записи
    dirrnn_vvod(nom_zap[kom]);
    goto naz1;

  case FK3:  //Удалить запись
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    sprintf(strsql,"delete from Reenn where nz=%d",nom_zap[kom]);
    sql_zapis(strsql,0,0);
    goto naz1;

  case SFK3:  //Удалить все записи в соответствии с условиями поиска
    dirrnn_udzap(&cur,kolstr,&poi);
    goto naz1;


  case FK4:  //Поиск
    if(dirrnn_poi(&poi) == 0)
     {
      dnz=poi.dn;
      mnz=poi.mn;
      gnz=poi.gn;

      dkz=poi.dk;
      mkz=poi.mk;
      gkz=poi.gk;
      poi.kodgr1.new_plus(poi.kodgr.ravno());  //для распечатки
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

  case FK5: //Распечатка
    poi.pr_ras.new_plus("-");
    oth.clear();

    /*Для налоговых накладных нужно определить тип периода для этого нужны две даты*/
    if(poi.dn != 0 && poi.dk == 0)
     {
      poi.dk=31;
      poi.mk=poi.mn;
      poi.gk=poi.gn;
      dpm(&poi.dk,&poi.mk,&poi.gk,5); //установить правильно последний день месяца
     }

    if(rasrnn_r(&poi,&oth) == 0)
     iceb_t_rabfil(&oth,"");

    goto naz;

  case FK6: //импорт документов

    ree_imp(2);
    goto naz1;

  case FK7: /*получение налоговой накладной в xml формате*/
    dirrrn_xml(nom_zap[kom]);
    goto naz;

  case FK8: /*Переставить запись*/
    dirrnn_pz(nom_zap[kom],&poi);
    goto naz1;

  case FK9: /*Переключение сортировки*/
    poi.metka_sort++;
    if(poi.metka_sort > 1)
     poi.metka_sort=0;
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

void sdirrnn(short dp,short mp,short gp, //Дата начала просмотра реестра
class xrnn_poi *poi)
{
int		i1;
int		X;

clear();

printw("%s",gettext("Реестр выданных налоговых накладных"));
if(dp != 0)
 printw(" %02d.%02d.%d",dp,mp,gp);

if(poi->metka_sort == 0)
 {
  printw(" %s",gettext("Cортировка по номерам накладных"));
  mvprintw(1,0,"      1         2      3     4       5     6   7    8");
  mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));
  mvprintw(++i1,X,"1 %s",gettext("дата документа"));
  mvprintw(++i1,X,"2 %s",gettext("номер документа"));
  mvprintw(++i1,X,"3 %s",gettext("вид документа"));
  mvprintw(++i1,X,"4 %s",gettext("сумма по док."));
  mvprintw(++i1,X,"5 %s",gettext("сумма НДС"));
  mvprintw(++i1,X,"6 %s",gettext("метка"));
  mvprintw(++i1,X,"7 %s",gettext("код оп-ции"));
  mvprintw(++i1,X,"8 %s",gettext("контрагент"));
 }
else
 {
  printw(" %s",gettext("Cортировка по номерам записей"));
 
  mvprintw(1,0,"      1       2      3     4       5      6    7  8     9");

  mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));
  mvprintw(++i1,X,"1 %s",gettext("дата документа"));
  mvprintw(++i1,X,"2 %s",gettext("номер записи"));
  mvprintw(++i1,X,"3 %s",gettext("номер документа"));
  mvprintw(++i1,X,"4 %s",gettext("вид документа"));
  mvprintw(++i1,X,"5 %s",gettext("сумма по док."));
  mvprintw(++i1,X,"6 %s",gettext("сумма НДС"));
  mvprintw(++i1,X,"7 %s",gettext("метка"));
  mvprintw(++i1,X,"8 %s",gettext("код оп-ции"));
  mvprintw(++i1,X,"9 %s",gettext("контрагент"));
 }
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
"F7",gettext("нал.нак."),
"F8",gettext("переставить"),
"F9",gettext("сортировка"),
"F10",gettext("вых."),NULL);

}
/******************************************/
/*Ввод и коррректировка*/
/*******************************************/

int dirrnn_vvod(int nom_zap)
{
int N=0;
int K=0;
class iceb_tu_str nomdi("");
char strsql[512];
SQL_str row;
SQLCURSOR cur;
short dd,md,gd;
short ddp=0,mdp=0,gdp=0;
int metka_dok=0;
int metka_pods=0;
short mz=0,gz=0;

double   nds1=20; /*Н.Д.С.*/
int metka_num_nn=0; /*0-годовая нумерация 1-месячная*/


if(iceb_t_poldan("Нумерация налоговых накладных обновляется каждый месяц",strsql,"matnast.alx") == 0)
 {
  
  if(SRAV(strsql,"Вкл",1) == 0)
   {
    metka_num_nn=1;
   }
 }


VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(124);//5
VV.VVOD_SPISOK_add_data(100);//6
VV.VVOD_SPISOK_add_data(100);//7
VV.VVOD_SPISOK_add_data(10);//8
VV.VVOD_SPISOK_add_data(10);//9
VV.VVOD_SPISOK_add_data(8);//10
VV.VVOD_SPISOK_add_data(11);//11
VV.VVOD_SPISOK_add_data(11);//12
VV.VVOD_SPISOK_add_data(20);//13




if(nom_zap == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

  //читаем запись которую надо корректировать
  sprintf(strsql,"select * from Reenn where nz=%d",nom_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    soobshw(&SOOB,stdscr,-1,-1,1,1);
    return(1);
   }

  VV.data_plus(0,row[2]);
  
  rsdat(&dd,&md,&gd,row[1],2);
//  dz=dd;
  mz=md;
  gz=gd;
  
  sprintf(strsql,"%d.%d.%d",dd,md,gd);
  VV.data_plus(1,strsql);
  VV.data_plus(2,row[3]);
  VV.data_plus(3,row[4]);
  VV.data_plus(4,row[6]);
  VV.data_plus(5,row[7]);
  metka_dok=atoi(row[5]);
  rsdat(&ddp,&mdp,&gdp,row[9],2);

  VV.data_plus(7,row[11]);
  VV.data_plus(8,row[14]);

  kzvz(atoi(row[12]),atol(row[13]),&VV);

  metka_pods=atoi(row[8]);
  nomdi.new_plus(row[10]);

  VV.data_plus(9,row[15]);
  VV.data_plus(10,row[16]);
  VV.data_plus(11,iceb_tu_datzap(row[17]));
  
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

    sprintf(strsql,"%d.%d.%d %s",ddp,mdp,gdp,nomdi.ravno());
    VV.VVOD_SPISOK_add_ZAG(strsql);

    if(row[15][0] != '\0') //код операции в документе        
     {
      if(metka_pods == 1)
       sprintf(strsql,"select naik from Rashod where kod='%s'",row[15]);
      if(metka_pods == 2)
       sprintf(strsql,"select naik from Usloper2 where kod='%s'",row[15]);
      if(metka_pods == 3)
       sprintf(strsql,"select naik from Uosras where kod='%s'",row[15]);
      if(metka_pods == 4)
       sprintf(strsql,"select naik from Ukrvkr where kod='%s'",row[201]);

      SQL_str row1;
      class SQLCURSOR cur1;

      if(readkey(strsql,&row1,&cur1) == 1)
       {
        sprintf(strsql,"%s:%s %s",gettext("Код операции"),row[15],row1[0]);
        VV.VVOD_SPISOK_add_ZAG(strsql);
        
       }
     }
        
   }
  else
   {
    if(ddp != 0)
     {
      sprintf(strsql,"%d.%d.%d",ddp,mdp,gdp);
      VV.data_plus(12,strsql);
     }
    VV.data_plus(13,row[10]);
   }
 }

VV.VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи налоговой накладной....."));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма по документу.................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Сумма НДС..........................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер......"));//4
VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента............"));//5
VV.VVOD_SPISOK_add_MD(gettext("Метка документа....................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.........................."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код группы документа................"));//8
VV.VVOD_SPISOK_add_MD(gettext("Код операции........................"));//9
VV.VVOD_SPISOK_add_MD(gettext("Вид документа......................."));//10
VV.VVOD_SPISOK_add_MD(gettext("Дата выписки налоговой накладной...."));//11
if(metka_pods == 0)
 {
  VV.VVOD_SPISOK_add_MD(gettext("Дата документа......................"));//12
  VV.VVOD_SPISOK_add_MD(gettext("Номер документа....................."));//13
 }
 

naz:;

//clear();
if(metka_dok == 0)
 {
  sprintf(strsql,gettext("Подлежит налогообложению по ставке %.f%%"),nds1);
  VV.data_plus(6,strsql);
 }

if(metka_dok == 1)
  VV.data_plus(6,gettext("Подлежит налогообложению по ставке 0%%"));

if(metka_dok == 2)
  VV.data_plus(6,gettext("Освобождено от налогообложения"));

if(metka_dok == 3)
  VV.data_plus(6,gettext("Экспорт товаров и сопутствующих услуг"));
    

helstr(LINES-1,0,"F2/+",gettext("запись"),
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
   {
//    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);
    
    if(N == 6)  //метка документа
     {
      VVOD MENU(3);
      sprintf(strsql,gettext("Подлежит налогообложению по ставке %.f%%"),nds1);
      MENU.VVOD_SPISOK_add_MD(strsql);

      MENU.VVOD_SPISOK_add_MD(gettext("Подлежит налогообложению по ставке 0%%"));
      MENU.VVOD_SPISOK_add_MD(gettext("Освобождено от налогообложения"));
      MENU.VVOD_SPISOK_add_MD(gettext("Экспорт товаров и сопутствующих услуг"));

      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


      while(menu3w(stdscr,&MENU,&metka_dok,-1,-1,0) != 0);

      N++;
      goto naz;
     }

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Номер налоговой накладной*/
     {
      if(rsdat(&dd,&md,&gd,VV.data_ravno(1),1) == 0 && VV.data_ravno(0)[0] != '\0')
       {
        dirrnn_provnnn(metka_num_nn,md,gd,VV.data_ravno(0));
       }
     }
    if(N == 1) /*Дата получения налоговой накладной*/
     {
      /*Проверяем может в этом году уже был такой номер*/
      if(rsdat(&dd,&md,&gd,VV.data_ravno(1),1) == 0 && VV.data_ravno(0)[0] != '\0')
       {
        dirrnn_provnnn(metka_num_nn,md,gd,VV.data_ravno(0));

       }
     }
    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //наименование контрагента
     {
      if(isdigit(VV.VVOD_SPISOK_return_data(N)[0]) != 0)  /*Значит символ число*/
       {
        sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%d'",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          if(row[2][0] == '\0')
            VV.data_plus(N,row[0]);
          else
            VV.data_plus(N,row[2]);
          
          VV.data_plus(4,row[1]);
          goto naz;
         }
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

    if(rsdat(&dd,&md,&gd,VV.data_ravno(1),1) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата выдачи налоговой накладной !"));
      goto naz;
     }
    if(VV.prov_dat(11,1) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата выписки!"));
      goto naz;
     }   
    if(iceb_t_pbpds(md,gd) != 0)
     goto naz;
    if(nom_zap != 0)
     if(iceb_t_pbpds(mz,gz) != 0)
      goto naz;
   
    if(metka_pods == 0 )
     {
      if(VV.data_ravno(12)[0] != '\0')
      if(rsdat(&ddp,&mdp,&gdp,VV.data_ravno(12),1) != 0)
       {
        sprintf(strsql,gettext("Неправильно введена дата документа !"));
        iceb_t_soob(strsql);
        goto naz;
       }
      nomdi.new_plus(VV.data_ravno(13));
     }

    char naim_kontr[VV.data_dlinna(5)*2+1];
    strcpy(naim_kontr,VV.data_ravno(5));
    sqlfiltr(naim_kontr,sizeof(naim_kontr));

    char koment[VV.data_dlinna(7)*2+1];
    strcpy(koment,VV.data_ravno(7));
    sqlfiltr(koment,sizeof(koment));

    char kodgr[VV.data_dlinna(8)*2+1];
    strcpy(kodgr,VV.data_ravno(8));
    sqlfiltr(kodgr,sizeof(kodgr));
    if(kodgr[0] != '\0')
     {
      sprintf(strsql,"select kod from Reegrup2 where kod='%s'",kodgr);
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
      dirrnn_provnnn(metka_num_nn,md,gd,VV.data_ravno(0));
      
      
      if(zapvree(dd,md,gd,VV.data_ravno(0),
      VV.data_ravno_atof(2),
      VV.data_ravno_atof(3),
      metka_dok,
      VV.data_ravno(4),
      naim_kontr,
      metka_pods,
      VV.ravno_sqldat(12),
      nomdi.ravno(),
      koment,
      kodgr,VV.data_ravno(9),VV.data_ravno(10),VV.ravno_sqldat(11)) != 0)
       goto naz;

     }
    else
     {
      sprintf(strsql,"update Reenn set \
datd='%04d-%02d-%02d',\
nnn='%s',\
sumd=%.2f,\
snds=%.2f,\
mt=%d,\
inn='%s',\
nkontr='%s',\
datdi='%04d-%02d-%02d',\
nomdi='%s',\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
gr='%s',\
ko='%s',\
vd='%s',\
datv='%s',\
nnni=%d \
where nz=%d",
      gd,md,dd,
      VV.data_ravno(0),
      VV.data_ravno_atof(2),
      VV.data_ravno_atof(3),
      metka_dok,
      VV.data_ravno(4),
      naim_kontr,
      gdp,mdp,ddp,
      nomdi.ravno(),
      koment,
      iceb_t_getuid(),
      vrem,
      kodgr,
      VV.data_ravno(9),
      VV.data_ravno(10),
      VV.ravno_sqldat(11),
      VV.data_ravno_atoi(0),
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
    class iceb_tu_str kodn("");
    class iceb_tu_str naimn("");


    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр контрагентов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп документов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Материальный учёт\""));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт основных средств\""));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций подсистемы \"Учёт услуг\""));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    class iceb_tu_str kodp("");
    class iceb_tu_str naimp("");
    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;


      case 0 :
        naimn.new_plus("");
        
        if(dirkontr(&kodn,&naimn,1) == 0)
         {
          VV.data_plus(5,iceb_t_get_pnk(kodn.ravno(),1));
          //Читаем индивидуальный налоговый номер
          sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kodn.ravno());
          if(readkey(strsql,&row,&cur) == 1)
           VV.data_plus(4,row[0]);
         }
        goto naz;

      case 1 :
        vibrek("Reegrup2",&kodn);

        VV.VVOD_SPISOK_zapis_data(8,kodn.ravno());
        goto naz;

      case 2 :
        if(dirrashod(1,&kodp,&naimp) == 0)
          VV.data_z_plus(9,kodp.ravno());
        break;

      case 3 :
        if(diruosras(1,&kodp,&naimp) == 0)
          VV.data_plus(9,kodp.ravno());
        break;

      case 4 :
        if(diruslop2(1,&kodp,&naimp) == 0)
          VV.data_plus(9,kodp.ravno());
        break;

     }





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

int dirrnn_poi(class xrnn_poi *poi)
{
char strsql[1024];
int N=0,K=0;


int kom1=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной.(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер.."));//3
VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента........"));//4
VV.VVOD_SPISOK_add_MD(gettext("Номер документа...........(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......................"));//6
VV.VVOD_SPISOK_add_MD(gettext("Код группы документа (,,)......."));//7
VV.VVOD_SPISOK_add_MD(gettext("Подсистема...........(,,)......."));//8
VV.VVOD_SPISOK_add_MD(gettext("Код операции.........(,,)......."));//9
VV.VVOD_SPISOK_add_MD(gettext("Вид документа........(,,)......."));//10
VV.VVOD_SPISOK_add_MD(gettext("Отметка........................."));//11

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
VV.VVOD_SPISOK_add_data(poi->inn.ravno(),128);//3
VV.VVOD_SPISOK_add_data(poi->naim_kontr.ravno(),128);//4
VV.VVOD_SPISOK_add_data(poi->nomdok.ravno(),128);//5
VV.VVOD_SPISOK_add_data(poi->koment.ravno(),128);//6
VV.VVOD_SPISOK_add_data(poi->kodgr.ravno(),128);//7
VV.VVOD_SPISOK_add_data(poi->podsistema.ravno(),128);//8
VV.VVOD_SPISOK_add_data(poi->kodop.ravno(),128);//9
VV.VVOD_SPISOK_add_data(poi->viddok.ravno(),112);//10
VV.VVOD_SPISOK_add_data(128);//11

naz:;
if(poi->metka_ins == 0)
 strcpy(strsql,gettext("Все документы"));
if(poi->metka_ins == 1)
 strcpy(strsql,gettext("Только отмеченные документы"));
if(poi->metka_ins == 2)
 strcpy(strsql,gettext("Только не отмеченные документы"));

VV.data_plus(11,strsql);//13

 
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

    if(N == 11)
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
/**********
        sprintf(strsql,"select naikon from Kontragent where kodkon='%d'",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(N,row[0]);
          goto naz;
         }
**************/
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
    if(poi->dn != 0 && poi->dk == 0)
     {
      poi->dk=31;
      poi->mk=poi->mn;
      poi->gk=poi->gn;
      dpm(&poi->dk,&poi->mk,&poi->gk,5); //установить правильно последний день месяца
     }
    poi->nnn.new_plus(VV.VVOD_SPISOK_return_data(2));
    poi->inn.new_plus(VV.VVOD_SPISOK_return_data(3));
    poi->naim_kontr.new_plus(VV.VVOD_SPISOK_return_data(4));
    poi->nomdok.new_plus(VV.VVOD_SPISOK_return_data(5));
    poi->koment.new_plus(VV.VVOD_SPISOK_return_data(6));
    poi->kodgr.new_plus(VV.VVOD_SPISOK_return_data(7));
    poi->podsistema.new_plus(VV.VVOD_SPISOK_return_data(8));
    poi->kodop.new_plus(VV.VVOD_SPISOK_return_data(9));
    poi->viddok.new_plus(VV.data_ravno(10));
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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    class iceb_tu_str kod("");
    class iceb_tu_str naim("");
    switch (kom1)
     {
      case 6 :
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
        if(dirrashod(1,&kod,&naim) == 0)
          VV.data_z_plus(9,kod.ravno());
        break;

      case 3 :
//        vibrek("Uosras",&poi->kodop);
        if(diruosras(1,&kod,&naim) == 0)
          VV.data_z_plus(9,kod.ravno());
        break;

      case 4 :
//        vibrek("Usloper2",&poi->kodop);
        if(diruslop2(1,&kod,&naim) == 0)
          VV.data_z_plus(9,kod.ravno());
        break;

      case 5 :
      
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
int dirrnn_poi_prov(SQL_str row,class xrnn_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);
 
if(poi->podsistema.ravno()[0] != '\0')
 {
  class iceb_tu_str metka_pod("");

  if(row[8][0] == '1')
   metka_pod.new_plus(ICEB_MP_MATU);
  if(row[8][0] == '2')
   metka_pod.new_plus(ICEB_MP_USLUGI);
  if(row[8][0] == '3')
   metka_pod.new_plus(ICEB_MP_UOS);
  if(row[8][0] == '4')
   metka_pod.new_plus(ICEB_MP_UKR);
  if(row[8][0] == '5')
   metka_pod.new_plus(gettext("ГК"));
  if(proverka(poi->podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
   return(1); 
 }
 
if(proverka(poi->nnn.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(poi->viddok.ravno(),row[16],0,0) != 0)
 return(1);

if(proverka(poi->nomdok.ravno(),row[10],0,0) != 0)
 return(1);

if(proverka(poi->kodgr.ravno(),row[14],0,0) != 0)
 return(1);

if(proverka(poi->kodop.ravno(),row[15],0,0) != 0)
 return(1);

/**********
if(poi->inn.ravno()[0] != '\0' )
  if(strstrm(row[6],poi->inn.ravno()) == 0)
    return(1);
if(poi->koment.ravno()[0] != '\0' )
  if(strstrm(row[11],poi->koment.ravno()) == 0)
    return(1);
*****************/
if(proverka(poi->inn.ravno(),row[6],4,0) != 0)
 return(1);
 
if(proverka(poi->koment.ravno(),row[11],4,0) != 0)
 return(1);
if(proverka(poi->naim_kontr.ravno(),row[7],4,0) != 0)
 return(1); 

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

void dirrnn_udzap(SQLCURSOR *cur,int kolstr,class xrnn_poi *poi)
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
char strsql[200];
cur->poz_cursor(0);
short d=0,m=0,g=0;
short mz=0,gz=0;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirrnn_poi_prov(row,poi) != 0)
   continue;

  rsdat(&d,&m,&g,row[1],2);
  
  if(mz != m || gz != g)
   {
    if(iceb_t_pbpds(m,g) != 0)
     return;
    mz=m;
    gz=g;
   }  
  
  sprintf(strsql,"delete from Reenn where nz=%s",row[0]);
  sql_zapis(strsql,1,0);

 } 
}
/********************************/
/*Получение налоговой накладной в формате xml*/
/********************************/
void dirrrn_xml(int nom_zap)
{
char strsql[512];
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select * from Reenn where nz=%d",nom_zap);
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не нашли запись в реестре!"));
  return;
 }
int metka_pods=atoi(row[8]);
class iceb_tu_str vid_dok(row[16]);
if(metka_pods == 0)
 {
  iceb_t_soob(gettext("Запись введена в реестр вручную!"));
  return;
 }

if(metka_pods == 4)
 {
  iceb_t_soob(gettext("Запись импортирована из подсистемы \"Учёт командировочных расходов\"!"));
  return;
 }

if(metka_pods == 5)
 {
  iceb_t_soob(gettext("Запись импортирована из подсистемы \"Главная книга\"!"));
  return;
 }

static int pnd=0;  /*порядковый номер документа*/
class iceb_tu_str por_nor_dok;
por_nor_dok.plus(pnd+1);
class VVOD VVOD1(2);
int kom1=0;
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите порядковый номер документа"));

if((kom1=vvod1(&por_nor_dok,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

pnd=por_nor_dok.ravno_atoi()-1; /*Отнимаем потому что увеличение номера выполняется при формировании документа*/

class iceb_tu_str imaf_xml("");

iceb_t_rnfxml(&rek_zag_nn); /*читаем данных для xml файлов*/

short dd,md,gd;
rsdat(&dd,&md,&gd,row[9],2);
kat_for_nal_nak.new_plus(""); //Каталог для налоговых накладных

if(metka_pods == 1)
 if(xml_nn_mat(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,vid_dok.ravno(),&imaf_xml) != 0)
  return;

if(metka_pods == 2)
 if(xml_nn_usl(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,vid_dok.ravno(),&imaf_xml) != 0)
  return;

if(metka_pods == 3)
  if(xml_nn_uos(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,vid_dok.ravno(),&imaf_xml) != 0)
   return;

class spis_oth oth;   
oth.spis_imaf.plus(imaf_xml.ravno());
oth.spis_naim.plus(gettext("Налоговая накладная в xml формате"));

iceb_t_rabfil(&oth,"");


}
/****************************/
/*перестановка записи в пределах одного дня*/
/********************************************/

void dirrnn_pz(int unnomzap,
class xrnn_poi *poi)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int dnz=0,dnz1=0;
short dz=0,mz=0,gz=0;
int unomz=0;

if(poi->metka_sort == 0)
 {
  iceb_t_soob(gettext("Включите сортировку по номерам записей!"));
  return;
 }





int metka_op=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Переместить вверх"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Переместить вниз"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

if(menu3w(stdscr,&MENU,&metka_op,-1,-1,0) != 0);
switch (metka_op)
 {
  case 2:
  case -1 :
    return;

  case 0:
   break;

  case 1:
   break;

 }




sprintf(strsql,"select datd,dnz from Reenn where nz=%d",unnomzap);
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найдена запись которую хотим переместить!"));
  return;
 }

dnz=atoi(row[1]);
rsdat(&dz,&mz,&gz,row[0],2);

if(metka_op == 0) /*вверх*/
 {
  if(dnz == 0)
   {
    iceb_t_soob(gettext("Выше некуда!"));
    return;
   }  
  sprintf(strsql,"select nz,dnz from Reenn where datd='%04d-%02d-%02d' and dnz < %d order by dnz desc limit 1",gz,mz,dz,dnz);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Выше нет записей!"));
    return;
   }
  unomz=atoi(row[0]);
  dnz1=atoi(row[1]);      
  /*меняем записи местами*/
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz1,unnomzap);
  sql_zapis(strsql,1,0);
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz,unomz);
  sql_zapis(strsql,1,0);

 }

if(metka_op == 1) /*вниз*/
 {
  sprintf(strsql,"select nz,dnz from Reenn where datd='%04d-%02d-%02d' and dnz > %d order by dnz asc limit 1",gz,mz,dz,dnz);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Ниже нет записей!"));
    return;
   }
  unomz=atoi(row[0]);
  dnz1=atoi(row[1]);      

  /*меняем записи местами*/
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz1,unnomzap);
  sql_zapis(strsql,1,0);
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz,unomz);
  sql_zapis(strsql,1,0);

 }







}
/**************************************/
/*Проверка номера налоговой накладной*/
/*************************************/
int dirrnn_provnnn(int metka_num_nn,short md,short gd,const char *nomnn)
{
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
if(metka_num_nn == 0)
  sprintf(strsql,"select datd from Reenn where datd >= '%04d-01-01' and datd <= '%04d-12-31' and nnn='%s'",
  gd,gd,nomnn);    
else
  sprintf(strsql,"select datd from Reenn where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nnn='%s'",
  gd,md,gd,md,nomnn);    
           
if(readkey(strsql,&row,&cur) > 0)    
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s!",gettext("Внимание"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  if(metka_num_nn == 0)
    SOOB.VVOD_SPISOK_add_MD(gettext("Уже есть такой номер налоговой накладной в этом году"));
  else
     SOOB.VVOD_SPISOK_add_MD(gettext("Уже есть такой номер налоговой накладной в этом месяце"));
  short d,m,g;
  rsdat(&d,&m,&g,row[0],2);
  sprintf(strsql,"%d.%d.%d",d,m,g);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);

  return(1);
 }
return(0);
}
