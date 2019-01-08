/* $Id: dirpodu.c,v 5.45 2013/09/26 09:43:30 sasa Exp $ */
/*13.07.2015    18.04.1997      Белых А.И.      dirpodu.c
Просмотр всех записей по конкретной услуге/материалу
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

int poizkart(const char *nomdp,const char *nomdp1,const char *kontrp,const char *kontrp1,const char *kodop,const char *kodop1,int tipzz,int tipzz1);

void dmtrs(int metka,int kodzap,short dn,short mn,short gn,short dk,short mk,short gk,short mos,const char *naim,int kgrm,const char *naimg,short metpoi);
void raszpod(SQLCURSOR *cur,int metka,int kodzap,short dn,short mn,short gn,short dk,short mk,short gk,const char *nomdp,const char *kontrp,const char *kodop,int tipzz);

extern double   okrcn;
extern short	startgodus; /*Стартовый год просмотров*/
	
void dirpodu(int metka,  //0-материал 1-услуга
int kodzap)  //Код материалла/услуги
{
SQL_str         row,row1;
char		strsql[1024];
int		kolstr;
long    	pozz,pozz1;
struct  tm      *bf;
int             K;
short           kom;
int             i;
short           x=0,y=3;
short           d,m,g;
short           dp,mp,gp;
short           dn,mn,gn;
short           dk,mk,gk;
int             kkkd;
short           mos=1; /*метка вывода остатков*/
int             tz=0;
class iceb_tu_str datdok("");
class iceb_tu_str kor(""); /*Код контрагента*/
class iceb_tu_str kop; /*Код операции*/
class iceb_tu_str naimg;
class iceb_tu_str naim;
int		kgrm;
double		kolih=0.;
char		bros[1024];
class iceb_tu_str tabl;
time_t		vrem;
class iceb_tu_str nomdp("");
char		dat1[32];
char		dat2[32];
class iceb_tu_str kontrp("");
class iceb_tu_str kodop("");
int		tipzz;
class iceb_tu_str datp("");
int		kom1,kom2;
short		metpoi; /*0-поиск не заказан 1-заказан*/
class iceb_tu_str nomdok;
class iceb_tu_str ktozz("");
int		podr=0;
double		cena=0.;
class iceb_tu_str ei;
class iceb_tu_str shetu;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kod("");
class iceb_tu_str naimkod("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
metpoi=0;

//Создаем класс меню
class VVOD SOOB(1);
class VVOD VV(0);
class VVOD MENU(3);
class VVOD DANET(1);

pozz=0;

memset(dat1,'\0',sizeof(dat1));
memset(dat2,'\0',sizeof(dat2));
tipzz=0;
clear();
printw("\n\n%s\n","Готовим просмотр");
GDITE();
kom=0;


/*Узнаем наименование материалла*/
if(metka == 0)
 {
  sprintf(strsql,"select kodgr,naimat  from Material where kodm=%d",
  kodzap);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найден код материалла в списке материалов !"));
    return;
   }
 }

/*Узнаем наименование услуги*/
if(metka == 1)
 {
  sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%d",
  kodzap);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найден код услуги в списке услуг !"));
    return;
   }
 }

kgrm=atoi(row[0]);
naim.new_plus(row[1]);


/*Узнаем наименование группы материалла*/
if(metka == 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",
  kgrm);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена группа материалла !"));
    return;
   }
 }

/*Узнаем наименование группы услуги*/
if(metka == 0)
 {
  sprintf(strsql,"select naik from Uslgrup where kod=%d",
  kgrm);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена группа услуги !"));
    return;
   }
 }
 
naimg.new_plus(row[0]);


dn=1; mn=1; 
gn=startgodus;
if(gn == 0)
 gn=gt;
sprintf(dat1,"%d.%d.%d",dn,mn,gn);
/*Текущая дата*/
gk=gt; dk=dt; mk=mt;
dpm(&dk,&mk,&gk,5); /*Установить последний день месяца*/
sprintf(dat2,"%d.%d.%d",dk,mk,gk);

