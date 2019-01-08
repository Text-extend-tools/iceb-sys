/* $Id: vkart.c,v 5.92 2013/09/26 09:43:43 sasa Exp $ */
/*16.04.2017	21.05.1999	Белых А.И.	vkart.c
Ввод, просмотр и корректировка карточки
*/
#include        <math.h>
#include        "buhg.h"

void koresh(int,short,short,const char*,const char*,const char*,int);
void            rasnu(short,short,int,double,short,const char*);
void		zarraspr(int,short,short,const char*);
void  zapkrtb(int,short,short,int,int,const char*,const char*,short,int,short,short,short,short,short,short,short,
const char*,short,const char*,const char*,const char*,const char*,const char*,short,const char*,const char*,const char*,short,short,short,const char*,
short,short,short,short,short,short,short,int,int,class iceb_tu_str *kodss);

void vkart1(int tabn,const char *fio,class iceb_tu_str *adres,class iceb_tu_str *nomp,class iceb_tu_str *vidan,class iceb_tu_str *telef,short *kodg,class iceb_tu_str *harac,class iceb_tu_str *inn,class iceb_tu_str *bankshet,short *dv,short *mv,short *gv,class iceb_tu_str *tipkk,
short *drog,short *mrog,short *grog,short *pol,short *dppz,short *mppz,short *gppz,int *kodbank,class iceb_tu_str *kodss);

int             dirzarp(short,short,int,int,short,const char*,const char*,short,short,short,short,short,short);
void		poifl(int);
void		rassocp(short,short,int,const char*);
void		dirsoc(int,short,short);
void		raszkart(int);
void	        rabdopinf(int);
void zarnrn(int tabnom);
void vkart_sf7(int);
void	rasnuptn(short,short,long,const char*);

extern short    ddd,mmm,ggg;
extern char	*imabaz;
extern char	*shetbu; /*Бюджетные счета удержаний*/
short		flagrk=0; //0-с каточкой нико не работает 1-работает другой оператор

