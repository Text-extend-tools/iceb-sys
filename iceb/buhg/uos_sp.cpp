/*$Id: uos_sp.c,v 5.14 2013/09/26 09:43:41 sasa Exp $*/
/*16.07.2015	09.09.2007	Белых А.И.	uos_sp.c
Расчёт списания/получения основных средств по счетам списания/получения
Если вернули 0- расчёт выполнен
             1- нет
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"


int uos_sp(int metka, /*1-по счетам приобретения 2-по счетам списания*/
class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str shets_p(""); //Счёт списания /получения
static class iceb_tu_str shetu_p(""); //Счёт учёта
static class iceb_tu_str kodop_p("");
static class iceb_tu_str kontr_p("");
static class iceb_tu_str kod_podr("");
static class iceb_tu_str kod_mo("");

int kom=0,kom1=0;
char strsql[1024];

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(shets_p.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu_p.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop_p.ravno(),128);
VV.VVOD_SPISOK_add_data(kontr_p.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_mo.ravno(),128);


if(metka == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Движение основных средств по счетам приобретения"));
if(metka == 2)
 VV.VVOD_SPISOK_add_ZAG(gettext("Движение основных средств по счетам списания"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));//1

if(metka == 1)
VV.VVOD_SPISOK_add_MD(gettext("Счёт получения.(,,)....."));//2
if(metka == 2)
VV.VVOD_SPISOK_add_MD(gettext("Счёт списания.(,,)......"));//2

VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта..(,,)........"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)...."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)...."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код мат.ответств. (,,).."));//7



naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
shets_p.new_plus(VV.data_ravno(2));
shetu_p.new_plus(VV.data_ravno(3));
kodop_p.new_plus(VV.data_ravno(4));
kontr_p.new_plus(VV.data_ravno(5));
kod_podr.new_plus(VV.data_ravno(6));
kod_mo.new_plus(VV.data_ravno(7));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_19.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов")); //0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операцій расходов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Kontragent",&kontr_p);
        VV.VVOD_SPISOK_zapis_data(5,kontr_p.ravno());
        goto naz;

      case 1 :
        vibrek("Uospri",&kodop_p);
        VV.VVOD_SPISOK_zapis_data(4,kodop_p.ravno());
        goto naz;

      case 2 :
        if(diruosras(1,&kodop_p,&naims) == 0)
          VV.data_z_plus(4,kodop_p.ravno());
        goto naz;
        

      case 3 :
        vibrek("Plansh",&kodop_p);
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


sprintf(strsql,"select datd,innom,nomd,podr,kodol,bs,iz,kodop,bsby,izby,shs from Uosdok1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tipz=%d and podt=1 order by shs,datd asc",
gn,mn,dn,gk,mk,dk,metka);

int kolstr=0;
class SQLCURSOR cur;
class SQLCURSOR cur1;
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

