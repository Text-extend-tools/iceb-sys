/* $Id: dirnu.c,v 5.203 2014/08/31 06:18:18 sasa Exp $ */
/*28.03.2018	21.09.2000	Белых А.И.	dirnu.c
Ввод и корректировка начисления или удержания
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

class dirnupoi
 {
  public:
  int		metpoi;
  class iceb_tu_str podrpoi;
  class iceb_tu_str tabelpoi;
  class iceb_tu_str fiopoi;
  class iceb_tu_str datpoin,datpoik;
  class iceb_tu_str shetpoi;
  short         ms, gs; //В счёт какого месяца
  class iceb_tu_str komentpoi; //Коментрарий
  class iceb_tu_str kategor;
  double        sumado;
  class iceb_tu_str grupa;
  class iceb_tu_str shet_vb;
  class iceb_tu_str kodbank;

  dirnupoi()
   {
    metpoi=0;
    clear();
   }
  void clear()
   {
    podrpoi.new_plus("");
    tabelpoi.new_plus("");
    fiopoi.new_plus("");
    shetpoi.new_plus("");
    datpoin.new_plus("");
    datpoik.new_plus("");
    ms=gs=0;
    komentpoi.new_plus("");
    kategor.new_plus("");
    sumado=0.;
    grupa.new_plus("");
    shet_vb.new_plus("");
    kodbank.new_plus("");   
   }

 };

void	sapdirnu(short,short,const char*,short,short,double,int,int,double,class dirnupoi*);
void	uddirnu(SQLCURSOR*,int,short,short,short,class dirnupoi*);
void	rasdirnu(short,short,short,short,int,class dirnupoi*);
void	zvb_nadra(short prn,char *zapros,class dirnupoi*);
void	zvb_kreditprom(short prn,char *zapros,class dirnupoi*);
void	zvb_ukrsoc(short,short,short,short,char*,class dirnupoi*);
void	zvb_ukrsib(short prn,char*,class dirnupoi*);
void	zapvsem(short,short,short,short);
void	impzarnu(short,short,short,short);
void	smeshnu(SQLCURSOR*,int,class dirnupoi*);
void	kasdirnu(short,short,short,short,class dirnupoi*);
void	zvb_ukrprom_m(short prn,short knah,short mv,short gv,class dirnupoi *poisk,SQLCURSOR *cur);
void	zvb_ukreksim(short prn,class dirnupoi *poisk,int,SQLCURSOR *cur);
void	zvb_pib(short prn,char*,class dirnupoi *poisk);
void imp_zar_kr(int,int kod_nah,short mes,short god);
void dirnu_sdz(SQLCURSOR *cur,short mp,short gp,int kolstr,class dirnupoi *poisk);
void	zvb_hreshatik(short prn,const char *zapros,class dirnupoi *poisk);
void dirnu_svkm(SQLCURSOR *cur,int kolstr,class dirnupoi *poisk);
void zvb_ibank2ua(short prn,const char *zapros,class dirnupoi *poisk,const char *kod00);
void zvb_alfabank(short prn,const char *zapros,class dirnupoi *poisk);
void zvb_ukrgaz(short prn,const char *zapros,class dirnupoi *poisk);
void zvb_promekonom(short prn,const char *zapros,class dirnupoi *poisk);
void zvb_kievrus(short prn,const char *zapros,class dirnupoi *poisk);
void zvb_corp2(short prn,const char *zapros,struct dirnupoi *poisk);
void zvb_privatbank(short prn,const char *zapros,struct dirnupoi *poisk);
int zarvds(class iceb_tu_str *data,class iceb_tu_str *shet,int *metka_vd);
int dirnu_poi(short *mp,short *gp,class dirnupoi *poisk);

extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern class iceb_tu_str shrpz; //счет расчётов по зарплате
extern int      kol_strok_na_liste;

void	dirnu(short prn,short knah,short mv,short gv)
{
int kod_ban=0;
class iceb_tu_str kod("");
class iceb_tu_str naimk("");
short 		dd=0;
int 		K;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kodmv(""); /*Код вновь введенного*/
int		kom,i,prc,kom1;
short		mspz;
class iceb_tu_str tabnom("");
class iceb_tu_str naim("");
double		sumai=0.,sumaib=0.;
double		sum;
class iceb_tu_str fio("");
short		d,m,g;
short		dn,mn,gn; //Дата приема на работу
short		du,mu,gu; //Дата увольнения с работы
short		mp,gp;    //Дата просмотра
class iceb_tu_str shet(""); //Счёт в карточке
class iceb_tu_str shetzap(""); //Счёт записи
short		kateg;
short		sovm;
short		zvan;
class iceb_tu_str lgot("");
short		prov=0;
class iceb_tu_str dolg("");
int		metka0=0; //0-все записи 1-только с ненулевым значением
class iceb_tu_str kod00("00");
class iceb_t_mstr spis_menu(0);
short metka_zarn=0;
class SPISOK nom_dok; //Список документов
/*Уникальный ключ записи*/
short		dz=0,mz=0,gz=0;
long		tabn=0;
short		godn,mesn;
int		podr=0;
class iceb_tu_str shetnu(""),shetnuz("");
int		nomz=0;
short           metka_sort=0; //0-по табельным номерам 1-по алфавиту
char stroka_menu[1024];
char zapros[1024];
class iceb_tu_str fio_in("");

class dirnupoi poisk;
SQLCURSOR curr;

/*************************/

pozz=pozz1=kom=0;
mp=mv;
gp=gv;


/*Читаем наименование*/
if(prn == 1)
 sprintf(strsql,"select naik,shet from Nash where kod=%d",knah);
if(prn == 2)
 sprintf(strsql,"select naik,shet from Uder where kod=%d",knah);
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  naim.new_plus(row[0]);
  shetnu.new_plus(row[1]);
 }

//Создаем класс меню
class VVOD VV(0);
class VVOD MENU(3);
class VVOD VVOD1(2);
class VVOD DANET(1);

naz1:;
GDITE();

int             kls=LINES-6; /*Количество строк в окне*/

sumai=sumaib=0.;

if(metka0 == 0)
  sprintf(zapros,"select Zarp.*, Kartb.fio from Zarp, Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn",
  gp,mp,gp,mp,prn,knah);


if(metka0 == 1)
  sprintf(zapros,"select Zarp.*, Kartb.fio from Zarp, Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn and suma <> 0.",
  gp,mp,gp,mp,prn,knah);

if(metka_sort == 0)
 strcat(zapros," order by tabn,datz,nomz asc");
if(metka_sort == 1)
 strcat(zapros," order by fio,datz,nomz asc");
 

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }  

if(kolstr == 0)
 {

lll:;

  sapdirnu(prn,knah,naim.ravno(),mp,gp,sumai,metka0,kolstr,sumaib,&poisk);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK3 || K == ENTER || K == SFK2 || K == FK5)
     goto lll;

  if(K == KEY_RESIZE) //Изменение размеров экрана
   goto naz1;
   
  goto vper;

 }
else
  while(cur.read_cursor(&row) != 0)
   {
   
    if(poidirnu(row,&poisk,&fio) != 0)
     continue;

    sum=atof(row[4]);
    sumai+=sum;
    if(prn == 1)
     if(proverka(shetb,row[5],0,1) == 0)
       sumaib+=sum;
    if(prn == 2)
     if(proverka(shetbu,row[5],0,1) == 0)
       sumaib+=sum;
    
   }

