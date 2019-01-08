/*$Id: usldupk.c,v 5.32 2014/07/31 07:09:17 sasa Exp $*/
/*08.02.2015	06.02.2015	Белых А.И.	usldupk.c
Отчёт по движению услуг по контрагентам
*/
#include        <errno.h>
#include        "buhg.h"
class usldupk_poi
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str podr;
   class iceb_tu_str grup_usl;
   class iceb_tu_str kodusl;
   class iceb_tu_str kontr;
   class iceb_tu_str kodop;
   class iceb_tu_str kodop1;
   class iceb_tu_str shetz;
   class iceb_tu_str nomdokp;
   class iceb_tu_str prix_rasx;
 usldupk_poi()
  {
   clear();
  }
 void clear()
  {
   dat1.new_plus("");
   dat2.new_plus("");
   podr.new_plus("");
   grup_usl.new_plus("");
   kodusl.new_plus("");
   kontr.new_plus("");
   kodop.new_plus("");
   kodop1.new_plus("");
   shetz.new_plus("");
   nomdokp.new_plus("");
   prix_rasx.new_plus("");
  }   
 };
 
int usldupk_provup(SQL_str row,class usldupk_poi *rekp,int *kodusli,class iceb_tu_str *ei,class iceb_tu_str*);

void usldupk_rasrr(class SPISOK *kodusl_ei,class SPISOK *spis_kontr,class masiv_din_double *m_kolih,
class masiv_din_double *m_summa,FILE *ff);
void usldupk_rasrr2(class SPISOK *kodusl_ei,class SPISOK *spis_kontr,class masiv_din_double *m_kolih,
class masiv_din_double *m_summa,double*,double*,FILE *ff);

void spis_p_kont_rekpoi(class usldupk_poi *rekpoi,FILE *ff);

void usldupk_strp(int kolih_km,int,FILE *ff);

//extern char	*maskmat; /*Массив кодов отмеченных материалов*/