naz1:;

GDITE();
short           kls=LINES-6;
class masiv_din_int nomer_zap;
nomer_zap.make_class(kls);
unsigned int ktoz[kls];

K=0;

sprintf(strsql,"select * from Usldokum2 where metka=%d and kodzap=%d and \
datp >= '%d-%02d-%02d' and datp <= '%d-%02d-%02d' order by datp asc",
metka,kodzap,gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 } 
if(kolstr == 0)
 {

lll:;

  dmtrs(metka,kodzap,dn,mn,gn,dk,mk,gk,mos,naim.ravno(),kgrm,naimg.ravno(),metpoi);

  SOOB.VVOD_delete();

  if(metpoi == 0)
     sprintf(strsql,"%s %d %d.%d > %d.%d.%d !",
     gettext("Нет подтверждающих записей за"),
     dn,mn,gn,dk,mk,gk);
  if(metpoi == 1)
     sprintf(strsql,gettext("Не найдено ни одной записи !"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);
   
   if(K == FK3 || K == SFK3 || K == FK5)
      goto lll;
   if(K == ENTER)
      goto lll;
   if(K == KEY_RESIZE)
    goto naz1;
   goto vper;
  }

naz:;
GDITE();
clear();
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();
nomer_zap.clear_class();

for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  kolih=atof(row[6]);

  /*Читаем код контрагента, код операции*/
  
  sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
  row[1],row[10],row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("\n%s\n",strsql);
      printw("Не найден документ ! Дата:%s Док.:%s Подр.: Тип:%s",
      row[1],row[3],row[10],row[0]);
      sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
      if(ostanov1(bros) == FK3)
       {
 
       }
      continue;
   }

  tz=atoi(row1[0]);
  kop.new_plus(row1[1]);
  kor.new_plus(row1[2]);    

  if(metpoi == 1)
   if(poizkart(nomdp.ravno(),row[3],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
    continue;

  nomer_zap.plus(atoi(row[13]),i);
  ktoz[i]=atoi(row[11]);
  
  strcpy(bros,"+");
  if(tz == 2)
   {
    kolih*=-1;
    strcpy(bros,"-");
    
   }  
  rsdat(&d,&m,&g,row[1],2);
  rsdat(&dp,&mp,&gp,row[2],2);
  
  sprintf(strsql,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s|%-*s|%10.10g|%s|%10.6g|%-*s|%s",
  dp,mp,gp,d,m,g,
  iceb_tu_kolbait(10,row[3]),row[3],
  iceb_tu_kolbait(6,kop.ravno()),kop.ravno(),
  iceb_tu_kolbait(5,kor.ravno()),kor.ravno(),
  kolih,bros,atof(row[7]),
  iceb_tu_kolbait(2,row[10]),row[10],
  row[11]);

  spis_menu.spis_str.plus(strsql);
  i++;

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
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


dmtrs(metka,kodzap,dn,mn,gn,dk,mk,gk,mos,naim.ravno(),kgrm,naimg.ravno(),metpoi);


if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

kkkd=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,kkkd,&K,"",0);


if(kom == -7) //Изменение размера экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i<1  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    /*Читаем код контрагента, код операции*/
    sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
    row[1],row[10],row[3],row[0]);
  

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      clearstr(22,0);
      printw("Не найден документ ! Дата:%s Док.:%s Подр.:",
      row[1],row[3],row[10]);
      sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
      if(ostanov1(bros) == FK3)
       {
 
       }
      continue;
     }

    tz=atoi(row1[0]);
    kop.new_plus(row1[1]);
    kor.new_plus(row1[2]);    
  
    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
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

  /*Читаем код контрагента, код операции*/
    sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
    row[1],row[10],row[3],row[0]);
  

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      clearstr(22,0);
      printw("Не найден документ ! Дата:%s Док.:%s Подр.:",
      row[1],row[3],row[10]);
      sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
      if(ostanov1(bros) == FK3)
       {
 
       }
      continue;
     }
  

    tz=atoi(row1[0]);
    kop.new_plus(row1[1]);
    kor.new_plus(row1[2]);    

    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
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

  /*Читаем код контрагента, код операции*/
  
    sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
    row[1],row[10],row[3],row[0]);
  

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      clearstr(22,0);
      printw("Не найден документ ! Дата:%s Док.:%s Подр.:",
      row[1],row[3],row[10]);
      sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
      if(ostanov1(bros) == FK3)
       {
 
       }
      continue;
     }

  tz=atoi(row1[0]);
  kop.new_plus(row1[1]);
  kor.new_plus(row1[2]);    

    if(metpoi == 1)
    if(poizkart(nomdp.ravno(),row[2],kontrp.ravno(),kor.ravno(),kodop.ravno(),kop.ravno(),tipzz,tz) != 0)
     continue;
    i++;
   }
  goto naz;
 }


