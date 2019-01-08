/* $Id: korprov.c,v 5.25 2013/09/26 09:43:35 sasa Exp $ */
/*15.07.2015    18.02.1993      Белых А.И.      korprov.c
Корректировка проводки
*/
#include        "buhg.h"


void korprov(VVOD *VV,short d,short m,short g, //Дата
const char *sh,  //Счёт
const char *shk, //Счёт корреспондент
const char *kor, //Код контрагента
time_t vrem,  //время записи
int ktoi,
const char *kto)
{
char		strsql[1024];
SQL_str   	row;
SQLCURSOR       cur;
class iceb_tu_str bros("");
time_t		vremm;
struct  tm      *bf;
int             par;
time_t          tmm;
struct OPSHET	shetv;
struct OPSHET	shetv1;

short           d1,m1,g1;
double		deb,kre;
short		i;
class iceb_tu_str kontr(""),kontr1("");
class iceb_tu_str naik(""),naik1("");
class iceb_tu_str komentz("");
int		strok=0;
int kekv=0;
/*
printw("Корректировка.\n");
OSTANOV();
*/

deb=ATOFM(VV->VVOD_SPISOK_return_data(4));
deb=okrug(deb,0.01);
kre=ATOFM(VV->VVOD_SPISOK_return_data(5));
kre=okrug(kre,0.01);
komentz.new_plus(VV->VVOD_SPISOK_return_data(3));


VV->VVOD_SPISOK_add_MD(gettext("Счёт........"));//0
VV->VVOD_SPISOK_add_MD(gettext("Счёт к-нт..."));//1
VV->VVOD_SPISOK_add_MD(gettext("Дата........"));//2
VV->VVOD_SPISOK_add_MD(gettext("Коментарий.."));//3
VV->VVOD_SPISOK_add_MD(gettext("Дебет......."));//4
VV->VVOD_SPISOK_add_MD(gettext("Кредит......"));//5
VV->VVOD_SPISOK_add_MD(gettext("Код контр. ."));//6
VV->VVOD_SPISOK_add_MD(gettext("КЭКЗ........"));//7


if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
 {
  naik.new_plus("");
  naik1.new_plus("");
  kontr.new_plus("");
  kontr1.new_plus("");
  if(polen(VV->VVOD_SPISOK_return_data(6),&kontr,1,'/') != 0)
   {
    kontr.new_plus(VV->VVOD_SPISOK_return_data(6));
    kontr1.new_plus(VV->VVOD_SPISOK_return_data(6));
   }
  else
   polen(VV->VVOD_SPISOK_return_data(6),&kontr1,2,'/');

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"1 %s %s !",gettext("Не найден код контрагента"),kontr.ravno());
    iceb_t_soob(strsql);
   }
  else
    naik.new_plus(row[0]);

  if(SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr1.ravno());
    if(readkey(strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"2 %s %s !",gettext("Не найден код контрагента"),kontr1.ravno());
      iceb_t_soob(strsql);
     }
    else
      naik1.new_plus(row[0]);
   }
 }

tmm=vrem;
bf=localtime(&tmm);

naz:;

VV->VVOD_delete_ZAG();

sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
gettext("Дата ввода проводки"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,bf->tm_sec);
VV->VVOD_SPISOK_add_ZAG(strsql);

VV->VVOD_SPISOK_add_ZAG(iceb_t_kszap(ktoi));

strok=4;

if(kontr.ravno()[0] != '\0')  
 {
  sprintf(strsql,gettext("Контрагент: %s %s"),kontr.ravno(),naik.ravno());
  VV->VVOD_SPISOK_add_ZAG(strsql);
  strok++;
 }

if(kontr1.ravno()[0] != '\0' && SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
 {
  strok++;
  sprintf(strsql,"%s:%s %s",gettext("Контрагент"),kontr1.ravno(),naik1.ravno());
  VV->VVOD_SPISOK_add_ZAG(strsql);
 }


int PY=-1;  
int PX=-1; /*Координаты вывода меню*/

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("контрагенты"),
"F10",gettext("выход"),NULL);

par=VV->vvod(0,1,1,PY,PX);

if(par == KEY_RESIZE)
  goto naz;
  
GDITE();


