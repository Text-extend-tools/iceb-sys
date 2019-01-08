/*$Id: dirdokummat4.c,v 5.27 2013/08/13 05:49:47 sasa Exp $*/
/*25.02.2017	22.02.2017	Белых А.И.	dirdokummat4.c
Ввод и корректирвка записей списания материалов на детали
*/
#include        <errno.h>
#include        "buhg.h"

class dirdokummat4_poi
 {
  public:

   class iceb_tu_str naim_kd;
   class iceb_tu_str naim_km;
   class iceb_tu_str kod_mat;
   class iceb_tu_str kod_det;
   short metka_poi;
      
   short dd,md,gd;
   int skl;
   class iceb_tu_str nomdok;
   class iceb_tu_str naim_skl;

  dirdokummat4_poi()
   {
    skl=0;
    dd=md=gd=0;
    nomdok.plus("");
    naim_skl.plus("");
    
    naim_kd.plus("");
    naim_km.plus("");
    kod_mat.plus("");
    kod_det.plus("");
    metka_poi=0; 
   }
 };
 
void dirdokummat4_vvod(int nomzap,class dirdokummat4_poi *poisk);
void dirdokummat4_sap(class dirdokummat4_poi*);
void	dirdokummat4_ras(SQLCURSOR *cur,class dirdokummat4_poi *poisk);
int dirdokummat4_poi(class dirdokummat4_poi *poisk);
int dirdokummat4_prov(SQL_str row,class dirdokummat4_poi *poisk);
int dirdokummat4_pps(class dirdokummat4_poi *poisk);



