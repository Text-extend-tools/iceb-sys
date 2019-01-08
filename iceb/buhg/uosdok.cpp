/* $Id: uosdok.c,v 5.91 2013/09/26 09:43:41 sasa Exp $ */
/*18.05.2016    26.06.1996      Белых А.И.      uosdok.c
Просмотр выбранного документа
*/
#include <ctype.h>
#include        "buhg.h"
#include        "uos.h"
#include "poiinpd.h"
#include "bsizt.h"

void            podtv(int,short,short,short,short,const char*,short,short,int);
void		ush5(short,short,short,short,short,double,double,double,double);
int		vind(short,short,short,short,short);
void		menshd(VVOD *VV,short,short,short);
int             uduosdok(short,short,short,const char*);
int		uduoszd(int,short,short,short,const char*);
void            rasdokuos(short,short,short,int,const char*,int);
int           rasnalnuos(short,short,short,int,const char*,char[]);
int		invnomer(int);
void		rasactuos(short,short,short,const char*);
void	        actpp(short,short,short,const char*);
int pudok(int podd,short d,short m,short g,const char *ndk,short tz);
void opluos(short d,short m,short g,int tipz,const char *nn,const char *kodop);
void	poduosd(short,short,short,const char*);
void	uossas(short,short,short,const char*);
void	uosact(short,short,short,const char*);
void	uossos(short,short,short,const char*);
void	uosrem(short,short,short,const char*);
void impuosost(int tipz,short dd,short md,short gd,const char *nomdok,const char *kodop,int podr,int kodol);
void uosdok_f8(short dd,short md,short gd);
void uosvvd(int tipz,short dd,short md,short gd,const char *nomdok,int podr,int kodmol,const char *kodop,int podr1,int kodmol1,const char *nomdokp);
void uosrapp(short dd,short md,short gd,const char *nomdok);
void uosavp(short dd,short md,short gd,const char *nomdok);
void uosdok_r1(short dd,short md,short gd);
void uosdok_r2(short dd,short md,short gd);
int uosasoz818(short dd,short md,short gd,const char *nomdok);
int uosastz818(short dd,short md,short gd,const char *nomdok);
int uosavve818(short dd,short md,short gd,const char *nomdok);

class iceb_rnfxml_data rek_zag_nn;
short           mdd;  /*0-обычный документ 1-двойной*/
extern class UOS uos;
extern char	*bnds1,*bnds2,*bnds3;

void            uosdok(short dd,short md,short gd,class iceb_tu_str *ndk)
{
class iceb_tu_str imaf_nn_xml("");
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
class spis_oth oth;
class iceb_tu_str bros("");
short           i;
short           x=0,y=6;
short           prov; /*0-проводки выполнены 1 - нет*/
int             K;
int              kom;
double          bs=0.,iz=0.,bsby=0.,izby=0.;
long            in=0;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[1024];
class iceb_tu_str naim("");
time_t		vrem;
short		podt;
int		prc;
short		metkaprov=0; //0-проводки делать 1-нет
double		cenaprod=0.;
class iceb_tu_str shet_sp("");
class SPISOK shet_spis;
class iceb_t_mstr spis_menu(0);
int max_dl_naim=0;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);
VVOD MENU(3);
start:;

kom=pozz=0;

clear();
//printw("uosdok-%d.%d.%d %s\n",dd,md,gd,ndk);
GDITE();
/**************Читаем документ*/
sprintf(strsql,"select * from Uosdok where datd='%d-%d-%d' and \
nomd='%s'",gd,md,dd,ndk->ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ !"),ndk->ravno(),dd,md,gd,
  gettext("г."));
  iceb_t_soob(strsql);
  return;

 }

uos.nomdok.new_plus(ndk->ravno());
uos.tipz=atoi(row[1]);
if(uos.tipz == 1)
 uos.tipzv=2;
if(uos.tipz == 2)
 uos.tipzv=1;
sprintf(strsql,"%d.%d.%d",dd,md,gd);
uos.datpr.new_plus(strsql);
if(row[2] != NULL)
 uos.kprr.new_plus(row[2]);
if(row[3] != NULL)
  uos.kontr.new_plus(row[3]);
uos.pod=atoi(row[5]);
uos.kodol=atoi(row[6]);
if(row[7] != NULL)
 uos.pod1=atoi(row[7]);
else
 uos.pod1=0;

if(row[8] != NULL) 
  uos.nomdokp.new_plus(row[8]);
if(row[9] != NULL)
  uos.kodol1=atoi(row[9]);
else 
  uos.kodol1=0;

  
if(row[10] != NULL)
  prov=atoi(row[10]);
else
  prov=0;
  
if(row[12] != NULL)
  uos.osn.new_plus(row[12]);
if(row[15] != NULL)
  uos.dover.new_plus(row[15]);

if(row[16] != NULL)
 rsdat(&uos.dvd,&uos.mvd,&uos.gvd,row[16],2);

if(row[17] != NULL)
 uos.forop.new_plus(row[17]);

if(row[18] != NULL)
  rsdat(&uos.dnn,&uos.mnn,&uos.gnn,row[18],2);
if(row[19] != NULL)
  uos.nomnn.new_plus(row[19]);
if(row[20] != NULL)
  uos.sherez.new_plus(row[20]);
if(row[21] != NULL)
  rsdat(&uos.dop,&uos.mop,&uos.gop,row[21],2);
if(row[23] != NULL)
  uos.pnds=atof(row[23]);

rsdat(&uos.dvnn,&uos.mvnn,&uos.gvnn,row[24],2);
uos.vid_dog.new_plus(row[26]);

/*Определяем вид операции*/
mvnp=0;
uos.naimpr.new_plus("");

if(uos.tipz == 1)
 sprintf(strsql,"select naik,vido,prov from Uospri where kod='%s'",uos.kprr.ravno());
if(uos.tipz == 2)
 sprintf(strsql,"select naik,vido,prov from Uosras where kod='%s'",uos.kprr.ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),uos.kprr.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  uos.naimpr.new_plus(row[0]);
  mvnp=atoi(row[1]);
  metkaprov=atoi(row[2]);
 }

