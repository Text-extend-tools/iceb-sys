/* $Id: rasnalnu.c,v 5.63 2014/07/31 07:09:17 sasa Exp $ */
/*07.12.2018    22.01.1994      Белых А.И.      rasnalnu.c
Распечатка налоговых накладных для услуг
*/
#include	"buhg.h"


int rasnalnu9(short dd,short md,short gd,int podr,const char nomdok[],const char imaf[],short lnds);

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/

int rasnalnu(short dd,short md,short gd,int podr,
const char nomdok[],const char imaf[],
short lnds) //Тип накладной
{

/*************************




if(sravmydat(dd,md,gd,1,1,2015) >= 0 && sravmydat(dd,md,gd,1,4,2016) < 0) 
 return(rasnalnu7(dd,md,gd,podr,nomdok,imaf,lnds));

if(sravmydat(dd,md,gd,1,4,2016) >= 0) 
*****************/
// return(rasnalnu8(dd,md,gd,podr,nomdok,imaf,lnds));

 return(rasnalnu9(dd,md,gd,podr,nomdok,imaf,lnds));

return(1);

}
/***************************************************/
/*Новая налоговая накладная 9*/
/***************************************************/
int rasnalnu9(short dd,short md,short gd,int podr,
const char nomdok[],const char imaf[],
short lnds) //Тип накладной
{
SQL_str         row,row1;
int		kolstr;
char		strsql[1024];
FILE            *f1;
short           i;
double          itogo;
double          bb;
double		sumkor;
class iceb_tu_str bros("");
class iceb_tu_str kpos("");
int		tipz;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str naim("");
double		cena,kolih;
class iceb_tu_str ei("");
short 		metka;
int		kodzap;
short		dnn,mnn,gnn; //Дата налоговой накладной
class iceb_tu_str kodop("");
float		procent=0.;
class iceb_tu_str naimnal("");

float pnds=0.;
SQLCURSOR curr;		
SQLCURSOR cur;		

class iceb_tu_str kodkontr00("00");

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tp,sumkor,datop,nomnn,forop,uslpr,osnov,datpnn,kodop,pn,k00 \
from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s-%s!\n%s:%d.%d.%d %s:%s %s:%d",
  __FUNCTION__,
  gettext("Не найден документ"),
  gettext("Дата"),
  dd,md,gd,
  gettext("Документ"),
  nomdok,
  gettext("Подразделение"),
  podr);
  iceb_t_soob(strsql);
  return(1);
 }
pnds=atof(row[10]);
//memset(datop,'\0',sizeof(datop));
sumkor=0.;
kodkontr00.new_plus(row[11]);


kpos.new_plus(row[0]);
tipz=atoi(row[1]);
sumkor=atof(row[2]);

rsdat(&dnn,&mnn,&gnn,row[8],2);


class iceb_tu_str kod_ei_usl;
iceb_t_poldan("Код единицы измерения услуги",&kod_ei_usl,"uslnast.alx");

nomnn.new_plus(row[4]);
kodop.new_plus(row[9]);

usldopnal(tipz,kodop.ravno(),&procent,&naimnal);

if(tipz == 2)
 if(SRAV(kodkontr00.ravno(),"00",0) == 0) 
  kodkontr00.new_plus(iceb_t_getk00(1,kodop.ravno()));

if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");

//печатаем заголовок налоговой накладной
if(iceb_t_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1) != 0)
 return(1);
iceb_t_insfil("nn9_hap.alx",f1);

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gd,md,dd,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
itogo=0.;
i=0;
int nomer_str=0;
class iceb_tu_str kod_usl("");
int metka_imp_tov=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;

  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=okrug(cena,okrcn);

  /*Читаем наименование материалла*/
  naim.new_plus("");
  
  if(metka == 0)
   sprintf(strsql,"select naimat,ku from Material where kodm=%d",kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius,ku from Uslugi where kodus=%d", kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s\n",strsql);
    if(metka == 0)
     printw("%s %d !\n",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     printw("%s %d !\n",gettext("Не найден код услуги"),kodzap);
    OSTANOV();
    kod_usl.new_plus("");
   }
  else
   {
    kod_usl.new_plus(row1[1]);    
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }
   }
  
  ei.new_plus(row[4]);

  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  /*Разпечатка строки*/
  if(proverka(kod_ei_usl.ravno(),ei.ravno(),0,1) == 0 )
   iceb_t_nn9_str(++nomer_str,1,naim.ravno(),ei.ravno(),kolih,cena,bb,"",metka_imp_tov,kod_usl.ravno(),"",f1);
  else
   iceb_t_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),kolih,cena,bb,"",metka_imp_tov,kod_usl.ravno(),"",f1);

  itogo+=bb;


 }

fprintf(f1,"ICEB_LST_END\n");

//Печатаем концовку налоговой накладной
iceb_t_nn9_end(tipz,lnds,"uslnast.alx",kodop.ravno(),kpos.ravno(),f1);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=itogo+sumkor+suma_nds;

/*записываем в шапку суммы*/
iceb_t_nn9_start_s(imaf,lnds,suma_dok,suma_nds,0);

iceb_t_ustpeh(imaf,3,&kolstr);
iceb_t_rnl(imaf,kolstr,"",&iceb_t_nn9_hap);
return(0);
}
