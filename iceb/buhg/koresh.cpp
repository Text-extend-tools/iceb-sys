/* $Id: koresh.c,v 5.31 2013/08/13 05:49:49 sasa Exp $ */
/*16.04.2017    11.12.1995      Белых А.И.      koresh.c
Распечатка корешков
*/
#include        <errno.h>
#include        "buhg.h"

extern short	*obud; /*Обязательные удержания*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

void koresh(int ttt,short mp,short gp,const char *fio,const char *inn,const char *dolgn,int kateg)
{
FILE            *kaw;
char            st[1024];
class iceb_tu_str str("");
class iceb_tu_str np("");
short           kodnah=0;
double          prom;
short           kd;
char            imaf[64];
short           im;
double          inah=0.,iu=0.,ius=0.;
double          inahb=0.,iub=0.,iusb=0.;
short           mkvk; /*0 не выдано кассой 1-выдано кассой*/
double		sal;
short		prn;
int		knah;
double		sym;
short		d,m,g;
short		mesn,godn;
char		strsql[1024];
SQL_str         row,row1;
long		kolstr;
int		kpd;
class iceb_tu_str naim("");
int		i1;
double		saldb;
SQLCURSOR curr;
clear();
printw("%d %d.%d\n",ttt,mp,gp);
GDITE();


/*Ищем запись*/
sprintf(strsql,"select podr from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,ttt);
/*
printw("%s\n",strsql);
refresh();
*/
if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найдено записи за %d.%dг. !"),mp,gp);
  iceb_t_soob(strsql);
  return;
 }
kpd=atoi(row1[0]);

sprintf(imaf,"koreh%d_%d.lst",ttt,getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

inah=iu=ius=0.;
/*Читаем сальдо*/
sal=zarsald(1,mp,gp,ttt,&saldb);
sprintf(strsql,"select naik from Podr where kod=%d",kpd);
if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
 {
  beep();
  printw("%s %d\n",gettext("Не найден код подразделения"),kpd);
  OSTANOV();
 }
else
  np.new_plus(row1[0]);

peh1(np.ravno(),fio,ttt,inn,mp,gp,dolgn,kateg,kaw,NULL,NULL,NULL,NULL); /*rasveds*/

sprintf(strsql,"select prn,knah,suma,datz,mesn,godn,kdn,podr,shet from \
Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and suma <> 0. order by prn,knah asc",gp,mp,gp,mp,ttt);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,gettext("Не найдено начислений и удержаний за %d.%dг. !"),mp,gp);
  iceb_t_soob(strsql);
  return;
 }
im=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],
  row[5],row[6]);
  refresh();
*/
  prn=atoi(row[0]);
  knah=atoi(row[1]);
  sym=atof(row[2]);
  rsdat(&d,&m,&g,row[3],2);
  mesn=atoi(row[4]);
  godn=atoi(row[5]);
  kd=atoi(row[6]);
    
  if(kodnah != prn)
   {
    if(prn == 1)
     {
      str.new_plus(gettext(" Н а ч и с л е н и я"));
      strfil(str.ravno(),str.ravno(),1,kaw);
     }
   if(prn == 2)
    {
     str.new_plus(gettext(" У д е р ж а н и я"));
     strfil(str.ravno(),str.ravno(),1,kaw);
    }
   kodnah=prn;
  }

 if(prn == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",knah);
 if(prn == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",knah);
    
 if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
  {
   beep();
   if(prn == 1)
    printw("%s %s\n",gettext("Не найден код начисления"),row[3]);
   if(prn == 2)
    printw("%s %s\n",gettext("Не найден код удержания"),row[3]);
   OSTANOV();
   naim.new_plus("");
  }
 else
   naim.new_plus(row1[0]);


 memset(st,'\0',sizeof(st));

 prom=sym;
 mkvk=0;
 if(prn == 2)
  {
   prom*=-1;

/*
   if(godn != gp || mesn != mp)
     mkvk=1;
*/
   if(obud != NULL)
    {
     for(i1=1; i1<= obud[0]; i1++)
      if(obud[i1] == knah)
       {
        break;
       }

     if(i1 > obud[0])
       if(godn != gp || mesn != mp)
          mkvk=1;

     }

   im++;
  }

 prnb(prom,st);

 if(kd != 0)
  sprintf(strsql,"%3d %-*.*s %2d %2d.%4d %-2s %12s",
  knah,iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),kd,mesn,godn,row[7],st);
 else
  sprintf(strsql,"%3d %-*.*s %2s %2d.%4d %-2s %12s",
  knah,iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno()," ",mesn,godn,row[7],st);

 if(prn == 2 && mkvk == 1)
  strfil(strsql," ",0,kaw);
 else
  strfil(strsql,strsql,0,kaw);

 if(prn == 1)
  {
   inah+=sym;
   if(proverka(shetb,row[8],0,1) == 0)
     inahb+=sym;
   
  }
 if(prn == 2)
  {
   iu+=sym;
   if(proverka(shetbu,row[8],0,1) == 0)
     iub+=sym;
   if(mkvk == 0)
    {
     ius+=sym;
     if(proverka(shetbu,row[8],0,1) == 0)
       iusb+=sym;

    }
  }

 }

if(im > 0 || sal != 0.)
 {
  itogiz(inah,iu,ius,sal,inahb,iub,iusb,saldb,kaw,NULL,NULL,NULL);
 }


podpis(kaw);
fclose(kaw);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка корешков"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);
iceb_t_rabfil(&oth,"");

}