if(kom >= 0)
 {
  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_out.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_out.ravno(),&datp,1,'|');
  polen(str_out.ravno(),&datdok,2,'|');
  polen(str_out.ravno(),&nomdok,3,'|');
  polen(str_out.ravno(),&kop,4,'|');
  polen(str_out.ravno(),&kor,5,'|');

  polen(str_out.ravno(),bros,sizeof(bros),7,'|');
  tz=1;
  if(bros[0] == '-')
    tz=2;

  polen(str_out.ravno(),&podr,9,'|');
  
  polen(str_out.ravno(),&ktozz,10,'|');
  rsdat(&dp,&mp,&gp,datp.ravno(),1);
  rsdat(&d,&m,&g,datdok.ravno(),1);
  
  sprintf(strsql,"select * from Usldokum2 where datd='%d-%d-%d' and \
datp='%d-%d-%d' and podr=%d and nomd='%s' and metka=%d and kodzap=%d \
and tp=%d and nz=%d",g,m,d,gp,mp,dp,podr,nomdok.ravno(),metka,kodzap,tz,nomer_zap.ravno(kom));
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("\n%s\n",strsql);
    iceb_t_soob(gettext("Не найдена запись !"));
    goto naz;
   }
  cena=atof(row1[7]);
  kolih=atof(row1[6]);
  ei.new_plus(row1[8]);
  shetu.new_plus(row1[9]);
  vrem=atol(row1[12]);
 }

