/*$Id: usl_shet_fak.c,v 5.26 2013/08/13 05:49:58 sasa Exp $*/
/*24.06.2013	22.02.2005	Белых А.И.	usl_shet_fak.c
Распечатка счёта фактуры для подсистемы "Учёт услуг"
Если вернули 0-сделали докумет
             1-нет
*/

#include        <errno.h>
#include	"buhg.h"


extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/

int usl_shet_fak(short dg,short mg,short gg, //Дата документа
int podr, //Подразделение
const char *nomdok, //Номер документа
char *imafil, //имя файла с распечаткой
int tipz)  //1-приход 2-расход
{
class iceb_tu_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_tu_str pkod(""),pkod1(""); //Код ЕДРПУО
class iceb_tu_str mfo(""),mfo1("");  /*МФО*/
class iceb_tu_str nsh(""),nsh1("");  /*Номер счета*/
char		strsql[1024];
SQL_str row;
SQLCURSOR cur;
int		kolstr;
class iceb_tu_str kodop("");
class iceb_tu_str kontr("");
double sumkor;
class iceb_tu_str naimnal("");
float		procent=0.;
class iceb_tu_str kodkontr00("");
class iceb_tu_str bros("");
short lnds=0; //Льготы по НДС
class iceb_tu_str dover("");
class iceb_tu_str sherez("");
short		mvnpp;
float pnds=0.;

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,sumkor,datop,osnov,nds,dover,sherez,pn from Usldokum \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mg,dg,podr,nomdok,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-Не найден документ !",__FUNCTION__);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dg,mg,gg,
  gettext("Документ"),
  nomdok,
  gettext("Подразделение"),
  podr);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
pnds=atof(row[8]);
kontr.new_plus(row[0]);
kodop.new_plus(row[1]);
sumkor=atof(row[2]);


lnds=atoi(row[5]);

dover.new_plus(row[6]);

sherez.new_plus(row[7]);

usldopnal(tipz,kodop.ravno(),&procent,&naimnal);

  
/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Usloper1");
if(tipz == 2)
 bros.new_plus("Usloper2");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros.ravno(),kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
else
 {
  mvnpp=atoi(row[1]);
 }

kodkontr00.new_plus("00");
if(tipz == 2)
 {
  if(iceb_t_poldan(kodop.ravno(),&bros,"uslnast.alx") == 0)
   {
    kodkontr00.new_plus(bros.ravno());
   }
 }
//printw("\nkodkontr00=%s\n",kodkontr00);
//OSTANOV();

/*Читаем реквизиты организации свои */


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  nmo.new_plus(row[1]);
  pkod.new_plus(row[5]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
 } 



/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(kontr.ravno_atof() != 0. || kontr.ravno()[0] != '0')
 {

  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
    iceb_t_soob(strsql);
   }
  else
   {
    if(row[16][0] == '\0')
      nmo1.new_plus(row[1]);
    else
      nmo1.new_plus(row[16]);
    pkod1.new_plus(row[5]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    
   }
 }

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gg,mg,dg,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
  return(1);

sprintf(imafil,"shfk%d.lst",getpid());
FILE *ff;

if((ff = fopen(imafil,"w")) == NULL)
 {
  error_op_nfil(imafil,errno,"");
  return(1);
 }

if(tipz == 1)
if(shet_fak_sap(dg,mg,gg,nomdok,nsh1.ravno(),mfo1.ravno(),pkod1.ravno(),nmo.ravno(),dover.ravno(),sherez.ravno(),ff) != 0)
 {
  fclose(ff);
  unlink(imafil);
  return(1);
 }

if(tipz == 2)
if(shet_fak_sap(dg,mg,gg,nomdok,nsh.ravno(),mfo.ravno(),pkod.ravno(),nmo1.ravno(),dover.ravno(),sherez.ravno(),ff) != 0)
 {
  fclose(ff);
  unlink(imafil);
  return(1);
 }


SQLCURSOR curr;
SQL_str row1;
int metka;
int kodzap;
double kolih;
double cena;
double cenasnd;
double itogo=0.;
double suma;
class iceb_tu_str naim("");
int nom_str=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
 
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=okrug(cena,okrcn);
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  itogo+=suma;
  cenasnd=cena+cena*pnds/100.;
  cenasnd=okrug(cenasnd,okrg1);
  
  /*Читаем наименование услуги*/

  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s\n",strsql);
    if(metka == 0)
     printw("%s %d !\n",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     printw("%s %d !\n",gettext("Не найден код услуги"),kodzap);
    OSTANOV();
    naim.new_plus("");
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }    
   }

  fprintf(ff," %3d %-*.*s %6.6g %-*.*s %8.8g %10.2f\n",
  ++nom_str,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  kolih,
  iceb_tu_kolbait(6,row[4]),iceb_tu_kolbait(6,row[4]),row[4],
  cena,suma);

  if(iceb_tu_strlen(naim.ravno()) > 30)
     fprintf(ff,"%3s %s\n"," ",iceb_tu_adrsimv(30,naim.ravno()));
 }
fprintf(ff,"\
---------------------------------------------------------------------\n");
int dlina=58;
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(dlina,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
  dok_end_usl(tipz,dg,mg,gg,nomdok,podr,dlina,sumkor,itogo,pnds,lnds,procent,naimnal.ravno(),ff);


shet_fak_end(ff); //распечатка концовки документа

fclose(ff);

iceb_t_ustpeh(imafil,3);

return(0);

}