/*Для внутреннего перемещения*/
mdd=0;
if(mvnp == 1)
  mdd=1;

/*Читаем наименование организации*/
uos.naimo.new_plus("");
polen(uos.kontr.ravno(),&bros,1,'-');
if(SRAV(bros.ravno(),"00",0) != 0)
 bros.new_plus(uos.kontr.ravno());
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros.ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),bros.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  uos.naimo.new_plus(row[0]);
 }

/*Читаем наименование подразделения*/

uos.naipod.new_plus("");
sprintf(strsql,"select naik from Uospod where kod=%d",uos.pod);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),uos.pod);
  iceb_t_soob(strsql);
 }
else
 {
  uos.naipod.new_plus(row[0]);
 }

uos.naipod1.new_plus("");
if(uos.pod1 != 0)
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",uos.pod1);

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),uos.pod1);
    iceb_t_soob(strsql);
   }
  else
   {
    uos.naipod1.new_plus(row[0]);
   }
 }

/*Читаем материально-ответственного*/

uos.naikol.new_plus("");
sprintf(strsql,"select naik from Uosol where kod=%d",uos.kodol);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),uos.kodol);
  iceb_t_soob(strsql);
 }
else
 {
  uos.naikol.new_plus(row[0]);
 }

uos.naikol1.new_plus("");
if(uos.kodol1 != 0)
 {
  sprintf(strsql,"select naik from Uosol where kod=%d",uos.kodol1);

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден мат.-ответственный %d !"),uos.kodol1);
    iceb_t_soob(strsql);
   }
  else
   {
    uos.naikol1.new_plus(row[0]);
   }
 }

/*Читаем наименование формы оплаты*/
if(uos.forop.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naik from Foroplat where kod='%s'",uos.forop.ravno());

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найдена форма оплаты %s"),uos.forop.ravno());
    iceb_t_soob(strsql);
   }
  else
   {
    uos.naiforop.new_plus(row[0]);
   }
 } 
naz1:;
GDITE();
clear();
short           kls=LINES-9;
int ktoi[kls];
time_t vremz[kls];
double bsi=0.,izi=0.,bsbyi=0.,izbyi=0.;

sprintf(strsql,"select innom,podt,bs,iz,bsby,izby,cena,shs,ktoz,vrem from Uosdok1 \
where datd='%04d-%02d-%02d' and nomd='%s' and tipz=%d",
gd,md,dd,uos.nomdok.ravno_filtr(),uos.tipz);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
max_dl_naim=0;
if(kolstr == 0)
 {
lll:;

  ush5(dd,md,gd,mvnp,mdd,bsi,izi,bsbyi,izbyi);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  sprintf(strsql,"%s:%d.%d.%d %s:%s %d",
  gettext("Дата"),dd,md,gd,
  gettext("Документ"),uos.nomdok.ravno(),uos.tipz);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK2 || K == PLU || K == FK10 || K == SFK3 || K == SFK1 || K == FK1 || K == SFK9 || K == FK9)
    goto vper;
    
  goto lll;

 }
else
 {
  while(cur.read_cursor(&row) != 0)
   {
    /*узнаем наименование оборудования*/
    sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      naim.new_plus("");
     }
    else
     naim.new_plus(row1[0]);

    if(max_dl_naim < iceb_tu_strlen(naim.ravno()))
      max_dl_naim= iceb_tu_strlen(naim.ravno());
      
    bsi+=atof(row[2]);
    izi+=atof(row[3]);
    bsbyi+=atof(row[4]);
    izbyi+=atof(row[5]);
   }
 }
naz:;
GDITE();
clear();

/*MM5.clearMEN5();*/

cur.poz_cursor(pozz);
pozz1=0;
shet_spis.free_class();

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  in=atol(row[0]);
  podt=atoi(row[1]);
  bs=atof(row[2]);
  iz=atof(row[3]);
  bsby=atof(row[4]);
  izby=atof(row[5]);
  ktoi[i]=atoi(row[8]);
  vremz[i]=atol(row[9]);
      
  /*узнаем наименование оборудования*/
  sprintf(strsql,"select naim from Uosin where innom=%ld",in);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %ld !",gettext("Не найден инвентарный номер"),in);
    iceb_t_soob(strsql);
    naim.new_plus("");
   }
  else
   naim.new_plus(row1[0]);

  if(podt == 1)
    bros.new_plus("**");
  if(podt == 0)
    bros.new_plus("");

  
  memset(strsql,'\0',sizeof(strsql));
  if(uos.tipz == 2  && mvnp == 0)
   {
    if(max_dl_naim > COLS-65)
     max_dl_naim=COLS-65;
    
    sprintf(strsql,"%-5ld|%-*.*s|%9.9g|%9.9g|%9.9g|%9.9g|%.10g|%s",
    in,
    iceb_tu_kolbait(max_dl_naim,naim.ravno()),iceb_tu_kolbait(max_dl_naim,naim.ravno()),naim.ravno(),
    bs,iz,bsby,izby,atof(row[6]),bros.ravno());
   }
  else
   {
    if(max_dl_naim > COLS-55)
     max_dl_naim=COLS-55;
    sprintf(strsql,"%-5ld|%-*.*s|%9.2f|%9.2f|%9.2f|%9.2f|%s",
    in,
    iceb_tu_kolbait(max_dl_naim,naim.ravno()),iceb_tu_kolbait(max_dl_naim,naim.ravno()),naim.ravno(),
    bs,iz,bsby,izby,bros.ravno());
   }  
  shet_spis.plus(row[7]);

  spis_menu.spis_str.plus(strsql);
  i++;

 }
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 goto lll;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


