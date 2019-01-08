/*$Id: uosdoo.c,v 5.14 2013/08/13 05:49:57 sasa Exp $*/
/*16.07.2015	18.12.2006	Белых А.И.	uosdoo.c
Дооценка основных средств (бухгалтерский учёт)
*/
#include <errno.h>
#include "buhg.h"
#include "bsizt.h"

void uosdoo()
{
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str proc_doo("");
static class iceb_tu_str inv_nom("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Дооценка ос-них средств с нулевой остаточной стоимостью (бух.учёт)"));

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Процент дооценки........"));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(proc_doo.ravno(),20); 
VV.VVOD_SPISOK_add_data(inv_nom.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
//"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
proc_doo.new_plus(VV.VVOD_SPISOK_return_data(1));
inv_nom.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("uos2_6.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

time_t vrem;
time(&vrem);

GDITE();
short dr,mr,gr;
if(rsdat(&dr,&mr,&gr,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(proc_doo.ravno_atof() == 0.)
 {
  iceb_t_soob(gettext("Процент не введено !"));
  goto naz;
 
 }

char kod_op_doo[64];
memset(kod_op_doo,'\0',sizeof(kod_op_doo));
if(iceb_t_poldan("Код операции дооценки",kod_op_doo,"uosnast.alx") != 0)
 {
  iceb_t_soob(gettext("В файле настройки не нашли \"Код операции дооценки\" !"));
  goto naz;
 
 }
float proc_nds=iceb_t_pnds();


//проверяем есть ли код операции в списке операций
sprintf(strsql,"select naik from Uospri where kod='%s'",kod_op_doo);
if(readkey(strsql) != 1)
 {
  sprintf(strsql,gettext("Не нашли код операции %s !"),kod_op_doo);
  iceb_t_soob(strsql);
  goto naz;
 }


clear();

float proc_d=proc_doo.ravno_atof();


//Сортируем основные средства по родраздениям и материально-ответственным
sprintf(strsql,"select innom from Uosin");
class SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosdoo"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
podr int not null,\
kod_mo int not null,\
inn int not null,\
suma double(12,2) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return;
 }  




int kolstr1=0;
SQL_str row;
int inn=0;
int podr;
int kod_mo;
//double sbs,siz,bs,iz,iz1,sbsby,sizby,bsby,izby,iz1by;
class bsizw_data bal_st;
double ost_b_s=0.; //Остаточная балансовая стоимость



while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(inv_nom.ravno(),row[0],0,0) != 0)
   continue;
  inn=atol(row[0]);
  if(poiin(inn,dr,mr,gr,&podr,&kod_mo) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода

  if(bsiz(inn,podr,dr,mr,gr,&bal_st,NULL) != 0)
   continue;
  ost_b_s=bal_st.sbsby-bal_st.sizby+bal_st.bsby-bal_st.izby-bal_st.iz1by;
  
  if(ost_b_s > 0.)
   continue;
  sprintf(strsql,"insert into %s values (%d,%d,%d,%.2f)",
  imatmptab,
  podr,kod_mo,inn,bal_st.sbsby+bal_st.bsby);
  
  sql_zapis(strsql,1,0);    

 }
int podr_zap=0;
int kod_mo_zap=0;
kolstr1=0;
class iceb_tu_str nomdok("");
double suma_doo=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  podr=atoi(row[0]);
  kod_mo=atoi(row[1]);
  inn=atoi(row[2]);
  bal_st.sbsby=atof(row[3]);
  
  suma_doo=bal_st.sbsby*proc_d/100.;
  suma_doo=okrug(suma_doo,0.01);  
  if(podr_zap != podr || kod_mo_zap != kod_mo)//делаем шапку документа
   {
    class lock_tables kkkk("LOCK TABLE Uosdok READ,icebuser READ");
    uosnd(gr,1,&nomdok);

    printw("%s\n",nomdok.ravno());
    
    sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,podt,ktoz,vrem,nds,pn) \
values ('%04d-%d-%d',%d,'%s','%s','%s',%d,%d,%d,%d,%d,%ld,%d,%.2f)",
    gr,mr,dr,
    1,
    kod_op_doo,
    "00",
    nomdok.ravno_filtr(),
    podr,
    kod_mo,
    1,
    0,
    iceb_t_getuid(),
    vrem,
    3,
    proc_nds);

    if(sql_zapis(strsql,1,0) != 0)
      break;   

    podr_zap=podr;
    kod_mo_zap=kod_mo;
   
   }

  sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%d-%d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",\
  gr,mr,dr,1,0,inn,nomdok.ravno_filtr(),podr,kod_mo,0,0.,0.,0.,
  kod_op_doo,iceb_t_getuid(),vrem,suma_doo,0.,0.,"");

  sql_zapis(strsql,1,0);
  
 }


printw_vr(vrem);

OSTANOV();

}