vper:;
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case ENTER: /*Расшифровать код контрагента*/

    SOOB.VVOD_delete();


  memset(bros,'\0',sizeof(bros));
  if(kor.ravno_atof() == 0. && POLE(kor.ravno(),bros,2,'-') == 0 && kor.ravno()[0] == '0')
    strncpy(bros,"00",sizeof(bros)-1);
  else
    strncpy(bros,kor.ravno(),sizeof(bros)-1);
  i=0;
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  bros);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kor.ravno());
   }
  else
   {
    sprintf(strsql,gettext("Контрагент: %s %s"),kor.ravno(),row1[0]);
   }
  SOOB.VVOD_SPISOK_add_MD(strsql);
  
  /*Расшифровываем код операции*/
  if(tz == 1)
    tabl.new_plus("Usloper1");
  if(tz == 2)
    tabl.new_plus("Usloper2");
    
  sprintf(strsql,"select naik from %s where kod='%s'",tabl.ravno(),kop.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден код операции %s"),kop.ravno());
   }
  else
    sprintf(strsql,gettext("Операция: %s %s"),kop.ravno(),row1[0]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  /*Расшифровываем единицу измерения*/
    
  sprintf(strsql,"select naik from Edizmer where kod='%s'",ei.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найдена единица измерения %s !"),ei.ravno());
   }
  else
    sprintf(strsql,"%s: %s %s",
    gettext("Единица измерения"),ei.ravno(),row1[0]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  /*Расшифровываем счет*/
    
  sprintf(strsql,"select nais from Plansh where ns='%s'",shetu.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найдена счёт %s в плане счетов !"),shetu.ravno());
   }
  else
    sprintf(strsql,"Счёт: %s %s",shetu.ravno(),row1[0]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s: %d.%d.%d",gettext("Дата документа"),d,m,g);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s: %d.%d.%d",gettext("Дата подтверждения"),dp,mp,gp);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s: %.10g",gettext("Количество"),kolih);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s: %.10g",gettext("Цена"),cena);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  /*Расшифровываем подразделение*/
    
  sprintf(strsql,"select naik from Uslpodr where kod=%d",podr);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найдена код подразделения"),podr);
   }
  else
    sprintf(strsql,"%s: %d %s",gettext("Подразделение"),podr,row1[0]);
  SOOB.VVOD_SPISOK_add_MD(strsql);


  SOOB.VVOD_SPISOK_add_MD(iceb_t_kszap(ktozz.ravno()));

  bf=localtime(&vrem);

  sprintf(strsql,"%s:%d.%d.%d%s %s:%d:%d:%d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;

  case FK1:       /*Помощь*/
   GDITE();
//   prosf(strsql);
   if(kolstr > 0)
      goto naz;
   else
      goto lll;
      

  case FK3:       /*Удалить запись*/
   
    rsdat(&d,&m,&g,datp.ravno(),0);
    if(iceb_t_pbpds(m,g) != 0)
      goto naz;

    if(usl_pvku(ktoz[kom]) != 0) /*проверка на хозяина записи*/
      goto naz;
      
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("                   В Н И М А Н И Е  !!!"));
    DANET.VVOD_SPISOK_add_MD(gettext("Документ будет помечен, как не подтвержденный и без проводок !"));
    DANET.VVOD_SPISOK_add_MD(gettext("            Удалить запись ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();   
      /*Удаляем запись в карточке*/
      rsdat(&dp,&mp,&gp,datp.ravno(),1);
      rsdat(&d,&m,&g,datdok.ravno(),1);
      sprintf(strsql,"delete from Usldokum2 where podr=%d and metka=%d \
and kodzap=%d and nomd='%s' and datp='%d-%02d-%02d' and \
datd='%d-%02d-%02d' and tp=%d",
      podr,metka,kodzap,nomdok.ravno(),gp,mp,dp,g,m,d,tz);
/*
      printw("\nstrsql=%s\n",strsql);
      OSTANOV();
*/

      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
         goto naz; 
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }

      if(tz == 1)
       {
        sprintf(strsql,"select vido from Usloper1 where kod='%s'",kop.ravno());
        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          sprintf(strsql,gettext("Не найдена операция %s !"),kop.ravno());
          iceb_t_soob(strsql);
         }
        else
         {
          /*Если операция внутреннего перемещения или изменения
           стоимости то проверять проводки не надо для прихода*/
          if(row1[0][0] != '0')
            goto naz1;
         }
       }
      sprintf(strsql,"update Usldokum set pod=0, pro=0 where \
datd='%d.%d.%d' and podr=%d and nomd='%s' and tp=%d",
      g,m,d,podr,nomdok.ravno(),tz);
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
       }
     }

    clear();
    goto naz1;

  case FK4:  /*Режим поиска*/

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска нужных записей в карточке"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа (,,).."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска...."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска....."));
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));
    VV.VVOD_SPISOK_add_MD(gettext("Код операции (,,)....."));
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)..."));

    VV.VVOD_SPISOK_add_data(nomdp.ravno(),64);
    VV.VVOD_SPISOK_add_data(dat1,11);
    VV.VVOD_SPISOK_add_data(dat2,11);
    
    VV.VVOD_SPISOK_add_data(kontrp.ravno(),64);
    VV.VVOD_SPISOK_add_data(kodop.ravno(),64);
    VV.VVOD_SPISOK_add_data(2);

