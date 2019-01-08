/* $Id: dirudok.c,v 5.132 2014/04/30 06:12:26 sasa Exp $ */
/*16.03.2017	29.02.2000	Белых А.И.	dirudok.c
Просмотр документа на услуги
*/
#include <math.h>
#include        <errno.h>
#include	"buhg.h"


void            sozfu(short,short,short,int,const char[],int,int,class SPISOK*,int);
void            sozfusal(short,short,short,int,const char[],int,int,class SPISOK*,int);
void		sdusl(short,short,short,const char[],int,const char[],const char[],const char[],const char[],const char[],double,int,double,int,const char[],double*,int,double*,float,int,int,double*,float);
short           prblok(short,short,short,int);
void            korzapu(short,short,short,int,int,const char*,int,int,int,float);
int		perecenu(short,short,short,const char*,int,int,float);
void		deletdokumu(class SPISOK*);
int             dirprus(short,short,short,const char[],int,const char[],const char[],int,double,double,int,const char[],float);
void usl_sm(int tipz,short dd,short md,short gd,int,int,const char *kontr,const char *nomdok,float);
int usl_shet_fak(short dg,short mg,short gg,int podr,const char *nomdok,char *imafil,int tipz);
int usl_act_vr(short dg,short mg,short gg,int podr,const char *nomdok,char *imafil,int tipz);
int dirudok_snn(short dg,short mg,short gg,int podr,const char *nomdok,int lnds,const char *imaf);

class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1;
extern double	okrcn;  /*Округление цены*/
extern class iceb_tu_str nameprinter;
extern char *imabaz;

void dirudok(short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int podr,  //Подразделение
const char *nomds) //Номер документа
{
int razmer=0;
int max_dlina_nazv=0;
short    mvnp=0; /*0-внешняя 1 -внутреняя*/
class iceb_tu_str vstr("");
class iceb_tu_str nomdok(nomds);
class spis_oth oth;
double		*maskor;
class iceb_tu_str naimkor("");
short		mpn; /*Метка пустой накладной*/
long		kolstr;
long		pozz,pozz1;
SQL_str         row,row1;
char		strsql[1024];
int		lnds;
short		kls;
int		kom,kom1;
double		sumkor=0.;
class iceb_tu_str naim("");
class iceb_tu_str dnaim("");
double		suma=0.,sum=0.,kolih=0.,kolihp=0.,cena=0.;
int		i,i1,K,prc;
class iceb_tu_str naimpodr("");
class iceb_tu_str kontr("");
class iceb_tu_str naimk("");
class iceb_tu_str kodop("");
class iceb_tu_str naiop("");  /*Наименование операции*/
short		dop,mop,gop; /*Дата оплаты*/
short           ddov=0,mdov=0,gdov=0; //Дата выдачи доверенности
int		blokpid;
int		metka=0; /*0-материал 1-услуга*/
int		kodzap=0;
short		d,m,g;
class iceb_tu_str ei("");
class iceb_tu_str shetu("");
class SPISOK	imaf;
char		bros[1024];
int		kolihz;
double		kolihu;
int		pro;
class iceb_tu_str vnomd(""); //Номер встречного документа
short		metkaprov=0;
short		dpnn=0,mpnn=0,gpnn=0;
float		procent=0.;
class iceb_tu_str naimnal("");
short		regh=0; /*0-основной 1-расширенный 2-цена с НДС 3-полное наименование*/
double		sumandspr=0.; //Сумма НДС для приходного документа введенная вручную
double		sumaitogo=0.;
time_t vrem_zap=0;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str imaf_nn_xml("");
class iceb_tu_str nomnn("");
class iceb_tu_str uslpr("");
class iceb_tu_str vid_dok("");
unsigned int kto_zap_hd=0; /*кто записал шапку документа*/

int pnd=0;
float pnds=0.; //Процент НДС на время создания документа
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR cur1;

/*
printw("\ndirudok-%d.%d.%d tipz=%d\n",dd,md,gd,tipz);
OSTANOV();
*/

VVOD MENU(3);
VVOD DANET(1);
VVOD VVOD1(2);
VVOD VV(0);

maskor=NULL;

naz2:;
GDITE();

sumkor=0.;
K=pozz=kom=0;
naimkor.new_plus("");


/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,nds,sumkor,uslpr,forop,nomnn,\
datop,blokir,pro,osnov,nomdp,datpnn,dover,datdov,sherez,pn,vrem,ktoi from Usldokum where \
datd='%d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
gd,md,dd,podr,nomdok.ravno(),tipz);

if((i=sql_readkey(&bd,strsql,&row,&cur1)) != 1)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s %s %s %d %s %d.%d.%d%s %s !",
  gettext("Документ"),
  nomdok.ravno(),
  gettext("в подразделении"),
  podr,
  gettext("за"),  
  dd,md,gd,
  gettext("г."),gettext("не найден"));
  SOOB.VVOD_SPISOK_add_MD(strsql);

  if(tipz == 1)
    sprintf(strsql,gettext("Приход"));
  if(tipz == 2)
    sprintf(strsql,gettext("Расход"));
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
kto_zap_hd=atoi(row[18]);
mpn=0;
kontr.new_plus(row[0]);
kodop.new_plus(row[1]);
lnds=atoi(row[2]);
sumkor=atof(row[3]);
uslpr.new_plus(row[4]);
polen(uslpr.ravno(),&vid_dok,4,':');

nomnn.new_plus(row[6]);
rsdat(&dop,&mop,&gop,row[7],2);
blokpid=atoi(row[8]);
pro=atoi(row[9]);
vnomd.new_plus(row[11]);
rsdat(&dpnn,&mpnn,&gpnn,row[12],2);

rsdat(&ddov,&mdov,&gdov,row[14],2);
pnds=atof(row[16]);
vrem_zap=atol(row[17]);

/*Читаем наименование контрагента*/
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
kontr.ravno());
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
  iceb_t_soob(strsql);
 }
else
 naimk.new_plus(row[0]);

/*Читаем наименование подразделения*/
sprintf(strsql,"select naik from Uslpodr where kod=%d",
podr);
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
  iceb_t_soob(strsql);
 }
else
 naimpodr.new_plus(row[0]);

/*Читаем наименование операции*/
mvnp=0;
if(tipz == 1)
 sprintf(strsql,"select naik,vido,prov from Usloper1 where kod='%s'",
 kodop.ravno());
if(tipz == 2)
 sprintf(strsql,"select naik,vido,prov from Usloper2 where kod='%s'",
 kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  naiop.new_plus(row[0]);
  mvnp=atoi(row[1]);
  metkaprov=atoi(row[2]);
 }