ush5(dd,md,gd,mvnp,mdd,bsi,izi,bsbyi,izbyi);
if(prov == 1)
 {
  move(4,49);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Проводки не выполнены !"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);

if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
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
/*
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
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
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str vibstr(spis_menu.spis_str.ravno(kom));

  if(SRAV(gettext("Конец списка"),vibstr.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(vibstr.ravno(),&in,1,'|');
//  in=atol(bros);
  polen(vibstr.ravno(),&bs,3,'|');
  polen(vibstr.ravno(),&iz,4,'|');
  polen(vibstr.ravno(),&bsby,5,'|');
  polen(vibstr.ravno(),&izby,6,'|');
  polen(vibstr.ravno(),&cenaprod,7,'|');
  shet_sp.new_plus(shet_spis.ravno(kom));
 }

vper:;

switch (K)
 {
  case TAB: /*Переход в парный документ*/
    if(mdd == 0)
      goto naz1;

    if(uos.nomdokp.ravno()[0] != '\0')
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Перейти в парный документ ?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz1;
       ndk->new_plus(uos.nomdokp.ravno());
       goto start;
      }
    goto naz1;
    
  case FK10: /*Выход*/
  case ESC : /*Выход*/

     poduosd(dd,md,gd,uos.nomdok.ravno());
     opluos(dd,md,gd,uos.tipz,uos.nomdok.ravno(),uos.kprr.ravno());

     if(mdd == 1)
      {
       poduosd(dd,md,gd,uos.nomdokp.ravno());
       opluos(dd,md,gd,uos.tipzv,uos.nomdokp.ravno(),uos.kprr.ravno());
      }
     if(metkaprov == 1)
       return;

     if(mvnp != 0 && uos.tipz == 1)
       return;

     if(uos.tipz == 2 || (uos.tipz == 1 && 
     SRAV(uos.kprr.ravno(),gettext("ОСТ"),0) != 0))
      {
       class SPISOK spshet;
       class masiv_din_double sumsh;
       class masiv_din_double sumshp;
       
       prpuos(dd,md,gd,uos.nomdok.ravno(),uos.pod,&spshet,&sumsh,&sumshp,1);
       prpuos(dd,md,gd,uos.nomdok.ravno(),uos.pod,&spshet,&sumsh,&sumshp,0);
      }

     return;


  case FK1:   /*Получение подсказки*/
    GDITE();
    iceb_t_pdoc("uos2_1_3.txt");
    clear();

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK1:  /*Корректировка шапки документа*/
    VV.VVOD_delete();
    sozuosvdok(&VV);
    menshd(&VV,dd,md,gd);

    if(uosvdok(&VV,1) == 0)
     {
      clear();
      rsdat(&dd,&md,&gd,uos.datpr.ravno(),1);
      ndk->new_plus(uos.nomdok.ravno());
     }
    goto start;
           
  case FK2:  /*Ввести запись*/
  case PLU:  /*Ввести запись*/
//    sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
    if(iceb_t_pbpds(md,gd) != 0)
      goto naz1;

     vind(dd,md,gd,mvnp,mdd);
     goto naz1;
     break;

  case SFK2:   /*Корректировать запись*/

     VV.VVOD_delete();

     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);

     VV.data_plus(0,bs);
     VV.data_plus(1,iz);
     VV.data_plus(2,bsby);
     VV.data_plus(3,izby);
     kzvz(ktoi[kom],vremz[kom],&VV);
     if(uos.tipz == 2  && mvnp == 0)
     VV.data_plus(4,shet_sp.ravno());
     if(cenaprod != 0.)
       VV.data_plus(5,cenaprod);
     
     VV.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для налогового учёта......"));//0
     VV.VVOD_SPISOK_add_MD(gettext("Износ для налогового учёта....................."));//1
     VV.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для бухгалтерского учёта.."));//2
     VV.VVOD_SPISOK_add_MD(gettext("Износ для бухгалтерского учёта................."));//3

     if(uos.tipz == 1  && mvnp == 0)
     VV.VVOD_SPISOK_add_MD(gettext("Cчёт получения................................."));//4

     if(uos.tipz == 2  && mvnp == 0)
     VV.VVOD_SPISOK_add_MD(gettext("Cчёт списания.................................."));//4

     if(uos.tipz == 2  && mvnp == 0)
     VV.VVOD_SPISOK_add_MD(gettext("Цена продажи..................................."));//5


sfk2:;

     helstr(LINES-1,0,"F2/+",gettext("запись"),
     "F10",gettext("выход"),NULL);

     i=VV.vvod(0,1,1,-1,-1);
     switch(i)
      {
       case FK2:
       case PLU:
         break;
       case FK10:
       case ESC:
         goto naz;
       default:
         goto sfk2;
      }

//     sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
     if(iceb_t_pbpds(md,gd) != 0)
        goto naz1;

     struct OPSHET opsh;
     if(VV.data_ravno(4)[0] != '\0')
     if(prsh1(VV.data_ravno(4),&opsh) != 0)
       goto sfk2;
     
     time(&vrem);
     sprintf(strsql,"update Uosdok1 \
set \
bs=%.2f,\
iz=%.2f,\
bsby=%.2f,\
izby=%.2f,\
cena=%.2f,\
ktoz=%d,\
vrem=%ld,\
shs='%s' \
where datd='%d-%d-%d' and nomd='%s' and innom=%ld",
ATOFM(VV.VVOD_SPISOK_return_data(0)),ATOFM(VV.VVOD_SPISOK_return_data(1)),
ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(3)),
ATOFM(VV.VVOD_SPISOK_return_data(5)),
iceb_t_getuid(),vrem,
VV.data_ravno(4),
gd,md,dd,uos.nomdok.ravno_filtr(),in);

     sql_zapis(strsql,0,0);
   

     goto naz1;

  case FK3:       /*Удалить запись*/
  case SFK3:
//    sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
    if(iceb_t_pbpds(md,gd) != 0)
      goto naz1;

    /*Проверка возможности удаления*/
    if(K == FK3 )
     {

      if(uos.tipz == 1)
       if(puzap(in,uos.pod,dd,md,gd,uos.nomdok.ravno()) != 0)
        {
         iceb_t_soob(gettext("Не возможно удаление записи, есть ещё документ!"));
         goto naz1;
        }

      if(uos.tipz == 2 && mdd == 1)
       if(puzap(in,uos.pod1,dd,md,gd,uos.nomdokp.ravno()) != 0)
        {
         iceb_t_soob(gettext("Не возможно удаление записи во встречном документе, есть ещё документ !"));
         goto naz1;
        }
     }

    if(K == SFK3)
     {
      if(uos.tipz == 1)
       if(pudok(uos.pod,dd,md,gd,uos.nomdok.ravno(),uos.tipz) != 0)
        {
         iceb_t_soob(gettext("Не возможно удаление документа !"));
         goto naz1;
        }
      if(uos.tipz == 2 && mdd == 1)
       {       
        if(pudok(uos.pod1,dd,md,gd,uos.nomdokp.ravno(),1) != 0)
         {
          iceb_t_soob(gettext("Не возможно удаление встречного документа !"));
          goto naz1;
         }
       }
     }
    DANET.VVOD_delete();
    if(K == SFK3)
      DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));
    if(K == FK3)
      DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {

      if(K == SFK3)
       {
        GDITE();

        /*Удаляем проводки*/
        if(udprgr(ICEB_MP_UOS,dd,md,gd,uos.nomdok.ravno(),uos.pod,uos.tipz) != 0)
          goto naz1;
        if(mdd == 1)
           if(udprgr(ICEB_MP_UOS,dd,md,gd,uos.nomdokp.ravno(),uos.pod1,uos.tipzv) != 0)
            goto naz1;

        /*Удаляем записи в документе*/
        uduoszd(0L,dd,md,gd,uos.nomdok.ravno());
        if(mdd == 1)
          uduoszd(0L,dd,md,gd,uos.nomdokp.ravno());

        /*Удаляем заголовок документа*/
        uduosdok(dd,md,gd,uos.nomdok.ravno());
        if(mdd == 1)
          uduosdok(dd,md,gd,uos.nomdokp.ravno());
        return;        

       }

      if(K == FK3)
       {
        GDITE();
        uduoszd(in,dd,md,gd,uos.nomdok.ravno());
        if(mdd == 1)
          uduoszd(in,dd,md,gd,uos.nomdokp.ravno());
        goto naz1;
       }
     }
    goto naz;

  case FK4:       /*Проводки*/

     if(mvnp == 1 && uos.tipz == 1)
      {
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("При внутреннем перемещении проводки выполняются только для"));
       SOOB.VVOD_SPISOK_add_MD(gettext("               расходных докуметов"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }
     if(uos.tipz == 1 && SRAV(uos.kprr.ravno(),gettext("ОСТ"),0) == 0)
      {
       iceb_t_soob(gettext("Для документа стартового остатка проводки не выполняются"));
       goto naz;
      }

     diruospr(dd,md,gd,uos.nomdok.ravno());

     goto start;

  case FK5:   /*Распечатать документ на принтер*/

     if(uos.tipz == 1)
      uosdok_r1(dd,md,gd);
     else
      uosdok_r2(dd,md,gd);
     goto naz1;

  case ENTER: /*Просмотр карточки*/
     VV.VVOD_delete();
     vos(&VV,in,2,dt,mt,gt,uos.pod,uos.kodol);
     clear();
     goto naz;

     break;

  case FK7:  /*подтверждение*/


    podtv(in,dd,md,gd,uos.tipz,uos.nomdok.ravno(),0,0,uos.pod);
    if(mdd == 1)
      podtv(in,dd,md,gd,uos.tipzv,uos.nomdokp.ravno(),1,0,uos.pod1);
 
    goto naz1;


  case SFK7:  /*Подтверждение всего документа*/

    podtv(0L,dd,md,gd,uos.tipz,uos.nomdok.ravno(),0,0,uos.pod);
    if(mdd == 1)
      podtv(0L,dd,md,gd,uos.tipzv,uos.nomdokp.ravno(),1,0,uos.pod1);

    goto naz1;


  case FK8:  /*Установка/снятие блокировки*/

     uosdok_f8(dd,md,gd);
     iceb_tl_blok();
     goto naz1;
     
  case FK9:  /*Получить все основные средства в документа*/

    if(uos.tipz == 1)
      impuosost(uos.tipz,dd,md,gd,uos.nomdok.ravno(),uos.kprr.ravno(),uos.pod,uos.kodol);

    if(uos.tipz == 2)
     {
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Загрузить в документ всё что числится? Вы уверены?"));

      if(danet(&DANET,2,stdscr) == 1)
        uosvvd(uos.tipz,dd,md,gd,uos.nomdok.ravno(),uos.pod,uos.kodol,uos.kprr.ravno(),uos.pod1,uos.kodol1,uos.nomdokp.ravno());
     }
    goto naz1;

    
  default:
     if(kolstr > 0)
       goto naz;
     else
       goto lll;
 }


}