if(par == FK2 || par == PLU)
 {

  if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введён счёт !"));
    goto naz;
   }
  if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введён кореспондирующий счёт !"));
    goto naz;
   }
  if(rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(2),1) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не верно ведена дата проводки !"));
    SOOB.VVOD_SPISOK_add_MD(VV->data_ravno(2));
    
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }

  if(ATOFM(VV->VVOD_SPISOK_return_data(4)) != 0. && ATOFM(VV->VVOD_SPISOK_return_data(5)) != 0.)
   {
    iceb_t_soob(gettext("Введено дебет и кредит !"));
    goto naz;
   }

  if(ATOFM(VV->VVOD_SPISOK_return_data(4)) == 0. && ATOFM(VV->VVOD_SPISOK_return_data(5)) == 0.)
   {
    iceb_t_soob(gettext("Не введено ни дебет, ни кредит!"));
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
  kontr1.new_plus("");
  if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
   { 
     if(polen(VV->VVOD_SPISOK_return_data(6),&kontr,1,'/') != 0)
      {
       kontr.new_plus(VV->VVOD_SPISOK_return_data(6));
       kontr1.new_plus(VV->VVOD_SPISOK_return_data(6));
      }
     else
      polen(VV->VVOD_SPISOK_return_data(6),&kontr1,2,'/');
   }
  /*Проверяем есть ли счёт в списке счетов*/

  if(prsh1(VV->VVOD_SPISOK_return_data(0),&shetv) != 0)
    goto naz;

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


  /*Проверяем есть ли счёт корреспондент в списке счетов*/

  if(prsh1(VV->VVOD_SPISOK_return_data(1),&shetv1) != 0)
    goto naz;

  if(shetv1.saldo == 3 && kontr1.ravno()[0] == '\0')
   {
    sprintf(strsql,gettext("Не введён контрагент для счёта %s !"),VV->VVOD_SPISOK_return_data(1));
    iceb_t_soob(strsql);
    goto naz;
   }

  /*Проверяем есть ли такой код контрагента*/
  if(shetv1.saldo == 3)
  if(kontr1.ravno()[0] != '\0')
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV->VVOD_SPISOK_return_data(1),kontr1.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,gettext("Нет кода контрагента %s для счёта %s !"),kontr1.ravno(),VV->VVOD_SPISOK_return_data(1));
      iceb_t_soob(strsql);
      goto naz;
     }
   }

  if(shetv1.saldo != shetv.saldo && SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
   {  
    iceb_t_soob(gettext("Не верно введён код контрагента !"));
    goto naz;
   }

  if((shetv1.saldo == 3 || shetv.saldo == 3) && (kontr.ravno()[0] == '\0' || kontr1.ravno()[0] == '\0'))
   {  
    iceb_t_soob(gettext("Не введён код контрагента !"));
    goto naz;
   }
  if((shetv1.saldo < 3 && shetv.saldo < 3) && (kontr.ravno()[0] != '\0' || kontr1.ravno()[0] != '\0'))
   {  
    iceb_t_soob(gettext("Оба счета со свернутым сальдо ! Код контрагента не вводится !"));
    goto naz;
   }

  if(iceb_t_pbsh(m,g,sh,shk,"") != 0)
   goto naz;
     
  if(udprov(0,g,m,d,sh,shk,vrem,deb,kre,komentz.ravno_filtr(),2) != 0)
    return;
  rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(2),0);
  deb=ATOFM(VV->VVOD_SPISOK_return_data(4));
  i=2;
  if(VV->VVOD_SPISOK_return_data(4)[0] == '-')
    i++;
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,i,'-') == 0)
    deb-=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'+') == 0)
    deb+=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'/') == 0)
    deb/=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(4),&bros,2,'*') == 0)
    deb*=bros.ravno_atof();

  kre=ATOFM(VV->VVOD_SPISOK_return_data(5));
  i=2;
  if(VV->VVOD_SPISOK_return_data(5)[0] == '-')
    i++;
  if(polen(VV->VVOD_SPISOK_return_data(5),&bros,i,'-') == 0)
    kre-=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(5),&bros,2,'+') == 0)
    kre+=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(5),&bros,2,'/') == 0)
    kre/=bros.ravno_atof();
  if(polen(VV->VVOD_SPISOK_return_data(5),&bros,2,'*') == 0)
    kre*=bros.ravno_atof();

  deb=okrug(deb,0.01);
  kre=okrug(kre,0.01);
  time(&vremm);
  kekv=VV->data_ravno_atoi(7);
  zapprov(0,g1,m1,d1,VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),kontr.ravno(),kontr1.ravno(),kto,"","",deb,kre,VV->data_ravno_filtr(3),2,0,vremm,0,0,0,0,kekv);
 }

if(par == FK3)
 {
  clear();
  class iceb_tu_str kodkontr("");
  class iceb_tu_str naimkontr("");
  if(dirkontr(&kodkontr,&naimkontr,1) == 0)
   {
    //Проверяем если только один из счетов имеет развернутое сальдо записывем код контрагента
    prsh1(VV->VVOD_SPISOK_return_data(0),&shetv);
    prsh1(VV->VVOD_SPISOK_return_data(1),&shetv1);

//    printw("\n%d %d\n",shetv.saldo,shetv1.saldo);
//    OSTANOV();

    if(shetv.saldo == 3 && shetv1.saldo != 3 )
     {
      naik.new_plus("");
      kontr1.new_plus("");
      naik1.new_plus("");
      VV->VVOD_SPISOK_zapis_data(6,kodkontr.ravno());

      kontr.new_plus(kodkontr.ravno());
      naik.new_plus(naimkontr.ravno());
     }       
    if(shetv.saldo != 3 && shetv1.saldo == 3)
     {
      naik.new_plus("");
      kontr1.new_plus("");
      naik1.new_plus("");
      VV->VVOD_SPISOK_zapis_data(6,kodkontr.ravno());
      kontr.new_plus(kodkontr.ravno());
      naik.new_plus(naimkontr.ravno());

     }       

    goto naz;
   }

  clear();
  goto naz;
 }
clear();
GDITE();
}
/***************************/
/*Создание списка данных меню*/
/*****************************/
void	sozkorprov(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(11);
VV->VVOD_SPISOK_add_data(100);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(6);

}