class iceb_tu_str imaf_s_logotip("");
class iceb_tu_str klush_peh("");
//читаем имя файла с логотипом

iceb_t_poldan("Путь на файл с логотипом",&imaf_s_logotip,"nastsys.alx");
if(imaf_s_logotip.ravno()[0] != '\0')
 {
  klush_peh.new_plus("-o logo=");
  klush_peh.plus(imaf_s_logotip.ravno());
 }
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

usldopnal(tipz,kodop.ravno(),&procent,&naimnal);

sprintf(bros,"%ld",vrem_zap);
/*Имя файла акта выполненых работ без учёта сальдо*/
if(tipz == 1)
  sprintf(strsql,"apu%s.lst",bros);
if(tipz == 2)
  sprintf(strsql,"aru%s.lst",bros);
imaf.plus(strsql);

/*имя файла налоговой накладной*/
if(tipz == 1)
     sprintf(strsql,"nnpu%s.lst",bros);
if(tipz == 2)
     sprintf(strsql,"nnru%s.lst",bros);
imaf.plus(strsql);

/*имя файла счета*/
if(tipz == 1)
     sprintf(strsql,"shpu%s.lst",bros);
if(tipz == 2)
     sprintf(strsql,"shru%s.lst",bros);
imaf.plus(strsql);

/*Имя файла акта выполненых работ с учётом сальдо*/
if(tipz == 1)
  sprintf(strsql,"apus%s.lst",bros);
if(tipz == 2)
  sprintf(strsql,"arus%s.lst",bros);
imaf.plus(strsql);


naz1:;
GDITE();
clear();
kls=LINES-1-8;
int nomzap[kls];
unsigned int ktoz[kls];

suma=0.;
kolihz=0;
kolihu=0.;
sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,dnaim,nz,shsp,ktoi from Usldokum1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d order by metka asc,kodzap asc",
gd,md,dd,podr,nomdok.ravno(),tipz);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

  sdusl(dd,md,gd,nomdok.ravno(),podr,naimpodr.ravno(),kontr.ravno(),naimk.ravno(),kodop.ravno(),naiop.ravno(),suma,kolihz,kolihu,blokpid,naimkor.ravno(),&sumkor,pro,maskor,procent,lnds,tipz,&sumaitogo,pnds);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  mpn=1;

  if( K == SFK2 || K == FK3 || K == FK7 || K == SFK7 ||
   K == FK5 || K == SFK5 || K == FK6)
      goto lll;

  if(K == KEY_RESIZE)
   goto naz1;
      
  goto vper;
 }
else
 {
   printw(gettext("Вычисляем сумму по документу\n"));
   refresh();
   double suma_str;
   max_dlina_nazv=0;

   while(cur.read_cursor(&row) != 0)
    {
     kolihz++;
     kolihu+=atof(row[2]);
     cena=atof(row[3]);
     cena=okrug(cena,okrcn);
     suma_str=atof(row[2])*cena;
     suma_str=okrug(suma_str,okrg1);
     suma+=suma_str;

     /*читаем наименование что бы определить длинну текста*/
     if(atoi(row[0]) == 0)
      sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
     else
       sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);

     if(readkey(strsql,&row1,&cur1) == 1)
      {
       razmer=iceb_tu_strlen(row1[0])+iceb_tu_strlen(row[6]);
       if(max_dlina_nazv < razmer)
        max_dlina_nazv=razmer;       
      }
    }     
   printw(gettext("Конец расчёта\n"));
   refresh();
 }

mpn=0;

naz:;

cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {

  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;  
  naim.new_plus("");
  if(row[0][0] == '0')
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      {
       beep();
       printw(gettext("Не найдено наименование материалла !"));
       printw("\n");
       OSTANOV();
      }
    else
     {
      naim.new_plus(row1[0]);
      if(row[6][0] != '\0')
       {
        naim.plus("+",row[6]);
       }
     }
   }  

  if(row[0][0] == '1')
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      {
       beep();
       printw("%s %s !\n",gettext("Не найден код услуги"),row[1]);
       OSTANOV();
      }
    else
     {
      naim.new_plus(row1[0]);
      if(row[6][0] != '\0')
       {
        naim.plus("+",row[6]);
       }
     }
   }  
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolihp=readkolu(dd,md,gd,podr,nomdok.ravno(),tipz,metka,kodzap,atoi(row[7]));
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=okrug(cena,okrcn);
   
  sum=cena*kolih;
  sum=okrug(sum,okrg1);

  nomzap[i]=atoi(row[7]);
  ktoz[i]=atoi(row[9]);
  
  memset(strsql,'\0',sizeof(strsql));
  if(regh == 0) /*Основной режим показа */
   {
    razmer=COLS-60;
    if(razmer < 20)
     razmer=20;
    if(max_dlina_nazv < razmer)
     razmer=max_dlina_nazv;
    sprintf(strsql,"%-4s|%-*.*s|%-*s|%-*s|%8.8g|%8.8g|%8.2f|%8.8g",
    row[1],
    iceb_tu_kolbait(razmer,naim.ravno()),iceb_tu_kolbait(razmer,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(4,row[4]),row[4],
    iceb_tu_kolbait(4,row[5]),row[5],
    kolih,cena,sum,kolihp);
  }  
  
  if(regh == 3) //Длинное наименование
   {
    sprintf(strsql,"%-4s|%-*s|%-*s|%-*s|%-*s|%8.8g|%8.8g",
    row[1],
    iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(4,row[2]),row[4],
    iceb_tu_kolbait(4,row[5]),row[5],
    iceb_tu_kolbait(4,row[8]),row[8],
    kolih,cena);

/*    sprintf(MM5.MD[i++],"%.*s",COLS-2,strsql);*/
   }  
    
  spis_menu.spis_str.plus(strsql);
  i++;

 }
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


clear();

sdusl(dd,md,gd,nomdok.ravno(),podr,naimpodr.ravno(),kontr.ravno(),naimk.ravno(),kodop.ravno(),naiop.ravno(),suma,kolihz,kolihu,blokpid,naimkor.ravno(),&sumkor,pro,maskor,procent,lnds,tipz,&sumaitogo,pnds);

sprintf(strsql,"update Usldokum set \
sumkor=%.2f where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d",sumkor,gd,md,dd,podr,nomdok.ravno(),tipz);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
   msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,5,0,NULL,strsql,prc,&K,"",0);
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
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  cur.poz_cursor(pozz+kom);
  cur.read_cursor(&row);
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  ei.new_plus(row[4]);
  shetu.new_plus(row[5]);
 }

vper:;