if(kodmv.getdlinna() > 1 && kolstr > kls  && pozz < kolstr-kls)
 {
  pozz=poivcur(kodmv.ravno(),1,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

nom_dok.free_class(); //обнуляем список документов

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  
  if(poidirnu(row,&poisk,&fio) != 0)
     continue;

  iceb_tu_fio(fio.ravno(),&fio_in);
  
  rsdat(&d,&m,&g,row[0],2);
  
  nom_dok.plus(row[14]);
  sprintf(stroka_menu,"%-5s|%-*.*s|%02d|%-*s|%02d.%04d|%-4s|%s|%8.2f|%*.*s|%s",
  row[1],
  iceb_tu_kolbait(15,fio_in.ravno()),iceb_tu_kolbait(15,fio_in.ravno()),fio_in.ravno(),
  d,
  iceb_tu_kolbait(6,row[5]),row[5],
  atoi(row[7]),
  atoi(row[6]),
  row[13],
  row[9],
  atof(row[4]),
  iceb_tu_kolbait(3,row[12]),
  iceb_tu_kolbait(3,row[12]),
  row[12],
  row[14]);


  spis_menu.spis_str.plus(stroka_menu);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapdirnu(prn,knah,naim.ravno(),mp,gp,sumai,metka0,kolstr,sumaib,&poisk);


prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,3,0,NULL,strsql,prc,&K,"",COLS-23);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(poidirnu(row,&poisk,&fio) != 0)
     continue;
    i++;
   }
  kom=kls;
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

    if(poidirnu(row,&poisk,&fio) != 0)
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
  printw("\n вперед pozz=%d\n",pozz);
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
    cur.read_cursor(&row);

    if(poidirnu(row,&poisk,&fio) != 0)
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
   goto naz;

  polen(str_vib.ravno(),&tabn,1,'|');
  polen(str_vib.ravno(),strsql,sizeof(strsql),3,'|');
  dz=atoi(strsql);
  mz=mp;
  gz=gp;
  polen(str_vib.ravno(),&shetnuz,4,'|');
  polen(str_vib.ravno(),strsql,sizeof(strsql),5,'|');
  rsdat1(&mesn,&godn,strsql);
  polen(str_vib.ravno(),&podr,6,'|');
  polen(str_vib.ravno(),&nomz,7,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_8_1.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case FK2:
  case PLU:
    if(dirnuprovb(mp,gp,0) != 0)
     goto naz;

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
    tabnom.new_plus("");
     
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите табельный номер"));
fk2:;
    if((kom1=vvod1(&tabnom,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      {
       if(kolstr > 0)
         goto naz;
       else
         goto lll;
      }
    if(kom1 == ESC)
      {
       if(kolstr > 0)
         goto naz;
       else
         goto lll;
      }
    if(tabnom.ravno()[0] == '\0')
     {
      int tabn=0;
      fio_in.new_plus("");
      if(dirtab(&tabn,&fio_in,0,1) == 0)
       {
        tabnom.new_plus(tabn);        
        clear();
        goto fk2;
       }  

      if(kolstr > 0)
         goto naz;
      else
         goto lll;

     }

    GDITE();

    //Проверяем блокировку карточки
    metka_zarn=0;
    sprintf(strsql,"select shet,blok from Zarn where god=%d and mes=%d and \
tabn=%s",gp,mp,tabnom.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
       if(atoi(row1[1]) != 0)
        {
         sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
         iceb_t_soob(strsql);

         if(kolstr > 0)
           goto naz;
         else
           goto lll;
        }
      metka_zarn=1;
     }    
    kodmv.new_plus("");
    /*Читаем карточку*/
    sprintf(strsql,"select fio,datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%s",tabnom.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),tabnom.ravno());
      iceb_t_soob(strsql);

      if(kolstr > 0)
        goto naz;
      else
        goto lll;
     }

    fio.new_plus(row1[0]);
    rsdat(&dn,&mn,&gn,row1[1],2);
    rsdat(&du,&mu,&gu,row1[2],2);
    shet.new_plus(row1[3]);
    podr=atoi(row1[4]);
    kateg=atoi(row1[6]);
    sovm=atoi(row1[7]);
    zvan=atoi(row1[8]);
    lgot.new_plus(row1[9]);
    dolg.new_plus(row1[10]);

    kodmv.new_plus(tabnom.ravno());
    tabn=tabnom.ravno_atoi();

    /*Проверяем записан ли стандартный набор*/
    provzzag(mp,gp,tabn,prn,podr);

    if(vnu(tabn,knah,prn,0,mp,gp,mp,gp,podr,shet.ravno(),0) == 0)
     {
      zarsoc(mp,gp,tabn); //запускать и для удержаний так как может быть добавлено удержание при наличии которого считается и отчисления в соц. фонд
      zaravpr(tabn,mp,gp,NULL);
      if(metka_zarn == 0)
       zapzarn(mp,gp,podr,tabn,kateg,sovm,zvan,shet.ravno(),lgot.ravno(),dn,mn,gn,du,mu,gu,prov,dolg.ravno());

      //Проверяем есть ли не нулевые
      sprintf(strsql,"select datz from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%ld and prn='%d' and knah=%d and suma <> 0.",
       gp,mp,gp,mp,tabn,prn,knah);
       if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
        {      
        //Удаляем нулевые начисления/удержания
        sprintf(strsql,"delete from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%ld and prn='%d' and knah=%d and suma=0.",
         gp,mp,gp,mp,tabn,prn,knah);
         if(sql_zap(&bd,strsql) != 0)
           msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
        }
     }
    

    goto naz1;

  case SFK2: //Корректировка записи
  case ENTER:

    GDITE();
    if(dirnuprovb(mp,gp,tabn) != 0)
     goto naz;


    if(vnu(tabn,knah,prn,dz,mz,gz,mesn,godn,podr,shetnuz.ravno(),nomz) == 0)
     {
      if(prn == 1)
       zarsoc(mp,gp,tabn);
      zaravpr(tabn,mp,gp,NULL);
     }

    clear();
    goto naz1;

  case FK3: //Удалить запись

    if(dirnuprovb(mp,gp,tabn) != 0)
     goto naz;
    if(nom_dok.ravno(kom)[0] != '\0')
     {
      iceb_t_soob(gettext("Записи сделанные из нарядов, удаляются только в нарядах !"));
      goto naz;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      zarudnu(gz,mz,dz,tabn,prn,knah,godn,mesn,podr,shetnuz.ravno(),nomz);

      kodmv.new_plus("");


      if(prn == 1)
        zarsoc(mp,gp,tabn);
      zaravpr(tabn,mp,gp,NULL);
      
      goto naz1;
     }           
    goto naz;
    
  case SFK3: //Удалить нулевые записи

    if(dirnuprovb(mp,gp,0) != 0)
     goto naz;

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Удалить записи с нулевыми суммами"));
    MENU.VVOD_SPISOK_add_MD(gettext("Удалить все записи"));
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

        DANET.VVOD_delete();
        DANET.VVOD_SPISOK_add_MD(gettext("Удалить записи с нулевыми суммами ? Вы уверены ?"));

        if(danet(&DANET,2,stdscr) == 1)
         {
          GDITE();
          sprintf(strsql,"delete from Zarp where datz>='%04d-%d-01' and \
datz <= '%04d-%d-31' and prn='%d' and knah=%d and suma=0.",
gp,mp,gp,mp,prn,knah);

          sql_zapis(strsql,0,0);

          kodmv.new_plus("");

          goto naz1;

      case 1 :
        DANET.VVOD_delete();
        DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

        if(danet(&DANET,2,stdscr) == 2)
         goto naz;
         
        uddirnu(&cur,kolstr,mp,gp,prn,&poisk);
        goto naz1;
      }
     break;

     }
    goto naz;
   
  case FK4: //Поиск
    dirnu_poi(&mp,&gp,&poisk);
    pozz=0;
    goto naz1;

  case FK5:  //Распечатка
    rasdirnu(prn,knah,mp,gp,metka_sort,&poisk);
    goto naz;

  case SFK5:  //Переключение сортировки записей
    metka_sort++;
    if(metka_sort == 2)
      metka_sort=0;
      
    goto naz1;

  case FK6:  //Ввод всем
    if(dirnuprovb(mp,gp,0) != 0)
     goto naz;
    zapvsem(prn,knah,mp,gp);
    goto naz1;

  case SFK6:  //Импорт начисления/удержания

      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

      MENU.VVOD_SPISOK_add_MD(gettext("Импорт из файла"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Импорт из подсистемы \"Учёт командировочных расходов\""));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

      clearstr(LINES-1,0);
      kom1=0;        
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 2:
        case -1:
         break;

        case 0:
         impzarnu(prn,knah,mp,gp);
         break;

        case 1:
         imp_zar_kr(prn,knah,mp,gp);
         break;
       }

    goto naz1;

  case FK7:  //Распечатка с выгрузкой на дискету

    if((kom1=zarbanks(&kod00,&kod_ban)) <= 0)
      goto naz;
      
    switch (kom1)
     {
      case -1:
       break;

      case 1:
        zvb_pib(prn,zapros,&poisk);
        break;
      case 2 :
        zvb_nadra(prn,zapros,&poisk);
        break;
      case 3 :
        zvb_ukrsoc(prn,knah,mp,gp,zapros,&poisk);
        break;
      case 4 :
        zvb_ukrsib(prn,zapros,&poisk);
        break;
      case 5 :
        zvb_kreditprom(prn,zapros,&poisk);
        break;

      case 6:
        zvb_ukrprom_m(prn,knah,mp,gp,&poisk,&cur);
        break;

      case 7:
        zvb_ukreksim(prn,&poisk,kolstr,&cur);
        break;
      case 8:
        zvb_hreshatik(prn,zapros,&poisk);
        break;
      case 9:
        zvb_ibank2ua(prn,zapros,&poisk,kod00.ravno());
        break;

      case 10:
        zvb_alfabank(prn,zapros,&poisk);
        break;

      case 11:
        zvb_ukrgaz(prn,zapros,&poisk);
        break;

      case 12:
        zvb_promekonom(prn,zapros,&poisk);
        break;

      case 13:
        zvb_kievrus(prn,zapros,&poisk);
        break;

      case 14:
        zvb_corp2(prn,zapros,&poisk);
        break;
      case 15:
        zvb_privatbank(prn,zapros,&poisk);
        break;
     }
    goto naz;

  case SFK7:  //Создание кассовых ордеров
    kasdirnu(prn,knah,mp,gp,&poisk);
    goto naz;

  case FK8:  //Переключение режима просмотра
    metka0++;
    if(metka0 == 2)
      metka0=0;
      
    goto naz1;


  case SFK8:  //Кооректировка номера счета
    if(dirnuprovb(mp,gp,0) != 0)
     goto naz;

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Смена счёта"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Смена дня"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Смена реквизита \"В счёт какого месяца\""));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3
    clearstr(LINES-1,0);
    kom1=0;        
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
        break;

      case 0:
        smeshnu(&cur,kolstr,&poisk);
        break;
      case 1:
        dirnu_sdz(&cur,mp,gp,kolstr,&poisk);
        break;
      case 2:
        dirnu_svkm(&cur,kolstr,&poisk);
        break;
     }
    goto naz1;

  case FK9:  //Уменьшить дату
    dpm(&dd,&mp,&gp,4);    
    goto naz1;

  case SFK9:  //Увеличить дату
    dpm(&dd,&mp,&gp,3);    
    goto naz1;

  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
 
}
/********/
/*Шапка*/
/********/