int usldupk(class spis_oth *oth)
{
static class usldupk_poi rekp;
int kodgr=0;
int kodusli=0;
int kom1=0;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт списания/получения услуг по контрагентам"));

VV.VVOD_SPISOK_add_data(rekp.dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(rekp.dat2.ravno(),11);//1

VV.VVOD_SPISOK_add_data(rekp.podr.ravno(),112);//2
VV.VVOD_SPISOK_add_data(rekp.kontr.ravno(),112);//3
VV.VVOD_SPISOK_add_data(rekp.grup_usl.ravno(),112);//4
VV.VVOD_SPISOK_add_data(rekp.kodop.ravno(),112);//5
VV.VVOD_SPISOK_add_data(rekp.kodusl.ravno(),112);//6
VV.VVOD_SPISOK_add_data(rekp.shetz.ravno(),112);//7
VV.VVOD_SPISOK_add_data(rekp.nomdokp.ravno(),112);//8
VV.VVOD_SPISOK_add_data(rekp.prix_rasx.ravno(),112);//9
VV.VVOD_SPISOK_add_data(rekp.kodop1.ravno(),112);//10

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Подразделение...(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Группа услуги...(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код услуги......(,,)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета......(,,)."));//7
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.(,,)."));//8
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-).."));//9
VV.VVOD_SPISOK_add_MD(gettext("Код операции +..(,,)."));//10

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,0,-1,-1);

rekp.dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
rekp.dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
rekp.podr.new_plus(VV.VVOD_SPISOK_return_data(2));
rekp.kontr.new_plus(VV.VVOD_SPISOK_return_data(3));
rekp.grup_usl.new_plus(VV.VVOD_SPISOK_return_data(4));
rekp.kodop.new_plus(VV.VVOD_SPISOK_return_data(5));
rekp.kodusl.new_plus(VV.VVOD_SPISOK_return_data(6));
rekp.shetz.new_plus(VV.VVOD_SPISOK_return_data(7));
rekp.nomdokp.new_plus(VV.VVOD_SPISOK_return_data(8));
rekp.prix_rasx.new_plus(VV.VVOD_SPISOK_return_data(9));
rekp.kodop1.new_plus(VV.VVOD_SPISOK_return_data(10));

class iceb_tu_str kods("");
class iceb_tu_str naims("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_21.txt");
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка услуг"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7 :
      case -1:
       goto naz;

      case 0 :
        if(dirpodrusl(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&rekp.kontr);
        VV.VVOD_SPISOK_zapis_data(3,rekp.kontr.ravno());
        goto naz;

      case 2 :
        vibrek("Uslgrup",&rekp.grup_usl);
        VV.VVOD_SPISOK_zapis_data(4,rekp.grup_usl.ravno());
        goto naz;


      case 3:
        if(diruslop2(1,&kods,&naims) == 0)
         VV.data_z_plus(5,kods.ravno());
        goto naz;
        
      case 4:
        if(dirusl(&kodgr,&kodusli,&naims,1,"") == 0)
         {
          VV.data_z_plus(6,kodusli);
         }
        
        goto naz;

      case 5 :
        vibrek("Plansh",&rekp.shetz);
        VV.VVOD_SPISOK_zapis_data(7,rekp.shetz.ravno());
        goto naz;

      case 6:
        if(diruslop1(1,&kods,&naims) == 0)
         VV.data_z_plus(10,kods.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

short dn,mn,gn,dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


GDITE();

char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr=0;
int kolstr1=0;

int tipz=0;

if(rekp.prix_rasx.ravno()[0] == '+')
 tipz=1;

if(rekp.prix_rasx.ravno()[0] == '-')
 tipz=2;

if(tipz != 0)
 sprintf(strsql,"select * from Usldokum2 where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' and tp=%d and metka=1",
 gn,mn,dn,gk,mk,dk,tipz);
else
 sprintf(strsql,"select * from Usldokum2 where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' and metka=1",
 gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
FILE *fftmp;
char imaftmp[64];
sprintf(imaftmp,"skontr%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }

class SPISOK kodusl_ei;
class SPISOK spis_kontr;

class SPISOK kodusl_ei_p;
class SPISOK spis_kontr_p;

class iceb_tu_str ei("");
class iceb_tu_str kontr_v_dok("");

//Создаём нужные списки
int kolstr2=0;
kolstr1=0;
int tipzz=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  
  tipzz=atoi(row[0]);

  if(usldupk_provup(row,&rekp,&kodusli,&ei,&kontr_v_dok) != 0) 
   continue;
  

  sprintf(strsql,"%d|%s",kodusli,ei.ravno());
    
  if(tipzz == 2)
   { 
    if(kodusl_ei.find(strsql) < 0)
     kodusl_ei.plus(strsql);

    if(spis_kontr.find_r(kontr_v_dok.ravno()) < 0)
      spis_kontr.plus(kontr_v_dok.ravno());
   }
  
  if(tipzz == 1)
   { 
    if(kodusl_ei_p.find(strsql) < 0)
     kodusl_ei_p.plus(strsql);

    if(spis_kontr_p.find_r(kontr_v_dok.ravno()) < 0)
     spis_kontr_p.plus(kontr_v_dok.ravno());
   }
   
  fprintf(fftmp,"%d|%s|%s|%s|%s|%d|\n",kodusli,ei.ravno(),kontr_v_dok.ravno(),row[6],row[7],tipzz);
  kolstr2++;
 }
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }
unlink(imaftmp);
//Создаём массивы для количества и суммы
class masiv_din_double m_kolih;
class masiv_din_double m_summa;

m_kolih.make_class(kodusl_ei.kolih()*spis_kontr.kolih());
m_summa.make_class(kodusl_ei.kolih()*spis_kontr.kolih());

class masiv_din_double m_kolih_p;
class masiv_din_double m_summa_p;

m_kolih_p.make_class(kodusl_ei_p .kolih()*spis_kontr_p.kolih());
m_summa_p.make_class(kodusl_ei_p.kolih()*spis_kontr_p.kolih());

int nomer_kodusl_ei;
int nomer_kontr;
double kolih,cena;

kolstr1=0;
double suma;
char stroka[1024];
int kolih_km=kodusl_ei.kolih();
int kolih_km_p=kodusl_ei_p.kolih();

while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  strzag(LINES-1,0,kolstr2,++kolstr1);

  polen(stroka,&kodusli,1,'|');

  polen(stroka,&ei,2,'|');

  polen(stroka,&kontr_v_dok,3,'|');

  polen(stroka,&kolih,4,'|');

  polen(stroka,&cena,5,'|');

  polen(stroka,&tipzz,6,'|');

  suma=kolih*cena;
  
  sprintf(strsql,"%d|%s",kodusli,ei.ravno());
  if(tipzz == 2)
   {
    if((nomer_kodusl_ei=kodusl_ei.find(strsql)) < 0)
     {
      sprintf(strsql,"В списке не найдено \"код услуги-единица измерения\" %s  !",strsql);
      iceb_t_soob(strsql);
      continue;
     }
    if((nomer_kontr=spis_kontr.find_r(kontr_v_dok.ravno())) < 0)
     {
      sprintf(strsql,"В списке не найдено код контрагента %s  !",kontr_v_dok.ravno());
      iceb_t_soob(strsql);
      continue;
     }

    m_kolih.plus(kolih,nomer_kontr*kolih_km+nomer_kodusl_ei);
    m_summa.plus(suma,nomer_kontr*kolih_km+nomer_kodusl_ei);
   }  
  if(tipzz == 1)
   {
    nomer_kodusl_ei=kodusl_ei_p.find(strsql);
    nomer_kontr=spis_kontr_p.find_r(kontr_v_dok.ravno());

    m_kolih_p.plus(kolih,nomer_kontr*kolih_km_p+nomer_kodusl_ei);
    m_summa_p.plus(suma,nomer_kontr*kolih_km_p+nomer_kodusl_ei);
   }  
 }

fclose(fftmp);
GDITE();
FILE *ff;


char imaf[64];
sprintf(imaf,"skontru%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Расчёт списания/получения услуг по контрагентам"),dn,mn,gn,dk,mk,gk,ff);

spis_p_kont_rekpoi(&rekp,ff);


if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания услуг по контрагентам"));
  usldupk_rasrr(&kodusl_ei,&spis_kontr,&m_kolih,&m_summa,ff);
 }


if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт получения услуг по контрагентам"));
  usldupk_rasrr(&kodusl_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,ff);
 }