switch (K)
 {
  case DEL:  /*Переключение режима вывода на экран*/
    
    if(regh == 3)
     regh=0; 
    else
     regh=3; 
    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;

  case ENTER:
     goto naz1;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("usl2_1_1.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case SFK1: /*Работа с шапкой документа*/

     if(vdnu(&dd,&md,&gd,&podr,&nomdok,tipz) == 0)
      {
       deletdokumu(&imaf);
      }
     goto naz2;

  case FK2: /*Ввод новой услуги*/
     metka=1;
  case PLU: 
     metka=1;

     if(prblok(0,md,gd,blokpid) != 0)
      {
       if(kolstr > 0)
         goto naz;
       else
        goto lll;
      }
     vusl(dd,md,gd,tipz,podr,nomdok.ravno(),metka,pnds);
     deletdokumu(&imaf);
     goto naz1;

  case OTN: //Ввод нового материалла
     metka=0;

     if(prblok(0,md,gd,blokpid) != 0)
      {
       if(kolstr > 0)
         goto naz;
       else
        goto lll;
      }
     vusl(dd,md,gd,tipz,podr,nomdok.ravno(),metka,pnds);
     deletdokumu(&imaf);
     goto naz1;

  case SFK2:   /*Корректировать запись*/
   if(prblok(0,md,gd,blokpid) != 0)
    {
     if(kolstr > 0)
       goto naz;
     else
      goto lll;
    }
    
   korzapu(dd,md,gd,tipz,podr,nomdok.ravno(),metka,kodzap,nomzap[kom],pnds);
   deletdokumu(&imaf);

   kom++;
   goto naz1;

  case FK3:   /*Удалить запись в документе*/
    if(prblok(0,md,gd,blokpid) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    if(usl_pvku(ktoz[kom]) != 0) /*проверяем на хозяина записи*/
     goto naz1;
         
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      uduvdok(tipz,dd,md,gd,nomdok.ravno(),podr,metka,kodzap,nomzap[kom]);
     }
    deletdokumu(&imaf);
    goto naz1;  

  case SFK3:   /*Удалить весь документ*/
    if(prblok(0,md,gd,blokpid) != 0)
      goto naz;

    if(usl_pvku(kto_zap_hd) != 0) /*проверяем на хозяина записи*/
     goto naz1;

    if(iceb_t_pbp(6,dd,md,gd,nomdok.ravno(),podr,tipz,gettext("Удалить документ невозможно!")) != 0)
      goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      if(udprgr(ICEB_MP_USLUGI,dd,md,gd,nomdok.ravno(),podr,tipz) != 0)
       goto naz1;
       
      deletdokumu(&imaf);
      if(uduvdok(tipz,dd,md,gd,nomdok.ravno(),podr,0,0,0) == 0)
        return;
     }

    if(kolstr > 0)
      goto naz;
    else
     goto lll;


  case FK4: /*Работа с проводками к документу*/
    if(metkaprov == 1)
     {
      iceb_t_soob(gettext("Для этой операции проводки не выполняются !"));
      goto naz;
     }
    if(mvnp != 0 && tipz == 1)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Для внутренних операций, проводки делаются только"));
      SOOB.VVOD_SPISOK_add_MD(gettext("для расходных документов !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }    
    i=dirprus(dd,md,gd,nomdok.ravno(),podr,kodop.ravno(),kontr.ravno(),tipz,suma,sumkor,lnds,vnomd.ravno(),pnds);
    pro=prosprus(0,podr,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds);
    if(i == 0)
      goto kon;
  
    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case FK5:   /*Распечатать*/

    sozfu(dd,md,gd,podr,nomdok.ravno(),tipz,lnds,&imaf,mvnp);
fff5_0:;
    MENU.VVOD_delete();

    memset(strsql,'\0',sizeof(strsql));
    if(nameprinter.getdlinna() > 1)
     sprintf(strsql,"%s:%-*.*s",
     gettext("Принтер"),
     iceb_tu_kolbait(20,nameprinter.ravno()),
     iceb_tu_kolbait(20,nameprinter.ravno()),
     nameprinter.ravno());
    else
     sprintf(strsql,"%s:%-*.*s",
     gettext("Принтер"),
     iceb_tu_kolbait(20,gettext("По умолчанию")),
     iceb_tu_kolbait(20,gettext("По умолчанию")),
     gettext("По умолчанию"));

    MENU.VVOD_SPISOK_add_ZAG(strsql);

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненых работ"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка налоговой накладной"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счета"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счета с учётом сальдо"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового ордера"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка счёта-фактуры"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта выполненных работ, списания материалов"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Удалить распечатки документов"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

fff5:;

    clearstr(LINES-1,0);
    i1=0;
    while(menu3w(stdscr,&MENU,&i1,-1,-1,0) != 0);
    char imaf_sh[64];
    switch(i1)
     {
      case FK10:
      case -1:
      case 9:
        goto naz;

      case 0: /*Распечатка акта выполненых работ*/
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
          pehf(imaf.ravno(0),0,printer.ravno());
        if(i1 == 2)
          pehf(imaf.ravno(0),0,printerloc.ravno());
        break;      

      case 1: /*Распечатка налоговой накладной*/

        if(dirudok_snn(dd,md,gd,podr,nomdok.ravno(),lnds,imaf.ravno(1)) != 0)
         break;
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
          pehf(imaf.ravno(1),0,printer.ravno());
        if(i1 == 2)
          pehf(imaf.ravno(1),0,printerloc.ravno());
        break;      

      case 2: /*Распечатка счета*/

        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
          pehf(imaf.ravno(2),0,printer.ravno());
        if(i1 == 2)
          pehf(imaf.ravno(2),0,printerloc.ravno());
        break;      

      case 3: /*Распечатка счета с учётом сальдо*/
        if(tipz == 1)
         {
          iceb_t_soob(gettext("Для приходного документа не печатают счета !"));
          goto naz;
         }
        sozfusal(dd,md,gd,podr,nomdok.ravno(),tipz,lnds,&imaf,mvnp);
        if((i1=vibprint()) == 0)
          goto naz;
        if(i1 == 1)
          pehf(imaf.ravno(3),0,printer.ravno());
        if(i1 == 2)
          pehf(imaf.ravno(3),0,printerloc.ravno());
        break;      

      case 4:
        char imafko[64];
        if(tipz == 2)
         {
          sprintf(imafko,"kor%d.lst",getpid());
          kasord1(imafko,"",dd,md,gd,"","",sumaitogo,naimk.ravno(),"","","",NULL);
          oth.clear();
          oth.spis_imaf.plus(imafko);
          oth.spis_naim.plus(gettext("Кассовый ордер"));
          iceb_t_rabfil(&oth,"");
         }

        if(tipz == 1)
         {
          sprintf(imafko,"kop%d.lst",getpid());
          kasord2(imafko,"",dd,md,gd,"","",sumaitogo,naimk.ravno(),"","","","",NULL);
          oth.clear();
          oth.spis_imaf.plus(imafko);
          oth.spis_naim.plus(gettext("Кассовый ордер"));
          iceb_t_rabfil(&oth,"");
         }

        break;
        
      case 5: //Распечатка счёта-фактуры

        if(usl_shet_fak(dd,md,gd,podr,nomdok.ravno(),imaf_sh,tipz) == 0)
         {
          oth.clear();
          oth.spis_imaf.plus(imaf_sh);
          oth.spis_naim.plus(gettext("Счёт-фактура"));
          iceb_t_rabfil(&oth,klush_peh.ravno());
         }
        break;

      case 6: //Распечатка акта выполненных работ, списания материалов
        if(usl_act_vr(dd,md,gd,podr,nomdok.ravno(),imaf_sh,tipz) == 0)
         {
          oth.clear();
          oth.spis_imaf.plus(imaf_sh);
          oth.spis_naim.plus(gettext("Счёт-фактура"));
          
          iceb_t_rabfil(&oth,klush_peh.ravno());
         }
        break;

      case 7:
        deletdokumu(&imaf);
        break;

      case 8 :  //Сменить имя принтера
        mprintcap();
        goto fff5_0;
        
      default:
        goto fff5;
     }

    if(kolstr > 0)
      goto naz;
    else
     goto lll;

  case SFK5:   /*Просмотр*/

    sozfu(dd,md,gd,podr,nomdok.ravno(),tipz,lnds,&imaf,mvnp);
    if(tipz == 1)
     {
      oth.clear();

      oth.spis_imaf.plus(imaf.ravno(0));
      oth.spis_ms.plus(imaf.ravno(0));
      oth.spis_naim.plus(gettext("Акт выполненных работ"));

      oth.spis_imaf.plus(imaf.ravno(2));
      oth.spis_ms.plus(imaf.ravno(2));
      oth.spis_naim.plus(gettext("Cчёт на оплату"));

      iceb_t_rabfil(&oth,"");
 
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр акта выполненых работ"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр налоговой накладной"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр счета"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр счета с учётом сальдо"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4
    clearstr(LINES-1,0);
    i=0;
    while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
    switch(i)
     {
      case 4:
      case -1:
        break;

      case 0:
        oth.clear();

        oth.spis_imaf.plus(imaf.ravno(0));
        oth.spis_ms.plus(imaf.ravno(0));
        oth.spis_naim.plus(gettext("Распечатка акта выполненных работ"));
        iceb_t_rabfil(&oth,"");

        break;

      case 1:
        if(dirudok_snn(dd,md,gd,podr,nomdok.ravno(),lnds,imaf.ravno(1)) != 0)
         goto naz1;

        oth.clear();

        oth.spis_imaf.plus(imaf.ravno(1));
        oth.spis_ms.plus(imaf.ravno(1));
        oth.spis_naim.plus(gettext("Распечатка налоговой накладной"));

        iceb_t_rnfxml(&rek_zag_nn);
        if(xml_nn_usl(&pnd,tipz,dd,md,gd,nomdok.ravno(),nomnn.ravno(),md,gd,md,1,vid_dok.ravno(),&imaf_nn_xml) == 0)
         {
          oth.spis_imaf.plus(imaf_nn_xml.ravno());
          oth.spis_naim.plus(gettext("Налоговая накладная в формате XML"));
         }

        iceb_t_rabfil(&oth,"");

        break;

      case 2:
        oth.clear();

        oth.spis_imaf.plus(imaf.ravno(2));
        oth.spis_ms.plus(imaf.ravno(2));
        oth.spis_naim.plus(gettext("Распечатка счёта"));
        iceb_t_rabfil(&oth,"");

        break;

      case 3:
        sozfusal(dd,md,gd,podr,nomdok.ravno(),tipz,lnds,&imaf,mvnp);
        oth.clear();

        oth.spis_imaf.plus(imaf.ravno(3));
        oth.spis_ms.plus(imaf.ravno(3 ));
        oth.spis_naim.plus(gettext("Распечатка счёта с учётом сальдо"));
        iceb_t_rabfil(&oth,"");
        break;

      default:
       break;
     }

    if(kolstr > 0)
      goto naz;
    else
     goto lll;
  
  case FK6: /*Просмотр подтверждающих записей*/
  
    dirpodu(metka,kodzap);
    goto naz;

  case FK7:  /*Запись подтверждающей записи*/
    if(prblok(1,md,gd,blokpid) != 0)
      goto naz;
   
   

    VV.VVOD_delete();
        
    kolihp=readkolu(dd,md,gd,podr,nomdok.ravno(),tipz,metka,kodzap,nomzap[kom]);
    if(fabs(kolih- kolihp) < 0.00001)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Запись уже подтверждена !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;

     }
    VV.VVOD_SPISOK_add_MD(gettext("Дата........."));
    VV.VVOD_SPISOK_add_MD(gettext("Количество..."));
    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(20);

    if(kolihp == 0.)
     {
      sprintf(strsql,"%d.%d.%d",dd,md,gd);
      VV.VVOD_SPISOK_zapis_data(0,strsql);
     }
    sprintf(strsql,"%.10g",kolih-kolihp);
    VV.VVOD_SPISOK_zapis_data(1,strsql);

nz:;

    helstr(LINES-1,0,"F2/+",gettext("запись"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);
    switch(kom1)
     {
      case FK10:
      case ESC:
         goto naz;
      case FK2:
      case PLU:
        if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(0),0) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата !"));
     	  goto nz;
         }
        
        if(iceb_t_pbpds(m,g) != 0)
          goto nz;

        if(SRAV1(g,m,d,gd,md,dd) > 0)
         {
          iceb_t_soob(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"));
          goto nz;
         }

        if(kolih-kolihp - ATOFM(VV.VVOD_SPISOK_return_data(1)) < -0.00001)
         {
          iceb_t_soob(gettext("Не верно введено количество !"));
   	  goto nz;
         }


        GDITE();
        kolih=VV.data_ravno_atof(1);
	zappodu(tipz,dd,md,gd,d,m,g,nomdok.ravno(),metka,kodzap,kolih,cena,ei.ravno(),shetu.ravno(),podr,nomzap[kom]);
        kom++;

        goto naz1;

      default:
       goto nz;
     }

  case SFK7:  /*Подтверждение всего документа*/

    if(prblok(1,md,gd,blokpid) != 0)
     goto naz;

    podtudok(dd,md,gd,podr,nomdok.ravno(),tipz);
    goto naz1;
 
  case FK8:  /*Меню*/

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Найти нужный код записи"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку даты"));//1
    if(blokpid == 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Заблокировать документ"));//2
    if(blokpid != 0)
      MENU.VVOD_SPISOK_add_MD(gettext("Разблокировать документ"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Снять подтверждение со всего документа"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести сумму или процент корректировки"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Копировать записи из других документов"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр приложения к документу"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Добавить к наименованию услуги"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Создать документ в материальном учёте"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//10

    clearstr(LINES-1,0);

    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 10 :
      case -1 :
        if(kolstr > 0)
          goto naz;
        else
          goto lll;

      case 0 :

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);


        bros[0]='\0';
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD("Введите код записи");
        vstr.new_plus("");
        if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
         {
          if(kolstr > 0)
            goto naz;
          else
            goto lll;
         }
        if(i == ESC)
         {
          if(kolstr > 0)
            goto naz;
          else
            goto lll;
         }
	kom=0;
        kodzap=vstr.ravno_atoi();
        pozz=0;
        cur.poz_cursor(pozz);

        while(cur.read_cursor(&row) != 0)
         {
          if(kodzap == atoi(row[1]))
            break;
          pozz++;          
         }

        goto naz;  

      case 1 :
        iceb_tl_blok();

        if(kolstr > 0)
          goto naz;
        else
         goto lll;

      case 2 : /*Установить / снять блокировку с документа*/


          if(blokpid != iceb_t_getuid())
           if(prblok(0,md,gd,blokpid) != 0)
            {
             if(kolstr > 0)
              goto naz;
             else
               goto lll;
            }

          if(blokpid == 0)
           blokpid=iceb_t_getuid();
          else
           blokpid=0;

          memset(strsql,'\0',sizeof(strsql)); 
          sprintf(strsql,"update Usldokum set blokir=%d \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
          blokpid,gd,md,dd,podr,nomdok.ravno(),tipz);

          sql_zapis(strsql,0,0);

          if(kolstr > 0)
           goto naz;
          else
            goto lll;

       case 3 : /*Снять подтверждение с документа*/

        if(blokpid != iceb_t_getuid())
        if(prblok(0,md,gd,blokpid) != 0)
            goto naz;  
        sprintf(strsql,"delete from Usldokum2 where datd='%d-%02d-%02d' \
and nomd='%s' and podr=%d and tp=%d",gd,md,dd,nomdok.ravno(),podr,tipz);

        sql_zapis(strsql,0,0);
        if(kolstr > 0)
         goto naz;
        else
          goto lll;

      case 4 :  /*Ввести сумму корректировки*/
        if(prblok(1,md,gd,blokpid) != 0)
          goto naz;

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму или процент (%%)"));
        vstr.new_plus("");
        if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
          goto naz;
        if(i == ESC)
          goto naz;

        deletdokumu(&imaf);
        naimkor.new_plus("");
        sumkor=vstr.ravno_atof();
        if(POLE(vstr.ravno(),strsql,1,'%') == 0)
          sumkor=suma*ATOFM(strsql)/100.;

        if(kolstr > 0)
         goto naz;
        else
          goto lll;

      case 5 :  /*Скопировать записи из других документов*/
        if(prblok(1,md,gd,blokpid) != 0)
          goto naz;

        copdoku(dd,md,gd,nomdok.ravno(),podr,tipz);

        goto naz1;  

      case 6 :  //Просмотр приложения к документа
        GDITE();
        //Установить флаг работы с придожением
        char flag[300];
        sprintf(flag,"usl%s%s%d%d",imabaz,nomdok.ravno(),gd,podr);

        if(sql_flag(&bd,flag,0,0) != 0)
         {
          iceb_t_soob(gettext("С приложением уже работает другой оператор !"));
          goto naz1;
         }
        sprintf(strsql,"prilog%d.tmp",getpid());
	prilog('-',podr,gd,tipz,nomdok.ravno(),strsql,"Usldokum3");
        vizred(strsql);
        GDITE();
	prilog('+',podr,gd,tipz,nomdok.ravno(),strsql,"Usldokum3");
        unlink(strsql);        
        sql_flag(&bd,flag,0,1);
        deletdokumu(&imaf);

        if(kolstr > 0)
         goto naz;
        else
          goto lll;

      case 7 :  //Добавить к наименованиею услуги
        if(kolstr == 0)
          goto lll;
        if(prblok(1,md,gd,blokpid) != 0)
          goto naz;

        helstr(LINES-1,0,"F10",gettext("выход"),NULL);

        dnaim.new_plus(row[6]);

        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дополнение к наименованию"));

        if(vvod1(&dnaim,112,&VVOD1,NULL,stdscr,-1,-1) == FK10)
         goto naz;

        sprintf(strsql,"update Usldokum1 set dnaim='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",
        dnaim.ravno_filtr(),gd,md,dd,podr,nomdok.ravno(),metka,kodzap,tipz,nomzap[kom]);

//        printw("\n%s\n",strsql);
//        OSTANOV();
        
        if(sql_zap(&bd,strsql) != 0)
         msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
        deletdokumu(&imaf);
        
        goto naz1;
        
      case 8 :  //Сменить имя принтера
        mprintcap();
        goto naz1;

      case 9 :  //Создать документ в материальном учёте
       usl_sm(tipz,dd,md,gd,podr,lnds,kontr.ravno(),nomdok.ravno(),pnds);
       if(kolstr > 0)
         goto naz;
       else
         goto lll;
        
     }     
    break;

  case SFK8:  /*Снятие блокировки по паролю*/
       if(blokpid != 0)
        {
         clear();

         printw(gettext("Режим снятия блокировки"));

         helstr(LINES-1,0,"F10",gettext("выход"),NULL);



         VVOD1.VVOD_delete();
         VVOD1.VVOD_SPISOK_add_ZAG(gettext("Режим снятия блокировки"));
         VVOD1.VVOD_SPISOK_add_MD(gettext("Введите пароль для снятия блокировки"));
         vstr.new_plus("");
         if(vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
          {
          if(kolstr > 0)
           goto naz;
          else
            goto lll;
          }         
         if(vstr.ravno_atoi() == dt+mt+gt)
          {
           memset(strsql,'\0',sizeof(strsql)); 
           sprintf(strsql,"update Usldokum set blokir=%d \
where datd='%d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
           0,gd,md,dd,podr,nomdok.ravno(),tipz);

           sql_zapis(strsql,0,0);
           blokpid=0;  
          }       
        }

        if(kolstr > 0)
         goto naz;
        else
          goto lll;

  case FK9:  /*Меню*/
    if(tipz == 1)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Ввести сумму НДС для документа"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 1 :
        case -1 :
          if(kolstr > 0)
            goto naz;
          else
            goto lll;

        case 0 :  //Ввести сумму НДС для приходного документа
          if(prblok(1,md,gd,blokpid) != 0)
            goto naz;
          helstr(LINES-1,0,"F10",gettext("выход"),NULL);

          bros[0]='\0';
          if(sumandspr != 0.)
            sprintf(bros,"%.2f",sumandspr);
          vstr.new_plus(bros);
          VVOD1.VVOD_delete();
          VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму НДС"));

          if(vvod1(&vstr,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
            goto naz;
          if(i == ESC)
            goto naz;
          sumandspr=vstr.ravno_atof();
          deletdokumu(&imaf);

          sprintf(strsql,"update Usldokum set sumnds=%.2f where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
          sumandspr,gd,md,dd,podr,nomdok.ravno(),tipz);
           

  //        printw("\n%s\n",strsql);
  //        OSTANOV();
          
          if(sql_zap(&bd,strsql) != 0)
           msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

       }
      if(kolstr > 0)
       goto naz;
      else
       goto lll;
     }      

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Переценить документ на заданный процент"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выбрать вид корректировки суммы по документу"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {

      case 2 :
      case -1 :
        if(kolstr > 0)
          goto naz;
        else
          goto lll;

      case 0 :  /*Переценить накладную на заданный процент*/
       if(prblok(1,md,gd,blokpid) != 0)
            goto naz;
       perecenu(dd,md,gd,nomdok.ravno(),podr,tipz,pnds);
       deletdokumu(&imaf);
       break;          

      case 1: /*Выбрать вид скидки*/
       if(prblok(1,md,gd,blokpid) != 0)
            goto naz;
       MENU.VVOD_delete();
      
       naimkor.new_plus("");
       if(makkor(&MENU,1,bros,&maskor,"uslkor.alx") != 0)
        break;       

       helstr(LINES-1,0,"F10",gettext("выход"),NULL);

       i=0;
       while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

       if( i == -1 || i == FK10)
        {
         if(kolstr > 0)
           goto naz;
         else
          goto lll;
        }
       deletdokumu(&imaf);

       naimkor.new_plus(MENU.VVOD_SPISOK_return_MD(i));
       makkor(&MENU,2,naimkor.ravno(),&maskor,"uslkor.alx");
       if(kolstr > 0)
         goto naz;
       else
        goto lll;


     }
  
     goto naz1;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

if(mpn == 1)
 {
  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ?"));

  if(danet(&DANET,2,stdscr) == 1)
   {
    GDITE();
    if(udprgr(ICEB_MP_USLUGI,dd,md,gd,nomdok.ravno(),podr,tipz) != 0)
     goto naz1;
     
    uduvdok(tipz,dd,md,gd,nomdok.ravno(),podr,0,0,0);
    deletdokumu(&imaf);
   }
 }

kon:;

  
podudok(dd,md,gd,nomdok.ravno(),podr,tipz);

if(mvnp != 0) /*Устанавливаем метку оплаты для внутренних документов*/
 {
  sprintf(strsql,"update Usldokum \
set \
mo=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nomdok.ravno(),tipz);


  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    {
     msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
    }
 }

if(metkaprov == 1)
 return;

if(SRAV(nomdok.ravno(),"000",0) == 0)
 {
  return;
 }

/*Если операция внутренняя и накладная на приход
Проводки выполняются только для расходных накладных*/

if(mvnp != 0 && tipz == 1)
 {
  return;
 }

/*Проверить на выполнение проводок*/
prosprus(3,podr,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds);

}

/*****************/
/*Шапка документа*/
/*****************/
void sdusl(short dd,short md,short gd,const char nomd[],
int podr,const char naimpodr[],const char *kontr,const char naimk[],const char *kodop,
const char *naiop,double suma,
int kolihz, //Количество записей
double kolihu, //Количество услуг/материалов
int blokpid,
const char naimkor[],double *sumkor,
int pro, //0-не выполнены проводки 1-выполнены
double *maskor,
float procent,
int lnds,
int tipz, //1-приход 2-расход
double *sumaitogo,
float pnds)
{
double		bb;
double		proc;
double		sumands=0.;
double		sumandspr=0.;
char		strsql[512];
SQL_str		row;
SQLCURSOR cur1;
if(lnds == 0 && tipz == 1)
 {
  sprintf(strsql,"select sumnds from Usldokum where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nomd,tipz);
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
   sumandspr=atof(row[0]);
 }
 
move(0,0);
printw("%s:%d.%d.%d %s:%s",
gettext("Дата"),dd,md,gd,
gettext("Номер документа"),nomd);

if(naimkor[0] != '\0')
 {
  attron(A_BLINK);
  printw(" %s",naimkor);
  attroff(A_BLINK);
 }

move(1,0);
printw("%s:%s %s",
gettext("Контрагент"),kontr,naimk);
if(blokpid != 0)
 {
  attron(A_BLINK);
  printw(" %s %d",gettext("Документ заблокировано !"),blokpid);
  attroff(A_BLINK);
 } 

if(maskor != NULL && naimkor[0] != '\0')
   *sumkor=makkorr(suma,maskor);

double suma_bez_nds=suma+*sumkor;
suma_bez_nds=okrug(suma_bez_nds,okrg1);

move(2,0);
printw("%s:%d %s",gettext("Подразделение"),podr,naimpodr);
move(3,0);
printw("%s:%s %s",gettext("Операция"),kodop,naiop);
if(tipz == 1)
 printw(" %s",gettext("(приход)"));
if(tipz == 2)
 printw(" %s",gettext("(расход)"));

if(pro == 0)
 {
  move(2,COLS-31);
  attron(A_BLINK);
  printw(gettext("Проводки не сделаны !"));
  attroff(A_BLINK);
 }
proc=0.;
if(suma != 0.)
 proc=(*sumkor)*100/suma;

move(4,0);
printw("%s:%d/%.10g %s:%.2f",
gettext("Количество"),kolihz,kolihu,
gettext("Сумма"),suma);

if(*sumkor != 0.)
 printw("(%.2f %.2f%% %.2f)",*sumkor,proc,suma+*sumkor);

sumands=0.;
if(lnds == 0)
 {
  if(sumandspr != 0.)
   sumands=sumandspr;
  else
   sumands=suma_bez_nds*pnds/100.;
 }

if(sumandspr != 0. && sumands != 0.)
  attron(A_BLINK);

printw("+%.2f",sumands);
attroff(A_BLINK);

bb=*sumaitogo=suma_bez_nds+sumands;
*sumaitogo=okrug(bb,okrg1);
printw("=%.2f",*sumaitogo);

if(procent != 0.)
 {
//  *sumaitogo+=sumands+*sumaitogo*procent/100.;
  *sumaitogo=suma_bez_nds*procent/100.+sumands+suma_bez_nds;
  bb=*sumaitogo;
  *sumaitogo=okrug(bb,okrg1);
  
  printw("/+%.1f%%=%.2f",procent,*sumaitogo);
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("пров."),
"F5",gettext("печать"),
"F6",gettext("карт."),
"F7",gettext("подтв."),
"F8",gettext("меню"),
"F9",gettext("меню"),
"F10",gettext("вых."),NULL);

}
/************************************************/
/*Проверка возможности корректировать накладную*/
/************************************************/
/*Если вернули 0 - можно корректировать
	       1 - нет
*/

short           prblok(short mp, //0-все проверять 1-только блокировку документа
short md,short gd,int blokpid)
{
char		strsql[512];

if(blokpid != 0 )
 {
  sprintf(strsql,"%s %s",gettext("Документ заблокирован"),iceb_t_kszap(blokpid));
  iceb_t_soob(strsql);
  return(1);
 }

if(mp == 1)
 return(0);
 
if(iceb_t_pbpds(md,gd) != 0)
  return(1);
return(0);
}
/*******************************/
/*Подтверждение всего документа*/
/*******************************/
void            podtudok(short dd,short md,short gd,int podr,
const char *nomdok,int tipz)
{
char		strsql[512];
short           d1,m1,g1;
short           i;
class iceb_tu_str vstr("");
move(0,0);

i=0;

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить весь документ ? Вы уверены ?"));

i=danet(&DANET,2,stdscr);

if(i == 2)
 return;

sprintf(strsql,"%d.%d.%d",dd,md,gd);
vstr.new_plus(strsql);
helstr(LINES-1,0,"F10",gettext("выход"),NULL);

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату подтверждения"));

if(vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1) == FK10)
   return;


if(rsdat(&d1,&m1,&g1,vstr.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  return;
 }

if(SRAV1(g1,m1,d1,gd,md,dd) > 0)
 {
  iceb_t_soob(gettext("Дата меньше чем дата в документе ! Подтверждение не возможно !"));
  return;
 }
//sprintf(strsql,"%s",iceb_t_imafnsi("uslugbl.alx"));
if(iceb_t_pbpds(m1,g1) != 0)
  return;

clear();
GDITE();
uslpoddok(dd,md,gd,d1,m1,g1,nomdok,podr,tipz,0);

printw("\n");
OSTANOV();
}

/***********************/
/*Корректировать запись*/
/***********************/

void korzapu(short dd,short md,short gd, //Дата документа
int tipz, //1-приход 2-расход
int podr, //Подразделение
const char *nomd, //Номер документа
int metka,int kodzap,int nomzap,float pnds)
{
int		i;
double		kolih;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1024];
class iceb_tu_str naimus("");
double		cena;
int     	N,K;
class iceb_tu_str ei("");
class iceb_tu_str shetu("");
double		bb1,cenabn;
struct OPSHET	shetv;
class iceb_tu_str kolihz("");
SQLCURSOR cur1;
VVOD VV(0);

N=K=0;

if(metka == 1)
 {
  sprintf(strsql,"select naius from Uslugi \
  where kodus=%d",kodzap);

  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodzap);
    iceb_t_soob(strsql);
    return;
   }
  naimus.new_plus(row[0]);
 }

if(metka == 0)
 {
  sprintf(strsql,"select naimat from Material \
  where kodm=%d",kodzap);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
    iceb_t_soob(strsql);
    return;
   }
  naimus.new_plus(row[0]);
 }

sprintf(strsql,"select kolih,cena,ei,shetu,shsp,ktoi,vrem from Usldokum1 \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",
gd,md,dd,podr,nomd,metka,kodzap,tipz,nomzap);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
kolihz.new_plus(row[0]);
kolih=atof(row[0]);
cena=atof(row[1]);
bb1=cena+cena*pnds/100.;
bb1=okrug(bb1,okrg1);

VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
kzvz(atoi(row[5]),atol(row[6]),&VV);

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(20);//5

VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Цена с НДС.........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Цена без НДС........"));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.........."));//4
if(tipz == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Счёт получения......"));//5
if(tipz == 2)
 VV.VVOD_SPISOK_add_MD(gettext("Счёт списания......."));//5

sprintf(strsql,"%.10g",kolih);
VV.VVOD_SPISOK_zapis_data(0,strsql);

sprintf(strsql,"%.2f",bb1);
VV.VVOD_SPISOK_zapis_data(1,strsql);

sprintf(strsql,"%.10g",cena);
VV.VVOD_SPISOK_zapis_data(2,strsql);

VV.VVOD_SPISOK_zapis_data(3,row[2]);
VV.VVOD_SPISOK_zapis_data(4,row[3]);
VV.VVOD_SPISOK_zapis_data(5,row[4]);

naz:;


//printw("НДС=%.2f Округление суммы=%f Округление цены=%f\n",
//pnds,okrg1,okrcn);

/*
if(metka == 0)
 printw(gettext("Материал %d %s"),kodzap,naimus);
if(metka == 1)
 printw("Услуга: %d %s",kodzap,naimus);
*/

helstr(LINES-1,0,"F2/+",gettext("запись"),
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

    if(N == 0) /*Количество*/
     {

     }

    if(N == 1) /*Ввод цены с ндс*/
     {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));

      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));
	cenabn=cenabn+cenabn*bb1/100.;
       }
      else
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));

      VV.data_plus(1,cenabn);

      cenabn=cenabn-cenabn*pnds/(100.+pnds);
      cenabn=okrug(cenabn,okrcn);
      VV.data_plus(2,cenabn);
      N+=2;
      goto naz;
     }
    if(N == 2) /*Ввод цены без НДС*/
     {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));
	cenabn=cenabn+cenabn*bb1/100.;
       }
      else
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));

      cenabn=okrug(cenabn,okrcn);
      VV.data_plus(2,cenabn);

      cenabn=cenabn+cenabn*pnds/100.;
      cenabn=okrug(cenabn,okrg1);

      VV.data_plus(1,cenabn);
      goto naz;
            
     }
    if(N == 3) /*Ввод единицы измерения*/
     {

      /*Проверяем единицу измерения*/
      sprintf(strsql,"select kod from Edizmer where kod='%s'",
      VV.VVOD_SPISOK_return_data(3));
      if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        refresh();
        sprintf(strsql,gettext("Не нашли единицу измерения %s !"),VV.VVOD_SPISOK_return_data(3));
        iceb_t_soob(strsql);
        goto naz;
       }
     }
    if(N == 4) /*Ввод Счета*/
     {
      if((i=prsh(VV.VVOD_SPISOK_return_data(4),&shetv))  == 0)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        refresh();
        sprintf(strsql,gettext("Нет счета %s в плане счетов !"),VV.VVOD_SPISOK_return_data(4));
        iceb_t_soob(strsql);
        goto naz;
       }

      if(i == 2)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        refresh();
        sprintf(strsql,gettext("Счёт %s имеет субсчета. Введите нужный субсчет."),VV.VVOD_SPISOK_return_data(4));
        iceb_t_soob(strsql);
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
     break;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      refresh();
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0'
     || VV.VVOD_SPISOK_return_data(3)[0] == '\0' || VV.VVOD_SPISOK_return_data(4)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(4)[0] != '\0')
      if(prsh1(VV.VVOD_SPISOK_return_data(4),&shetv)  != 0)
       goto naz;

     if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
      if(prsh1(VV.VVOD_SPISOK_return_data(5),&shetv)  != 0)
       goto naz;


    /*Проверяем единицу измерения*/
    sprintf(strsql,"select kod from Edizmer where kod='%s'",
    VV.VVOD_SPISOK_return_data(3));
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не нашли единицу измерения %s !"),VV.VVOD_SPISOK_return_data(3));
      iceb_t_soob(strsql);
      goto naz;
     }

    cenabn=ATOFM(VV.VVOD_SPISOK_return_data(2));
    cenabn=okrug(cenabn,okrcn);

    if(cenabn == 0.)
     {
      iceb_t_soob(gettext("Не введена цена продажи !"));
      goto naz;
     }

    GDITE(); 

    /*узнаём хозяина записи*/
    sprintf(strsql,"select ktoi from Usldokum1 where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",gd,md,dd,podr,nomd,metka,kodzap,tipz,nomzap);

    if(readkey(strsql,&row,&cur) == 1)
     if(usl_pvku(atoi(row[0])) != 0)
      goto naz;

    kolih=ATOFM(VV.VVOD_SPISOK_return_data(0));
    ei.new_plus(VV.VVOD_SPISOK_return_data(3));
    shetu.new_plus(VV.VVOD_SPISOK_return_data(4));

    sprintf(strsql,"update Usldokum1 set \
kolih=%.10g,\
cena=%.10g,\
ei='%s',\
shsp='%s',\
shetu='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",kolih,cenabn,ei.ravno(),VV.VVOD_SPISOK_return_data(5),shetu.ravno(),
gd,md,dd,podr,nomd,metka,kodzap,tipz,nomzap);
    if(sql_zap(&bd,strsql) != 0)
     {
      if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
       {
        iceb_t_soob(gettext("Такая запись уже есть !"));
       }
      else
        msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
      goto naz;
     }

    /*Исправляем где количество совпадает*/
    sprintf(strsql,"update Usldokum2 set \
kolih=%.10g \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d and kolih=%s",kolih,
gd,md,dd,podr,nomd,metka,kodzap,tipz,nomzap,kolihz.ravno());
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

    sprintf(strsql,"update Usldokum2 set \
cena=%.10g,\
ei='%s',\
shetu='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",cenabn,ei.ravno(),shetu.ravno(),
gd,md,dd,podr,nomd,metka,kodzap,tipz,nomzap);
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

}
/*********************************/
/*Удаление всех файлов документов*/
/**********************************/