void		vkart(int tabn,int mz) //0-ввод нового 1-просмотр с корректировкой
{
int		kom;
int		stbn;
char		bros[512];
long		kolstr;
SQL_str         row;
char		strsql[512];
class iceb_tu_str fio("");
class iceb_tu_str sfio("");
class iceb_tu_str dolg("");
short		zvan;
int		podr,podrz=0;
int		kateg;
short		dn,mn,gn;
short		du,mu,gu;
class iceb_tu_str shet(""),shetz("");
short		sovm;
class iceb_tu_str inn("");
class iceb_tu_str adres("");
class iceb_tu_str nomp("");
class iceb_tu_str vidan("");
class iceb_tu_str telef("");
class iceb_tu_str bankshet("");
short		kodg;
class iceb_tu_str harac("");
class iceb_tu_str lgot("");
short		mp,gp;
int		K,N;
short           mettab;/*Метка табеля*/
double		sumd,sumk;
double		bb,nah,nahprov,uder,nahb,uderb,sald;
double		uderprov=0.;
double		uderbprov=0.;
float		dnei,has,kdn;
short		Y,X;
class iceb_tu_str kod("");
int		i,i1;
struct OPSHET	shetv;
short		uvol; /*0-работает 1-уволен*/
double		saldb;
double		okld;
short		prov; //0-проводки не сделаны 1-сделаны
short		blok; //0-карточка не заблокирована 1-заблокирована
short           mtd;  /*0-дата не заблокирована 1-заблокирована*/
short		dv,mv,gv;
class iceb_tu_str tipkk("");
short           drog=0,mrog=0,grog=0; //Дата рождения
short           pol=0;  //0-мужчина 1-женщина
short           dppz=0,mppz=0,gppz=0; //Дата последнего повышения зарплаты
class iceb_tu_str kodss("");
int kodbank=0;
int metka_flagrk=0;
class icebt_sql_flag flag_katr;
SQLCURSOR curr;

GDITE();
VVOD PAR(2);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

flagrk=0; //0-с каточкой нико не работает 1-работает другой оператор
stbn=tabn;

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Фамилия............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Должность..........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Звание.............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Подразделение......."));//4
VV.VVOD_SPISOK_add_MD(gettext("Категория..........."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата приёма/уволь. ."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт................"));//7
VV.VVOD_SPISOK_add_MD(gettext("Код льготы.........."));//8
VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(40);//1
VV.VVOD_SPISOK_add_data(40);//2
VV.VVOD_SPISOK_add_data(40);//3
VV.VVOD_SPISOK_add_data(40);//4
VV.VVOD_SPISOK_add_data(40);//5
VV.VVOD_SPISOK_add_data(23);//6
VV.VVOD_SPISOK_add_data(10);//7
VV.VVOD_SPISOK_add_data(11);//8

mp=mmm;
gp=ggg;
blok=uvol=dn=mn=gn=du=mu=gu=N=0;
if(tabn != 0)
 N=1;
bb=nah=uder=nahb=uderb=sald=0;

sovm=podr=kateg=dn=mn=gn=du=mu=gu=zvan=0;
dv=mv=gv=0;
kodg=0;

if(mz == 0)
  tabn=tabnom();

if(mz == 1)
 {
  //Установить флаг работы с карточкой
  sprintf(strsql,"z%s%d",imabaz,stbn);

  //if(sql_flag(&bd,strsql,0,0) != 0)
  if(flag_katr.flag_on(strsql) != 0)
    flagrk=1;
 }

read:;  //Может не быть промежуточной записи

if(mz == 1)
 {
  sprintf(strsql,"select * from Kartb where tabn=%d",tabn);
  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
    iceb_t_soob(strsql);
    return;
   }
  fio.new_plus(row[1]);
  sfio.new_plus(row[1]);
  dolg.new_plus(row[2]);
  zvan=atoi(row[3]);
  podr=atoi(row[4]);
  kateg=atoi(row[5]);
  rsdat(&dn,&mn,&gn,row[6],2);
  uvol=0;
  if(rsdat(&du,&mu,&gu,row[7],2) == 0)
    uvol=1;
  shet.new_plus(row[8]);
  sovm=atoi(row[9]);
  inn.new_plus(row[10]);
  adres.new_plus(row[11]);
  nomp.new_plus(row[12]);
  vidan.new_plus(row[13]);
  telef.new_plus(row[14]);
  kodg=atoi(row[15]);
  harac.new_plus(row[16]);
  lgot.new_plus(row[17]);
  bankshet.new_plus(row[18]);
  rsdat(&dv,&mv,&gv,row[19],2);

  tipkk.new_plus(row[20]);
  rsdat(&drog,&mrog,&grog,row[21],2);
  pol=atoi(row[22]);
  rsdat(&dppz,&mppz,&gppz,row[23],2);
  kodbank=atoi(row[24]);
  kodss.new_plus(row[25]);
  
  blok=0;
  /*Читаем текущие настройки если они есть*/
  sprintf(strsql,"select podr,kateg,sovm,zvan,shet,lgot,\
datn,datk,dolg,blok from Zarn where god=%d and mes=%d and tabn=%d",gp,mp,tabn);
  if(readkey(strsql,&row,&curr) == 1)
   {
    zvan=atoi(row[3]);
    podr=atoi(row[0]);
    kateg=atoi(row[1]);
    sovm=atoi(row[2]);
    shet.new_plus(row[4]);
    lgot.new_plus(row[5]);
    rsdat(&dn,&mn,&gn,row[6],2);
    if(rsdat(&du,&mu,&gu,row[7],2) == 0)
      uvol=1;
    else
      uvol=0;
      
    dolg.new_plus(row[8]);

    blok=atoi(row[9]);
    
   }
 }


naz:;

if(iceb_t_pblok(mp,gp,ICEB_PS_ZP) == 0) //именно эта функция
  mtd=0; //Дата не заблокирована
else
  mtd=1; //Дата заблокирована
 
clear();
GDITE();
mettab=0;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%d",gp,mp,tabn);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

Y=1;X=67;
move(Y++,X);
printw(gettext("Табель:"));
dnei=has=kdn=0.;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  mettab=1;
  move(Y++,X);
  printw("%s=%.6g/%.6g/%.6g",row[0],atof(row[1]),atof(row[2]),atof(row[3]));
  dnei+=atof(row[1]);
  has+=atof(row[2]);
  kdn+=atof(row[3]);
 }