void	sapdirnu(short prn,short knah,const char *naim,short mp,short gp,\
double sumai,
int metka0, //0-все записи 1-только с не нулевыми значениями
int kolstr, //Количество записей в курсоре
double sumaib,
class dirnupoi *poisk)
{
int		Y,X;

clear();

move(0,0);
printw("%s %d.%d%s %s:%.2f/%.2f/%.2f(%d)",gettext("Дата"),mp,gp,
gettext("г."),
gettext("Сумма всех записей"),sumai,sumaib,sumai-sumaib,kolstr);
if(metka0 == 1)
 { 
  attron(A_BLINK);
  printw(" %s !",gettext("Не нулевые записи"));
  attroff(A_BLINK);
 }

move(1,0);
if(prn == 1)
 printw(gettext("Начисление"));
if(prn == 2)
 printw(gettext("Удержание"));
printw(":%d %s",knah,naim);

move(2,0);
printw("    1         2         3    4       5     6   7    8      9   10");

mvprintw(Y=1,X=COLS-19,gettext("Расшифровка полей:"));
mvprintw(++Y,X,gettext("1-таб.номер"));
mvprintw(++Y,X,gettext("2-фамилия"));
mvprintw(++Y,X,gettext("3-день"));
mvprintw(++Y,X,gettext("4-счет"));
mvprintw(++Y,X,"5-%s",gettext("в счёт месяца"));
mvprintw(++Y,X,gettext("6-подразделение"));
mvprintw(++Y,X,gettext("7-ном.записи"));
mvprintw(++Y,X,gettext("8-сумма"));
mvprintw(++Y,X,gettext("9-комментарий"));
mvprintw(++Y,X,"10-%s",gettext("номер докум."));

if(poisk->metpoi == 1)
 {
//  attron(A_BLINK);
  mvprintw(Y+=2,X,gettext("Поиск:"));
  if(poisk->podrpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Подр."),poisk->podrpoi.ravno());
   }
  if(poisk->tabelpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Таб.н."),poisk->tabelpoi.ravno());
   }
  if(poisk->fiopoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Фамил."),poisk->fiopoi.ravno());
   }
  if(poisk->datpoin.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("День н."),poisk->datpoin.ravno());
   }
  if(poisk->datpoik.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("День к."),poisk->datpoik.ravno());
   }
  if(poisk->kategor.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Катег."),poisk->kategor.ravno());
   }
  if(poisk->shetpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Счёт"),poisk->shetpoi.ravno());
   }
  if(poisk->ms != 0 && poisk->gs != 0)
   {
    move(++Y,X);
    printw("%s:%d.%d",gettext("В счёт мес."),poisk->ms,poisk->gs);
   }
  if(poisk->komentpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Ком-рий"),poisk->komentpoi.ravno());
   }
  if(poisk->sumado != 0.)
   {
    move(++Y,X);
    printw("%s:%.2f",gettext("Сумма"),poisk->sumado);
   }
  if(poisk->grupa.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Группа"),poisk->grupa.ravno());
   }
  if(poisk->shet_vb.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Счёт в б."),poisk->shet_vb.ravno());
   }
  if(poisk->kodbank.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%.12s",gettext("Код банка"),poisk->kodbank.ravno());
   }

//  attroff(A_BLINK);
  
 }
helstr(LINES-1,0,"1",gettext("помощь"),
"2",gettext("запись"),
"3",gettext("удалить"),
"4",gettext("поиск"),
"5",gettext("печать"),
"6",gettext("запись"),
"7",gettext("банк"),
"8",gettext("сорт."),
"9",gettext("смен.дат."),
"10",gettext("выход"),NULL);


}
/**********************/
/*Проверка блокировок*/
/*********************/
int	dirnuprovb(short mp,short gp,int tabn)
{
char		strsql[512];

if(iceb_t_pbpds(mp,gp) != 0)
 return(1);

if(iceb_t_pvglkni(mp,gp) != 0)
 return(1);

if(tabn != 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,tabn);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   if(atoi(row[0]) != 0)
      {
       sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
       iceb_t_soob(strsql);
       return(1);
      }

 }
return(0);

}

/*************************/
/*Проверка условий поиска*/
/*************************/

