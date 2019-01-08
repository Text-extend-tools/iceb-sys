/* $Id: uosrem.c,v 5.24 2013/08/13 05:49:57 sasa Exp $ */
/*15.10.2015    30.05.1997      Белых А.И.      uosrem.c
Распечатка акта ремонта/модернизации основных средств
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

void uosrem(short d,short m,short g, //Дата документа
const char *ndk) //Номер документа
{
class iceb_tu_str str("");
FILE            *ff1;
char            imaf1[64];
//short           i;
long            in;
int             pdr;
class bsizw_data bal_st;
double		pbs;
long		kolstr,kolstr1;
SQL_str         row,row1,row_alx;
SQLCURSOR curr,cur_alx;
class poiinpdw_data rekin;
char		strsql[2048];
short		dv,mv,gv;
class iceb_tu_str naim("");
class iceb_tu_str model("");
short		tipz;
class iceb_tu_str kpos("");
char		datpr[32];
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
tipz=atoi(row1[0]);
kpos.new_plus(row1[1]);
kprr.new_plus(row1[3]);

if(tipz == 1)
  sprintf(imaf1,"arp%d.lst",getpid());
if(tipz == 2)
  sprintf(imaf1,"arr%d.lst",getpid());

errno=0;
if((ff1 = fopen(imaf1,"r")) == NULL)
 if(errno != ENOENT)
   {
    error_op_nfil(imaf1,errno,"");
    return;
   }

if(errno != ENOENT) /*Файл сущесвует*/
 {
  VVOD DANET(1);
  
  fclose(ff1);
  DANET.VVOD_SPISOK_add_MD(gettext("Распечатка уже есть. Переформировать ?"));

  if(danet(&DANET,2,stdscr) == 2)
   {
    class spis_oth oth;
    oth.spis_imaf.plus(imaf1);
    oth.spis_naim.plus(gettext("Распечатка акта ремонта/модернизации основного средства."));
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


/****************************
memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"%s","uosrem.alx");
if(iceb_t_alxout(imaf) != 0)
 return;
if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
**********************/
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }
/*************************
iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,iceb_t_get_pnk("00",0),0,39,1);
fprintf(ff1,"%s",str.ravno());
for(i=0; i< 19;i++)
 {
  iceb_tu_fgets(&str,ff);
  fprintf(ff1,"%s",str.ravno());
 }

iceb_tu_fgets(&str,ff);
iceb_tu_vstav(&str,ndk,47,56,1);
sprintf(datpr,"%d.%d.%d",d,m,g);
iceb_tu_vstav(&str,datpr,59,68,1);
iceb_tu_vstav(&str,kprr.ravno(),88,97,1);
fprintf(ff1,"%s",str.ravno());

for(i=0; i< 13;i++)
 {
  iceb_tu_fgets(&str,ff);
  fprintf(ff1,"%s",str.ravno());
 }

**********************/

sprintf(strsql,"select str from Alx where fil='uosrem.alx' order by ns asc");
if((kolstr1=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr1 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosrem.alx");
  iceb_t_soob(strsql);
  return;
 }

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

    case 21 :
     iceb_tu_vstav(&str,ndk,47,56,1);
     sprintf(datpr,"%d.%d.%d",d,m,g);
     iceb_tu_vstav(&str,datpr,59,68,1);
     iceb_tu_vstav(&str,kprr.ravno(),88,97,1);
     break;

    case 35 :
      goto vper;
   }
  fprintf(ff1,"%s",str.ravno());
 }
vper:;

while(cur.read_cursor(&row) != 0)
 {
  in=atol(row[0]);
  pdr=atoi(row[1]);
  pbs=atof(row[2]);

  bsiz(in,pdr,d,m,g,&bal_st,NULL);
  bal_st.bs+=bal_st.sbs;
  bal_st.iz+=bal_st.siz;
  bal_st.iz+=bal_st.iz1;
  /*Читаем переменные данные*/
  poiinpd(in,m,g,&rekin);

  dv=mv=gv=0;

  /*Читаем постоянную часть*/   
  sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model from Uosin \
where innom=%ld",in);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %ld !\n",gettext("Не найден инвентарный номер"),in);
    OSTANOV();
    naim.new_plus("");
    model.new_plus("");
    zavnom.new_plus("");
   }
  else
   {
    rsdat(&dv,&mv,&gv,row[2],2);
    zavnom.new_plus(row1[3]);
    naim.new_plus(row1[4]);
    model.new_plus(row1[5]);
   }

  if(tipz == 1)
   fprintf(ff1," %*s %*.*s %*.*s %5.5s %9.9s %9.9g %10ld %*.*s\n",
   iceb_tu_kolbait(30,kpos.ravno()),kpos.ravno(),
   iceb_tu_kolbait(5,rekin.shetu.ravno()),iceb_tu_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_tu_kolbait(9,rekin.hau.ravno()),iceb_tu_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   " "," ",bal_st.bs,in,
   iceb_tu_kolbait(10,zavnom.ravno()),iceb_tu_kolbait(10,zavnom.ravno()),zavnom.ravno());

  if(tipz == 2)
   fprintf(ff1," %-30d %5.5s %9.9s %*.*s %*.*s %9.9g %10ld %*.*s\n",
   pdr," "," ",
   iceb_tu_kolbait(5,rekin.shetu.ravno()),iceb_tu_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_tu_kolbait(9,rekin.hau.ravno()),iceb_tu_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   pbs*(-1),in,
   iceb_tu_kolbait(10,zavnom.ravno()),iceb_tu_kolbait(10,zavnom.ravno()),zavnom.ravno());

 }

/***************
iceb_tu_fgets(&str,ff);
fprintf(ff1,"%s",str.ravno());

iceb_tu_fgets(&str,ff);
sprintf(strsql,"%s %s",naim.ravno(),model.ravno());
iceb_tu_vstav(&str,strsql,20,99,1);
fprintf(ff1,"%s",str.ravno());

for(;;)
 {
  iceb_tu_fgets(&str,ff);
  if(str.ravno()[0] == '=')
   break;
  fprintf(ff1,"%s",str.ravno());

 }
fprintf(ff1,"\f\n");
while(iceb_tu_fgets(&str,ff) == 0)
  fprintf(ff1,"%s",str.ravno());
*********************/
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
    case 36:
     sprintf(strsql,"%s %s",naim.ravno(),model.ravno());
     iceb_tu_vstav(&str,strsql,20,99,1);
     break;
     
   }
  fprintf(ff1,"%s",str.ravno());
 }
 
//fclose(ff);
fclose(ff1);
//unlink(imaf);

class spis_oth oth;
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Распечатка акта ремонта/модернизации основного средства"));
iceb_t_ustpeh(imaf1,3);
iceb_t_rabfil(&oth,"");
}