move(Y++,X);
printw("---------");
move(Y++,X);
printw("S=%.6g/%.6g/%.6g",dnei,has,kdn);

VV.VVOD_clear_data();

if(tabn != 0)
 VV.data_plus(0,tabn);
if(fio.ravno()[0] != '\0')
 VV.data_plus(1,fio.ravno());
if(dolg.ravno()[0] != '\0')
 VV.data_plus(2,dolg.ravno());
if(zvan != 0)
 {
  sprintf(strsql,"select naik from Zvan where kod=%d",zvan);
  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код звания"),zvan);
    iceb_t_soob(strsql);
   }
  else
    VV.data_plus(3,row[0]);
 }
if(podr != 0)
 {
  sprintf(strsql,"select naik from Podr where kod=%d",podr);
  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
    iceb_t_soob(strsql);
   }
  else
   { 
    sprintf(strsql,"%d %.30s",podr,row[0]); 
    VV.VVOD_SPISOK_zapis_data(4,strsql);
   } 
 }
if(kateg != 0)
 {
  sprintf(strsql,"select naik from Kateg where kod=%d",kateg);
  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код категории"),kateg);
    iceb_t_soob(strsql);
   }
  else
    VV.data_plus(5,row[0]);
   
 }

if(dn != 0)
 {
  sprintf(strsql,"%d.%d.%d",dn,mn,gn);
  VV.data_plus(6,strsql);
 }
if(uvol != 0)
 {
  sprintf(bros,"/%d.%d.%d",du,mu,gu);
  VV.data_strcat(6,bros);
 } 
if(shet.ravno()[0] != '\0')
 VV.VVOD_SPISOK_zapis_data(7,shet.ravno());
if(lgot.ravno()[0] != '\0')
 VV.VVOD_SPISOK_zapis_data(8,lgot.ravno());
 

move(0,0);
printw("%s %d.%d%s",gettext("Текущая дата"),mp,gp,
gettext("г."));

if(uvol != 0)
 {
//  attron(A_BLINK); /*Включить подчеркивание*/
  move(0,30);
  printw(gettext("Уволен"));
//  attroff(A_BLINK); /*Выключить подчеркивание*/
 }
if(sovm == 1)
 {
//  attron(A_BLINK); /*Включить мерцание*/
  move(0,40);
  printw(gettext("Совместитель"));
//  attroff(A_BLINK); /*Выключить мерцание*/
 }
int mhas=0; //метка часовой ставки
if((okld=oklad(tabn,mp,gp,&mhas)) != 0.)
 {
  
  mvprintw(0,60,"%s:%.2f",gettext("Оклад"),okld);
  if(mhas == 1)
   printw("Ч");
  if(mhas == 2)
   printw("Д");
 }  

sald=zarsald(1,mp,gp,tabn,&saldb);

nahud(gp,mp,tabn,&nah,&nahprov,&uder,&uderprov,&nahb,&uderb,&uderbprov,0,sald,0,fio.ravno());
memset(bros,'\0',sizeof(bros));
sumd=sumk=0.;
if(tabn != 0)
 {
  sprintf(bros,"%d-%d",mp,tabn);

  bb=sumprz(mp,gp,bros,0,&sumd,&sumk);

 }


prov=0;
if(shetbu == NULL)
 { 
  move(Y=14,X=0);
  printw("%-*s:%15s",iceb_tu_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),prnbr(sald));
  move(++Y,X);
  printw("%-*s:%15s",iceb_tu_kolbait(10,gettext("Начислено")),gettext("Начислено"),prnbr(nah));
  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма по дебету")),gettext("Сумма по дебету"),sumk);

  move(++Y,X);
  printw("%-*s:%15s",iceb_tu_kolbait(10,gettext("Удержано")),gettext("Удержано"),prnbr(uder));
  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма по кредиту")),gettext("Сумма по кредиту"),sumd);
  bb=nah+uder+sald;
  move(++Y,X);

  if(bb >= 0.)
   printw("%-*s:%15s",iceb_tu_kolbait(10,gettext("К выдаче")),gettext("К выдаче"),prnbr(bb));
  else
   printw("%-*s:%15s",iceb_tu_kolbait(10,gettext("Долг")),gettext("Долг"),prnbr(bb));

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) <= 0.009 )
   prov=1;

  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма выполненных проводок")),gettext("Сумма выполненных проводок"),sumd+sumk);
 }

