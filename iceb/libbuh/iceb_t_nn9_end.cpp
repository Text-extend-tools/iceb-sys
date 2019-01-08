/*$Id:$*/
/*22.03.2017	22.03.2017	Белых А.И.	iceb_t_nn9_end.c
Распечатка концовки налоговой накладной 
*/

#include "buhl.h"

void iceb_t_nn9_end(int tipz, 
short lnds,
const char *imaf_nastr,
const char *kodop,
const char *kod_kontr,
FILE *ff)
{
class iceb_tu_str stroka("");
int nomer_st=0;
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str fio(iceb_t_getfioop()); /*берём фамилию оператора на случай если не определим фамилию по табельному номеру*/
class iceb_tu_str inn_op("");



/*Узнаём индивидуальный налоговый номер оператора*/
sprintf(strsql,"select tn from icebuser where login='%s'",iceb_tu_getlogin());
if(readkey(strsql,&row_alx,&cur_alx) == 1)
 {
  if(atoi(row_alx[0]) != 0)
   {
    class iceb_t_fioruk_rk rp;
    if(iceb_t_get_rtn(atoi(row_alx[0]),&rp) == 0)
     {
      inn_op.new_plus(rp.inn.ravno());
      fio.new_plus(rp.fio.ravno());
     }
   }
 }


sprintf(strsql,"select str from Alx where fil='nn9_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nn9_end.alx");
  iceb_t_soob(strsql);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
     continue;
  nomer_st++;
  stroka.new_plus(row_alx[0]);
  switch (nomer_st)
   {  

    case 5: /*Фамилия исполнителя*/
      if(tipz == 1)
        break;
/*****************
      fio.new_plus("");
      iceb_t_poldan("Исполнитель",&fio,"matnast.alx");
      if(fio.getdlinna() <= 1)
       {
        fio.new_plus(iceb_t_getfioop());
       }
****************/
      iceb_tu_vstav(&stroka,fio.ravno(),33,86,1);
      iceb_tu_vstav(&stroka,inn_op.ravno(),91,120,2);
          
      break;

    case 13: /*пункти налогового кодекса которыми пердусмотрено освобождение от НДС*/
      if(lnds == 3) //Освобождение от НДС
       {
        class iceb_tu_str punkt_zak(""); //Пункт закона на основании которого есть освобождение от НДС
        class iceb_tu_str naim_nast("Пункт закона освобождения от НДС для операции");
        naim_nast.plus(" ",kodop);

        iceb_t_poldan(naim_nast.ravno(),&punkt_zak,imaf_nastr);

        if(punkt_zak.ravno()[0] != '\0')
         {        
          iceb_tu_vstav(&stroka,punkt_zak.ravno(),3,154,1);
         }      
       } 
       break;
   }
 fprintf(ff,"%s",stroka.ravno());
}
fprintf(ff,"%s\n",kod_kontr);

if(iceb_t_poldan_vkl("Системная подпись в налоговой накладной","matnast.alx") == 0)
 {
  podpis(ff);
 }

fclose(ff);

}