podpis(ff);

fclose(ff);


char imaf2[64];
sprintf(imaf2,"skontru2%d.lst",getpid());

if((ff = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Расчёт списания/получения услуг по контрагентам"),dn,mn,gn,dk,mk,gk,ff);

spis_p_kont_rekpoi(&rekp,ff);


double itogo_k_p=0.;
double itogo_s_p=0.;
double itogo_k_r=0.;
double itogo_s_r=0.;

if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания услуг по контрагентам"));
  usldupk_rasrr2(&kodusl_ei,&spis_kontr,&m_kolih,&m_summa,&itogo_k_r,&itogo_s_r,ff);
 }

if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт поучения услуг по контрагентам"));
  usldupk_rasrr2(&kodusl_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,&itogo_k_p,&itogo_s_p,ff);
 }
podpis(ff);

fclose(ff);



VVOD SOOB(1);

SOOB.VVOD_SPISOK_add_MD(gettext("Получено"));

sprintf(strsql,"%s:%10.10g",gettext("Количество"),itogo_k_p);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s:%10.2f",gettext("Сумма"),itogo_s_p);
SOOB.VVOD_SPISOK_add_MD(strsql);

SOOB.VVOD_SPISOK_add_MD("-------------------------------");

SOOB.VVOD_SPISOK_add_MD(gettext("Списано"));

sprintf(strsql,"%s:%10.10g",gettext("Количество"),itogo_k_r);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s:%10.2f",gettext("Сумма"),itogo_s_r);
SOOB.VVOD_SPISOK_add_MD(strsql);


soobshw(&SOOB,stdscr,-1,-1,0,1);

 
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт списания/получения услуг по контрагентам"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Расчёт списания/получения услуг по контрагентам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);
}
/***********************************/
/*Проверка на условия поиска*/
/*******************************/

