/*$Id: pvbanks.c,v 5.53 2014/08/31 06:18:18 sasa Exp $*/
/*23.07.2018	06.10.2003	Белых А.И.	pvbanks.c
Расчёт перечисления в банк на карт счета на основе конечного сальдо
*/
#include        <errno.h>
#include "buhg.h"

void pvbavks_pib(SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,const char *shet,const char *shet_kor,short dr,short mr,short gr,int kod_banka);
void pvbavks_uks(SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,const char *shet,const char *shet_kor,short dr,short mr,short gr,int kod_banka);

void pvbanks_txt(int metkabk,SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,const char *shet,const char *shet_kor,short dr,short mr,short gr,const char *kod00,int kod_banka);

void pbanks_dbf(int,SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,const char *shet,const char *shet_kor,short dr,short mr,short gr,int kod_banka);

int pvbanks_rozn(class iceb_tu_str *shet_kor,short *dr,short *mr,short *gr);
void pvbanks_make_prov(const char*shet,const char*shet_kor,short dr,short mr,short gr,int metkadk,const char*kontr,const char*fio,double sum);

extern short	startgodb; /*Стартовый год*/

void	pvbanks()
{
class iceb_tu_str bros("");
char strsql[512];
int kolstr;
int kolkontr=0,kolstr1=0;
class SPISOK KONT;
class SQLCURSOR cur;
short dk,mk,gk;
static class iceb_tu_str shet("");
static class iceb_tu_str kontr("");
static class iceb_tu_str dat2("");
static class iceb_tu_str grupk("");
class iceb_tu_str naim("");
int kom=0,kom1=0;
short       gods=startgodb;
int metka_rozn=0;

int metkadk=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка документов для перечисления на карт-счета"));

VV.VVOD_SPISOK_add_MD(gettext("Счёт.................(,,)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата сальдо........(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента........(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код группы контрагента.(,,)."));//3

VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(kontr.ravno(),128); 
VV.VVOD_SPISOK_add_data(grupk.ravno(),128); 

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F6",gettext("расчитать"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

shet.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kontr.new_plus(VV.data_ravno(2));
grupk.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Gkont",&grupk,&naim);
        VV.VVOD_SPISOK_zapis_data(3,grupk.ravno());
      
        goto naz;


      case 1 :
        vibrek("Plansh",&shet,&naim);
        VV.VVOD_SPISOK_zapis_data(0,shet.ravno());
        goto naz;

      case 2 :
        vibrek("Kontragent",&kontr,&naim);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;
     }

  case FK6:
     metka_rozn=1;
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(shet.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт !"));
  goto naz;
 }

if(rsdat(&dk,&mk,&gk,dat2.ravno(),1) != 0)
 goto naz;

short dr=0,mr=0,gr=0;
class iceb_tu_str shet_kor;

if(metka_rozn == 1) /*расчёт с выполнением проводок*/
 {
  struct OPSHET rek_shet;
  if(prsh1(shet.ravno(),&rek_shet) != 0)
   goto naz;

  if(rek_shet.saldo != 3)
   {
    iceb_t_soob(gettext("Счёт имеет свёрнутое сальдо!"));
    goto naz;
   }

  if(pvbanks_rozn(&shet_kor,&dr,&mr,&gr) != 0)
   goto naz;

 }


if(gods == 0)
 gods=gk;

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Дебет"));
MENU.VVOD_SPISOK_add_MD(gettext("Кредит"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
metkadk=0;
while(menu3w(stdscr,&MENU,&metkadk,-1,-1,0) != 0);
switch (metkadk)
 {
  case  2 :
  case -1 :
   goto naz;

 }
int metka_bank=0;
int kod_banka=0;
class iceb_tu_str kod00("00");
if((metka_bank=zarbanks(&kod00,&kod_banka)) <= 0)
  goto naz;
          
GDITE();

int kolshet=pole2(shet.ravno(),',');

//Составляем список всех контрагентов
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon from Skontr where ns='%s'",shet.ravno());
else
  sprintf(strsql,"select ns,kodkon from Skontr");

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
SQL_str row;
kolkontr=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],2,0) != 0)
    continue;
  if(KONT.find_r(row[1]) < 0)
   {
    kolkontr++;
    KONT.plus(row[1]);
   }
 }