int poidirnu(SQL_str row,
class dirnupoi *poisk,
class iceb_tu_str *fio)  //Возвращаемое значение
{
char		strsql[1024];
SQL_str		row1;
class iceb_tu_str podr("");
short		d,m,g;
class iceb_tu_str kateg("");
class iceb_tu_str bankshet("");

SQLCURSOR curr;
fio->new_plus("");
/*Читаем фамилию*/
sprintf(strsql,"select fio,podr,kateg,bankshet,kb from Kartb where tabn=%s",row[1]);

if(readkey(strsql,&row1,&curr) == 1)
 {
  fio->new_plus(row1[0]);
  podr.new_plus(row1[1]);
  kateg.new_plus(row1[2]);
  bankshet.new_plus(row1[3]);

  if(proverka(poisk->kodbank.ravno(),row1[4],0,0) != 0)
    return(1);
 }

if(poisk->metpoi == 0)
 return(0);

if(poisk->shet_vb.ravno()[0] == '+')
 if(bankshet.getdlinna() <= 1)
  return(1);

if(poisk->shet_vb.ravno()[0] == '-')
 if(bankshet.getdlinna() > 1)
  return(1);
  
if(proverka(poisk->podrpoi.ravno(),podr.ravno(),0,0) != 0)
  return(1);


if(proverka(poisk->tabelpoi.ravno(),row[1],0,0) != 0)
  return(1);


if(proverka(poisk->kategor.ravno(),kateg.ravno(),0,0) != 0)
  return(1);

if(proverka(poisk->shetpoi.ravno(),row[5],0,0) != 0)
  return(1);

if(proverka(poisk->fiopoi.ravno(),fio->ravno(),4,0) != 0)
 return(1);
/*************** 
if(poisk->fiopoi.getdlinna() > 1)
  if(strstrm(fio->ravno(),poisk->fiopoi.ravno()) == 0)
     return(1);
*******************/
if(proverka(poisk->komentpoi.ravno(),row[12],4,0) != 0)
 return(1);
/*****************
if(poisk->komentpoi.getdlinna() > 1)
  if(strstrm(row[12],poisk->komentpoi.ravno()) == 0)
     return(1);
***************/

d=m=g=0;
if(poisk->datpoin.ravno()[0] != '\0')
 {
  rsdat(&d,&m,&g,row[0],2);
  if(d < poisk->datpoin.ravno_atoi())
    return(1);
 }

if(poisk->datpoik.ravno()[0] != '\0')
 {
  if(d == 0)
   rsdat(&d,&m,&g,row[0],2);
  if(d > poisk->datpoik.ravno_atoi())
    return(1);
 }
if(poisk->ms != 0 && poisk->gs != 0)
 {
  if(atoi(row[7]) != poisk->ms || atoi(row[6]) != poisk->gs)
    return(1);
 }

if(poisk->sumado != 0. && poisk->sumado < atof(row[4]))
  return(1);

if(poisk->grupa.getdlinna() > 1)
 {
  sprintf(strsql,"select grup from Podr where kod=%s",podr.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   if(proverka(poisk->grupa.ravno(),row1[0],0,0) != 0)
     return(1);
 }


return(0);
}

/*************/
/*Распечатка */
/*************/

void rasdirnu(short prn,short knah,short mv,
short gv,
int metka_sort,
class dirnupoi *poisk)
{
SQL_str		row,row1;
class iceb_tu_str fio("");
class iceb_tu_str famil("");
class iceb_tu_str imq("");
class iceb_tu_str otch("");
double		sumai=0.,sum=0.;;
char		imaf[64],imafved[64];
char		imafpodr[64];
FILE		*ff=NULL,*ffved=NULL,*ffpodr=NULL;
char		strsql[1024];
class  tm      *bf;
time_t		tmm;
short		d,m,g;
class iceb_tu_str naim("");
int		kolstr=0,kolstr1=0;
class iceb_tu_str naipod("");
double          itogo=0.,itogoo=0.;
class iceb_tu_str podrz("");
class iceb_tu_str nairas("");
short		metkasap=0;
int		kolstrok=0;
int		kollist=0;
class SQLCURSOR curr;
static class iceb_tu_str data_zar("");
static class iceb_tu_str shet_zar(shrpz);
static int metka_vd=0; /*0-ведомость на зарплату по подразделениям 1-всех в одной ведомости 2-каждого в отдельной ведомости*/
short mr=0,gr=0;
short dt=0,mt=0,gt=0;

poltekdat(&dt,&mt,&gt);

if(data_zar.getdlinna() <= 1)
 { 
  sprintf(strsql,"%02d.%d",mv,gv);
  data_zar.new_plus(strsql);
 }

if(zarvds(&data_zar,&shet_zar,&metka_vd) != 0)
 return;

rsdat1(&mr,&gr,data_zar.ravno());

GDITE();


if(metka_sort == 0)
  sprintf(strsql,"select Zarp.*, Kartb.fio from Zarp, Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn and suma <> 0. \
order by podr,tabn,datz,nomz asc",
  gv,mv,gv,mv,prn,knah);
if(metka_sort == 1)
  sprintf(strsql,"select Zarp.*, Kartb.fio from Zarp,Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn and suma <> 0. \
order by podr,Kartb.fio,datz,nomz asc",
  gv,mv,gv,mv,prn,knah);

//printw("%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


/*Читаем наименование*/
if(prn == 1)
 sprintf(strsql,"select naik from Nash where kod=%d",knah);
if(prn == 2)
 sprintf(strsql,"select naik from Uder where kod=%d",knah);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 naim.new_plus(row[0]);

sprintf(imaf,"rasnu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

class masiv_din_double SUMPODR(0);
class masiv_din_int KOLLPODR(0);

sprintf(imafved,"rasved%d.lst",getpid());
if((ffved = fopen(imafved,"w")) == NULL)
 {
  error_op_nfil(imafved,errno,"");
  return;
 }

sprintf(imafpodr,"rpodr%d.lst",getpid());
if((ffpodr = fopen(imafpodr,"w")) == NULL)
 {
  error_op_nfil(imafpodr,errno,"");
  return;
 }
sapreestp(mv,gv,0,ffpodr);


time(&tmm);
bf=localtime(&tmm);


if(prn == 1)
 nairas.new_plus(gettext("Распечатка начислений"));
if(prn == 2)
 nairas.new_plus(gettext("Распечатка удержаний"));

fprintf(ff,"%s %s %d.%d%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
nairas.ravno(),
gettext("за"),
mv,gv,
gettext("г."),

gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(prn == 1)
 fprintf(ff,gettext("Начисление"));
if(prn == 2)
 fprintf(ff,gettext("Удержание"));
fprintf(ff,":%d %s\n",knah,naim.ravno());

if(poisk->metpoi == 1)
 {
  if(poisk->podrpoi.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Подразделение"),poisk->podrpoi.ravno());
  if(poisk->tabelpoi.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Табельный номер"),poisk->tabelpoi.ravno());
  if(poisk->fiopoi.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Фамилия"),poisk->fiopoi.ravno());
  if(poisk->datpoin.ravno()[0] != '\0')
   fprintf(ff,"%s: %s\n",gettext("День начала поиска"),poisk->datpoin.ravno());
  if(poisk->datpoik.ravno()[0] != '\0')
   fprintf(ff,"%s: %s\n",gettext("День конца поиска"),poisk->datpoik.ravno());
  if(poisk->kategor.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Категория"),poisk->kategor.ravno());
  if(poisk->shetpoi.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Счёт"),poisk->shetpoi.ravno());
  if(poisk->shet_vb.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Карт-счёт"),poisk->shet_vb.ravno());
  if(poisk->ms != 0)
   fprintf(ff,"%s:%02d.%d\n",gettext("В счёт какого месяца"),poisk->ms,poisk->gs);
  if(poisk->grupa.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Код группы подразделения"),poisk->grupa.ravno());
  if(poisk->komentpoi.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Коментарий"),poisk->komentpoi.ravno());
 }

fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Т/н |       Фамилия      |День| Счёт | Дата  |Под.|Н/з|  Сумма   |Коментарий\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

sumai=0.;
kolstr1=0;
podrz.new_plus("");
int nomer_ved=0;

if(metka_vd == 1) /*всех в одной ведомости*/
 {
  kolstrok=0;
  if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&kolstrok,ffved) != 0)
    return;
  sspp1(ffved,1,&kolstrok);
  kollist=1;
 }

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  if(metka_vd == 0)    
  if(SRAV(podrz.ravno(),row[13],0) != 0)
   {
    if(podrz.ravno()[0] != '\0' && metkasap != 0)    
     {
      fprintf(ffpodr,"%3s %-*.*s %10.2f\n",podrz.ravno(),
      iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),
      itogo);
      fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(55,gettext("Итого по подразделению")),gettext("Итого по подразделению"),itogo);

      itog(ffved,itogo);

      SUMPODR.plus(itogo,-1);
      KOLLPODR.plus(kollist,-1);
      itogo=0.;
      nomer_ved=0;
     }

    podrz.new_plus(row[13]);
    metkasap=0;
   }

  rsdat(&d,&m,&g,row[0],2);

  sum=atof(row[4]);
  if(sum == 0.)
    continue;
  if(prn == 2)
    sum*=-1;
  
  polen(fio.ravno(),&famil,1,' ');
  polen(fio.ravno(),&imq,2,' ');
  polen(fio.ravno(),&otch,3,' ');
  sprintf(strsql,"%s %*.*s. %*.*s.",
  famil.ravno(),
  iceb_tu_kolbait(1,imq.ravno()),
  iceb_tu_kolbait(1,imq.ravno()),
  imq.ravno(),
  iceb_tu_kolbait(1,otch.ravno()),
  iceb_tu_kolbait(1,otch.ravno()),
  otch.ravno());
  
  fprintf(ff,"%-5s|%-*.*s|%4d|%-*s|%02d.%d|%4s|%3s|%10.2f|%s\n",
  row[1],
  iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql,
  d,
  iceb_tu_kolbait(6,row[5]),row[5],
  atoi(row[7]),atoi(row[6]),row[13],row[9],
  sum,row[12]);

  if(metkasap == 0 and metka_vd == 0)
   {
    //Узнаем наименование подразделения
    naipod.new_plus("");
    sprintf(strsql,"select naik from Podr where kod=%s",row[13]);
    if(readkey(strsql,&row1,&curr) == 1)
     {
      naipod.new_plus(row1[0]);
      strcpy(strsql,naipod.ravno_udsimv("#"));
      naipod.new_plus(strsql);
     }


    if(SUMPODR.kolih() > 0)
      fprintf(ffved,"\f");
    kolstrok=0;
    if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&kolstrok,ffved) != 0)
      return;
    sspp1(ffved,1,&kolstrok);
    kollist=1;
   }

  kolstrok+=2;

  if(kolstrok > kol_strok_na_liste)
   {
    kollist++;
    kolstrok=2;
    sspp1(ffved,kollist,&kolstrok);
   }

  if(metka_vd == 2) /*каждого в отдельной ведомости*/
   {
    //Узнаем наименование подразделения
    naipod.new_plus("");
    sprintf(strsql,"select naik from Podr where kod=%s",row[13]);
    if(readkey(strsql,&row1,&curr) == 1)
     {
      naipod.new_plus(row1[0]);
      strcpy(strsql,naipod.ravno_udsimv("#"));
      naipod.new_plus(strsql);
     }

    if(SUMPODR.kolih() > 0)
     fprintf(ffved,"\f");

    kolstrok=0;
    if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&kolstrok,ffved) != 0)
      return;
    sspp1(ffved,1,&kolstrok);
    kollist=1;
   }

  zarstved(&nomer_ved,atoi(row[1]),fio.ravno(),sum,ffved); /*вывод строки в ведомость*/

  if(metka_vd == 2) /*каждого в отдельной ведомости*/
   {
    itog(ffved,sum);

    SUMPODR.plus(sum,-1);
    KOLLPODR.plus(kollist,-1);
   }

  metkasap++;
  sumai+=sum;
  itogo+=sum; 
  itogoo+=sum; 


 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(55,gettext("Итого по подразделению")),gettext("Итого по подразделению"),itogo);
 
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(55,gettext("Итого")),gettext("Итого"),sumai);

if(metkasap != 0)    
 {
  if(metka_vd != 2)
   itog(ffved,itogo);
  SUMPODR.plus(itogo,-1);
  KOLLPODR.plus(kollist,-1);
 }
fprintf(ffpodr,"%3s %-*.*s %10.2f\n",podrz.ravno(),
iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),itogo);
fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,"%*s %10.2f\n",iceb_tu_kolbait(44,gettext("Итого")),gettext("Итого"),itogoo);