int usldupk_provup(SQL_str row,
class usldupk_poi *rekp,
int *kodusli,
class iceb_tu_str *ei,
class iceb_tu_str *kontr_v_dok)
{
char strsql[512];
SQL_str row1;
SQLCURSOR cur1;

if(proverka(rekp->podr.ravno(),row[10],0,0) != 0)
  return(1);
if(proverka(rekp->nomdokp.ravno(),row[3],0,0) != 0)
  return(1);

sprintf(strsql,"select kontr,kodop from Usldokum where datd='%s' and podr=%s and nomd='%s'",
row[1],row[10],row[3]);
if(readkey(strsql,&row1,&cur1) != 1)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s !",gettext("Не найдена шапка документа"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s %s %s",row[4],row[0],row[2]);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }


class iceb_tu_str kontr_dok("");
kontr_dok.new_plus(row1[0]);
if(SRAV(kontr_dok.ravno(),"00-",1) == 0)
   kontr_dok.new_plus("00");

kontr_v_dok->new_plus(kontr_dok.ravno());

if(proverka(rekp->kontr.ravno(),kontr_dok.ravno(),0,0) != 0)
  return(1);

if(atoi(row[0]) == 1)
 if(proverka(rekp->kodop1.ravno(),row1[1],0,0) != 0)
  return(1);

if(atoi(row[0]) == 2)
 if(proverka(rekp->kodop.ravno(),row1[1],0,0) != 0)
  return(1);


if(proverka(rekp->kodusl.ravno(),row[5],0,0) != 0)
  return(1);


*kodusli=atoi(row[5]);

if(rekp->grup_usl.ravno()[0] != '\0') //Узнаём группу услуги
 {
  sprintf(strsql,"select kodgr from Uslugi where kodus=%s",row[5]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),row[5]);
    iceb_t_soob(strsql);
    return(1);

   }
  if(proverka(rekp->grup_usl.ravno(),row1[0],0,0) != 0)
   return(1);
 }     


if(proverka(rekp->shetz.ravno(),row[9],0,0) != 0)
   return(1);
ei->new_plus(row[8]);
return(0);
}

/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void usldupk_rasrr(class SPISOK *kodusl_ei,
class SPISOK *spis_kontr,
class masiv_din_double *m_kolih,
class masiv_din_double *m_summa,
FILE *ff)
{

char strsql[512];
class iceb_tu_str kodusl("");
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;
int kolih_km=kodusl_ei->kolih();


usldupk_strp(kolih_km,0,ff);


fprintf(ff,"  Код   |  Контрагент        |");
//Пишем первую строку с наименованиями услуг
for(int ii=0; ii < kolih_km; ii++)
 {
  polen(kodusl_ei->ravno(ii),&kodusl,1,'|');

  sprintf(strsql,"select naius from Uslugi where kodus=%s",kodusl.ravno());  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");  
  sprintf(strsql,"%s %s",kodusl.ravno(),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,gettext("Итого")),iceb_tu_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//Пишем вторую строку с единицами измерения

for(int ii=0; ii < kolih_km; ii++)
 {
  polen(kodusl_ei->ravno(ii),&kodusl,2,'|');

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,kodusl.ravno()),iceb_tu_kolbait(20,kodusl.ravno()),kodusl.ravno());
 }
fprintf(ff,"%-20.20s|","");
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//строка разделения

for(int ii=0; ii < kolih_km+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"        |                    |");


