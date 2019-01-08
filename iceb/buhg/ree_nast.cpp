/*$Id: ree_nast.c,v 5.16 2013/09/26 09:43:39 sasa Exp $*/
/*15.07.2015	30.08.2005	Белых А.И.	ree_nast.c
Чтение необходимых настроек для работы реестра налоговых накладных
*/
#include	"buhg.h"



double		okrcn=0.;  /*Округление цены*/
double		okrg1=0.;  /*Округление суммы*/
short           startgodb=0; //Стартовый год для подсистемы "Главная книга"
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
class iceb_rnfxml_data rek_zag_nn; /*Класс с данными для создания xml файлов*/

short dp=0,mp=0,gp=0; //Дата начала просмотра реестров

int ree_nast()
{
class iceb_tu_str st1("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

okrg1=okrcn=0.;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&st1,"nastrb.alx") == 0)
 {
  startgodb=st1.ravno_atoi();
 }

vplsh=0;
iceb_t_poldan("Многопорядковый план счетов",&st1,"nastrb.alx");
if(SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     polen(row_alx[0],&okrg1,2,'|');
//     okrg1=ATOFM(st1);
     continue;
    }
  if(SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     polen(row_alx[0],&okrcn,2,'|');
//     okrcn=ATOFM(st1);
     continue;
    }
 }


sprintf(strsql,"select str from Alx where fil='rnn_nast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"rnn_nast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Дата начала просмотра реестра",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     rsdat(&dp,&mp,&gp,st1.ravno(),1);
     continue;
    }
 }
iceb_t_rnfxml(&rek_zag_nn);
return(0);

}