void deletdokumu(class SPISOK *imaf)
{
int	i;
for(i=0; i < imaf->kolih();i++)
 unlink(imaf->ravno(i));

}
/********************************/
/*Если файлов нет то создаем их*/
/********************************/
void sozfu(short dg,short mg,short gg,int podr,const char nomdok[],
int tipz,int lnds,class SPISOK *imaf,
int mvnp) //0-внешняя 1 -внутреняя 2-изменение стоимости
{
FILE            *ff;

GDITE();

/*Создание акта выполненых работ*/
errno=0;

if((ff = fopen(imaf->ravno(0),"r")) == NULL)
 if(errno != ENOENT)
  {
   error_op_nfil(imaf->ravno(0),errno,"");
   return;
  }

if(errno == ENOENT)
 {
  if((ff = fopen(imaf->ravno(0),"w")) == NULL)
    {
     error_op_nfil(imaf->ravno(0),errno,"");
     return;
    }
  startfil(ff);
  rasdoku(dg,mg,gg,podr,nomdok,lnds,0,ff,tipz,0);
  fclose(ff);
 }
else
  fclose(ff);


/*Если операция внутреннего перемещения то налоговая накладная не нужна*/
if(mvnp != 0)
 return;

/*Создание счета без сальдо*/
if((ff = fopen(imaf->ravno(2),"r")) == NULL)
 if(errno != ENOENT)
    {
     error_op_nfil(imaf->ravno(2),errno,"");
     return;
    }

if(errno == ENOENT)
 {
  if((ff = fopen(imaf->ravno(2),"w")) == NULL)
    {
     error_op_nfil(imaf->ravno(2),errno,"");
     return;
    }
  startfil(ff);

  rasdoku(dg,mg,gg,podr,nomdok,lnds,1,ff,tipz,0);
  fclose(ff);
  refresh();
 }
else
  fclose(ff);


}
/*************************/
/*Создание счета с сальдо*/
/*************************/
void            sozfusal(short dg,short mg,short gg,int podr,
const char nomdok[],
int tipz,int lnds,class SPISOK *imaf,
int mvnp) //0-внешняя 1 -внутреняя 2-изменение стоимости
{
FILE	*ff;

GDITE();
/*Создание счета с сальдо*/
if((ff = fopen(imaf->ravno(3),"r")) == NULL)
 if(errno != ENOENT)
   {
    error_op_nfil(imaf->ravno(3),errno,"");
    return;
   }

if(errno == ENOENT)
 {
  if((ff = fopen(imaf->ravno(3),"w")) == NULL)
   {
    error_op_nfil(imaf->ravno(3),errno,"");
    return;
   }
  startfil(ff);

  rasdoku(dg,mg,gg,podr,nomdok,lnds,1,ff,tipz,1);
  fclose(ff);
 }
else
  fclose(ff);
}
/********************************/
/*создание налоговой накладной*/
/*******************************/
int dirudok_snn(short dg,short mg,short gg,int podr,const char *nomdok,int lnds,
const char *imaf)
{
FILE *ff;

if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
    {
     error_op_nfil(imaf,errno,"");
     return(1);
    }
  if(errno == ENOENT)
   {  
    return(rasnalnu(dg,mg,gg,podr,nomdok,imaf,lnds));
   }
 }
else
  fclose(ff);
return(0);
}