fff4:;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);

    switch(kom1)
     {
      case FK10:
      case ESC:
        metpoi=0;
        break;

      case FK2:
      case PLU:
        nomdp.new_plus(VV.VVOD_SPISOK_return_data(0));
        strncpy(dat1,VV.VVOD_SPISOK_return_data(1),sizeof(dat1)-1);
        strncpy(dat2,VV.VVOD_SPISOK_return_data(2),sizeof(dat2)-1);
        if(dat1[0] != '\0')
         {
          if(rsdat(&dp,&mp,&gp,dat1,1) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto fff4;
           }
          dn=dp;
          mn=mp;
          gn=gp;
         }
        else
         {
          dn=1; mn=1; gn=gt;
         }
        if(dat2[0] != '\0')
         {
          if(rsdat(&dp,&mp,&gp,dat2,1) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата конца !"));
            goto fff4;
           }
          dk=dp;
          mk=mp;
          gk=gp;
         }
        else
         {
          dk=dn; mk=mn; gk=gn;
          dpm(&dk,&mk,&gk,5);
         } 
        kontrp.new_plus(VV.VVOD_SPISOK_return_data(3));
        kodop.new_plus(VV.VVOD_SPISOK_return_data(4));
        tipzz=0;
        if(VV.VVOD_SPISOK_return_data(5)[0] == '+')
          tipzz=1;
        if(VV.VVOD_SPISOK_return_data(5)[0] == '-')
          tipzz=2;

        metpoi=0;
        if(nomdp.getdlinna() > 1)
         metpoi=1;
        if(kontrp.getdlinna() > 1)
         metpoi=1;
        if(dat1[0] != '\0')
         metpoi=1;
        if(dat2[0] != '\0')
         metpoi=1;
        if(kodop.getdlinna() > 1)
         metpoi=1;
        if(tipzz != 0)
         metpoi=1;
        kom=pozz=0;

        goto naz1;

      case FK3:
        MENU.VVOD_delete();

        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        
        kom2=0;
        while(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0);

        kod.new_plus("");
        naimkod.new_plus("");

        switch(kom2)
         {
          case 3 :
          case -1:
           break;

          case 0 :
            vibrek("Kontragent",&kontrp);
            goto fff4;

          case 1 :
             if(diruslop1(1,&kod,&naimkod) == 0)
               kodop.z_plus(kod.ravno());              
            goto fff4;

          case 2 :
             if(diruslop2(1,&kod,&naimkod) == 0)
               kodop.z_plus(kod.ravno());              
            goto fff4;

         }
        break;
        
      case FK4:
        VV.VVOD_clear_data();
        tipzz=0;
        goto fff4;              
         
      default:
        goto fff4; 
     }
   if(kolstr > 0)
     goto naz;
   else 
     goto lll;
     
  case FK5:  /*Распечатать*/

    raszpod(&cur,metka,kodzap,dn,mn,gn,dk,mk,gk,nomdp.ravno(),kontrp.ravno(),kodop.ravno(),tipzz);
    goto naz;

  case FK6:  /*Следующая дата*/
    dpm(&dn,&mn,&gn,3);
    dpm(&dk,&mk,&gk,3);
    goto naz1;
  
  case SFK6:  /*Предыдущая дата*/
    dpm(&dn,&mn,&gn,4);
    dpm(&dk,&mk,&gk,4);
    goto naz1;
    break;


  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}

