/* $Id: podvdok.c,v 5.18 2013-04-07 10:22:43 sasa Exp $ */
/*31.10.2009    23.03.1994      Белых А.И.      podvdokf.c
Просматриваем соответcтвует ли количесво в карточках
количеству материалла по записи в накладных
Если нет то выствляем метку не подтверждённого документа
Если да то снимаем метку не подтверждённого документа
*/
#include        <math.h>
#include "bezcurses.h"

void podvdok(short d,short m,short g,const char *nn,int skll)
{
double          kol1,kol2;
short           mz; /*mz=0 подтверждена 1-нет*/
short		dd,mm,gg;
SQL_str         row;
char		strsql[512];
int		kolstr;
int		pod,nk;

//printw("podvdok-%d.%d.%d %s %d\n",d,m,g,nn,skll);
//OSTANOV();


mz=0;

sprintf(strsql,"select datd,nomkar,kolih from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skll,nn);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr <= 0)
 { 
  mz=1;
  goto vp;
 }
  
kol1=0.;
while(cur.read_cursor(&row) != 0)
 {

  kol1=atof(row[2]);
  nk=atoi(row[1]);
  /*Читаем в карточке количество реально выданное-полученное*/
  rsdat(&dd,&mm,&gg,row[0],2);
  kol2=readkolk(skll,nk,dd,mm,gg,nn);

  if(fabs(kol1-kol2) > 0.00001)
   {
    mz=1;
/*
    printw("\n%-3d %-3d %-4s %8.8g %8.8g\n",
    mk2.kodm,mk2.nk,mk2.ei,mk2.kolih,mk2.cena);
    OSTANOV();
  */
    break;
   }

 }

if(kol1 == 0.)
 mz=1;


//printw("\npodvdok-mz=%d kol1=%f kol2=%f\n",mz,kol1,kol2);
//OSTANOV();


vp:;

pod=0; //Не подтвержден
if(mz == 0) /*Подтверждена*/
  pod=1; //Подтвержден

//Если дата заблокирована то не устанавливаем метку не подтвержденного документа
if(pod == 0)
 {
//  sprintf(strsql,"%s",iceb_t_imafnsi("blokmak.alx"));
  if(iceb_t_pblok(m,g,ICEB_PS_MU) != 0) //Дата заблокирована не должно быть сообщения!!!
    return;
 }

sprintf(strsql,"update Dokummat \
set \
pod=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
pod,g,m,d,skll,nn);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  msql_error(&bd,__FUNCTION__,strsql);

}