int dirdokummat4(short dd,short md,short gd,
int skl,
const char *nomdok)
{
char		strsql[2048];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class dirdokummat4_poi poisk;
class iceb_tu_str naim_kd("");
class iceb_tu_str naim_km("");
class SQLCURSOR cur,cur1;
class iceb_tu_str kodmat("");
class iceb_tu_str nomkart("");

poisk.skl=skl;
poisk.nomdok.new_plus(nomdok);
poisk.dd=dd;
poisk.md=md;
poisk.gd=gd;


sprintf(strsql,"select naik from Sklad where kod=%d",skl);
if(readkey(strsql,&row,&cur) == 1)
 poisk.naim_skl.new_plus(row[0]);


//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

int nom_zap[kls];

sprintf(strsql,"select km,nk,kd,kol,nz,kom from Dokummat4 where datd='%04d-%02d-%02d' and \
skl=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirdokummat4_sap(&poisk);
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

  if(dirdokummat4_prov(row,&poisk) != 0)
   continue;
  
  nom_zap[i]=atoi(row[4]);
  
  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_km.new_plus(row1[0]);
  else
   naim_km.new_plus("");
  
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kd.new_plus(row1[0]);
  else
   naim_kd.new_plus("");
   
  sprintf(strsql,"%-6s|%-6s|%-*.*s|%-6s|%-*.*s|%.6g",
  row[0],
  row[1],
  iceb_tu_kolbait(20,naim_km.ravno()),
  iceb_tu_kolbait(20,naim_km.ravno()),
  naim_km.ravno(),
  row[2],
  iceb_tu_kolbait(20,naim_kd.ravno()),
  iceb_tu_kolbait(20,naim_kd.ravno()),
  naim_kd.ravno(),  
  atof(row[3]));

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

dirdokummat4_sap(&poisk);

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
    if(dirdokummat4_prov(row,&poisk) != 0)
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
    if(dirdokummat4_prov(row,&poisk) != 0)
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
    if(dirdokummat4_prov(row,&poisk) != 0)
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

  polen(str_vib.ravno(),&kodmat,1,'|');
  polen(str_vib.ravno(),&nomkart,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     dirdokummat4_pps(&poisk);
     return(1);

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirdokummat4_vvod(nom_zap[kom],&poisk);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:

    if(iceb_t_pbpds(md,gd) != 0)
      goto naz;

    dirdokummat4_vvod(0,&poisk);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(md,gd) != 0)
      goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Dokummat4 where nz=%d",nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  case FK4:       /*Поиск*/

     dirdokummat4_poi(&poisk);

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirdokummat4_ras(&cur,&poisk);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**************************/
/*Шапка                   */
/**************************/

void	dirdokummat4_sap(class dirdokummat4_poi *poisk)
{
short		y=2;
short           x=COLS-20;

clear();



printw(gettext("Ввод и корректировка списания материалов на изделия"));
printw("\n%s:%02d.%02d.%04d %s:%s %s:%d %s",
gettext("Дата документа"),poisk->dd,poisk->md,poisk->gd,
gettext("Номер"),poisk->nomdok.ravno(),
gettext("Склад"),poisk->skl,poisk->naim_skl.ravno());
                         
mvprintw(2,0,"     1      2              3            4           5              6");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код материала"));
mvprintw(y++,x,"2-%s",gettext("номер карточки"));
mvprintw(y++,x,"3-%s",gettext("наименование мат."));
mvprintw(y++,x,"4-%s",gettext("код дет."));
mvprintw(y++,x,"5-%s",gettext("наименование дет."));
mvprintw(y++,x,"6-%s",gettext("количество"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));

  if(poisk->kod_mat.getdlinna() > 1)
   mvprintw(y++,x,"%s-%s",gettext("Код мат."),poisk->kod_mat.ravno());

  if(poisk->naim_km.getdlinna() > 1)
   mvprintw(y++,x,"%s-%s",gettext("Наим.мат."),poisk->naim_km.ravno());

  if(poisk->kod_det.getdlinna() > 1)
   mvprintw(y++,x,"%s-%s",gettext("Код дет."),poisk->kod_det.ravno());

  if(poisk->naim_kd.getdlinna() > 1)  
   mvprintw(y++,x,"%s-%s",gettext("Наим.дет."),poisk->naim_kd.ravno());

  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}

/**************************************/
/*получение списка для выбора материала*/
/***************************************/
int dirdokummat4_ps(short dd,short md,short gd,
int skl,
const char *nomdok,
class iceb_tu_str *kodmat,class iceb_tu_str *nomkart)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
char strsql[1024];
class SPISOK spm;
class iceb_tu_str naim("");


sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",gd,md,dd,skl,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("В документе нет ни одной записи!"));
  return(1);
 }
 
while(cur.read_cursor(&row) != 0)
 {
  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");


  sprintf(strsql,"%-6s|%-6s|%s",row[1],row[0],naim.ravno());  
  spm.plus(strsql);
 }

if(kolstr == 1)
 {
  polen(spm.ravno(0),kodmat,1,'|');
  polen(spm.ravno(0),nomkart,2,'|');
  return(0);  
 }

int kod_kl=0;

int voz=dirmasiv(&spm,-1,-1, 0,gettext("Выберите материал"),1,&kod_kl);

if(voz >= 0)
 {
  polen(spm.ravno(voz),kodmat,1,'|');
  polen(spm.ravno(voz),nomkart,2,'|');
 } 

return(voz);


}
/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int dirdokummat4_zap(class VVOD *VV,short dd,short md,short gd,
int skl,
const char *nomdok,
int kodmat,
int nomkart,
int nomzap)
{
char	strsql[2048];
SQL_str row;
class SQLCURSOR cur;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));


sprintf(strsql,"select naimat from Material where kodm=%d",VV->data_ravno_atoi(0));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код детали"),VV->data_ravno_atoi(0));
  iceb_t_soob(strsql);
  return(1);
 }

if(VV->data_ravno_atof(1) == 0.)
 {
  iceb_t_soob(gettext("Не введено количество!"));
  return(1);
 }