//podpis(ffved); Делаем это в itog
//fprintf(ffved,"\x1b\x6C%c",1); Делаем это ниже

podpis(ffpodr);

fclose(ffpodr);

podpis(ff);
fclose(ff);
fclose(ffved);

sumprved(&SUMPODR,&KOLLPODR,imafved);


class spis_oth oth;
oth.spis_imaf.plus(imaf);
if(prn == 1)
 oth.spis_naim.plus(gettext("Распечатка начислений"));
if(prn == 2)
 oth.spis_naim.plus(gettext("Распечатка удержаний"));

oth.spis_imaf.plus(imafved);
oth.spis_naim.plus(gettext("Распечатка ведомости на выплату зарплаты"));
oth.spis_imaf.plus(imafpodr);
oth.spis_naim.plus(gettext("Свод по подразделениям"));

for(int nomer=0; nomer < oth.spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nomer),1);
 
iceb_t_rabfil(&oth,"");
}

/***************************/
/*Ввести всем              */
/***************************/

void	zapvsem(short prn,short knah,short mv,short gv)
{
int	i=0;
char	strsql[1024];
short	d,m,g;
short	mp,gp;
int	kolstr;
SQL_str         row,row1;
double	suma=0.;
class iceb_tu_str koment("");
int	nomz=0;
long	tabn;
int	podr;
class iceb_tu_str shet("");
int	kateg;
int	sovm;
int	zvan;
class iceb_tu_str lgot("");
short	dn,mn,gn;
short	du,mu,gu;
class iceb_tu_str dolg("");
class iceb_tu_str naim("");
class iceb_tu_str shetu("");
int	K=0,N=0;
int	pol;//0-все 1-мужчина 2-женщина
class ZARP zp;
short	metkaproc=0;
double	proc=0.;
SQLCURSOR curr;
VVOD MENU(3);
VVOD VV(0);


/*Читаем наименование*/
if(prn == 1)
 sprintf(strsql,"select naik,shet from Nash where kod=%d",knah);
if(prn == 2)
 sprintf(strsql,"select naik,shet from Uder where kod=%d",knah);
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  naim.new_plus(row[0]);
  shetu.new_plus(row[1]);
 }

sprintf(strsql,"%d %s",knah,naim.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Сумма.......................")); //0
VV.VVOD_SPISOK_add_MD(gettext("День записи..........(д)....")); //1
VV.VVOD_SPISOK_add_MD(gettext("В счёт какого месяца.(м.г)..")); //2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..................")); //3
VV.VVOD_SPISOK_add_MD(gettext("Номер записи................")); //4
VV.VVOD_SPISOK_add_MD(gettext("Пол........................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения...........")); //6

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
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

    if(N == 5) //Пол
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));
      sprintf(strsql,"%s & %s",gettext("Мужчина"),
      gettext("Женщина"));
      MENU.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,"%s",gettext("Мужчина"));
      MENU.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,"%s",gettext("Женщина"));
      MENU.VVOD_SPISOK_add_MD(strsql);

      pol=0;
      while(menu3w(stdscr,&MENU,&pol,-1,-1,0) != 0);

      switch(pol)
       {
        case -1:
        case 0:
          pol=0;
          VV.VVOD_SPISOK_zapis_data(5,"");
          break;

        case 1:
          VV.VVOD_SPISOK_zapis_data(5,gettext("Мужчина"));
          break;

        case 2:
          VV.VVOD_SPISOK_zapis_data(5,gettext("Женщина"));
          break;
       }
      goto naz;
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(N == 6 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      sprintf(strsql,"select kod from Podr where kod=%s",VV.VVOD_SPISOK_return_data(6));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),VV.VVOD_SPISOK_return_data(6));
        iceb_t_soob(strsql);
        VV.VVOD_SPISOK_zapis_data(6,"");
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

  switch(K)
   {
    case FK1:   /*Помощь*/
     GDITE();
     iceb_t_pdoc("zarp2_8_1_1.txt");
     clear();
     goto naz;

    case FK2:
    case PLU:
     goto vper;

    case FK10:
    case ESC:
      attroff(VV.VVOD_return_cs(iceb_CFS)); 
      return;
   }
 }