GDITE();
double deb[kolkontr];
double kre[kolkontr];
memset(&deb,'\0',sizeof(deb));
memset(&kre,'\0',sizeof(kre));

//Читаем сальдо
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",gods,shet.ravno());
else
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d",gods);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int nomer=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],2,0) != 0)
    continue;
//  printw("%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }

GDITE();
//Читаем все проводки
if(kolshet == 0)
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> '' and sh='%s'",gods,1,1,gk,mk,dk,shet.ravno());
else
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> ''",gods,1,1,gk,mk,dk);

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
kolstr1=0;
double itogo[2];
itogo[0]=itogo[1]=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row[1],2,0) != 0)
    continue;
//printw("%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
  itogo[0]+=atof(row[2]);
  itogo[1]+=atof(row[3]);
 }

class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
int kolih_simv;
SQL_str row1;
SQLCURSOR cur1;


kolih_simv=iceb_tu_strlen(ICEB_ZAR_PKTN);
iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    glavbuh.new_plus(row1[0]);
 }
switch(metka_bank)
 {
  case 1: //Проминвестбанк
   pvbavks_pib(&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  case 3: //Укрсоцбанк
   pvbavks_uks(&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;


  case 4: //Укрсиббанк
   pbanks_dbf(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  case 7: //Укрэксимбанк
   pvbanks_txt(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod00.ravno(),kod_banka);
   break;

  case 8: /*хресщатик*/
   pvbanks_txt(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod00.ravno(),kod_banka);
   break;

  case 9: /*ibank2ua*/
   pvbanks_txt(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod00.ravno(),kod_banka);
   break;

  case 11: //Укргазбанк
   pbanks_dbf(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  case 13: //Киевская русь
   pbanks_dbf(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  case 14: //Система СORP2 "Державний ощадний банк України"
   pbanks_dbf(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  case 15: //ПриватБанк
   pbanks_dbf(metka_bank,&KONT,metkadk,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,grupk.ravno(),shet.ravno(),shet_kor.ravno(),dr,mr,gr,kod_banka);
   break;

  default:
   sprintf(strsql,"%s %d!",gettext("Не известный номер банка"),kod_banka);
   iceb_t_soob(strsql);
   break;

 }


}

/***************************/
/*Распечатка для банка ПИБ*/
/***************************/
void pvbavks_pib(SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,int kod_banka)
{
int  kolstrkartr=0;
char imafsp[64];
char imafdis[64];
class iceb_tu_str bankshet("");
class iceb_tu_str fio("");
class iceb_tu_str tabnom("");
double sumalist=0;
double sum=0;
double itogoo=0.;
SQLCURSOR cur1;


sprintf(imafsp,"pib%d.lst",getpid());
sprintf(imafdis,"pibd%d.txt",getpid());
class iceb_fopen fil_sp;
if(fil_sp.start(imafsp,"w") != 0)
 return;
 
hrvnks(fil_sp.ff,&kolstrkartr);
class iceb_fopen fil_dis;
if(fil_dis.start(imafdis,"w") != 0)
 return;

int kolh=0;
int kolkontr=KONT->kolih();
int kol_na_liste=0;
class iceb_tu_str inn("");

for(int snom=0 ; snom < kolkontr; snom++)
 {
  
  if(pvbanks_kprov(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka) != 0)
   continue;

  pib_itl(&kolstrkartr,&sumalist,&kol_na_liste,rukov,glavbuh,fil_sp.ff);

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
  
  pib_str(kolh,bankshet.ravno(),sum,fio.ravno(),inn.ravno(),fil_sp.ff);    
  
  kol_na_liste++;
  
  fprintf(fil_dis.ff,"%-16s %8.2f %s\n",bankshet.ravno(),sum,fio.ravno());
  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(), sum);
 }

pib_end_dok(kol_na_liste,sumalist,itogoo,kolh,rukov,glavbuh,fil_sp.ff);

fil_sp.end();
fil_dis.end();

//Записываем шапку и концовку
hdisk(kolh,itogoo,imafdis);

class spis_oth oth;

oth.spis_imaf.plus(imafsp);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imafdis);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
/***************************/
/*Распечатка для Укрсоцбанка*/
/***************************/
void pvbavks_uks(SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,int kod_banka)
{
char imaf[64];
char imaftmp[64];
char imafzag[64];
class iceb_tu_str bankshet("");
class iceb_tu_str fio("");
class iceb_tu_str tabnom("");
double sumalist=0;
double sum=0;
double itogoo=0.;
char strsql[512];
SQLCURSOR cur1;


sprintf(imaftmp,"ukrsoc%d.tmp",getpid());
class iceb_fopen fil_tmp;
if(fil_tmp.start(imaftmp,"w") != 0)
 return;

class iceb_fopen fil_ff;
sprintf(imaf,"ukrsoc%d.lst",getpid());

if(fil_ff.start(imaf,"w") != 0)
 return;

time_t          vrem;

time(&vrem);
struct tm *bf=localtime(&vrem);

fprintf(fil_ff.ff,"\
   Зведена відомість сум для зарахування на спеціальні карткові рахунки N_____\n\
------------------------------------------------------------------------------\n\
| Дата                             |             %02d.%02d.%04d                  |\n\
| Назва організації                |%-41.41s|\n\
| Назва та N філії АКБ \"Укрсоцбанк\"|Вінницька облана філія N050              |\n\
| Додаткова інформація             |                                         |\n\
------------------------------------------------------------------------------\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
iceb_t_get_pnk("00",0));

fprintf(fil_ff.ff,"\
--------------------------------------------------------------------------------------\n\
 N  |     Прізвище, ім'я по батькові         | Картковий счёт |Сумма до зарахування|\n\
--------------------------------------------------------------------------------------\n"); 
int kolh=0;
int kolkontr=KONT->kolih();
class iceb_tu_str inn("");
int nomerstr=0;

for(int snom=0 ; snom < kolkontr; snom++)
 {

  if(pvbanks_kprov(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka) != 0)
   continue;

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
    

  fprintf(fil_ff.ff,"%-4d %-40.40s %-19.19s %10.2f\n",
  ++nomerstr,fio.ravno(),bankshet.ravno(),sum);
  fprintf(fil_tmp.ff,"@%s,0,%.2f\n",bankshet.ravno(),sum);

  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum);

 }
fprintf(fil_ff.ff,"\
--------------------------------------------------------------------------------------\n"); 

fprintf(fil_ff.ff,"\
                                                          Всього: %10.2f\n",itogoo);

fprintf(fil_ff.ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);

fprintf(fil_ff.ff,"\n\
Відмітки банку:\n\
Дата приймання Зведеної відомості на паперовому носії та магнітному носії_____________\n\
Посада та призвище, ім'я та по батькові працівника Банку, який прийняв Зведену відомість\n\n\
_____________________________________________________\n\n\
_____________________________________________________\n\n\
підпис_______________________________________________\n");

podpis(fil_ff.ff);

fil_ff.end();
fil_tmp.end();

if(fil_tmp.start(imaftmp,"r") != 0)
  return;

sprintf(imafzag,"ukrsoc%d.txt",getpid());
if(fil_ff.start(imafzag,"w") != 0)
 return;

fprintf(fil_ff.ff,"@29244050010001,0,%.2f\n\
@302010,0,0,0.00\n",itogoo);

while(fgets(strsql,sizeof(strsql),fil_tmp.ff) != NULL)
  fprintf(fil_ff.ff,"%s",strsql);
  
fil_ff.end();
fil_tmp.end();
unlink(imaftmp);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imafzag);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
/**************************************/
/*Для Укрексимбанка*/
/****************************************/

void pvbanks_txt(int metkabk,SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
const char *kod00,
int kod_banka)
{
char imafsp[64];
char imafdis[64];
class iceb_tu_str vidnahis("");
short d_pp=0,m_pp=0,g_pp=0; /*Дата платёжки*/
double sumai=0.;
double sum=0.;
class iceb_tu_str fio("");
class iceb_tu_str bankshet("");
class iceb_tu_str inn("");
class iceb_tu_str tabnom("");

int kolkontr=KONT->kolih();

char imaftmp[64];
FILE *ffr;
FILE *fftmp;

if(metkabk == 7) /*Укрэксим*/
  if(ukreksim_hap(imafsp,imaftmp,&fftmp,&ffr) != 0)
   return;

if(metkabk == 8) /*Хресщатик*/
 {
  for(int snom=0 ; snom < kolkontr; snom++)
   {
    if(pvbanks_kprov(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka) != 0)
     continue;

    sumai+=sum;
   }
  if(zvb_hreshatik_start(imafsp,imafdis,sumai,&d_pp,&m_pp,&g_pp,&fftmp,&ffr) != 0)
   return;
 }

if(metkabk == 9) /*ibank2ua*/
 if(zvb_ibank2ua_start(imafsp,imafdis,&vidnahis,kod00,&fftmp,&ffr) != 0)
  return;

sum=0.;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
int nomer=0;
sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {
  sum=0.;
  if(pvbanks_kprov(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka) != 0)
   continue;

  sumai+=sum;

  if(metkabk == 7) /*укрэксим*/
   ukreksim_zstr(fftmp,ffr,&nomer,sum,fio.ravno(),bankshet.ravno(),tabnom.ravno(),dt,mt,gt);

  if(metkabk == 8) /*Хресщатик*/
    zvb_hreshatik_str(d_pp,m_pp,g_pp,&nomer,fio.ravno(),bankshet.ravno(),inn.ravno(),sum,fftmp,ffr);

  if(metkabk == 9) /*ibank2ua*/
   zvb_ibank2ua_str(&nomer,fio.ravno(),bankshet.ravno(),inn.ravno(),sum,fftmp,ffr);
   
  if(dr != 0) /*нужно делать проводки*/
   pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum);

 }

if(metkabk == 7) /*укрэксим*/
  ukreksim_end(nomer,imaftmp,imafdis,sumai,rukov,glavbuh,fftmp,ffr);

if(metkabk == 8) /*Хресщатик*/
 zvb_hreshatik_end(sumai,nomer,ffr,fftmp);

if(metkabk == 9) /*ibank2ua*/
 zvb_ibank2ua_end(sumai,vidnahis.ravno(),ffr,fftmp);

class spis_oth oth;

oth.spis_imaf.plus(imafsp);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imafdis);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
/**************************************/
/*Для банков у которых електронный формат dbf*/
/****************************************/

void pbanks_dbf(int metka_banka, /*1-ukrsib 2-ukrgaz*/
SPISOK *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,int kod_banka)
{
char imafsp[64];
char imafdis[64];

int kolkontr=KONT->kolih();

char imaftmp[64];
FILE *ffr;
FILE *fftmp;
class iceb_tu_str ope_tor("");
static class iceb_tu_str branch("");
static class iceb_tu_str zpkod("");
static class iceb_tu_str rlkod("");

if(metka_banka == 4)
 if(ukrsib_start(imafsp,imaftmp,&ope_tor,&fftmp,&ffr) != 0)
  return;

if(metka_banka == 11)
 if(zvb_ukrgaz_start(imafsp,imafdis,imaftmp,&fftmp,&ffr) != 0)
  return;

if(metka_banka == 13)
  if(zvb_kievrus_start(imafsp,imafdis,imaftmp,&fftmp,&ffr) != 0)
   return;

if(metka_banka == 14)
  if(zvb_corp2_start(imafsp,imafdis,imaftmp,&fftmp,&ffr) != 0)
   return;

if(metka_banka == 15)
  if(zvb_privatbank_start(imafsp,imafdis,imaftmp,&branch,&zpkod,&rlkod,&fftmp,&ffr) != 0)
   return;

double sum=0.;
class iceb_tu_str fio("");
class iceb_tu_str bankshet("");
class iceb_tu_str inn("");
class iceb_tu_str tabnom("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
int nomer=0;
double sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {
  
  if(pvbanks_kprov(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka) != 0)
   continue;
  
  sumai+=sum;
  if(metka_banka == 4)
    ukrsib_str(dt,mt,gt,sum,bankshet.ravno(),fio.ravno(),&nomer,tabnom.ravno(),ope_tor.ravno(),ffr,fftmp);
  if(metka_banka == 11)
    zvb_ukrgaz_str(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ffr);
  if(metka_banka == 13)
    zvb_kievrus_str(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ffr);
  if(metka_banka == 14)
    zvb_corp2_str(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ffr);
  if(metka_banka == 15)
    zvb_privatbank_str(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),branch.ravno(),zpkod.ravno(),rlkod.ravno(),fftmp,ffr);

  if(dr != 0) /*нужно делать проводки*/
      pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum);
 }

if(metka_banka == 4)
  ukrsib_end(imafdis,imaftmp,sumai,nomer,rukov,glavbuh,ffr,fftmp);
if(metka_banka == 11)
  zvb_ukrgaz_end(imafdis,imaftmp,sumai,nomer,ffr,fftmp);
if(metka_banka == 13)
 zvb_kievrus_end(imafdis,imaftmp,sumai,nomer,ffr,fftmp);
if(metka_banka == 14)
 zvb_corp2_end(imafdis,imaftmp,sumai,nomer,ffr,fftmp);
if(metka_banka == 14)
 zvb_privatbank_end(imafdis,imaftmp,sumai,nomer,ffr,fftmp);

class spis_oth oth;

oth.spis_imaf.plus(imafsp);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imafdis);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}