if(nomzap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Dokummat4 (datd,skl,nomd,km,nk,kd,kol,kom,ktoz,vrem) \
  values('%04d-%02d-%02d',%d,'%s',%d,%d,%d,%.6g,'%s',%d,%ld)",
  gd,md,dd,
  skl,
  nomdok,
  kodmat,
  nomkart,
  VV->data_ravno_atoi(0),
  VV->data_ravno_atof(1),
  VV->data_ravno_filtr(2),  
  iceb_t_getuid(),
  time(NULL));
else
  sprintf(strsql,"update Dokummat4 set \
kd=%d,\
kol=%.6f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",  
  VV->data_ravno_atoi(0),
  VV->data_ravno_atof(1),
  VV->data_ravno_filtr(2),  
  iceb_t_getuid(),
  time(NULL),
  nomzap);  

if(sql_zapis(strsql,0,0) != 0)
 return(1);


return(0);


}
/******************************/
/*Узнаём количество уже списанного материала*/
/************************************/
double dirdokummat4_ksm(int nomkart,class dirdokummat4_poi *poisk)
{
class SQLCURSOR cur;
char strsql[1024];
double kolih=0.;
int kolstr=0;
SQL_str row;

sprintf(strsql,"select kol from Dokummat4 where datd='%04d-%02d-%02d' and \
skl=%d and nomd='%s' and nk=%d",
poisk->gd,poisk->md,poisk->dd,poisk->skl,poisk->nomdok.ravno(),nomkart);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }


return(kolih);
}


/**********************/
/*Ввод и корректировка*/
/**********************/