vper:;

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

if(rsdat1(&mp,&gp,VV.VVOD_SPISOK_return_data(2)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

sprintf(strsql,"%s.%d.%d",VV.VVOD_SPISOK_return_data(1),mv,gv);
if(rsdat(&d,&m,&g,strsql,0) != 0)
 {
  iceb_t_soob(gettext("Не верно введён день начисления/удержания !"));
  goto naz;
 }
if(POLE(VV.VVOD_SPISOK_return_data(0),strsql,1,'%') == 0)
 {
  metkaproc=1;
//  printw("\nПроцент\n");
//  OSTANOV();
 }
proc=suma=ATOFM(VV.VVOD_SPISOK_return_data(0));
suma=okrug(suma,okrg);
if(prn == 2)
 {
  proc*=-1;
  suma*=-1;
 }
koment.new_plus(VV.VVOD_SPISOK_return_data(3));
nomz=(int)ATOFM(VV.VVOD_SPISOK_return_data(4));

clear();
GDITE();
if(VV.VVOD_SPISOK_return_data(6)[0] == '\0')
  sprintf(strsql,"select tabn,fio,podr,shet,kodg,kateg,sovm,zvan,lgoti,\
datn,datk,dolg,pl from Kartb where datn <= '%04d-%d-31' and \
(datk >= '%04d-%d-31' or datk = '0000-00-00')",
  gv,mv,gv,mv);
else
  sprintf(strsql,"select tabn,fio,podr,shet,kodg,kateg,sovm,zvan,lgoti,\
datn,datk,dolg,pl from Kartb where datn <= '%04d-%d-31' and \
(datk >= '%04d-%d-31' or datk = '0000-00-00') and podr=%s",
  gv,mv,gv,mv,VV.VVOD_SPISOK_return_data(6));

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
class iceb_tu_str shetkar("");

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(pol == 1 && atoi(row[12]) != 0)
      continue;      

  if(pol == 2 && atoi(row[12]) != 1)
      continue;      

  printw("%4s %s %s\n",row[0],row[1],row[12]);

  tabn=atol(row[0]);
  podr=atoi(row[2]);  

  if(shetu.ravno()[0] != '\0')
   {
    shet.new_plus(shetu.ravno());
   }
  else
    shet.new_plus(row[3]);

  shetkar.new_plus(row[3]);
  
  kateg=atoi(row[5]);
  sovm=atoi(row[6]);
  zvan=atoi(row[7]);
  lgot.new_plus(row[8]);
  rsdat(&dn,&mn,&gn,row[9],2);
  rsdat(&du,&mu,&gu,row[10],2);
  dolg.new_plus(row[11]);
  
  //Проверяем блокировку карточки
  sprintf(strsql,"select blok,podr from Zarn where god=%d and mes=%d \
and tabn=%s",gv,mv,row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    if(atoi(row1[0]) != 0)
     {
      printw("%s %s",gettext("Заблокирована !"),row1[0]);
      printw("\n");
      continue;
     }
    podr=atoi(row1[1]);
   }
  else
   zapzarn(mv,gv,podr,tabn,kateg,sovm,zvan,shetkar.ravno(),lgot.ravno(),dn,mn,gn,du,mu,gu,0,dolg.ravno());

  /*Проверяем записан ли стандартный набор*/
  sprintf(strsql,"select datz from Zarp where datz >='%04d-%02d-01' and \
  datz <= '%04d-%02d-31' and tabn=%ld and prn='%d' order by knah,nomz asc",
  gv,mv,gv,mv,tabn,prn);

  if(readkey(strsql) <= 0)
    zapzagot(mv,gv,tabn,prn,podr);

  //Удаляем нулевые начисления/удержания
  sprintf(strsql,"delete from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%ld and prn='%d' and knah=%d and suma=0.",
  gv,mv,gv,mv,tabn,prn,knah);
  if(sql_zap(&bd,strsql) != 0)
       msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

  zp.clear();

  zp.dz=d;
  zp.mesn=mv; zp.godn=gv;
  zp.nomz=nomz;
  zp.podr=podr;
  strcpy(zp.shet,shet.ravno());
  if(metkaproc == 1)
   {
    suma=oklad(tabn,mv,gv,&i)*proc/100.;
//    suma=okrug(suma,okrg);
    suma=okrug(suma,10.);
   }

  zapzarp(d,mv,gv,tabn,prn,knah,suma,shet.ravno(),mp,gp,0,nomz,koment.ravno_filtr(),podr,"",zp); 
  zapzarn1(tabn,prn,knah,0,0,0,shet.ravno());

  if(suma != 0.)
   {
    zarsoc(mv,gv,tabn);

    zaravpr(tabn,mv,gv,NULL);
   }  

 }

OSTANOV();
}

/**************************/
/*Удаление записей        */
/**************************/

void	uddirnu(SQLCURSOR *cur,
int kolstr,
short mp,short gp, //Дата 
short prn, //1-начисление 2-удержание
class dirnupoi *poisk)
{
SQL_str row;
SQL_str row1;
long tabn;
char	strsql[1024];
short  dz,mz,gz;
short  knah;
short  godn,mesn;
class iceb_tu_str shetnuz("");
short  nomz;
int    podr;
SQLCURSOR curr;
int kolstr1=0;
cur->poz_cursor(0);
int kolzapnar=0;
class iceb_tu_str fio("");
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  if(row[14][0] != '\0')
   {
    kolzapnar++;
    continue;
   }
  tabn=atol(row[1]);

  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%ld",gp,mp,tabn);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
   if(atoi(row1[0]) != 0)
    {
     VVOD SOOB(1);
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,"%s %ld",gettext("Табельный номер"),tabn);
     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     continue;
    }
   }

  rsdat(&dz,&mz,&gz,row[0],2);
  knah=atoi(row[3]);
  godn=atoi(row[6]);
  mesn=atoi(row[7]);
  shetnuz.new_plus(row[5]);
  nomz=atoi(row[9]);
  podr=atoi(row[13]);
    
  zarudnu(gz,mz,dz,tabn,prn,knah,godn,mesn,podr,shetnuz.ravno(),nomz);

  if(prn == 1)
    zarsoc(mp,gp,tabn);
  zaravpr(tabn,mp,gp,NULL);
   
 }
if(kolzapnar != 0)
 {
     VVOD SOOB(1);
     SOOB.VVOD_SPISOK_add_MD(gettext("Записи сделанные из нарядов, удаляются только в нарядах !"));
     soobshw(&SOOB,stdscr,-1,-1,0,1);
  
 }
}
/********************************************/
/* Изменить счёт                            */
/********************************************/

void	smeshnu(SQLCURSOR *cur,
int kolstr,
class dirnupoi *poisk)
{
SQL_str   row;
SQL_str   row1;
SQLCURSOR cur1;
int kolstr1=0;
class iceb_tu_str shet1("");
class iceb_tu_str shet2("");
int	kom=0;
class OPSHET	shetv;
char	strsql[1024];

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Смена счета"));

VV.VVOD_SPISOK_add_MD(gettext("Счёт который надо сменить....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт на который надо сменить.."));

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20); 

naz:;

//clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

shet1.new_plus(VV.VVOD_SPISOK_return_data(0));
shet2.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_8_1_3.txt");
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


if(SRAV(shet1.ravno(),"*",0) != 0)
 if(prsh1(shet1.ravno(),&shetv) != 0)
  goto naz;

if(prsh1(shet2.ravno(),&shetv) != 0)
  goto naz;

