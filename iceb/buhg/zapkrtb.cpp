/* $Id: zapkrtb.c,v 5.20 2013/05/17 14:56:16 sasa Exp $ */
/*22.02.2010	24.05.1999	Белых А.И.	zapkrtb.c
Запись карточки табельного номера в заработной плате
*/
#include	"buhg.h"


void zapkrtb(int mz,//*0-запись нового 1-корректировка
short mp,short gp,  //Дата просмотра карточки
int stbn,int tabn,const char *fio,
const char *dolg,short zvan,int podr,short kateg,short dn,short mn,short gn,
short du,short mu,short gu,const char *shet,short sovm,const char *inn,
const char *adres,const char *nomp,const char *vidan,const char *telef,short kodg,
const char *harac,const char *lgoti,const char *bankshet,
short dv,short mv,short gv, //Дата выдачи документа
const char *tipkk,   //тип кредитной карты
short dr,short mr,short gr, //Дата рождения
short pl, //0-мужчина 1-женщина
short dppz,short mppz,short gppz, //Дата последнего повышения зарплаты
int kodban, /*Код банка для перечисления зарплаты на карт счета*/
int metkazap, //0-переменные данные можно записывать 1-нет
class iceb_tu_str *kodss) /*Код спечстажа*/
{
char		strsql[2048];
class iceb_tu_str fio1(fio);
class iceb_tu_str dolg1(dolg);
class iceb_tu_str adres1(adres);
class iceb_tu_str nomp1(nomp);
class iceb_tu_str vidan1(vidan);
class iceb_tu_str telef1(telef);
class iceb_tu_str harac1(harac);
SQL_str		row;
SQLCURSOR curr;
int		i;
short		kart=0;
char		bros[512];

/*
printw("zapkrtb mz=%d\n",mz);
OSTANOV();
*/

if(mz == 0)
 {   
  sprintf(strsql,"insert into Kartb \
values (%d,'%s','%s',%d,%d,%d,'%04d-%02d-%02d','%04d-%02d-%02d',\
'%s',%d,'%s','%s','%s','%s','%s',%d,'%s','%s','%s','%04d-%d-%d','%s',\
'%04d-%02d-%02d',%d,'%04d-%02d-%02d',%d,'%s')",
tabn,fio1.ravno_filtr(),dolg1.ravno_filtr(),zvan,podr,kateg,gn,mn,dn,gu,mu,du,shet,sovm,inn,adres1.ravno_filtr(),
nomp1.ravno(),vidan1.ravno_filtr(),telef1.ravno_filtr(),kodg,harac1.ravno_filtr(),lgoti,bankshet,gv,mv,dv,tipkk,
gr,mr,dr,pl,gppz,mppz,dppz,kodban,kodss->ravno());


  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
     return;
   else
    {
     sprintf(bros,"zapkrtb-%s",gettext("Ошибка записи !"));
     msql_error(&bd,bros,strsql);
    }
   }
 }

if(mz == 1)
 {
  //Запись постоянной информации
  sprintf(strsql,"update Kartb \
set \
tabn=%d,\
fio='%s',\
inn='%s',\
adres='%s',\
nomp='%s',\
vidan='%s',\
telef='%s',\
kodg=%d,\
harac='%s',\
bankshet='%s',\
datvd='%04d-%d-%d',\
tipkk='%s',\
denrog='%04d-%02d-%02d',\
pl=%d,\
dppz='%04d-%02d-%02d',\
kb=%d,\
kss='%s' \
where tabn=%d",
  tabn,fio1.ravno_filtr(),inn,adres1.ravno_filtr(),nomp1.ravno(),vidan1.ravno_filtr(),telef1.ravno_filtr(),kodg,harac1.ravno_filtr(),bankshet,gv,mv,dv,tipkk,
  gr,mr,dr,pl,gppz,mppz,dppz,kodban,kodss->ravno(),
  stbn);

/*
  printw("\n%s\n",strsql);
  OSTANOV();
*/
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
     return;
   else
    {
     sprintf(bros,"zapkrtb-%s",gettext("Ошибка записи !"));
     msql_error(&bd,bros,strsql);
    }
   }
  if(metkazap != 0)
   return;
  //Проверка необходимости записи переменных данных
  sprintf(strsql,"select tabn from Zarn where ((god > %d or \
(god=%d and  mes >=%d)) and tabn=%d)",
  gp,gp,mp,tabn);
//  printw("\n%s\n",strsql);
//  OSTANOV();
  if((i=sql_readkey(&bd,strsql,&row,&curr)) <= 1)
      kart=1;

  struct  tm      *bf;
  time_t          tmm;

  time(&tmm);
  bf=localtime(&tmm);

  if(bf->tm_mon+1 == mp && bf->tm_year+1900 == gp)
    kart=1;
/*
  printw("kart=%d\n",kart);
  OSTANOV();
*/  
  if(kart != 0)
   {
    sprintf(strsql,"update Kartb \
set \
dolg='%s',\
datn='%04d-%02d-%02d',\
datk='%04d-%02d-%02d',\
zvan=%d,\
podr=%d,\
kateg=%d,\
shet='%s',\
sovm=%d,\
lgoti='%s' \
where tabn=%d",
    dolg1.ravno_filtr(),gn,mn,dn,gu,mu,du,zvan,podr,kateg,shet,sovm,lgoti,
    stbn);
/*
    printw("\n%s\n",strsql);
    OSTANOV();
*/
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,"zapkrtb-2 Ошибка корректировки записи карточки !",strsql);
/*
    Если удалить все начисления и удержания то при вводе даты увольнения
    промежуточная запись не обновляется
*/
    sprintf(strsql,"update Zarn \
set \
podr=%d,\
datn='%04d-%02d-%02d',\
datk='%04d-%02d-%02d' \
where god=%d and mes=%d and tabn=%d",
    podr,gn,mn,dn,gu,mu,du,gp,mp,tabn);
//    printw("\n%s\n",strsql);
//    refresh();
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,"zapkrtb-3 Ошибка корректировки записи !",strsql);
   }   
/*
  printw("\ni=%d\n%s\n",i,strsql);
  OSTANOV();  
*/

 }


}