/**************/
/*Выдача шапки*/
/**************/
void ush5(short dd,short md,short gd,short mvnp,short mdd,
double bsi,double izi,double bsbyi,double izbyi)
{
move(0,0);
printw("%s: %d %s",gettext("Подразделение"),uos.pod,uos.naipod.ravno());
move(1,0);
printw("%s: %s %s",gettext("Контрагент"),uos.kontr.ravno(),uos.naimo.ravno());
move(2,0);
printw(gettext("Операция: %s %s"),uos.kprr.ravno(),uos.naimpr.ravno());
if(uos.tipz == 1)
 printw(" %s\n",gettext("(приход)"));
if(uos.tipz == 2)
 printw(" %s\n",gettext("(расход)"));

move(3,0);
printw("%s: %s %s:%.2f %.2f %.2f %.2f",
gettext("Номер документа"),uos.nomdok.ravno(),
gettext("Итого"),
bsi,izi,bsbyi,izbyi);

move(4,0);
printw("%s: %d.%d.%dр. ",
gettext("Дата"),dd,md,gd);
move(5,0);
printw("%s:%s",gettext("Номер парного документа"),uos.nomdokp.ravno());

if(mdd == 1 && mvnp == 1)
 {
  move(0,50);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Двойной документ"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mvnp == 2)
 {
  move(0,50);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Смена стоимости"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

if(uos.tipz == 1)
 helstr(LINES-1,0,"F1",gettext("помощь"),\
"F2/+",gettext("запись"),\
"F3",gettext("удалить"),\
"F4",gettext("пров."),\
"F5",gettext("печать"),\
"F7",gettext("подтв."),\
"F8",gettext("меню"),\
"F9",gettext("импорт"),\
"F10",gettext("выход"),NULL);

if(uos.tipz == 2)
 helstr(LINES-1,0,"F1",gettext("помощь"),\
"F2/+",gettext("запись"),\
"F3",gettext("удалить"),\
"F4",gettext("пров."),\
"F5",gettext("печать"),\
"F7",gettext("подтв."),\
"F8",gettext("меню"),\
"F9",gettext("списать"),\
"F10",gettext("выход"),NULL);
}
/*************************************/
/*Ввод инвентарных номеров в документ*/
/*************************************/
/*
Если вернули 0-ввели инвентарный номер
	     1-нет
*/
int             vind(short d,short m,short g,short mvnp,short mdd)
{
class poiinpdw_data rekin;
short           i;
class iceb_tu_str inv("");
long            in;
class bsizw_data bal_st;
short		kvrt;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
short		ohib;
int		podr;
short		kol;
time_t		vrem;
int		kodotl;
short		dr,mr,gr;
double nal_bs=0.,nal_iz=0.;
double buh_bs=0.,buh_iz=0.;

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите инвентарный номер"));

naz:;

if(uos.tipz == 1)
  helstr(LINES-1,0,"F2",gettext("своб.номер"),
  "F10",gettext("выход"),NULL);
if(uos.tipz == 2)
  helstr(LINES-1,0,"F10",gettext("выход"),NULL);

if((i=vvod1(&inv,20,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return(1);
if(uos.tipz == 1 && i == FK2)
 {
  inv.new_plus(invnomer(0));
  goto naz;
 }    
if(inv.getdlinna() <= 1)
 return(1);

GDITE();
if(isdigit(inv.ravno()[0]) != 0)  /*Значит символ число*/
 {
  in=inv.ravno_atol();

naz1:;

  /*Если расход или изменение стоимости то всегда должен быть инвентарный номер
   Если расход является внутренним перемещением или изменением стоимости
   то инвентарный
   номер должен быть в подразделении поставщике
   перед расходом должна быть посчитана амортизация
   
   */

  if(uos.tipz == 2 )
   {
    sprintf(strsql,"select innom from Uosin where innom=%ld",in);
    ohib=sql_readkey(&bd,strsql,&row,&curr);
    if(ohib < 1)
     {
      iceb_t_soob(gettext("Не найден инвентарный номер !"));
      goto naz;
     }

    /*Проверяем есть ли приход и не было ли расхода*/
    if((i=poiin(in,d,m,g,&podr,&kodotl)) != 0)
     {
      if(i == 1)
       {       
        iceb_t_soob(gettext("Не найден инвентарный номер !"));
        goto naz;
       }
      if(i == 2)
       {       
        iceb_t_soob(gettext("Инвентарный номер не числится. Произошел расход !"));
        goto naz;
       }

     }

    if(uos.pod != podr)
     {
      sprintf(strsql,"%s %d.",
      gettext("Этот инвентарный номер находится в подразделении"),podr);
      iceb_t_soob(strsql);
      goto naz;
     }

    if(uos.kodol != kodotl)
     {
      sprintf(strsql,"%s %d !",
      gettext("Этот инвентарный номер числится за мат.ответственным"),kodotl);
      iceb_t_soob(strsql);
      goto naz;
     }

    /*Читаем переменные данные*/
    if(poiinpd(in,m,g,&rekin) != 0)
     {
      sprintf(strsql,"%s %ld !",
      gettext("Не найдены переменные данные для"),in);
      iceb_t_soob(strsql);
      goto naz;
     }
    uos.shetu.new_plus(rekin.shetu.ravno());
    uos.hzt.new_plus(rekin.hzt.ravno());
    uos.hna.new_plus(rekin.hna.ravno());
    uos.popkf=rekin.popkf;
    uos.soso=rekin.soso;
    uos.nomz.new_plus(rekin.nomz.ravno());
    uos.hnaby.new_plus(rekin.hnaby.ravno());
    uos.popkfby=rekin.popkfby;
        
    /*Проверяем посчитана ли амортизация*/
    if(uosprovgr(uos.hna.ravno()) == 0)
     {
//      sprintf(strsql,"select innom from Uosamor where god=%d and mes=%d and innom=%ld",g,m,in);
      sprintf(strsql,"select innom from Uosamor where da='%04d-%02d-01' and innom=%ld",g,m,in);
      if(readkey(strsql,&row,&curr) == 0) /*Не расчитана амортизация*/
       { 
        kvrt=0; 
        if(SRAV(uos.hna.ravno(),"1",0) == 0)
         {
          if(m < 4)
           kvrt=1;
          if(m >= 4 && m < 7)
           kvrt=2;
          if(m >= 7 && m < 10)
           kvrt=3;
          if(m >= 10 && m <= 12)
           kvrt=4;
         }
        amort(in,m,g,2,kvrt,"",NULL);
       }
     }
    sprintf(strsql,"select innom from Uosamor1 where da='%04d-%02d-01' and innom=%ld",g,m,in);
    if(readkey(strsql,&row,&curr) == 0) /*не расчитана амортизация*/
     {
      amortby(in,m,g,2,uos.hnaby.ravno(),NULL);
     }
    dr=1;
    gr=g;
    
    if(m <= 12)
     mr=12;
    if(m <= 9)
     mr=9;
    if(m <= 6)
     mr=6;
    if(m <= 3)
     mr=3;

    dpm(&dr,&mr,&gr,3);//на начало следующего квартала

    //Для налогового учёта берем все до конца квартала
    bsiz(in,uos.pod,dr,mr,gr,&bal_st,NULL);
    nal_bs=bal_st.bs+bal_st.sbs;
    nal_iz=bal_st.iz+bal_st.siz+bal_st.iz1;

    dr=1; mr=m; gr=g;
    dpm(&dr,&mr,&gr,3);//на начало следующего месяца
    //Для бухгалтерского учёта берем все до конца месяца

    bsiz(in,uos.pod,dr,mr,gr,&bal_st,NULL);
    buh_bs=bal_st.bsby+bal_st.sbsby;
    buh_iz=bal_st.izby+bal_st.sizby+bal_st.iz1by;

   }

  /*Если приход внешний то это всегда новый инвентарный номер
  Если приход-внутреннее перемещение или изменение стоимости 
  то этот инвентарный номер должен быть в подразделении поставщике
  */

  if(uos.tipz == 1 )
   {
    if(mvnp == 0)
     {
      if((ohib=provinnom(inv.ravno())) != 0)
       goto naz;
     }    
    else
     {
      sprintf(strsql,"select innom from Uosin where innom=%ld",in);
      ohib=sql_readkey(&bd,strsql,&row,&curr);
     }

    if(mvnp == 1 && ohib < 1)
     {
      iceb_t_soob(gettext("Нет такого инвентарного номера !"));
      goto naz;
     }

    if((mvnp == 1 || mvnp == 2) && ohib >= 1)
     {
      if(poiin(in,d,m,g,&podr,&kodotl) != 0)
       {
        iceb_t_soob(gettext("Нет такого инвентарного номера !"));
        goto naz;
       }

      if(uos.pod != podr)
       {
        sprintf(strsql,"%s %d. (%d)",
        gettext("Этот инвентарный номер находится в подразделении"),podr,uos.pod);
        iceb_t_soob(strsql);
        goto naz;
       }
     if(uos.kodol != kodotl)
       {
        sprintf(strsql,"%s %d. (%d)",
        gettext("Этот инвентарный номер числится за мат-отв."),kodotl,uos.kodol);
        iceb_t_soob(strsql);
        goto naz;
       }

     }

    if(ohib < 1)
     {
      VVOD VV(0);
      if(vos(&VV,in,1,d,m,g,uos.pod,uos.kodol) != 0)
	return(1);

      nal_bs=ATOFM(VV.VVOD_SPISOK_return_data(15));
      nal_iz=ATOFM(VV.VVOD_SPISOK_return_data(16));
      buh_bs=ATOFM(VV.VVOD_SPISOK_return_data(17));
      buh_iz=ATOFM(VV.VVOD_SPISOK_return_data(18));
      
     }
    else
     if(mdd == 1)
      {
//       bsiz(in,uos.pod1,d,m,g,&sbs,&siz,&bs,&iz,&iz1,&sbsby,&sizby,&bsby,&izby,&iz1by,0);
       bsiz(in,uos.pod1,d,m,g,&bal_st,NULL);
       nal_bs=bal_st.bs+bal_st.sbs;
       nal_iz=bal_st.iz+bal_st.iz1+bal_st.siz;
       buh_bs=bal_st.bsby+bal_st.sbsby;
       buh_iz=bal_st.izby+bal_st.iz1by+bal_st.sizby;
      }
   }

  if(mvnp == 2)
   {
     VVOD VV(0);
     VV.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для налогового учёта......"));
     VV.VVOD_SPISOK_add_MD(gettext("Износ для налогового учёта....................."));
     VV.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для бухгалтерского учёта.."));
     VV.VVOD_SPISOK_add_MD(gettext("Износ для бухгалтерского учёта................."));

     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     VV.VVOD_SPISOK_add_data(20);
     
     helstr(LINES-1,0,"F2/+",gettext("запись"),
     "F10",gettext("выход"),NULL);

nzz:;

     i=VV.vvod(0,1,1,-1,-1);

     switch(i)
      {
       case FK2:
       case PLU:
         break;
       case FK10:
       case ESC:
        return(1);
       default:
        goto nzz;
      }

      nal_bs=ATOFM(VV.VVOD_SPISOK_return_data(0));
      nal_iz=ATOFM(VV.VVOD_SPISOK_return_data(1));
      buh_bs=ATOFM(VV.VVOD_SPISOK_return_data(2));
      buh_iz=ATOFM(VV.VVOD_SPISOK_return_data(3));

   }

  
  kol=1;
  if(mvnp == 2)
     kol=0;
  if(uos.tipz == 2)
     kol*=(-1);

  time(&vrem);

  sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%d-%d',%d,%d,%ld,'%s',%d,%d,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",\
  g,m,d,uos.tipz,0,in,uos.nomdok.ravno_filtr(),uos.pod,uos.kodol,kol,nal_bs,nal_iz,0.,
  uos.kprr.ravno(),iceb_t_getuid(),vrem,buh_bs,buh_iz,0.,"");
//  printw("%s\n",strsql);
  GDITE();
  if(sql_zapis(strsql,1,0) != 0)
    goto naz;
  
  if(mdd == 1)
   {
    sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%02d-%02d',%d,%d,%ld,'%s',%d,%d,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
    g,m,d,uos.tipzv,0,in,uos.nomdokp.ravno_filtr(),uos.pod1,uos.kodol1,kol*(-1),nal_bs,nal_iz,0.,
    uos.kprr.ravno(),iceb_t_getuid(),vrem,buh_bs,buh_iz,0.,"");

    GDITE();
    if(sql_zapis(strsql,1,0) != 0)
      goto naz;
   }

 }
else
 {

  if(diros(&in,&inv,1) != 0)
   return(1);

  goto naz1;
 }

return(0);

}

/*************************/
/*Подтверждение документа*/
/*************************/
void            podtv(int in,short d,short m,short g,short tz,
const char *ndk,short met, //0-с запросом 1- без
short mo, //0-установить подтверждение 1-снять подтв.
int podr)
{
char		strsql[512];

/*
printw("\npodtv-%d %d.%d.%d tz=%d ndk=%s met=%d mo=%d podr=%d\n",
in,d,m,g,tz,ndk,met,mo,podr);
OSTANOV();
*/

if(met == 0)
 {
  VVOD DANET(1);
  if(in == 0)
   {
    if(mo == 0)
      DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить весь документ ? Вы уверены ?"));
    if(mo == 1)
      DANET.VVOD_SPISOK_add_MD(gettext("Снять подтверждение со всего документа ? Вы уверены ?"));
   }
  else
    DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить запись ? Вы уверены ?"));
  if(danet(&DANET,2,stdscr) == 2)
   return;
 }

/*Подтвердить*/

if(mo == 0)
 {
  if(in != 0)
    sprintf(strsql,"update Uosdok1 \
set \
podt=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d and \
innom=%d",
    g,m,d,podr,ndk,tz,in);
  else
    sprintf(strsql,"update Uosdok1 \
set \
podt=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d",
    g,m,d,podr,ndk,tz);
 }

/*Снять подтверждение*/

if(mo == 1)
 {
  if(in != 0)
    sprintf(strsql,"update Uosdok1 \
set \
podt=0 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d and \
innom=%d",
    g,m,d,podr,ndk,tz,in);
  else
    sprintf(strsql,"update Uosdok1 \
set \
podt=0 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d",
    g,m,d,podr,ndk,tz);
 }

/*
printw("\n%s\n",strsql);
OSTANOV();
*/

sql_zapis(strsql,1,0);

}

/*********************************/
/*Подготовка меню шапки документа*/
/*********************************/
void menshd(VVOD *VV,short dd,short md,short gd)
{
int	i1;
char strsql[512];
float pnds=iceb_t_pnds(dd,md,gd);

VV->VVOD_clear_data();
if(uos.tipz == 1)
 {
  if(mdd == 1)
   {
    
    VV->data_plus_dat(0,dd,md,gd);
    VV->data_plus(1,uos.nomdok.ravno());
    VV->data_plus(2,uos.pod);
    VV->data_plus(3,uos.kodol);
    VV->data_plus(4,uos.nomdokp.ravno());
    VV->data_plus(5,uos.pod1);
    VV->data_plus(6,uos.kodol1);
    VV->data_plus(7,uos.osn.ravno());
   }
  else
   {
    VV->data_plus_dat(0,dd,md,gd);
    VV->data_plus(1,uos.nomdok.ravno());
    VV->data_plus(2,uos.pod);
    VV->data_plus(3,uos.kodol);
    VV->data_plus(4,uos.kontr.ravno());
    VV->data_plus(5,uos.nomdokp.ravno());
    VV->data_plus(6,uos.osn.ravno());

    VV->data_plus(7,uos.dover.ravno());

    if(uos.dvd != 0)
      VV->data_plus_dat(8,uos.dvd,uos.mvd,uos.gvd);
    VV->data_plus(9,uos.sherez.ravno());
    VV->data_plus(10,uos.nomnn.ravno());
    if(uos.dnn != 0)
      VV->data_plus_dat(11,uos.dnn,uos.mnn,uos.gnn);
    if(uos.dop != 0)
      VV->data_plus_dat(12,uos.dop,uos.mop,uos.gop);

    if(bnds1 != NULL)
     if(bnds1[0] != '\0')
      if(pole1(bnds1,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds1,uos.kprr.ravno(),0) == 0)
         uos.nds=1;
    if(bnds2 != NULL)
     if(bnds2[0] != '\0')
      if(pole1(bnds2,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds2,uos.kprr.ravno(),0) == 0)
         uos.nds=2;
    if(bnds3 != NULL)
     if(bnds3[0] != '\0')
      if(pole1(bnds3,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds3,uos.kprr.ravno(),0) == 0)
         uos.nds=3;

    if(uos.nds == 0)
     {
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV->data_plus(13,strsql);
     }
    if(uos.nds == 1)
      VV->data_plus(13,gettext("0% НДС реализация на територии Украины"));
    if(uos.nds == 2)
      VV->data_plus(13,gettext("0% НДС экспорт"));
    if(uos.nds == 3)
      VV->data_plus(13,gettext("Освобождение от НДС"));

    VV->data_plus(14,uos.forop.ravno());
    if(uos.dvnn != 0)
      VV->data_plus_dat(15,uos.dvnn,uos.mvnn,uos.gvnn);

    VV->data_plus(16,uos.vid_dog.ravno());

  }
 }

if(uos.tipz == 2)
 {
  if(mdd == 1)
   {
    VV->data_plus_dat(0,dd,md,gd);
    VV->data_plus(1,uos.nomdok.ravno());
    VV->data_plus(2,uos.pod);
    VV->data_plus(3,uos.kodol);
    VV->data_plus(4,uos.nomdokp.ravno());
    VV->data_plus(5,uos.pod1);
    VV->data_plus(6,uos.kodol1);
    VV->data_plus(7,uos.osn.ravno());

   }
  else
   {
    VV->data_plus_dat(0,dd,md,gd);
    VV->data_plus(1,uos.nomdok.ravno());
    VV->data_plus(2,uos.pod);
    VV->data_plus(3,uos.kodol);
    VV->data_plus(4,uos.kontr.ravno());
    VV->data_plus(5,uos.osn.ravno());
    VV->data_plus(6,uos.dover.ravno());
    if(uos.dvd != 0)
      VV->data_plus_dat(7,uos.dvd,uos.mvd,uos.gvd);
    VV->data_plus(8,uos.sherez.ravno());
    if(uos.forop.ravno()[0] != '\0')
     {
      VV->data_plus(9,uos.forop.ravno());
      VV->data_strcat(9,"|");
      VV->data_strcat(9,uos.naiforop.ravno());
     }
    VV->data_plus(10,uos.nomnn.ravno());
    if(uos.dnn != 0)
      VV->data_plus_dat(11,uos.dnn,uos.mnn,uos.gnn);
    if(uos.dop != 0)
      VV->data_plus_dat(12,uos.dop,uos.mop,uos.gop);

    if(bnds1 != NULL)
     if(bnds1[0] != '\0')
      if(pole1(bnds1,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds1,uos.kprr.ravno(),0) == 0)
         uos.nds=1;
    if(bnds2 != NULL)
     if(bnds2[0] != '\0')
      if(pole1(bnds2,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds2,uos.kprr.ravno(),0) == 0)
         uos.nds=2;
    if(bnds3 != NULL)
     if(bnds3[0] != '\0')
      if(pole1(bnds3,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds3,uos.kprr.ravno(),0) == 0)
         uos.nds=3;

    if(uos.nds == 0)
     {
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV->data_plus(13,strsql);
     }
    if(uos.nds == 1)
      VV->data_plus(13,gettext("0% НДС реализация на територии Украины"));
    if(uos.nds == 2)
      VV->data_plus(13,gettext("0% НДС экспорт"));
    if(uos.nds == 3)
      VV->data_plus(13,gettext("Освобождение от НДС"));

    VV->data_plus(14,uos.vid_dog.ravno());

   }
 }

}
/*****************************/
/*Варианты аста приема передачи*/
/********************************/

void	actpp(short dd,short md,short gd,const char *nomdok)
{
int	kom=0;

VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт приема/передачи"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт приема/передачи (Типовая форма N03-1)"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт приема/передачи (Приказ 818)"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

naz:;

kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0 :
    rasactuos(dd,md,gd,nomdok);
    return;

  case 1 :
    uosact(dd,md,gd,nomdok);
    return;

  case 2 :
    uosrapp(dd,md,gd,nomdok);
    return;

  default:
   goto naz;
 }

}
/****************************************/
/*МЕНЮ Ф8*/
/*****************************************/

void uosdok_f8(short dd,short md,short gd)
{
int		kom;

VVOD MENU(3);

kom=0;


MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
//MENU.VVOD_SPISOK_add_ZAG(gettext("Импорт информации в базу"));

MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку даты"));
MENU.VVOD_SPISOK_add_MD(gettext("Снять подтверждение со всех записей"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

naz:;


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
class spis_oth oth;
switch (kom)
 {
  case 2 :
  case -1 :
    return;

  case 0:
    iceb_tl_blok();
    return;

  case 1:
    podtv(0L,dd,md,gd,uos.tipz,uos.nomdok.ravno(),0,1,uos.pod);
    if(mdd == 1)
      podtv(0L,dd,md,gd,uos.tipzv,uos.nomdokp.ravno(),1,1,uos.pod1);
    return;
  default:
    kom=0;
    goto naz;
 }
}
/******************************/
/*Распечатки для приходных документов*/
/***************************************/
void uosdok_r1(short dd,short md,short gd)
{
class iceb_tu_str imaf_nn_xml("");
class VVOD MENU(3);
class spis_oth oth;
int pnd=0;
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт приема/передачи"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт ремонта/дооборудования"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать накладную"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать налоговую накладную"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт введения в эксплуатацию"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5
int kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 5 :
  case -1 :
    return;
  case 0 :
    actpp(dd,md,gd,uos.nomdok.ravno());
    break;

  case 1 :
    uosrem(dd,md,gd,uos.nomdok.ravno());
    break;

  case 2 :
    rasdokuos(dd,md,gd,uos.pod,uos.nomdok.ravno(),uos.tipz);
    break;

  case 3 :
     
    char imaf[64];
    sprintf(imaf,"nn%s.lst",uos.nomdok.ravno());
    if(rasnalnuos(dd,md,gd,uos.pod,uos.nomdok.ravno(),imaf) != 0)
      break;

     oth.spis_imaf.plus(imaf);
     oth.spis_naim.plus(gettext("Налоговая накладная"));

     iceb_t_rnfxml(&rek_zag_nn);
     if(xml_nn_uos(&pnd,uos.tipz,dd,md,gd,uos.nomdok.ravno(),uos.nomnn.ravno(),md,gd,md,1,"",&imaf_nn_xml) == 0)
      {
       oth.spis_imaf.plus(imaf_nn_xml.ravno());
       oth.spis_naim.plus(gettext("Налоговая накладная в формате XML"));
      }

     iceb_t_rabfil(&oth,"");
    break;
  case 4 :
    uosavve818(dd,md,gd,uos.nomdok.ravno());
    break;
 }
}
/*******************************/
/*распечатка актов списания*/
/***************************/
void uosdok_ras(short dd,short md,short gd)
{
class VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт списания основного средства"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт списания транспортного средства"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт списания основного средства (приказ 818)"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт списания транспортного средства (приказ 818)"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4
int kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 4 :
  case -1 :
    return;
  case 0 :
     uossos(dd,md,gd,uos.nomdok.ravno());
     break;

  case 1 :
     uossas(dd,md,gd,uos.nomdok.ravno());
     break;
  case 2 :
    uosasoz818(dd,md,gd,uos.nomdok.ravno());
    break;
  case 3 :
    uosastz818(dd,md,gd,uos.nomdok.ravno());
    break;
 }


}




/******************************/
/*Распечатки для приходных документов*/
/***************************************/
void uosdok_r2(short dd,short md,short gd)
{
class iceb_tu_str imaf_nn_xml("");
class spis_oth oth;
int pnd=0;

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт приема/передачи"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт ремонта/дооборудования"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать накладную"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать налоговую накладную"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт списания основного средства"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать акт внутреннего перемещения основных средств"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6
int kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 6 :
  case -1 :
    return;
  case 0 :
    actpp(dd,md,gd,uos.nomdok.ravno());
    break;

  case 1 :
    uosrem(dd,md,gd,uos.nomdok.ravno());
    break;

  case 2 :
    rasdokuos(dd,md,gd,uos.pod,uos.nomdok.ravno(),uos.tipz);
    break;

  case 3 :
     
    char imaf[64];
    sprintf(imaf,"nn%s.lst",uos.nomdok.ravno());
    if(rasnalnuos(dd,md,gd,uos.pod,uos.nomdok.ravno(),imaf) != 0)
      break;

     oth.spis_imaf.plus(imaf);
     oth.spis_naim.plus(gettext("Налоговая накладная"));

     iceb_t_rnfxml(&rek_zag_nn);
     if(xml_nn_uos(&pnd,uos.tipz,dd,md,gd,uos.nomdok.ravno(),uos.nomnn.ravno(),md,gd,md,1,"",&imaf_nn_xml) == 0)
      {
       oth.spis_imaf.plus(imaf_nn_xml.ravno());
       oth.spis_naim.plus(gettext("Налоговая накладная в формате XML"));
      }

     iceb_t_rabfil(&oth,"");
    break;
  
  case 4 :
    uosdok_ras(dd,md,gd);
//     uossos(dd,md,gd,uos.nomdok.ravno());
     break;


  case 5 :
     uosavp(dd,md,gd,uos.nomdok.ravno());
     break;

 }
}