short d,m,g;
long tabn;
class iceb_tu_str fio("");
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",
  row[1],g,m);
  if(readkey(strsql,&row1,&cur1) == 1)
   if(atoi(row1[0]) != 0)
    {
     VVOD SOOB(1);
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,"%s: %s %s",gettext("Табельный номер"),row[1],row[15]);

     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     continue;
    }

  if(SRAV(shet1.ravno(),"*",0) == 0)
    sprintf(strsql,"update Zarp set shet='%s' where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    shet2.ravno(),row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
  else
    sprintf(strsql,"update Zarp set shet='%s' where shet='%s' and datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    shet2.ravno(),shet1.ravno(),row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
//  printw("%s\n",strsql);

  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  
  sprintf(strsql,"update Zarn1 set shet='%s' where shet='%s' and tabn=%s and prn=%s and knah=%s",
  shet2.ravno(),shet1.ravno(),row[1],row[2],row[3]);

  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  tabn=atoi(row[1]);  
  zaravpr(tabn,m,g,NULL);
    
 }

//OSTANOV();

}

/***************************************************************/
/*Попрограмма создания кассовых ордеров с учётом условий поиска*/
/***************************************************************/

void	kasdirnu(short prn,short knah,
short mv,
short gv,
class dirnupoi *poisk)
{
char		strsql[1024];
int		kolstr=0;
SQL_str		row;
static class iceb_tu_str kassa("");
static class iceb_tu_str dat1("");
static class iceb_tu_str kodop("");
int		kom=0,kom1=0;
class iceb_tu_str kod("");
class iceb_tu_str naimkas("");
class iceb_tu_str naimop("");
short		dko,mko,gko;
class iceb_tu_str shetk("");
class iceb_tu_str naimnah("");
SQLCURSOR 	curr;

if(prn == 1)
 sprintf(strsql,"select naik from Nash where kod=%d",knah);
if(prn == 2)
 sprintf(strsql,"select naik from Uder where kod=%d",knah);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  naimnah.new_plus(row[0]);
  

if(dat1.ravno()[0] == '\0' && poisk->datpoin.ravno()[0] != '\0')
 {
  sprintf(strsql,"%d.%d.%d",poisk->datpoin.ravno_atoi(),mv,gv);
  dat1.new_plus(strsql);
 }


VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_MD(gettext("Касса..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции...."));

VV.VVOD_SPISOK_add_data(kassa.ravno(),16);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kodop.ravno(),32);

naz:;
clear();

VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых ордеров\""));
sprintf(strsql,"%s:%d.%d%s %d %s",
gettext("Дата"),mv,gv,gettext("г."),
knah,naimnah.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);
if(naimkas.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimkas.ravno());
if(naimop.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimop.ravno());

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

kassa.new_plus(VV.VVOD_SPISOK_return_data(0));
dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
kodop.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_8_1_4.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
    break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));
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
        naimkas.new_plus("");
        if(dirkas(1,&kod,&naimkas) == 0)
          VV.VVOD_SPISOK_zapis_data(0,kod.ravno());
        goto naz;

      case 1 :
        naimop.new_plus("");
        if(dirkasop(prn,1,&kod,&naimop) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kod.ravno());
        goto naz;

     }

    break;
         
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

//Проверяем кассу
sprintf(strsql,"select kod from Kas where kod=%s",VV.VVOD_SPISOK_return_data(0));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена касса"),VV.VVOD_SPISOK_return_data(0));
  iceb_t_soob(strsql);
  goto naz;
 }

