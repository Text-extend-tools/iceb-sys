/* $Id: uossas.c,v 5.21 2013/08/13 05:49:57 sasa Exp $ */
/*16.10.2015    29.05.1997      Белых А.И.      uossas.c
Акт списания автотранспортного средства
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

void uossas(short d,short m,short g, //Дата
const char *ndk) //Номер документа
{
class iceb_tu_str str("");
FILE            *ff1;
char            imaf1[64];
long            in;
int             pdr;
double		pbs,piz;
class bsizw_data bal_st;
long		kolstr;
SQL_str         row,row1,row_alx;
SQLCURSOR curr,cur_alx;
char		strsql[1024];
float		kof;
short		dv,mv,gv;
short		god;
class iceb_tu_str naim("");
class iceb_tu_str model("");
char		datpr[64];
class iceb_tu_str kprr("");
class iceb_tu_str zavnom("");

/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(readkey(strsql,&row1,&curr) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",gettext("Не найден документ !"),ndk,d,m,g,gettext("г."));
  iceb_t_soob(strsql);
  return;
 }
else
 {
  kprr.new_plus(row1[3]);
 }  


sprintf(imaf1,"asa%d.lst",getpid());

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
    oth.spis_naim.plus(gettext("Распечатка акта списания автотранспортного средства"));
    
    iceb_t_rabfil(&oth,"");
    return;
   }
 }


sprintf(strsql,"select innom,podr,bs,iz from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
SQLCURSOR cur;
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

sprintf(strsql,"select str from Alx where fil='uossas.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uossas.alx");
  iceb_t_soob(strsql);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }
sprintf(datpr,"%d.%d.%d",d,m,g);
/************************
memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"%s","uossas.alx");
if(iceb_t_alxout(imaf) != 0)
 return;
if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }



iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,iceb_t_get_pnk("00",0),0,39,1);
fprintf(ff1,"%s",str.ravno());
for(i=0; i< 13;i++)
 {
  iceb_tu_fgets(&str,ff);
  fprintf(ff1,"%s",str.ravno());
 }

iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,ndk,149,159,1);
iceb_tu_vstav(&str,datpr,161,170,1);
iceb_tu_vstav(&str,kprr.ravno(),173,180,1);
fprintf(ff1,"%s",str.ravno());

for(i=0; i< 13;i++)
 {
  iceb_tu_fgets(&str,ff);
  fprintf(ff1,"%s",str.ravno());
 }
******************************/
int kkk=0;
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

    case 15:
     iceb_tu_vstav(&str,ndk,149,159,1);
     iceb_tu_vstav(&str,datpr,161,170,1);
     iceb_tu_vstav(&str,kprr.ravno(),173,180,1);
     break;
    case 29:
     goto vper;
   }
  fprintf(ff1,"%s",str.ravno());
 }
vper:;
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  in=atol(row[0]);
  pdr=atoi(row[1]);
  pbs=atof(row[2]);
  piz=atof(row[3]);
  
  bsiz(in,pdr,d,m,g,&bal_st,NULL);
  bal_st.bs+=bal_st.sbs;
  bal_st.iz+=bal_st.siz;
  bal_st.iz+=bal_st.iz1;
  /*Читаем переменные данные*/
  poiinpd(in,m,g,&rekin);

  /*Читаем поправочный коэффициент*/
  kof=0.;
  sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());

  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден код группы"),rekin.hna.ravno());
    OSTANOV();
   }
  else
   kof=atof(row1[0]);

  god=0;
  dv=mv=gv=0;

  /*Читаем постоянную часть*/   
  sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model from Uosin \
where innom=%ld",in);

  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %ld !\n",gettext("Не найден инвентарный номер"),in);
    OSTANOV();
    zavnom.new_plus("");
    naim.new_plus("");
    model.new_plus("");
   }
  else
   {
    god=atoi(row1[0]);
    rsdat(&dv,&mv,&gv,row[2],2);
    zavnom.new_plus(row1[3]);
    naim.new_plus(row1[4]);
    model.new_plus(row1[5]);
   }

/*               1   2   3   4   5    6        7     8
    9     10   11    12    13   14  15  16    17*/

  fprintf(ff1," %7d %5s %9s %*s %*s %10.10g %11.11g %10ld\
 %*.*s %*s %6.6g %8.8g %8.8g %*s %*s %4d %02d.%d\n",
  pdr," "," ",
  iceb_tu_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
  iceb_tu_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
  pbs*(-1),piz*(-1),in,
  iceb_tu_kolbait(10,zavnom.ravno()),iceb_tu_kolbait(10,zavnom.ravno()),zavnom.ravno(),
  iceb_tu_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
  0.,kof,0.,
  iceb_tu_kolbait(7,rekin.hna.ravno()),rekin.hna.ravno(),
  iceb_tu_kolbait(7,rekin.hna.ravno()),rekin.hna.ravno(),
  god,mv,gv);

 }
/************************* 
iceb_tu_fgets(&str,ff);
fprintf(ff1,"%s",str.ravno());

iceb_tu_fgets(&str,ff);
fprintf(ff1,"%s",str.ravno());

iceb_tu_fgets(&str,ff);
fprintf(ff1,"%s",str.ravno());


iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,naim.ravno(),53,72,1);
iceb_tu_vstav(&str,model.ravno(),90,109,1);
fprintf(ff1,"%s",str.ravno());

iceb_tu_fgets(&str,ff);
fprintf(ff1,"%s",str.ravno());

iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,rekin.nomz.ravno(),55,64,1);
fprintf(ff1,"%s",str.ravno());
i=0;
for(;;)
 {
  iceb_tu_fgets(&str,ff);
  if(str.ravno()[0] == '=')
   {
    if(i == 0)
     {
      fprintf(ff1,"\f\n");
      i++;
      continue;
     }
    else
      break;
   }
  fprintf(ff1,"%s",str.ravno());

 }
fprintf(ff1,"\f\n");
while(iceb_tu_fgets(&str,ff) == 0)
  fprintf(ff1,"%s",str.ravno());
************************/
kkk=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kkk++;
  str.new_plus(row_alx[0]);

  if(str.ravno()[0] == '=')
   {
    fprintf(ff1,"\f\n");
    continue;
   }  
  switch(kkk)
   {
    case 3:
     iceb_tu_vstav(&str,naim.ravno(),53,72,1);
     iceb_tu_vstav(&str,model.ravno(),90,109,1);
     break;    

    case 5:
     iceb_tu_vstav(&str,rekin.nomz.ravno(),55,64,1);
     break;

     
   }
  fprintf(ff1,"%s",str.ravno());
 }

//fclose(ff);
fclose(ff1);
//unlink(imaf);
class spis_oth oth;
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Распечатка акта списания автотранспортного средства"));

iceb_t_ustpeh(imaf1,0);

iceb_t_rabfil(&oth,"");

}