/**************/
/*Печать шапки*/
/**************/
void dmtrs(int metka,int kodzap,short dn,short mn,short gn,
short dk,short mk,short gk,short mos,const char *naim,int kgrm,const char *naimg,
short metpoi)
{
short		y=2;
short           x=COLS-20;

move(0,0);

if(metka == 1)
 printw("%s: %d %s",gettext("Услуга"),kodzap,naim);
if(metka == 0)
 printw("%s: %d %s",gettext("Материал"),kodzap,naim);

if(metpoi == 1)
 {
  move(0,50);
  attron(A_BLINK); /*Включение мерцания*/
  printw(gettext("Режим поиска !"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

move(1,0);
printw("%s - %d/%s  ",gettext("Группа"),kgrm,naimg);

move(1,50);
printw("%s:%d.%d.%d>%d.%d.%d\n",
gettext("Дата"),
dn,mn,gn,dk,mk,gk);
int nom=1;
mvprintw(2,0,"      1           2          3       4      5       6      7      8     9  10");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"%d-%s",nom++,gettext("дата документа"));
mvprintw(y++,x,"%d-%s",nom++,gettext("дата подтверждения"));
mvprintw(y++,x,"%d-%s",nom++,gettext("номер документа"));
mvprintw(y++,x,"%d-%s",nom++,gettext("код операции"));

if(metka == 1)
  mvprintw(y++,x,"%d-%s",nom++,gettext("код услуги"));
else
  mvprintw(y++,x,"%d-%s",nom++,gettext("код материала"));

mvprintw(y++,x,"%d-%s",nom++,gettext("количество"));
mvprintw(y++,x,"%d-%s",nom++,gettext("приход/расход"));
mvprintw(y++,x,"%d-%s",nom++,gettext("цена"));
mvprintw(y++,x,"%d-%s",nom++,gettext("подразделение"));
mvprintw(y++,x,"%d-%s",nom++,gettext("кто записал"));

 
helstr(LINES-1,0,"F1",gettext("помощь"),
//"F2",gettext("объед."),
"F3",gettext("удаление"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("смен.даты"),
//"F7",gettext("остат."),
"F10",gettext("выход"),NULL);
}

/****************************************/
/*Проверка соответствия условиям поиска*/
/***************************************/

int poizkart(const char *nomdp,const char *nomdp1,const char *kontrp,
const char *kontrp1,const char *kodop,const char *kodop1,int tipzz,int tipzz1)
{
int		i1;

if(nomdp[0] != '\0')
 if(pole1(nomdp,nomdp1,',',0,&i1) != 0)
  if(SRAV(nomdp,nomdp1,0) != 0)
    return(1);

if(kontrp[0] != '\0')
 if(pole1(kontrp,kontrp1,',',0,&i1) != 0)
  if(SRAV(kontrp,kontrp1,0) != 0)
    return(2);

if(kodop[0] != '\0')
 if(pole1(kodop,kodop1,',',0,&i1) != 0)
  if(SRAV(kodop,kodop1,0) != 0)
    return(3);

if(tipzz != 0 && tipzz != tipzz1)
   return(4);
return(0);
}

/*********************/
/*Распечатка подтверждающих записей*/
/*********************/
void raszpod(SQLCURSOR *cur,int metka,int kodzap,
short dn,short mn,short gn,short dk,short mk,short gk,const char *nomdp,
const char *kontrp,const char *kodop,int tipzz)
{
short		dp,mp,gp;
short		d,m,g;
double		kolih,cena;
short		tp;
char		bros[512];
SQL_str         row,row1;
char		strsql[512];
struct  tm      *bf;
time_t          tmm;
FILE            *f1;
char            imaf[64];
double          it;
class iceb_tu_str br1("");
class iceb_tu_str naim("");
class iceb_tu_str naimg("");
int		kgrm;
class iceb_tu_str kop(""),kor("");
SQLCURSOR curr;
clear();
GDITE();
/*Узнаем наименование материалла*/
if(metka == 0)
 {
  sprintf(strsql,"select kodgr,naimat  from Material where kodm=%d",kodzap);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найден код материалла в списке материалов !"));
    return;
   }
 }

/*Узнаем наименование услуги*/
if(metka == 1)
 {
  sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%d",kodzap);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найден код услуги в списке услуг !"));
    return;
   }
 }

kgrm=atoi(row[0]);
naim.new_plus(row[1]);


/*Узнаем наименование группы материалла*/
if(metka == 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена группа материалла !"));
    return;
   }
 }

