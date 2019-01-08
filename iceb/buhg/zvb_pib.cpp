/*$Id: zvb_pib.c,v 5.17 2013/09/26 09:43:46 sasa Exp $*/
/*18.10.2014	19.07.2005	Белых А.И.	zvb_pib.c
*/
#include        <errno.h>
#include    "buhg.h"

extern int      kol_strok_na_liste;

void	zvb_pib(short prn,char *zapros,struct dirnupoi *poisk)
{
SQL_str		row,row1;
class iceb_tu_str fio("");
double		sumai=0.,sum=0.;;
char            imafkartr[64];
char		imafkartf[64];
FILE            *ffkartr=NULL; //Распечатка выгрузки на картсчета
FILE		*ffkartf=NULL; //Файл выгрузки на дискету
int		kolstrkartr=0;
double          sumalist=0.;
char		strsql[512];
short		d,m,g;
int		kolstr=0,kolstr1=0;
class iceb_tu_str bros("");
int		kolz=0;
class iceb_tu_str bankshet("");
double          itogo=0.,itogoo=0.;
int		nomer=0;
class iceb_tu_str inn("");
int		kolstrok=0;
class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
SQLCURSOR curr;
GDITE();

iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    glavbuh.new_plus(row1[0]);
 }


SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }


sprintf(imafkartr,"kartr%d.lst",getpid());
sprintf(imafkartf,"kartf%d.txt",getpid());



if((ffkartr = fopen(imafkartr,"w")) == NULL)
 {
  error_op_nfil(imafkartr,errno,"");
  return;
 }
hrvnks(ffkartr,&kolstrkartr);

if((ffkartf = fopen(imafkartf,"w")) == NULL)
 {
  error_op_nfil(imafkartf,errno,"");
  return;
 }



sumai=0.;
kolstr1=0;
int kol_na_liste=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  

  rsdat(&d,&m,&g,row[0],2);

  sum=atof(row[4]);
  if(sum == 0.)
    continue;
  if(prn == 2)
    sum*=-1;
  


  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден табельный номер"),row[1]);
    OSTANOV();
    continue;
   }
  fio.new_plus(row1[0]);
  inn.new_plus(row1[2]);
  bankshet.new_plus(row1[1]);
  if(bankshet.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }

  
  kolz++;
 
  pib_itl(&kolstrkartr,&sumalist,&kol_na_liste,rukov.ravno(),glavbuh.ravno(),ffkartr);

  sumalist+=sum;
  pib_str(kolz,bankshet.ravno(),sum,fio.ravno(),inn.ravno(),ffkartr);
  
  kol_na_liste++;    
  fprintf(ffkartf,"%-16s %8.2f %s %s\n",bankshet.ravno(),sum,inn.ravno(),fio.ravno());

  kolstrok+=2;
  sumai+=sum;
  itogo+=sum; 
  itogoo+=sum; 
  nomer++;
 }

  pib_end_dok(kol_na_liste,sumalist,itogoo,nomer,rukov.ravno(),glavbuh.ravno(),ffkartr);

  fclose(ffkartr);

  fclose(ffkartf);

  //Записываем шапку и концовку
  hdisk(nomer,itogoo,imafkartf);
  

class spis_oth oth;

oth.spis_imaf.plus(imafkartr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета."));

oth.spis_imaf.plus(imafkartf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}
