/* $Id: podudok.c,v 5.19 2013/09/26 09:43:37 sasa Exp $ */
/*15.07.2015	15.05.2000	Белых А.И.	podudok.c
Проверка подтверждения документа для услуг
*/
#include        <math.h>
#include	"buhg.h"


void podudok(const char *datdok,const char *nn,int podr,int tipz)
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,datdok,1);
podudok(d,m,g,nn,podr,tipz);
}
/************************************************/

void podudok(short d,short m,short g,const char *nn,int podr,int tipz)
{
double          kol1,kol2;
short           mz; /*mz=0 подтверждена 1-нет*/
SQL_str         row;
char		strsql[512];
int		kolstr;
int		pod;
int		metka;
int		kodzap;

mz=0;

sprintf(strsql,"select metka,kodzap,kolih,shetu,nz from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
g,m,d,podr,nn,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

  
kol1=0.;
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kol1=atof(row[2]);

  kol2=readkolu(d,m,g,podr,nn,tipz,metka,kodzap,atoi(row[4]));

  if(fabs(kol1-kol2) > 0.00001)
   {
    mz=1;
    break;
   }

 }

if(kol1 == 0.)
 mz=1;


pod=0;  //Не подтвержден
if(mz == 0) /*Подтверждена*/
  pod=1; //Подтвержден

//Если дата заблокирована то не устанавливаем метку не подтвержденного документа
if(pod == 0)
 {
//  sprintf(strsql,"%s",iceb_t_imafnsi("uslugbl.alx"));
  if(iceb_t_pblok(m,g,ICEB_PS_UU) != 0) //Дата заблокирована БЕЗ СООБЩЕНИЯ!!!
    return;
 }

sprintf(strsql,"update Usldokum \
set \
pod=%d \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
pod,g,m,d,podr,nn,tipz);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   msql_error(&bd,__FUNCTION__,strsql);
  }

}