/*Узнаем наименование группы услуги*/
if(metka == 0)
 {
  sprintf(strsql,"select naik from Uslgrup where kod=%d",kgrm);
  if(readkey(strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена группа услуги !"));
    return;
   }
 }
 
naimg.new_plus(row[0]);

memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"upz%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
bf=localtime(&tmm);

fprintf(f1,"\x0F");
fprintf(f1,"%s\n",iceb_t_get_pnk("00",0));
if(metka == 1)
 fprintf(f1,"%s %d %s \n",
 gettext("Распечатка подтверждающих записей услуги"),kodzap,naim.ravno());
if(metka == 0)
 fprintf(f1,"%s %d %s \n",
 gettext("Распечатка подтверждающих записей материалла"),kodzap,naim.ravno());

fprintf(f1,"\
%s %d.%d.%d%s %s - %d:%d\n\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(f1,"%s - %d %s",gettext("Группа"),kgrm,naimg.ravno());
fprintf(f1,"\n");

fprintf(f1,gettext("Движение за период %d.%d.%d > %d.%d.%d\n"),dn,mn,gn,dk,mk,gk);
if(nomdp[0] != '\0')
 fprintf(f1,gettext("Номер документа: %s\n"),nomdp);
if(kontrp[0] != '\0')
 fprintf(f1,gettext("Контрагент: %s\n"),kontrp);
if(kodop[0] != '\0')
 fprintf(f1,"%s: %s\n",gettext("Код операции"),kodop);
if(tipzz == 1)
 fprintf(f1,gettext("Только приходы.\n"));
if(tipzz == 2)
 fprintf(f1,gettext("Только расходы.\n"));

fprintf(f1,"\n\
-----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(f1,"\
Дата подт.|Дата док. | Ном. доку. |Операц.  |Код конт.|Наименование контра.|  Количество |     Цена    |Ед.изм.| Счёт |Кто записал|\n");
fprintf(f1,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");


it=0.;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {

  /*Читаем код организации, код операции*/
  sprintf(strsql,"select tp,kodop,kontr from Usldokum \
where datd='%s' and podr=%s and nomd='%s' and tp=%s",
  row[1],row[10],row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("\n%s\n",strsql);
      printw("Не найден документ ! Дата:%s Док.:%s Подр.:%s Тип:%s",
      row[1],row[3],row[10],row[0]);
      sprintf(bros,"%s F3-delete",gettext("Нажмите любую клавишу для продолжения"));
      if(ostanov1(bros) == FK3)
       {
 
       }
      continue;
   }
  tp=atoi(row1[0]);
  kop.new_plus(row1[1]);
  kor.new_plus(row1[2]);    

  if(poizkart(nomdp,row[2],kontrp,kor.ravno(),kodop,kop.ravno(),tipzz,tp) != 0)
   continue;

  memset(bros,'\0',sizeof(bros));
  if(ATOFM(kor.ravno()) == 0. && POLE(kor.ravno(),bros,2,'-') == 0 && kor.ravno()[0] == '0')
    strncpy(bros,"00",sizeof(bros)-1);
  else
    strncpy(bros,kor.ravno(),sizeof(bros)-1);

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  bros);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
     beep();
     printw("%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     OSTANOV();
   }
  else
    strcpy(bros,row1[0]);

  br1.new_plus("+");
  if(tp == 2)
   {  
    br1.new_plus("-");
    kolih*=(-1);
   }
  rsdat(&dp,&mp,&gp,row[2],2);
  rsdat(&d,&m,&g,row[1],2);

  kolih=atof(row[6]);  
  cena=atof(row[7]);  

  it+=kolih;

  fprintf(f1,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s %s|%-*s|%-*.*s|\
%13.13g|%13.10g|%-*s|%-6s|%s\n",
  dp,mp,gp,d,m,g,
  iceb_tu_kolbait(12,row[3]),row[3],
  iceb_tu_kolbait(7,kop.ravno()),kop.ravno(),br1.ravno(),
  iceb_tu_kolbait(9,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(20,bros),iceb_tu_kolbait(20,bros),bros,
  kolih,cena,
  iceb_tu_kolbait(7,row[8]),row[8],
  row[9],iceb_t_kszap(row[11]));
 }


if(fabs(it) < 0.0000000001)
  it=0.;
fprintf(f1,"\
-----------------------------------------------------------------------------------------------------------------------------------\n\
%*s: %13.10g\n",iceb_tu_kolbait(74,gettext("Итого")),gettext("Итого"),it);

podpis(f1);
fclose(f1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка подтверждающих записей"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");

}