if(shetbu != NULL)
 {
  move(Y=14,X=0);
  printw(gettext("              Итог   Бюджет  Остальное"));
  
  move(++Y,X=0);
  printw("%-*s:%9.2f %9.2f %9.2f",iceb_tu_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),sald,saldb,sald-saldb);
  
  move(++Y,X);
  printw("%-*s:%9.2f %9.2f %9.2f",iceb_tu_kolbait(10,gettext("Начислено")),gettext("Начислено"),nah,nahb,nah-nahb);
  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма по дебету")),gettext("Сумма по дебету"),sumk);

  move(++Y,X);
  printw("%-*s:%9.2f %9.2f %9.2f",iceb_tu_kolbait(10,gettext("Удержано")),gettext("Удержано"),uder,uderb,uder-uderb);
  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма по кредиту")),gettext("Сумма по кредиту"),sumd);
  bb=nah+uder+sald;
  move(++Y,X);

  printw("%-*s:%9.2f %9.2f %9.2f",iceb_tu_kolbait(10,gettext("Сальдо")),gettext("Сальдо"),
  bb,nahb+uderb+saldb,(nah-nahb)+(uder-uderb)+(sald-saldb));

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) <= 0.009 )
   prov=1;

  printw("%*s:%9.2f",iceb_tu_kolbait(30,gettext("Сумма выполненных проводок")),gettext("Сумма выполненных проводок"),sumd+sumk);


 }

/*
printw("\nsaldo=%d mettab=%d and mtd=%d shet=%s\n",
saldo,mettab,mtd,shet);
OSTANOV();
*/


if(tabn > 0 && podr != 0 && kateg != 0 && blok == 0)
 {
  if(nah != 0 || uder != 0. || sald != 0. || mettab == 1)
   {

    if(mtd == 0 && blok == 0 && flagrk == 0)
      zapzarn(mp,gp,podr,tabn,kateg,sovm,zvan,shet.ravno(),lgot.ravno(),dn,mn,gn,du,mu,gu,prov,dolg.ravno());
   }
 }
if(blok != 0)
 {
  
//  attron(A_BLINK); /*Включить мерцание*/
  mvprintw(LINES-2,0,"%s %s",gettext("Заблокирована !"),iceb_t_kszap(blok));
//  attroff(A_BLINK); /*Выключить мерцание*/
 }



VV.vramka(1,0,0);
refresh();

