/* $Id: rasdolgi1.c,v 5.27 2013/08/13 05:49:52 sasa Exp $ */
/*15.07.2015    18.01.1993      Белых А.И.      rasdolgi1.c
Программа получения распечатки должников перед организацией
*/
#include        <errno.h>
#include        "buhg.h"

extern short    mmm,ggg;
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int rasdolgi1(class spis_oth *oth)
{
struct  tm      *bf;
time_t          tmm;
FILE            *kaw;
char            imaf[64];
int           tabb;
double          sum;
double          itogo;
unsigned int    kol;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[512];
short		kom1;
short		mr,gr;
static class iceb_tu_str dat1("");
double		saldb;
double		sald;
double		sm,sumb,sumnb;
int 		i1;
double		itogb,itognb;
class iceb_tu_str fio("");
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка должников"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F10",gettext("выход"),NULL);

kom1=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1);

switch(kom1)
 {
  case FK1:
   GDITE();
   iceb_t_pdoc("zarp4_3_10.txt");
   goto naz;
  
  case FK10:
  case ESC:
   return(1);
  case ENTER:
   break;
  default:
    goto naz;   
 }

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

clear();
printw(gettext("Список должников.\n"));
GDITE();
sprintf(imaf,"spidol.lst");

if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
time(&tmm);
bf=localtime(&tmm);

fprintf(kaw,"\
%s\n\n\
%s %d.%d%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Список должников за"),
mr,gr,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(kaw,"\
----------------------------------------------------\n");
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",
gr,mr);
SQLCURSOR cur;
SQLCURSOR curr;
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

kol=0;
itogb=itognb=itogo=sum=0.;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  tabb=atol(row[0]);

  sum=0.;
  /*Читаем сальдо*/
  sum=sald=zarsald(1,mr,gr,tabb,&saldb);

  sprintf(strsql,"select suma,prn,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d",
  gr,mr,gr,mr,tabb);
  SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  sumnb=sald-saldb;
  sumb=saldb;
  while(cur1.read_cursor(&row1) != 0)
   {
    sm=atof(row1[0]);
    sum+=sm;
    if(shetbu != NULL && shetb != NULL)
     {
      if(row1[1][0] == '1')
       {
       if(pole1(shetb,row1[2],',',0,&i1) == 0 || SRAV(shetb,row1[2],0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
      if(row1[1][0] == '2')
       {
       if(pole1(shetbu,row1[2],',',0,&i1) == 0 || SRAV(shetbu,row1[2],0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
     }
   }
   
  if(sum < -0.009 && shetbu == NULL) 
   {
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      fio.new_plus("");
      beep();
      printw("%s %d !\n",gettext("Не найден табельный номер"),tabb);
      OSTANOV();
      continue;
     }
    else
      fio.new_plus(row1[0]);
    printw("%-5d %-*s %15s\n",
    tabb,
    iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    prnbr(sum));
    
    fprintf(kaw,"%-5d %-*s %15s\n",tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),prnbr(sum));
    itogo+=sum;
    kol++;
   }
  if(shetbu != NULL && (sumb < -0.009 || sumnb < -0.009 ))
   {
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      fio.new_plus("");
      beep();
      printw("%s %d !\n",gettext("Не найден табельный номер"),tabb);
      OSTANOV();
      continue;
     }
    else
      fio.new_plus(row1[0]);
    if( sumb < -0.009)
     {
      printw("%-5d %-*s %15s Бюджет\n",tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),prnbr(sumb));
      fprintf(kaw,"%-5d %-*s %15s Бюджет\n",tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),prnbr(sumb));
      itogb+=sumb;
     }
    if( sumnb < -0.009)
     {
      printw("%-5d %-30s %15s\n",tabb,fio.ravno(),prnbr(sumnb));
      fprintf(kaw,"%-5d %-30s %15s\n",tabb,fio.ravno(),prnbr(sumnb));
      itognb+=sumnb;
     }
    kol++;

   }
 }

if(kol == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного должника !"));
 }
else
 {
  printw("%s - %d\n",gettext("Количество должников"),kol);
  fprintf(kaw,"\
----------------------------------------------------\n");
  if(shetbu == NULL)
   {
    printw("%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом :"),"Разом :",prnbr(itogo));
    fprintf(kaw,"%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом :"),"Разом :",prnbr(itogo));
   }
  else
   {
    if(itogb != 0.)
     {
      printw("%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом бюджет:"),"Разом бюджет:",prnbr(itogb));
      fprintf(kaw,"%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом бюджет:"),"Разом бюджет:",prnbr(itogb));
     }
    if(itognb != 0.)
     {
      printw("%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом не бюджет:"),"Разом не бюджет:",prnbr(itognb));
      fprintf(kaw,"%5s %*s %15s\n"," ",iceb_tu_kolbait(30,"Разом не бюджет:"),"Разом не бюджет:",prnbr(itognb));
     }
   }
  fprintf(kaw,"%s - %d\n",gettext("Количество должников"),kol);

//  printw("\n%s \"%s\"\n\n",gettext("Распечатку выгружено в файл"),imaf);

  OSTANOV();
 }
podpis(kaw);
fclose(kaw);

if(kol != 0)
 {
  oth->spis_imaf.plus(imaf);
  oth->spis_naim.plus(gettext("Распечатка списка должников"));
  iceb_t_ustpeh(imaf,0);
  return(0);
 }
else
 unlink(imaf);
return(1); 
}
