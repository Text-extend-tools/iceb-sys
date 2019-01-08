/* $Id: uosact.c,v 5.30 2013/08/13 05:49:56 sasa Exp $ */
/*16.07.2015    17.02.1997      Белых А.И.      uosact.c
Распечатка акта приема передачи основных средств
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"


void uosact(short d,short m,short g, //Дата документа
const char *ndk) //номер документа
{
class poiinpdw_data rekin;
class iceb_tu_str str("");
FILE            *ff1;
char            imaf1[64];
long            in;
int             pdr;
class bsizw_data bal_st;
double		pbs;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
float		kof;
short		dv,mv,gv;
short		god;
class iceb_tu_str pasp("");
class iceb_tu_str naim("");
class iceb_tu_str model("");
short		tipz;
class iceb_tu_str osnov("");
class iceb_tu_str kpos("");
char datpr[64];
class iceb_tu_str kprr("");
class iceb_tu_str zavnom("");
int		kkk=0;
class iceb_tu_str godchar("");
class iceb_tu_str mesqc("");
class iceb_tu_str kodol("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
SQLCURSOR cur;
SQLCURSOR curr;
class iceb_tu_str kodedrpou("");

/*Узнаём код */
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row1,&cur) == 1)
 kodedrpou.new_plus(row1[0]);

godchar.new_plus(g);
/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop,kodol from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(readkey(strsql,&row1,&cur) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ !"),ndk,d,m,g,
  gettext("г."));
  iceb_t_soob(strsql);
  return;
 }
else
 {
  tipz=atoi(row1[0]);
  kpos.new_plus(row1[1]);
  osnov.new_plus(row1[2]);
  kprr.new_plus(row1[3]);
  kodol.new_plus(row1[4]);
 }  

if(tipz == 1)
  sprintf(imaf1,"acp%d.lst",getpid());
if(tipz == 2)
  sprintf(imaf1,"acr%d.lst",getpid());

errno=0;
if((ff1 = fopen(imaf1,"r")) == NULL)
 if(errno != ENOENT)
   {
    error_op_nfil(imaf1,errno,"");
    return;
   }

if(errno != ENOENT) /*Файл сущесвует*/
 {
  fclose(ff1);
  VVOD DANET(1);
  DANET.VVOD_SPISOK_add_MD(gettext("Распечатка уже есть. Переформировать ?"));

  if(danet(&DANET,2,stdscr) == 2)
   {
    class spis_oth oth;
    oth.spis_imaf.plus(imaf1);
    oth.spis_naim.plus(gettext("Распечатка акта приёмки/передачи основных средств"));
    iceb_t_rabfil(&oth,"");
    return;
   }
 }

sprintf(datpr,"%d.%d.%d",d,m,g);


sprintf(strsql,"select innom,podr,bsby from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
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

sprintf(strsql,"select str from Alx where fil='uosact.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosact.alx");
  iceb_t_soob(strsql);
  return;
 }


if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }


kkk=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kkk++;
  str.new_plus(row_alx[0]);
  switch(kkk)
   {
    case 1 :
     iceb_tu_vstav(&str,iceb_t_get_pnk("00",0),0,39,1);
     break;

   case 3 :
     iceb_tu_vstav(&str,kodedrpou.ravno(),29,48,1);
     break;

    case 6 :
     mesc(m,1,&mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc.ravno(),g);

     iceb_tu_vstav(&str,strsql,123,149,1);
     break;

    case 14 :
     iceb_tu_vstav(&str,ndk,130,140,1);
     iceb_tu_vstav(&str,datpr,142,152,1);
     iceb_tu_vstav(&str,kodol.ravno(),155,169,1);
     iceb_tu_vstav(&str,kprr.ravno(),172,178,1);
     break;

    case 28 :
      while(cur.read_cursor(&row) != 0)
       {
               
        in=atol(row[0]);
        pdr=atoi(row[1]);
        pbs=atof(row[2]);
        

        bsiz(in,pdr,d,m,g,&bal_st,NULL);


        bal_st.bsby+=bal_st.sbsby;
        bal_st.izby+=bal_st.sizby;
        bal_st.izby+=bal_st.iz1by;

        /*Читаем переменные данные*/
        poiinpd(in,m,g,&rekin);

        /*Читаем поправочный коэффициент*/
        kof=0.;
        sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код группы"),rekin.hnaby.ravno());
          iceb_t_soob(strsql);
         }
        else
         kof=atof(row1[0]);

        god=0;
        dv=mv=gv=0;
        
        /*Читаем постоянную часть*/   
        sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model \
from Uosin where innom=%ld",in);

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          beep();
          printw("%s %ld !\n",gettext("Не найден инвентарный номер"),in);
          OSTANOV();
          pasp.new_plus("");
          naim.new_plus("");
          model.new_plus("");
          zavnom.new_plus("");
         }
        else
         {
          god=atoi(row1[0]);
          pasp.new_plus(row1[1]);
          rsdat(&dv,&mv,&gv,row1[2],2);
          zavnom.new_plus(row1[3]);
          naim.new_plus(row1[4]);
          model.new_plus(row1[5]);
         }

        if(tipz == 1)
         fprintf(ff1," %*.*s %5.5d %*.*s %*.*s %5.5s %9.9s %9.9g %10ld\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         iceb_tu_kolbait(4,kpos.ravno()),
         iceb_tu_kolbait(4,kpos.ravno()),
         kpos.ravno(),pdr,
         iceb_tu_kolbait(5,rekin.shetu.ravno()),
         iceb_tu_kolbait(5,rekin.shetu.ravno()),
         rekin.shetu.ravno(),
         iceb_tu_kolbait(9,rekin.hau.ravno()),
         iceb_tu_kolbait(9,rekin.hau.ravno()),
         rekin.hau.ravno(),
         " "," ",pbs,in,
         iceb_tu_kolbait(10,zavnom.ravno()),
         iceb_tu_kolbait(10,zavnom.ravno()),
         zavnom.ravno(),
         iceb_tu_kolbait(17,rekin.hzt.ravno()),
         iceb_tu_kolbait(17,rekin.hzt.ravno()),
         rekin.hzt.ravno(),
         iceb_tu_kolbait(6,rekin.hnaby.ravno()),
         iceb_tu_kolbait(6,rekin.hnaby.ravno()),
         rekin.hnaby.ravno(),
         kof,0.,
         rekin.popkf," "," ",bal_st.izby,god,mv,gv,
         iceb_tu_kolbait(14,pasp.ravno()),
         iceb_tu_kolbait(14,pasp.ravno()),
         pasp.ravno());

        if(tipz == 2)
         fprintf(ff1," %4.4d %*.*s %5.5s %9.9s %*.*s %*.*s %9.9g %10ld\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         pdr,
         iceb_tu_kolbait(5,kpos.ravno()),
         iceb_tu_kolbait(5,kpos.ravno()),
         kpos.ravno(),
         " "," ",
         iceb_tu_kolbait(5,rekin.shetu.ravno()),
         iceb_tu_kolbait(5,rekin.shetu.ravno()),
         rekin.shetu.ravno(),
         iceb_tu_kolbait(9,rekin.hau.ravno()),
         iceb_tu_kolbait(9,rekin.hau.ravno()),
         rekin.hau.ravno(),
         pbs,in,
         iceb_tu_kolbait(10,zavnom.ravno()),
         iceb_tu_kolbait(10,zavnom.ravno()),
         zavnom.ravno(),
         iceb_tu_kolbait(17,rekin.hzt.ravno()),
         iceb_tu_kolbait(17,rekin.hzt.ravno()),
         rekin.hzt.ravno(),
         iceb_tu_kolbait(6,rekin.hnaby.ravno()),
         iceb_tu_kolbait(6,rekin.hnaby.ravno()),
         rekin.hnaby.ravno(),kof,0.,
         rekin.popkf," "," ",bal_st.izby*(-1),god,mv,gv,
         iceb_tu_kolbait(14,pasp.ravno()),
         iceb_tu_kolbait(14,pasp.ravno()),
         pasp.ravno());
       }
     break;

    case 29 :
     iceb_tu_vstav(&str,osnov.ravno(),34,73,1);
     break;

    case 33 :
     sprintf(strsql,"%s %s",naim.ravno(),model.ravno());
     iceb_tu_vstav(&str,strsql,62,139,1);
     break;

    case 31 :
     iceb_tu_vstav(&str,godchar.ravno(),22,27,1);
     break;

    case 41 :
     fprintf(ff1,"\f\n");
     continue;

    case 61 :
     mesc(m,1,&mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc.ravno(),g);

     iceb_tu_vstav(&str,strsql,69,89,1);

     break;
   }

  fprintf(ff1,"%s",str.ravno());

 }

podpis(ff1);

fclose(ff1);

class spis_oth oth;
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Распечатка акта приёмки/передачи основных средств"));

iceb_t_ustpeh(imaf1,0);

iceb_t_rabfil(&oth,"");

}