if(mz == 1)
 if(flagrk == 1 && metka_flagrk == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С карточкой уже работает другой оператор !"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Ваши изменения записаны не будут !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  metka_flagrk++;
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("начис."),
"F3",gettext("удалить"),
"F4",gettext("удерж."),
"F5",gettext("печать"),
"F6",gettext("табель"),
"F7",gettext("совм."),
"F8",gettext("НСИ"),
"F9",gettext("льгот."),
"F10",gettext("выход"),NULL);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
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

    if(mtd == 1 || blok != 0 || flagrk != 0)
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      if(provblokzarp(tabn,mp,gp) != 0) //dirzarp.c
       goto naz;
     }
    VV.vprintw_clear(N,stdscr);


    if(N == 0)   /*Табельный номер*/
     {
      VV.vgetstr(N,stdscr);
      
      sprintf(strsql,"select fio from Kartb where tabn=%d",VV.data_ravno_atoi(N));
      if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        sprintf(strsql,gettext("Табельний номер %s уже есть !"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        VV.data_plus(N,"");
        goto naz;
      
       }
      if(mz == 1 && stbn != atol(VV.VVOD_SPISOK_return_data(N)))
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        iceb_t_soob(gettext("Корректировка табельного номера не возможна !"));

        VV.data_plus(N,stbn);
        
        goto naz;
   
       }
      tabn=atol(VV.VVOD_SPISOK_return_data(0));

     }

    if(N == 1)   /*фамилия имя отчество*/
     {
      VV.vgetstr(N,stdscr);
      fio.new_plus(VV.VVOD_SPISOK_return_data(N));
      if(SRAV(sfio.ravno(),fio.ravno(),0) != 0)
       {
        sprintf(strsql,"select tabn from Kartb where fio='%s'",fio.ravno_filtr());
        if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));

          sprintf(strsql,gettext("Такая фамилия уже есть. Таб. номер %s !"),row[0]);
          iceb_t_soob(strsql);

          fio.new_plus(VV.VVOD_SPISOK_return_data(N));
          N++;
          goto naz;        
         }
       }
      fio.new_plus(VV.VVOD_SPISOK_return_data(N));
     }

    if(N == 2)   /*Должность*/
     {
      VV.vgetstr(N,stdscr);
      dolg.new_plus(VV.VVOD_SPISOK_return_data(N));
      attroff(VV.VVOD_return_cs(iceb_CFV));
      N++;
      goto naz;
     }
     
    if(N == 3)   /*Звание*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));

      class iceb_tu_str naik("");
      if(dirzvan(2,&kod,&naik) == 0)
        zvan=kod.ravno_atoi();
      N++;
      goto naz;

     }

    if(N == 4)   /*Подразделение*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      podrz=podr;
      class iceb_tu_str naik("");
//      if((i1=vvodkod("Podr",&kod,&naik,-1,-1,0)) != 0)
      if(dirzarpodr(2,&kod,&naik) == 0)
       {
        podr=kod.ravno_atoi();
        if(podrz != 0 && podrz != podr)
         {
          //Исправляем код подразделения в начислениях/удержаниях
          sprintf(strsql,"update Zarp set podr=%d \
where datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and tabn=%d and podr=%d",
          podr,gp,mp,gp,mp,tabn,podrz);

          sql_zapis(strsql,1,0);

          /*Исправляем подразделение в записи настроек*/

          sprintf(strsql,"update Zarn set podr=%d where tabn=%d and god=%d and mes=%d",podr,tabn,gp,mp);

          sql_zapis(strsql,1,0);
                    
         }
       }
      N++;
      goto naz;
     }

    if(N == 5)   /*Категория*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naik("");
      if((i1=vvodkod("Kateg",&kod,&naik,-1,-1,0)) != 0)
        kateg=kod.ravno_atoi();
      N++;
      goto naz;
     }

    if(N == 6)   /*Дата приема/увольнения*/
     {
//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      du=mu=gu=0;
      dn=mn=gn=0;
      
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        if(POLE(VV.VVOD_SPISOK_return_data(N),bros,1,'/') != 0)
          strcpy(bros,VV.VVOD_SPISOK_return_data(N));
        if(rsdat(&dn,&mn,&gn,bros,0) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата начала работы !"));
          dn=mn=gn=0;
         }
        else
         if(sravmydat(dn,mn,gn,dppz,mppz,gppz) > 0)
          dppz=mppz=gppz=0;
          
        uvol=0;
        if(POLE(VV.VVOD_SPISOK_return_data(N),bros,2,'/') == 0)
         {
         if(rsdat(&du,&mu,&gu,bros,0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата увольнения !"));
           du=mu=gu=0;
          }
         else
          uvol=1;
         }
        N++;
        goto naz;
       }  
     
     }

    if(N == 7 )   /*Счёт*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        shetz.new_plus(shet.ravno());
        attroff(VV.VVOD_return_cs(iceb_CFV));
        if(prsh1(VV.VVOD_SPISOK_return_data(N),&shetv) != 0)
          goto naz;
        
        shet.new_plus(VV.VVOD_SPISOK_return_data(N));            
        if(SRAV(shetz.ravno(),shet.ravno(),0) != 0)
         {
          //Меняем номер счета в начислениях
          sprintf(strsql,"update Zarp set \
shet='%s' where datz >= '%d-%d-1' and datz <= '%d-%d-31' and tabn=%d and shet='%s' and prn='1'",
          shet.ravno(),gp,mp,gp,mp,tabn,shetz.ravno());

          if(sql_zap(&bd,strsql) != 0)
           msql_error(&bd,gettext("Ошибка коректировки записи !"),strsql);

          sprintf(strsql,"update Zarn1 set \
shet='%s' where tabn=%d and shet='%s'",
          shet.ravno(),tabn,shetz.ravno());

          if(sql_zap(&bd,strsql) != 0)
           msql_error(&bd,gettext("Ошибка коректировки записи !"),strsql);

          zaravpr(tabn,mp,gp,NULL);
         }
        N++; 
        goto naz;
      }
      shet.new_plus(VV.VVOD_SPISOK_return_data(N));            
     }

    if(N == 8)   /*Код льготи*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      lgot.new_plus(VV.VVOD_SPISOK_return_data(N));
      goto naz;
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
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

  switch(K)
   {
    case FK10:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      N=-1;
      goto kon;

    case ESC:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
 #ifndef M_UNIX
      K=getch();
 #endif
      N=-1;
      goto kon;

    case FK1:   /*Помощь*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      GDITE();
      iceb_t_pdoc("zarp2_1.txt");
      clear();
      goto naz;

    case FK2: /*Начисления*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(uvol != 0)
       {
        sprintf(strsql,"%s ! %s",gettext("Внимание"),gettext("Работника уволено !"));
        iceb_t_soob(strsql);
       }

      if(fio.ravno()[0] == '\0')
       {
        iceb_t_soob(gettext("Не введена фамилия !"));
        goto naz;
       }

      if(tabn == 0)
       {
        iceb_t_soob(gettext("Не введён табельный номер !"));
        goto naz;
       }

      if(podr == 0)
       {
        iceb_t_soob(gettext("Не введено подразделение !"));
        goto naz;
       }
      if(kateg == 0)
       {
        iceb_t_soob(gettext("Не введена категория !"));
        goto naz;
       }

      /*Запись карточки*/
      if(mtd == 0 && blok == 0 && flagrk == 0)
       {
        zapkrtb(mz,mp,gp,stbn,tabn,fio.ravno(),dolg.ravno(),zvan,podr,kateg,dn,mn,gn,du,mu,gu,
        shet.ravno(),sovm,inn.ravno(),adres.ravno(),nomp.ravno(),vidan.ravno(),telef.ravno(),kodg,harac.ravno(),lgot.ravno(),bankshet.ravno(),dv,mv,gv,tipkk.ravno(),
        drog,mrog,grog,pol,dppz,mppz,gppz,kodbank,mtd+blok,&kodss);
        stbn=tabn;
        mz=1;
       }
      dirzarp(mp,gp,tabn,podr,1,fio.ravno(),shet.ravno(),dn,mn,gn,du,mu,gu);
      if(mtd == 0 && blok == 0 && flagrk == 0)
       {
        zar_pr_start(3,tabn,mp,gp);
        zarsoc(mp,gp,tabn);
        zaravpr(tabn,mp,gp,NULL);
       }