//Проверяем код операции
if(prn == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",VV.VVOD_SPISOK_return_data(2));
if(prn == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",VV.VVOD_SPISOK_return_data(2));

if(readkey(strsql,&row,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(2));
  iceb_t_soob(strsql);
  goto naz;
 }

naimop.new_plus(row[0]);
shetk.new_plus(row[1]);

//Проверяем дату
if(rsdat(&dko,&mko,&gko,VV.VVOD_SPISOK_return_data(1),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }


MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Кассовые ордера по подразделениям"));
MENU.VVOD_SPISOK_add_MD(gettext("Кассовые ордера по каждому человеку отдельно"));
MENU.VVOD_SPISOK_add_MD(gettext("Один кассовый ордер на всех"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

clearstr(LINES-1,0);
int metka_ks=0; //0-по подразделениям 1- по каждому человеку отдельно
while(menu3w(stdscr,&MENU,&metka_ks,-1,-1,0) != 0);
switch (metka_ks)
 {
  case 3 :
  case -1:
   return;

 }

clear();

GDITE();
sprintf(strsql,"select * from Zarp where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and suma <> 0. \
order by podr,tabn asc",
gv,mv,gv,mv,prn,knah);

//printw("%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }


SQL_str	row1;
class iceb_tu_str fio("");
int kolstr1=0;
class iceb_tu_str podr("");
class iceb_tu_str podrz("");
class iceb_tu_str nomd("");
double suma=0.;
class iceb_tu_str tabnomz("");
class iceb_tu_str kodkont("");


if(metka_ks == 2)
 {
  lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_t_nomnak(gko,kassa.ravno(),&nomd,prn,2,1);
  zaphkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
  printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
 }

while(cur.read_cursor(&row) != 0)
 {
//   printw("%s %s %s\n",row[0],row[1],row[2]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  sprintf(strsql,"select podr from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s %s !",gettext("Не найден табельный номер"),row[1],fio.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  podr.new_plus(row1[0]);

  if(metka_ks == 0)
  if(SRAV(podrz.ravno(),podr.ravno(),0) != 0)
   {
    if(podrz.ravno()[0] != '\0')
     {
      kodkont.new_plus(ICEB_ZAR_PKTN);
      kodkont.plus(tabnomz.ravno());
      zarzapkk(kodkont.ravno());/*записываем в общий список контрагентов если его там нет*/
      zapzkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont.ravno(),suma,"","");
      tabnomz.new_plus("");
     }

    lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_t_nomnak(gko,kassa.ravno(),&nomd,2,2,1);
    zaphkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
    printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    podrz.new_plus(podr.ravno());
   }

  if(SRAV(tabnomz.ravno(),row[1],0) != 0)
   {
    if(tabnomz.ravno()[0] != '\0')
     {
      if(metka_ks == 1)
       {
        lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
        iceb_t_nomnak(gko,kassa.ravno(),&nomd,2,2,1);
        zaphkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
        printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
       }
      kodkont.new_plus(ICEB_ZAR_PKTN);
      kodkont.plus(tabnomz.ravno());
      zarzapkk(kodkont.ravno());/*записываем в общий список контрагентов если его там нет*/
      zapzkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont.ravno(),suma,"","");
     }
    suma=0.;
    tabnomz.new_plus(row[1]);
   }

  if(prn == 1)
    suma+=atof(row[4]);
  if(prn == 2)
    suma+=atof(row[4])*-1;
 }

if(metka_ks == 1)
 {
  lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_t_nomnak(gko,kassa.ravno(),&nomd,2,2,1);
  zaphkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
  printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
 }

kodkont.new_plus(ICEB_ZAR_PKTN);
kodkont.plus(tabnomz.ravno());
zarzapkk(kodkont.ravno());/*записываем в общий список контрагентов если его там нет*/
zapzkor(0,kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont.ravno(),suma,"","");




OSTANOV();

}
/**************************/
/*Корректировка дня записи*/
/**************************/
void dirnu_sdz(SQLCURSOR *cur,
short mp,short gp,
int kolstr,
class dirnupoi *poisk)
{
class VVOD VVOD1(2);
class iceb_tu_str den("");

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите день на который нужно заменить"));
helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
int kom1=0;
if((kom1=vvod1(&den,3,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
 return;

if(kom1 == ESC)
 return;
 
short dn=den.ravno_atoi();
char strsql[512];
short d,m,g;

sprintf(strsql,"%d.%d.%d",dn,mp,gp);
if(rsdat(&d,&m,&g,strsql,1) != 0)
 {
  iceb_t_soob(gettext("Неправильно ввели день !"));
  return;
 }
SQL_str row,row1;
class SQLCURSOR cur1;
cur->poz_cursor(0);
int kolstr1=0;
int nom_zap=0;
class iceb_tu_str fio("");
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",row[1],g,m);
  if(readkey(strsql,&row1,&cur1) == 1)
   if(atoi(row1[0]) != 0)
    {
     VVOD SOOB(1);
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,"%s: %s %s",gettext("Табельный номер"),row[1],row[15]);

     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     continue;
    }
  nom_zap=atoi(row[9]);
  for(;nom_zap < 1000 ;nom_zap++)
   {
    
    sprintf(strsql,"update Zarp set datz='%04d-%02d-%02d', nomz=%d where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    g,m,dn,nom_zap,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
    
    if(sql_zapis(strsql,1,1) == 0)
      break;

   }
  zaravpr(atoi(row[1]),mp,gp,NULL);

 }
}

/*********************************/
/*Смена даты в счёт какого месяца*/
/********************************/
void dirnu_svkm(SQLCURSOR *cur,
int kolstr,
class dirnupoi *poisk)
{
class VVOD VVOD1(2);
class iceb_tu_str vkm("");

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату на которую нужно изменить реквизит"));
VVOD1.VVOD_SPISOK_add_MD(gettext("\"В счёт какого месяца\"(м.г)"));
helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
int kom1=0;
if((kom1=vvod1(&vkm,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
 return;

if(kom1 == ESC)
 return;
 
char strsql[512];
short d,m,g;
short mz,gz;
if(rsdat1(&mz,&gz,vkm.ravno()) != 0)
 {
  iceb_t_soob(gettext("Неправильно введи дату!"));
  return;
 }
SQL_str row,row1;
class SQLCURSOR cur1;
cur->poz_cursor(0);
int kolstr1=0;
int nom_zap=0;
class iceb_tu_str fio("");
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",
  row[1],g,m);
  if(readkey(strsql,&row1,&cur1) == 1)
   if(atoi(row1[0]) != 0)
    {
     VVOD SOOB(1);
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     SOOB.VVOD_SPISOK_add_MD(strsql);
     sprintf(strsql,"%s: %s %s",gettext("Табельный номер"),row[1],row[15]);

     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);
     continue;
    }
  nom_zap=atoi(row[9]);
  for(;nom_zap < 1000 ;nom_zap++)
   {
    
    sprintf(strsql,"update Zarp set godn=%d,mesn=%d, nomz=%d where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    gz,mz,nom_zap,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
    
    if(sql_zapis(strsql,1,1) == 0)
      break;

   }
 }

}
/*******************************************/
/*поиск*/
/********************************************/
int dirnu_poi(short *mp,short *gp,class dirnupoi *poisk)
{
char strsql[1024];
class VVOD VV(0);
class VVOD MENU(3);
class iceb_tu_str kod("");
class iceb_tu_str naimk("");
short mv=0,gv=0;
int kom1=0;

VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Фамилия...........(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата.......(м.г)........"));//3
VV.VVOD_SPISOK_add_MD(gettext("День начала..(д)........"));//4
VV.VVOD_SPISOK_add_MD(gettext("День конца...(д)........"));//5
VV.VVOD_SPISOK_add_MD(gettext("Категория.........(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт..............(,,).."));//7
VV.VVOD_SPISOK_add_MD(gettext("В счёт к-го месяца......"));//8
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........(,,).."));//9
VV.VVOD_SPISOK_add_MD(gettext("Сумма до................"));//10
VV.VVOD_SPISOK_add_MD(gettext("Группа под-ления..(,,).."));//11
VV.VVOD_SPISOK_add_MD(gettext("Счёт в банке.....(+/-).."));//12
VV.VVOD_SPISOK_add_MD(gettext("Код банка.........(,,).."));//13
VV.VVOD_SPISOK_add_data(poisk->podrpoi.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->tabelpoi.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->fiopoi.ravno(),128);
sprintf(strsql,"%d.%d",*mp,*gp);
VV.VVOD_SPISOK_add_data(strsql,8);
VV.VVOD_SPISOK_add_data(poisk->datpoin.ravno(),3);
VV.VVOD_SPISOK_add_data(poisk->datpoik.ravno(),3);
VV.VVOD_SPISOK_add_data(poisk->kategor.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->shetpoi.ravno(),128);
VV.VVOD_SPISOK_add_data(8);
if(poisk->ms != 0)
 {
  sprintf(strsql,"%d.%d",poisk->ms,poisk->gs);
  VV.VVOD_SPISOK_zapis_data(8,strsql);
 }
VV.VVOD_SPISOK_add_data(poisk->komentpoi.ravno(),128);
VV.VVOD_SPISOK_add_data(20);
if(poisk->sumado != 0.)
 {
  sprintf(strsql,"%.2f",poisk->sumado);
  VV.VVOD_SPISOK_zapis_data(10,strsql);
 }
VV.VVOD_SPISOK_add_data(poisk->grupa.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->shet_vb.ravno(),2);
VV.VVOD_SPISOK_add_data(poisk->kodbank.ravno(),8);

fk4:;

poisk->metpoi=0;


helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom1=VV.vvod(0,1,1,-1,-1);

poisk->podrpoi.new_plus(VV.VVOD_SPISOK_return_data(0));
poisk->tabelpoi.new_plus(VV.VVOD_SPISOK_return_data(1));
poisk->fiopoi.new_plus(VV.VVOD_SPISOK_return_data(2));
poisk->datpoin.new_plus(VV.VVOD_SPISOK_return_data(4));
poisk->datpoik.new_plus(VV.VVOD_SPISOK_return_data(5));
poisk->kategor.new_plus(VV.VVOD_SPISOK_return_data(6));
poisk->shetpoi.new_plus(VV.VVOD_SPISOK_return_data(7));
poisk->komentpoi.new_plus(VV.VVOD_SPISOK_return_data(9));
poisk->sumado=ATOFM(VV.VVOD_SPISOK_return_data(10));
if(fabs(poisk->sumado) < 0.01)
  poisk->sumado=0.;
poisk->grupa.new_plus(VV.VVOD_SPISOK_return_data(11));
poisk->shet_vb.new_plus(VV.VVOD_SPISOK_return_data(12));
poisk->kodbank.new_plus(VV.VVOD_SPISOK_return_data(13));

switch(kom1)
 {
  case FK10:
  case ESC:
   poisk->metpoi=0;
//   pozz=0;
   return(1);;
    
  case FK2:
  case PLU:
    GDITE();
    if(VV.VVOD_SPISOK_return_data(3)[0] != '\0')
     {
       if(rsdat1(&mv,&gv,VV.VVOD_SPISOK_return_data(3)) != 0)
        {
         iceb_t_soob(gettext("Не верно введена дата !"));
         goto fk4;
        }
       else
        {
         *mp=mv; *gp=gv;
        }
     }
    poisk->ms=poisk->gs=0;
    if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
     if(rsdat1(&poisk->ms,&poisk->gs,VV.VVOD_SPISOK_return_data(8)) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto fk4;
      }
    if(poisk->shet_vb.getdlinna() > 1 && poisk->shet_vb.ravno()[0] != '+' && poisk->shet_vb.ravno()[0] != '-')
     {
      iceb_t_soob(gettext("Не верно введена метка присутствия/отсутствия счета в банке !"));
      goto fk4;
     } 
    poisk->metpoi=1;
//    pozz=0;
    break;
 
  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка банков"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
    
    clearstr(LINES-1,0);
    kom1=0;        
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case 4 :
      case -1:
       goto fk4;


      case 0 :
        kod.new_plus("");
        naimk.new_plus("");
        if(dirzarpodr(2,&kod,&naimk) == 0)       
          VV.data_z_plus(0,kod.ravno());
        goto fk4;

      case 1 :
        if(vibrek("Kateg",&poisk->kategor) == 0)
         VV.VVOD_SPISOK_zapis_data(10,poisk->kategor.ravno());
        goto fk4;

      case 2 :
        if(vibrek("Zargrupp",&poisk->grupa) == 0)
         VV.VVOD_SPISOK_zapis_data(11,poisk->grupa.ravno());
         
        goto fk4;

      case 3 :
        kod.new_plus("");
        naimk.new_plus("");
        if(dirsb(2,&kod,&naimk) == 0)
         VV.VVOD_SPISOK_zapis_data(13,kod.ravno());
         
        goto fk4;
      }

 }
return(0);
}