/* $Id: korprovnbs.c,v 5.20 2013/09/26 09:43:35 sasa Exp $ */
/*16.04.2017	07.06.2001	Белых А.И.	korprovnbs.c
Корректировка проводок для небалансовых счетов
*/
#include        "buhg.h"


void            korprovnbs(VVOD *VV,short d,short m,short g, //Дата
const char *sh,  //Счёт
const char *kor, //Код контрагента
time_t vrem,  //время записи
int ktoi,
const char *kto)
{
char		strsql[1024];
SQL_str   	row;
SQLCURSOR       cur;
time_t		vremm;
struct  tm      *bf;
short           d1,m1,g1;
double		deb,kre;
short		i;
class iceb_tu_str kontr("");
class iceb_tu_str naik("");
class iceb_tu_str komentz("");
class iceb_tu_str bros("");

bf=localtime(&vrem);

komentz.new_plus(VV->VVOD_SPISOK_return_data(2));
deb=ATOFM(VV->VVOD_SPISOK_return_data(3));
deb=okrug(deb,0.01);
kre=ATOFM(VV->VVOD_SPISOK_return_data(4));
kre=okrug(kre,0.01);

VV->VVOD_SPISOK_add_MD(gettext("Счёт........"));
VV->VVOD_SPISOK_add_MD(gettext("Дата........"));
VV->VVOD_SPISOK_add_MD(gettext("Коментарий.."));
VV->VVOD_SPISOK_add_MD(gettext("Дебет......."));
VV->VVOD_SPISOK_add_MD(gettext("Кредит......"));
VV->VVOD_SPISOK_add_MD(gettext("Код контр. ."));

naz:;


if(VV->VVOD_SPISOK_return_data(5)[0] != '\0')
 {
  naik.new_plus("");
  kontr.new_plus(VV->VVOD_SPISOK_return_data(5));

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  kontr.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"1 %s %s !",gettext("Не найден код контрагента"),kontr.ravno());
    iceb_t_soob(strsql);
   }
  else
    naik.new_plus(row[0]);

 }

VV->VVOD_delete_ZAG();

sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
gettext("Дата ввода проводки"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,bf->tm_sec);
VV->VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s: %s",gettext("Записал"),iceb_t_kszap(ktoi));
VV->VVOD_SPISOK_add_ZAG(strsql);

if(kontr.ravno()[0] != '\0')  
 {
  sprintf(strsql,gettext("Контрагент: %s %s"),kontr.ravno(),naik.ravno());
  VV->VVOD_SPISOK_add_ZAG(strsql);
 }

int PY=-1; 
int PX=-1; /*Координаты вывода меню*/


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("контрагенты"),
"F10",gettext("выход"),NULL);

int par=VV->vvod(0,1,1,PY,PX);

GDITE();

if(par == FK2 || par == PLU)
 {
  if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введён счёт !"));
    goto naz;
   }

  if(rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(1),0) != 0)
   {
    iceb_t_soob(gettext("Не верно ведена дата проводки !"));
    goto naz;
   }

  if(ATOFM(VV->VVOD_SPISOK_return_data(3)) != 0. && ATOFM(VV->VVOD_SPISOK_return_data(4)) != 0.)
   {
    iceb_t_soob(gettext("Введено дебет и кредит !"));
    goto naz;
   }
  if(iceb_t_pvglkni(m,g) != 0)
   goto naz;
  if(iceb_t_pvglkni(m1,g1) != 0)
   goto naz;

  if(iceb_t_pvkup(ktoi) != 0) /*корректировать может только хозяин или тот кому разрешена блокировкa/разблокировка*/
   goto naz;
   

  if(prvkp(kto) != 0)
     goto naz;

  kontr.new_plus("");
  if(VV->VVOD_SPISOK_return_data(5)[0] != '\0')
     kontr.new_plus(VV->VVOD_SPISOK_return_data(5));

  /*Проверяем есть ли счёт в списке счетов*/
  OPSHET shetv;
  if(prsh1(VV->VVOD_SPISOK_return_data(0),&shetv) != 0)
    goto naz;

  if(shetv.stat == 0)
   {
    sprintf(strsql,gettext("Счёт %s балансовый !"),VV->VVOD_SPISOK_return_data(0));
    iceb_t_soob(strsql);
    goto naz;

   }
  if(shetv.saldo == 3 && kontr.ravno()[0] == '\0')
   {
    sprintf(strsql,gettext("Не введён контрагент для счёта %s !"),VV->VVOD_SPISOK_return_data(0));
    iceb_t_soob(strsql);
    goto naz;
   }


  /*Проверяем есть ли такой код контрагента*/
  if(shetv.saldo == 3)
  if(kontr.ravno()[0] != '\0')
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV->VVOD_SPISOK_return_data(0),kontr.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,gettext("Нет кода контрагента %s для счёта %s !"),kontr.ravno(),VV->VVOD_SPISOK_return_data(0));
      iceb_t_soob(strsql);
      goto naz;
     } 
   }



  if( shetv.saldo == 3 && kontr.ravno()[0] == '\0' )
   {  
    iceb_t_soob(gettext("Не введён код контрагента !"));
    goto naz;
   }
  if(shetv.saldo < 3 && kontr.ravno()[0] != '\0')
   {  
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Счёт не с развернутым сальдо !"));
    SOOB.VVOD_SPISOK_add_MD(gettext("Код контрагента не вводится !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }

  if(iceb_t_pbsh(m,g,sh,"","") != 0)
   goto naz;
     
  if(udprov(-1,g,m,d,sh,"",vrem,deb,kre,komentz.ravno_filtr(),1) != 0)
     return;

  rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(1),0);
  deb=ATOFM(VV->VVOD_SPISOK_return_data(3));
  i=2;
  if(VV->VVOD_SPISOK_return_data(3)[0] == '-')
    i++;
  if(polen(VV->VVOD_SPISOK_return_data(3),&bros,i,'-') == 0)
    deb-=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(3),&bros,2,'+') == 0)
    deb+=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(3),&bros,2,'/') == 0)
    deb/=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(3),&bros,2,'*') == 0)
    deb*=bros.ravno_atof();

  kre=ATOFM(VV->VVOD_SPISOK_return_data(4));
  i=2;
  if(VV->VVOD_SPISOK_return_data(4)[0] == '-')
    i++;
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,i,'-') == 0)
    kre-=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'+') == 0)
    kre+=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'/') == 0)
    kre/=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'*') == 0)
    kre*=bros.ravno_atof();

  deb=okrug(deb,0.01);
  kre=okrug(kre,0.01);
  time(&vremm);
  zapprov(-1,g1,m1,d1,VV->VVOD_SPISOK_return_data(0)," ",kontr.ravno(),kontr.ravno(),kto,"","",deb,kre,VV->data_ravno_filtr(2),1,0,vremm,0,0,0,0,0);
 }

if(par == FK3)
 {
  clear();
  class iceb_tu_str kodkontr("");
  class iceb_tu_str naimkontr("");

  dirkontr(&kodkontr,&naimkontr,1);

  clear();

  goto naz;
 }
clear();
GDITE();
}
/**********************************/
void	sozkorprovnbs(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(11);
VV->VVOD_SPISOK_add_data(100);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
}