//      goto naz;
      goto read;
      
    case FK3: /*Удалить карточку*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(zarudkar(tabn) == 0)
        return;
      else      
        goto naz;

    case FK4: /*Удержания*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(uvol != 0 && sald == 0.)
       {
        sprintf(strsql,"%s ! %s",gettext("Внимание"),gettext("Работника уволено !"));
        iceb_t_soob(strsql);
       }
      if(fio.ravno()[0] == '\0')
       {
        iceb_t_soob(gettext("Не введена фамилия !"));
        goto naz;
       }

      if(tabn == 0)
       {
        iceb_t_soob(gettext("Не введён табельный номер !"));
        goto naz;
       }

      if(podr == 0)
       {
        iceb_t_soob(gettext("Не введено подразделение !"));
        goto naz;
       }
      if(kateg == 0)
       {
        iceb_t_soob(gettext("Не введена категория !"));
        goto naz;
       }

      /*Запись карточки*/
      if(mtd == 0 && blok == 0 && flagrk == 0)
       {
        zapkrtb(mz,mp,gp,stbn,tabn,fio.ravno(),dolg.ravno(),zvan,podr,kateg,dn,mn,gn,du,mu,gu,
        shet.ravno(),sovm,inn.ravno(),adres.ravno(),nomp.ravno(),vidan.ravno(),telef.ravno(),kodg,harac.ravno(),lgot.ravno(),bankshet.ravno(),dv,mv,gv,tipkk.ravno(),
        drog,mrog,grog,pol,dppz,mppz,gppz,kodbank,mtd+blok,&kodss);

        stbn=tabn;
        mz=1;
       }
      dirzarp(mp,gp,tabn,podr,2,fio.ravno(),shet.ravno(),dn,mn,gn,du,mu,gu);

      if(mtd == 0 && blok == 0 && flagrk == 0)      
       {
        zar_pr_start(3,tabn,mp,gp);
      
        zarsoc(mp,gp,tabn);
        zaravpr(tabn,mp,gp,NULL);
       }