/***************************/
/*Ввод счёта кореспонданта и даты розноски*/
/*****************************************/
/*Если вернули 0-розносим 1-нет*/
int pvbanks_rozn(class iceb_tu_str *shet_kor,
short *dr,short *mr,short *gr)
{
static class iceb_tu_str shet_k("");
static class iceb_tu_str dat1("");


VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG("Введите дополнительные данные для розноски");

VV.VVOD_SPISOK_add_MD(gettext("Счёт корреспондент.........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата перечисления (д.м.г)..."));


VV.VVOD_SPISOK_add_data(shet_k.ravno(),20);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11); 

naz:;
clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

shet_k.new_plus(VV.VVOD_SPISOK_return_data(0));
dat1.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(rsdat(dr,mr,gr,dat1.ravno(),1) != 0)
 goto naz;

struct OPSHET rek_sh;
if(prsh1(VV.data_ravno(0),&rek_sh) != 0)
 goto naz;

shet_kor->new_plus(VV.data_ravno(0));
return(0);



}
/*************************************/
/*Выполнение проводок*/
/***************************/
void pvbanks_make_prov(const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int metkadk,
const char *kontr,
const char *fio,
double sum)
{

double deb=0.;
double kre=0.;


if(metkadk == 0) /*Дебет*/
 kre=sum;
if(metkadk == 1) /*кредит*/
 deb=sum;

class iceb_tu_str koment(gettext("Перечислено"));
koment.plus(" ",fio);
 
zapprov(0,gr,mr,dr,shet,shet_kor,kontr,kontr,"","","",deb,kre,koment.ravno(),2,0,time(NULL),0,0,0,0,0);
}
