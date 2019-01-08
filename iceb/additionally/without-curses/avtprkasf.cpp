/* $Id: avtprkas.c,v 5.30 2013/05/17 14:55:55 sasa Exp $ */
/*06.09.2013	05.10.2000	Белых А.И.	avtprkasf.c
Автоматическое выполнение пpоводок для кассовых оpдеpов
*/
#include        <errno.h>
#include        <time.h>
#include        <math.h>
#include        "bezcurses.h"
//#include <unistd.h>

void avtprkas_kekv(int tipz,const char *kod_op,const char *shet_kor,int *kekv);


void avtprkas(const char *kassa,short tipz,const char *nomd,
short dd,short md,short gd, //Дата документа
const char *shetk,const char *shetd,const char *kodop)
{
int kekv=0;
char		strsql[1024];
int		kolstr;
SQL_str		row;
short		d,m,g;
double		deb=0.,kre=0.;
class iceb_tu_str kor1("");
class iceb_tu_str kor2("");
time_t		vrem;
class iceb_tu_str kto(ICEB_MP_KASA);
double		suma;
double		sum;
struct OPSHET	shetkk;
struct OPSHET	shetdd;
class SQLCURSOR cur;
/*
printw("\navtprkas-%s %d %s %d.%d.%d %s %s %s\n",
kassa,tipz,nomd,dd,md,gd,shetk,shetd,kodop);
OSTANOV();
*/
 
if(prsh1(shetk,&shetkk) != 0)
   return;

if(prsh1(shetd,&shetdd) != 0)
   return;
if(shetkk.stat == 1)
 {
  printf("%s %s %s!\n",gettext("Счет"),shetk,gettext("внебалансовый"));
  return;
 }
if(shetdd.stat == 1)
 {
  printf("%s %s %s!\n",gettext("Счет"),shetk,gettext("внебалансовый"));
  return;
 }
class iceb_tu_str kod_val("");
/*Узнаём код валюты*/
sprintf(strsql,"select kv from Kas where kod=%d",atoi(kassa));
if(readkey(strsql,&row,&cur) == 1)
 {
  kod_val.new_plus(row[0]);
 }
class iceb_getkue_dat kue(kod_val.ravno());

class lock_tables kkkk("LOCK TABLES Prov WRITE,Kasord READ,Kasord1 READ,Alx READ,Blok READ,icebuser READ,Glkkv READ");

sprintf(strsql,"select datp,kontr,suma,koment from Kasord1 where \
kassa=%s and nomd='%s' and tp=%d and god=%d and suma != 0. and \
datp != '0000-00-00'",kassa,nomd,tipz,gd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/

if(kolstr == 0)
 {
  return;
 } 


/*Удаляем все проводки с проверкой возможности удаления*/
if(udprgr(ICEB_MP_KASA,dd,md,gd,nomd,atoi(kassa),tipz) != 0)
 return;

avtprkas_kekv(tipz,kodop,shetd,&kekv); /*Определяем kekv*/

/*Узнаём основание*/
SQL_str row1;
class SQLCURSOR cur1;
class iceb_tu_str osnov("");

sprintf(strsql,"select osnov from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",nomd,atoi(kassa),tipz,gd);

if(readkey(strsql,&row1,&cur1) > 0)
 osnov.new_plus(row1[0]);

class iceb_tu_str kom_new("N");
kom_new.plus(nomd);
kom_new.plus(" ",osnov.ravno());
//sprintf(kom_new,"N%s %s",nomd,osnov);

class iceb_tu_str koment;
koment.plus(strsql);
suma=0.;
time(&vrem);
while(cur.read_cursor(&row) != 0)
 {
  koment.new_plus(kom_new.ravno());

  if(rsdat(&d,&m,&g,row[0],2) == 0)
   {
    if(iceb_t_pvglkni(m,g) != 0)
     continue;
   }
  sum=kue.fromue(row[2],row[0]);
//  sum=atof(row[2]);
  if(tipz == 1)
   {
    deb=sum;
    kre=0.;
   }
  if(tipz == 2)
   {
    deb=0;
    kre=sum;
   }
   
  kor1.new_plus("");
  kor2.new_plus("");

  if(shetkk.saldo == 3 || shetdd.saldo == 3)
   {
    kor1.new_plus(row[1]);
    kor2.new_plus(row[1]);
   }  

  if(row[3][0] != '\0')
   koment.plus("/",row[3]);
  zapprov(0,g,m,d,shetk,shetd,kor1.ravno(),kor2.ravno(),kto.ravno(),nomd,kodop,deb,kre,koment.ravno_filtr(),2,atoi(kassa),vrem,gd,md,dd,tipz,kekv);
  suma+=sum;
 }

}
/**************************/
/*Определение кода экономических затрат для проводки*/
/*****************************************************/

void avtprkas_kekv(int tipz,const char *kod_op,const char *shet_kor,int *kekv)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
*kekv=0;

class iceb_tu_str imaf_nastr;
if(tipz == 1)
 imaf_nastr.new_plus("avtprokasp.alx");
if(tipz == 2)
 imaf_nastr.new_plus("avtprokasr.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nastr.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

class iceb_tu_str kod_opf("");
class iceb_tu_str shet_korf("");
int kekvf=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;  
  if(polen(row_alx[0],&kod_opf,1,'|') != 0)
   continue;

  if(SRAV(kod_opf.ravno(),kod_op,0) != 0)
   continue;

  if(polen(row_alx[0],&shet_korf,2,'|') != 0)
   continue;

  if(SRAV(shet_korf.ravno(),shet_kor,0) != 0)
   continue;

  if(polen(row_alx[0],&kekvf,3,'|') != 0)
   continue;

  *kekv=kekvf;
  break;  

 }

}