//      goto naz;
      goto read;

    case FK5:  /*Распечатать начисления и удержания*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать начисления/удержания на экран"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать проводки на экран"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать кассовый ордер"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать корешок"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать соц. отчисления"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатка карточки работника"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

      switch (kom)
       {
        case 0 :
 	 clear(); /*Очистить экран и закрасить фоновым цветом*/

         rasnuptn(mp,gp,tabn,fio.ravno());
         break;

        case 1 :
          zarraspr(tabn,mp,gp,fio.ravno());
          break;

        case 2 :
          DANET.VVOD_delete();
          DANET.VVOD_SPISOK_add_MD(gettext("Распечатать все ?"));

          i=danet(&DANET,2,stdscr);

          nah=uder=0.;
          if(i == 2)
            rasnu(gp,mp,tabn,sald,0,fio.ravno());
          else
            nahud(gp,mp,tabn,&nah,&nahprov,&uder,&uderprov,&nahb,&uderb,&uderbprov,1,sald,0,fio.ravno());
          break;

        case 3 :
          koresh(tabn,mp,gp,fio.ravno(),inn.ravno(),dolg.ravno(),kateg);
          break;
        case 4 :
          rassocp(mp,gp,tabn,fio.ravno());
          break;

        case 5 :
          raszkart(tabn);
          break;
       }
      goto naz;

    case SFK5: /*Работа с проводками*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      dirprzar(tabn,mp,gp,fio.ravno());
       
      goto naz;

    case FK6: /*Табель*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      dirtabel(mp,gp,tabn,fio.ravno());  
      goto naz;

    case SFK6: //Просмотр и корректировка отчислений на соц-страх
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      dirsoc(tabn,mp,gp);
      if(mtd == 0 && blok == 0 && flagrk == 0)
        zaravpr(tabn,mp,gp,NULL);

      goto naz;

    case FK7: /*Отметить совместителя*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(provblokzarp(tabn,mp,gp) != 0) //dirzarp.c
          goto naz;

      clearstr(LINES-1,0);
      DANET.VVOD_delete();
      if(sovm == 0)
        DANET.VVOD_SPISOK_add_MD(gettext("Пометить как совместителя ?"));
      if(sovm == 1)
        DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку совместителя ?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz;
      sovm++;
      if(sovm == 2)
        sovm=0;
              
      goto naz;

    case SFK7: /*Работа с настройками автоматического расчёта начислений*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(flagrk != 0)
       {
        iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
        goto naz;
       }

      vkart_sf7(tabn);
  
      
      goto naz;
      
    case FK8:  /*Ввести НСИ*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      vnsi(-1);
      goto naz;

    case SFK8:  /*Заблокировать/разблокировать карточку*/
      attroff(VV.VVOD_return_cs(iceb_CFS));

      clearstr(LINES-1,0);
      DANET.VVOD_delete();
      if(blok == 0)
        DANET.VVOD_SPISOK_add_MD(gettext("Заблокировать карточку ? Вы уверены ?"));
      if(blok != 0 )
        DANET.VVOD_SPISOK_add_MD(gettext("Разблокировать карточку ? Вы уверены ?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz;
      
      if(blok != 0 && blok != iceb_t_getuid())
       {
        PAR.VVOD_delete();
        sprintf(strsql,"%s %s",gettext("Карточка заблокирована"),iceb_t_kszap(blok));
        PAR.VVOD_SPISOK_add_MD(strsql);
        PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

        if(parolv(&PAR,ddd,mmm,ggg,0) != 0)
          goto naz;
        
       }

      if(blok == 0)
       blok=iceb_t_getuid();
      else
       blok=0;

      sprintf(strsql,"update Zarn set blok=%d \
where tabn=%d and god=%d and mes=%d",blok,tabn,gp,mp);
/*
          printw("strsql=%s\n",strsql);
          OSTANOV();
*/
      if(sql_zap(&bd,strsql) != 0)
         msql_error(&bd,"Ошибка ввода записи !",strsql);
      goto naz;

    case DELETE:  /*Безусловное снятие блокировки*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      if(blok == 0)
       goto naz;
       
      clear();

      helstr(LINES-1,0,"F10",gettext("выход"),NULL);

      PAR.VVOD_delete();
      PAR.VVOD_SPISOK_add_ZAG(gettext("Режим снятия блокировки"));
      PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
      move(0,0);

      if(parolv(&PAR,ddd,mmm,ggg,0) != 0)
        goto naz;

      blok=0;        
      sprintf(strsql,"update Zarn set blok=0 \
where tabn=%d and god=%d and mes=%d",tabn,gp,mp);

      sql_zapis(strsql,0,0);

      goto naz;
           
    case FK9:     /*Посмотреть льготы*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      clear();
      poifl(tabn);
      goto naz;

    case SFK9:     /*Ввод дополнительной информации*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(flagrk != 0)
       {
        iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
        goto naz;
       }

      rabdopinf(tabn);

      goto naz;
        
    case TAB: /*Работать с дополнительной информацией*/

      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      if(tabn == 0 || fio.ravno()[0] == '\0')
       {
        VVOD SOOB(1);
        if(tabn == 0)
          SOOB.VVOD_SPISOK_add_MD(gettext("Не введён табельный номер !"));
        if(fio.ravno()[0] == '\0')
          SOOB.VVOD_SPISOK_add_MD(gettext("Не введена фамилия !"));

        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }

      vkart1(tabn,fio.ravno(),&adres,&nomp,&vidan,&telef,&kodg,&harac,&inn,&bankshet,&dv,&mv,&gv,&tipkk,
      &drog,&mrog,&grog,&pol,&dppz,&mppz,&gppz,&kodbank,&kodss);

      goto naz;

    case PD:  /*Увеличить дату*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      mp++;
      if(mp == 13)
       {
        mp=1;
        gp++;
       }
      goto read;
      
     case PU:    /*Уменьшить дату*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
       mp--;
       if(mp == 0)
        {
         mp=12;
         gp--;
        }
       goto read;

   }

     
 }

kon:;

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CFM));

/* Записываем данные */
if(tabn == 0 || fio.ravno()[0] == '\0')
 {
  VVOD SOOB(1);
  if(tabn == 0)
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён табельный номер !"));
  if(fio.ravno()[0] == '\0')
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введена фамилия !"));

  SOOB.VVOD_SPISOK_add_MD(gettext("Карточка не записывается"));

  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
else
 {
/*
    printw("\nЗаписываем mz=%d kodg=%d\n",mz,kodg);
    OSTANOV();
*/
//  if(mtd == 0 && blok == 0 && flagrk == 0)
   zapkrtb(mz,mp,gp,stbn,tabn,fio.ravno(),dolg.ravno(),zvan,podr,kateg,dn,mn,gn,
   du,mu,gu,shet.ravno(),sovm,inn.ravno(),adres.ravno(),nomp.ravno(),vidan.ravno(),telef.ravno(),kodg,harac.ravno(),lgot .ravno(),
   bankshet.ravno(),dv,mv,gv,tipkk.ravno(),drog,mrog,grog,pol,dppz,mppz,gppz,kodbank,mtd+blok,&kodss);
 }

/*Снять флаг работы с карточкой*/
//sprintf(strsql,"z%s%d",imabaz,stbn);
//sql_flag(&bd,strsql,0,1);

attron(VV.VVOD_return_cs(iceb_CB));

}
/*************************************/
/*Работа с дополнительной информацией*/
/*************************************/

void	rabdopinf(int tabn)
{
char	strsql[512];
char	bros[512];
GDITE();


sprintf(strsql,"tabn%d.tmp",getpid());
sprintf(bros,"%d",tabn);
dikont('-',strsql,bros,"Kartb1","");
vizred(strsql);
GDITE();
dikont('+',strsql,bros,"Kartb1","");
unlink(strsql);


}
/**********************/
/*выбор режима*/
/********************/
void vkart_sf7(int tabn)
{

int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка настройки автоматического расчёта начислений"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка должностного оклада"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
 continue;

switch (kom)
 {
  case 3:
  case -1 :
    return;


  case 0:
   zarnrn(tabn);
   break;

  case 1:
   dirzarsdo(tabn);
   break;

 }



}
