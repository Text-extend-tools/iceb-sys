/*$Id: upl_sp_top.c,v 5.9 2013-04-07 10:22:44 sasa Exp $*/
/*30.11.2009	23.06.2005	Белых А.И.	upl_sp_top.c
Программа ввода и коррертировки записей на списание топлива в путевом листе
*/
#include "buhg.h"


int upl_sp_top(int metka, //0-ввод новой записи 1-корректировка
short dd,short md,short gd,
const char *podr,
const char *nomd,
const char *kodavt,
const char *kodvod,
int nom_zap_v_dok, //номер записи в документе
int nom_zap) //Номер документа в одной дате по одной и тойже машине
{
char strsql[512];
class iceb_tu_str kod("");

int kom=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(20); //0
VV.VVOD_SPISOK_add_data(20); //1
VV.VVOD_SPISOK_add_data(80); //2

if(metka == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи на списание топлива."));

if(metka == 1)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи на списание топлива."));
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select kodtp,kolih,kom,ktoz,vrem from Upldok1 where \
datd='%04d-%02d-%02d' and kp=%s and nomd='%s' and nzap=%d",
  gd,md,dd,podr,nomd,nom_zap_v_dok);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки !"));
    return(1);
   }

  VV.data_plus(0,row[0]);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]);
  kzvz(atoi(row[3]),atol(row[4]),&VV);

 }  
VV.VVOD_SPISOK_add_MD(gettext("Код топлива..")); //0
VV.VVOD_SPISOK_add_MD(gettext("Количество...")); //1
VV.VVOD_SPISOK_add_MD(gettext("Коментарий...")); //2


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

naz:;

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

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

  case FK3:
     kod.new_plus("");
     if(dirmt(&kod,1) == 0)
      VV.data_plus(0,kod.ravno());
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

//Проверяем код топлива
sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV.data_ravno(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),VV.data_ravno(0));
  iceb_t_soob(strsql);
  goto naz; 
 }

if(metka == 0)
  if(upl_zapvdok(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),0,2,
  VV.data_ravno(0),VV.data_ravno_atof(1),VV.data_ravno(2),0,nom_zap) != 0)
     goto naz;

if(metka == 1)
 {
  time_t vrem;
  time(&vrem);
  
  sprintf(strsql,"update Upldok1 set \
kodtp='%s',\
kolih=%.3f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%d-%d-%d' and kp=%s and nomd='%s' and nzap=%d",
  VV.data_ravno(0),
  VV.data_ravno_atof(1),
  VV.data_ravno(2), 
  iceb_t_getuid(),vrem,
  gd,md,dd,podr,nomd,nom_zap_v_dok);

  if(sql_zapis(strsql,0,0) != 0)
   goto naz;
 }
return(0);
}