void dirdokummat4_vvod(int nomzap,class dirdokummat4_poi *poisk)
{
char		strsql[2048];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class iceb_tu_str kodmat("");
class iceb_tu_str nomkart("");
class iceb_tu_str naim_km("");
class iceb_tu_str naim_kd("");
class SQLCURSOR cur;
double kol_spis_mat=0.;
double kol_spis_dok=0.;
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(60);//2


if(nomzap != 0)
 {

  sprintf(strsql,"select km,nk,kd,kol,kom,ktoz,vrem from Dokummat4 where nz=%d",nomzap);
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  kodmat.new_plus(row[0]);
  nomkart.new_plus(row[1]);  
  VV.VVOD_SPISOK_zapis_data(0,row[2]);

  sprintf(strsql,"%.6g",atof(row[3]));
  VV.VVOD_SPISOK_zapis_data(1,strsql);

  VV.VVOD_SPISOK_zapis_data(2,row[4]);
  ktozap=atoi(row[5]);
  vremz=atol(row[6]);

     
 }
else
 {

  if(dirdokummat4_ps(poisk->dd,poisk->md,poisk->gd,poisk->skl,poisk->nomdok.ravno(),&kodmat,&nomkart) < 0)
   return;

  /*узнаём количество списанного материала в карточке*/
  kol_spis_mat=readkolk(poisk->skl,nomkart.ravno_atoi(),poisk->dd,poisk->md,poisk->gd,poisk->nomdok.ravno());  

  if(kol_spis_mat == 0.)
   {
    iceb_t_soob(gettext("Материал не подтверждён!"));
    return;
   }

  VV.data_plus(1,kol_spis_mat);  

  /*узнаём количесво уже списанного*/
  kol_spis_dok=dirdokummat4_ksm(nomkart.ravno_atoi(),poisk);

  if(kol_spis_dok >= kol_spis_mat)
   {
    iceb_t_soob(gettext("Материал уже списан!"));
    return;
   }

  VV.data_plus(1,kol_spis_mat-kol_spis_dok);  

 }


/*узнаём наименование материала*/
sprintf(strsql,"select naimat from Material where kodm=%d",kodmat.ravno_atoi());
if(readkey(strsql,&row,&cur) == 1)
 naim_km.new_plus(row[0]);
else
 naim_km.new_plus("");

/*узнаём наименование детали*/
if(nomzap > 0)
 {
  sprintf(strsql,"select naimat from Material where kodm=%d",VV.data_ravno_atoi(0));
  if(readkey(strsql,&row,&cur) == 1)
   naim_kd.new_plus(row[0]);
  else
   naim_kd.new_plus("");
 }

VV.VVOD_SPISOK_add_MD(gettext("Код детали............"));//0
VV.VVOD_SPISOK_add_MD(gettext("Количество материала.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Коментарий............"));//2


naz:;

VV.VVOD_delete_ZAG();

if(nomzap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktozap,vremz,&VV);
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

sprintf(strsql,"%s:%d %s",gettext("Материал"),kodmat.ravno_atoi(),naim_km.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

if(naim_kd.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Деталь"),naim_kd.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 } 


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("детали"),
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



    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*код детали*/
     {
      sprintf(strsql,"select naimat from Material where kodm=%d",VV.data_ravno_atoi(N));
      if(readkey(strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %d",gettext("Не найден код детали"),VV.data_ravno_atoi(N));
        iceb_t_soob(strsql);
        naim_kd.new_plus("");
       }
      else
       {
        naim_kd.new_plus(row[0]);
        N++;
        goto naz;

       }      
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
    if(iceb_t_pbpds(poisk->md,poisk->gd) != 0)
      goto naz;
    if(nomzap == 0)
    if(VV.data_ravno_atof(1) > kol_spis_mat-kol_spis_dok)
     {
      iceb_t_soob(gettext("Количество больше чем можно списать!"));
      goto naz;
     }

    if(dirdokummat4_zap(&VV,poisk->dd,poisk->md,poisk->gd,poisk->skl,poisk->nomdok.ravno(),kodmat.ravno_atoi(),nomkart.ravno_atoi(),nomzap) != 0)
     goto naz;
    break;
   }

  if(K == FK3 || K == TAB) /*просмотр списка материалов*/
   {
    int kgr=0;
    int kodm=0;
    class iceb_tu_str naim("");
    if(dirmatr(&kgr,&kodm,&naim,1,0,"",0,0) == 0)
     {
      naim_kd.new_plus(naim.ravno());
      VV.data_plus(0,kodm);
     }
    clear();
    goto naz;

   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}




/********************/
/*Распечатать       */
/********************/

void dirdokummat4_ras(class SQLCURSOR *cur,class dirdokummat4_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
class iceb_tu_str naim_km("");
class iceb_tu_str naim_kd("");
char strsql[1024];
class iceb_tu_str ei("");

sprintf(imaf,"spis%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Ведомость списания материалов на детали"),ff);

fprintf(ff,"%s:%02d.%02d.%04d %s:%s %s:%d %s\n",
gettext("Дата документа"),poisk->dd,poisk->md,poisk->gd,
gettext("Номер"),poisk->nomdok.ravno(),
gettext("Склад"),poisk->skl,poisk->naim_skl.ravno());
if(poisk->metka_poi == 1)
 {
  if(poisk->kod_mat.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код материала"),poisk->kod_mat.ravno());

  if(poisk->naim_km.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование материала"),poisk->naim_km.ravno());

  if(poisk->kod_det.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код детали"),poisk->kod_det.ravno());

  if(poisk->naim_kd.getdlinna() > 1)  
   fprintf(ff,"%s:%s\n",gettext("Наименование детали"),poisk->naim_kd.ravno());
 }
 
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|Ном.к.|   Наименование материала     |Код д.|   Наименование детали        |Ед.изм.|Количество|\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(dirdokummat4_prov(row,poisk) != 0)
   continue;

  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_km.new_plus(row1[0]);
  else
   naim_km.new_plus("");
  
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kd.new_plus(row1[0]);
  else
   naim_kd.new_plus("");
   
  /*узнаём единицу измерения*/
  sprintf(strsql,"select ei from Kart where sklad=%d and nomk=%s",poisk->skl,row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   ei.new_plus(row1[0]);
  else
   ei.new_plus("");

  fprintf(ff,"%-6s|%-6s|%-*.*s|%-6s|%-*.*s|%*s|%10.6g|\n",
  row[0],
  row[1],
  iceb_tu_kolbait(30,naim_km.ravno()),
  iceb_tu_kolbait(30,naim_km.ravno()),
  naim_km.ravno(),
  row[2],
  iceb_tu_kolbait(30,naim_kd.ravno()),
  iceb_tu_kolbait(30,naim_kd.ravno()),
  naim_kd.ravno(),  
  iceb_tu_kolbait(7,ei.ravno()),
  ei.ravno(),  
  atof(row[3]));

   
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Ведомость списания материалов на детали"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/********************************************/
/*ввод реквизитов поиска*/
/*******************************************/
int dirdokummat4_poi(class dirdokummat4_poi *poisk)
{

class VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Код материала...........(,,).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование материала..(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код детали..............(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Наименование детали.....(,,).."));//3

VV.VVOD_SPISOK_add_data(poisk->kod_mat.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->naim_km.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->kod_det.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->naim_kd.ravno(),128);

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kl=VV.vvod(0,1,1,-1,-1);

switch(kl)
 {
  case FK10:
  case ESC:
    poisk->metka_poi=0;
    break;
    
  case FK2:
  case PLU:

   poisk->kod_mat.new_plus(VV.VVOD_SPISOK_return_data(0));
   poisk->naim_km.new_plus(VV.VVOD_SPISOK_return_data(1));
   poisk->kod_det.new_plus(VV.VVOD_SPISOK_return_data(2));
   poisk->naim_kd.new_plus(VV.VVOD_SPISOK_return_data(3));

   poisk->metka_poi=1;

   return(0);

 }
 
return(1);
}
/**************************************/
/*проверка строки*/
/*************************************/
int dirdokummat4_prov(SQL_str row,class dirdokummat4_poi *poisk)
{
SQL_str row1;
class SQLCURSOR cur1;
char strsql[1024];

if(proverka(poisk->kod_mat.ravno(),row[0],0,0) != 0)
 return(1);

if(proverka(poisk->kod_det.ravno(),row[2],0,0) != 0)
 return(1);

if(poisk->naim_km.getdlinna() > 1)
 {
  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   if(proverka(poisk->naim_km.ravno(),row1[0],4,0) != 0)
     return(1);

 }  
if(poisk->naim_kd.getdlinna() > 1)
 {
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   if(proverka(poisk->naim_kd.ravno(),row1[0],4,0) != 0)
     return(1);
 }
 
return(0);
}
/***********************************/
/*проверка полноты списания*/
/**********************************/
int dirdokummat4_pps(class dirdokummat4_poi *poisk)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
double kolih_pod=0.;
double kolih_dok=0.;
int voz=0;

sprintf(strsql,"select distinct nk from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s'",
poisk->gd,poisk->md,poisk->dd,poisk->skl,poisk->nomdok.ravno());


if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {

  sprintf(strsql,"select km,kol from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s' and nk=%s",
  poisk->gd,poisk->md,poisk->dd,poisk->skl,poisk->nomdok.ravno(),row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(1);
   }
  int kod_mat=0;
  kolih_dok=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    kod_mat=atoi(row1[0]);
    kolih_dok+=atof(row1[1]);
   }

  /*узнаём количество списанного материала в карточке*/
  kolih_pod=readkolk(poisk->skl,atoi(row[0]),poisk->dd,poisk->md,poisk->gd,poisk->nomdok.ravno());  

  if(kolih_dok < kolih_pod)
   {
    sprintf(strsql,"%s %d %s! %f < %f",gettext("Материал"),kod_mat,gettext("списан не полностью"),kolih_dok,kolih_pod);
    iceb_t_soob(strsql);
    voz=1;
   }
 }


return(voz);
}