FILE *ff;
char imaf[64];
sprintf(imaf,"uos_sp%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
if(metka == 1)
 zagolov(gettext("Движение основных средств по счетам приобретения."),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);
if(metka == 2)
 zagolov(gettext("Движение основных средств по счетам списания."),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);
 
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инв.номер |      Наименование            |Счёт спис.|Счёт учёта|Бал.с.н.у.|Износ н.у.|Бал.с.б.у.|Износ б.у.|Дата доку.|Номер доку|Подраздел.|Мат.отв.л.|Код оп.|Контраген.|\n"));

/*************
1234567890 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567 1234567890 
**********/

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


SQL_str row;
SQL_str row1;
int kolstr1=0;
long in_nom;
short d,m,g;
class poiinpdw_data rekin;
class iceb_tu_str naim_uos("");
double bs,iz,bsbu,izbu;
double ibs=0.,iiz=0.,ibsbu=0.,iizbu=0.;
double ibs_shet=0.,iiz_shet=0.,ibsbu_shet=0.,iizbu_shet=0.;
int podr=0;
int kodol=0;
class iceb_tu_str podr_char("");
class iceb_tu_str kodol_char("");
class iceb_tu_str kontr("");
class iceb_tu_str shet_sp("");
class iceb_tu_str shet_sp_zap("");

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(shets_p.ravno(),row[10],0,0) != 0)
    continue;

  if(proverka(kodop_p.ravno(),row[7],0,0) != 0)
    continue;

  if(proverka(kod_podr.ravno(),row[3],0,0) != 0)
    continue;

  if(proverka(kod_mo.ravno(),row[4],0,0) != 0)
    continue;
  
  sprintf(strsql,"select kontr from Uosdok where datd='%s' and nomd='%s'",row[0],row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   kontr.new_plus(row1[0]);
  else
   kontr.new_plus("");
   
  if(kontr_p.ravno()[0] != '\0')
   {
    if(proverka(kontr_p.ravno(),kontr.ravno(),0,0) != 0)
      continue;    
   }
  in_nom=atol(row[1]);
  rsdat(&d,&m,&g,row[0],2);
  
  poiinpd(in_nom,m,g,&rekin);

  if(proverka(shetu_p.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;
  
  poiin(atol(row[1]),d,m,g,&podr,&kodol);
  
  sprintf(strsql,"select naim from Uosin where innom=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_uos.new_plus(row1[0]);
  else
   naim_uos.new_plus("");

  podr_char.new_plus(podr);
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(readkey(strsql,&row1,&cur1) == 1)
   podr_char.plus(row1[0]);

  kodol_char.new_plus(kodol);
  sprintf(strsql,"select naik from Uosol where kod=%d",kodol);
  if(readkey(strsql,&row1,&cur1) == 1)
   kodol_char.plus(row1[0]);
  
  shet_sp.new_plus(row[10]);
  if(row[10][0] == '\0')
   shet_sp.new_plus("???");  
  
  bs=atof(row[5]);
  iz=atof(row[6]);  
  bsbu=atof(row[8]);
  izbu=atof(row[9]);  
  
  ibs+=bs;
  iiz+=iz;
  ibsbu+=bsbu;
  iizbu+=izbu;

  ibs_shet+=bs;
  iiz_shet+=iz;
  ibsbu_shet+=bsbu;
  iizbu_shet+=izbu;
    
  if(SRAV(shet_sp_zap.ravno(),shet_sp.ravno(),0) != 0)
   {
    if(shet_sp_zap.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp_zap.ravno());
      fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
      iceb_tu_kolbait(63,strsql),strsql,ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);
      ibs_shet=0.;
      iiz_shet=0.;
      ibsbu_shet=0.;
      iizbu_shet=0.;
      
     }
    shet_sp_zap.new_plus(shet_sp.ravno());
   }

  fprintf(ff,"%10s %-*.*s %-*s %-*s %10.2f %10.2f %10.2f %10.2f %02d.%02d.%04d %-10s %-10.10s %-10.10s %-7s %s\n",
  row[1],
  iceb_tu_kolbait(30,naim_uos.ravno()),iceb_tu_kolbait(30,naim_uos.ravno()),naim_uos.ravno(),
  iceb_tu_kolbait(10,shet_sp.ravno()),shet_sp.ravno(),
  iceb_tu_kolbait(10,rekin.shetu.ravno()),rekin.shetu.ravno(),
  bs,iz,bsbu,izbu,d,m,g,row[2],podr_char.ravno(),kodol_char.ravno(),row[7],kontr.ravno());

 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp_zap.ravno());

fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(63,strsql),strsql,ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);

fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(63,gettext("Итого")),gettext("Итого"),
ibs,iiz,ibsbu,iizbu);

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
if(metka == 1)
 oth->spis_naim.plus(gettext("Движение основных средств по счетам приобретения"));
if(metka == 2)
 oth->spis_naim.plus(gettext("Движение основных средств по счетам списания"));

iceb_t_ustpeh(imaf,3);

return(0);

}