for(int ii=0; ii < kolih_km+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(9,gettext("Количество")),
  iceb_tu_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

usldupk_strp(kolih_km,0,ff);
class iceb_tu_str kontr("");
//Распечатываем массив
for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
 {
  if(SRAV(spis_kontr->ravno(skontr),"00-",1) == 0)
   kontr.new_plus("00");
  else
   kontr.new_plus(spis_kontr->ravno(skontr));
  
  //узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");
  fprintf(ff,"%-*s|%-*.*s|",
  iceb_tu_kolbait(8,spis_kontr->ravno(skontr)),spis_kontr->ravno(skontr),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno());
  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_km; ii++)
   {
    double kolih=m_kolih->ravno(skontr*kolih_km+ii);
    double suma=m_summa->ravno(skontr*kolih_km+ii);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
  
 }

usldupk_strp(kolih_km,0,ff);

fprintf(ff,"%*s|",iceb_tu_kolbait(29,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_km; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_km+ii);
    itogo_po_kol_suma+=m_summa->ravno(skontr*kolih_km+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}


/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void usldupk_rasrr2(class SPISOK *kodusl_ei,
class SPISOK *spis_kontr,
class masiv_din_double *m_kolih,
class masiv_din_double *m_summa,
double *itogo_k,
double *itogo_s,
FILE *ff)
{

char strsql[512];
class iceb_tu_str kodusl("");
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;
class iceb_tu_str kontr("");
int kolih_kontr=spis_kontr->kolih();
int kolih_km=kodusl_ei->kolih();


usldupk_strp(kolih_kontr,1,ff);


fprintf(ff," Код |Наименование услуги    |Ед.из.|");
//          12345 12345678901234567890123 123456     
//Пишем первую строку с наименованиями контрагентов
for(int ii=0; ii < kolih_kontr; ii++)
 {
  if(SRAV(spis_kontr->ravno(ii),"00-",1) == 0)
   kontr.new_plus("00");
  else
   kontr.new_plus(spis_kontr->ravno(ii));

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");  
  sprintf(strsql,"%s %s",kontr.ravno(),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,gettext("Итого")),iceb_tu_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");

//строка разделения

for(int ii=0; ii < kolih_kontr+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");


for(int ii=0; ii < kolih_kontr+1; ii++)
 {
//  fprintf(ff,"%-9.9s|%-10.10s|",gettext("Количество"),gettext("Сумма"));
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(9,gettext("Количество")),
  iceb_tu_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

usldupk_strp(kolih_kontr,1,ff);
class iceb_tu_str ei("");
//Распечатываем массив
for(int skontr=0; skontr < kodusl_ei->kolih(); skontr++)
 {
  polen(kodusl_ei->ravno(skontr),&kodusl,1,'|');
  polen(kodusl_ei->ravno(skontr),&ei,2,'|');


  sprintf(strsql,"select naius from Uslugi where kodus=%s",kodusl.ravno());  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"%-5s|%-*.*s|%-*.*s|",kodusl.ravno(),
  iceb_tu_kolbait(23,naim.ravno()),iceb_tu_kolbait(23,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),iceb_tu_kolbait(6,ei.ravno()),ei.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_kontr; ii++)
   {
    double kolih=m_kolih->ravno(ii*kolih_km+skontr);
    double suma=m_summa->ravno(ii*kolih_km+skontr);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
  
 }

usldupk_strp(kolih_kontr,1,ff);

fprintf(ff,"%*s|",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
*itogo_k=0.;
*itogo_s=0.;

for(int ii=0; ii < kolih_kontr; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kodusl_ei->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(ii*kolih_km+skontr);
    itogo_po_kol_suma+=m_summa->ravno(ii*kolih_km+skontr);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  *itogo_k+=itogo_po_kol_kolih;
  *itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",*itogo_k,*itogo_s);

fprintf(ff,"\n");

/***********
VVOD SOOB(1);
sprintf(strsql,"%s:%10.10g",gettext("Количество"),itogo_k);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s:%10.10g",gettext("Сумма"),itogo_s);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);
**************/

}


/*******************************/
/*строка подчёркивания*/
/*************************/
void usldupk_strp(int kolih_km,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"------------------------------");
if(metka == 1)
  fprintf(ff,"-------------------------------------");
for(int ii=0; ii < kolih_km+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}

/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void spis_p_kont_rekpoi(class usldupk_poi *rekpoi,FILE *ff)
{
if(rekpoi->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),rekpoi->podr.ravno());
if(rekpoi->grup_usl.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Группа"),rekpoi->grup_usl.ravno());
if(rekpoi->kodusl.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код услуги"),rekpoi->kodusl.ravno());
if(rekpoi->kontr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),rekpoi->kontr.ravno());

if(rekpoi->kodop.getdlinna() > 1)
 fprintf(ff,"%s+:%s\n",gettext("Код операции"),rekpoi->kodop.ravno());

if(rekpoi->kodop1.getdlinna() > 1)
 fprintf(ff,"%s-:%s\n",gettext("Код операции"),rekpoi->kodop1.ravno());

if(rekpoi->shetz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),rekpoi->shetz.ravno());
if(rekpoi->nomdokp.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),rekpoi->nomdokp.ravno());

}
