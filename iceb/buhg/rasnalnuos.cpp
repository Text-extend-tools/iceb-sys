/* $Id: rasnalnuos.c,v 1.48 2014/07/31 07:09:17 sasa Exp $ */
/*07.12.2018    22.01.1994      Белых А.И.      rasnalnuos.c
Распечатка налоговых накладных для Учета основных средств
*/
#include	"buhg.h"

int rasnalnuos9(short dd,short md,short gd,int podr,const char *nomdok,char imaf[]);

extern double          okrcn;  /*Округление цены*/
extern double okrg1;

int rasnalnuos(short dd,short md,short gd,int podr,const char *nomdok,char imaf[])
{

/**************




if(sravmydat(dd,md,gd,1,1,2015) >= 0 && sravmydat(dd,md,gd,1,4,2016) < 0)
 return(rasnalnuos7(dd,md,gd,podr,nomdok,imaf));

if(sravmydat(dd,md,gd,1,4,2016) >= 0)
**************************/
// return(rasnalnuos8(dd,md,gd,podr,nomdok,imaf));
 return(rasnalnuos9(dd,md,gd,podr,nomdok,imaf));
return(1);

}


/*********************/
/*Новая программа 9*/
/***********************/
int rasnalnuos9(short dd,short md,short gd,int podr,const char *nomdok,char imaf[])
{
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		strsql[1024];
FILE            *f1;
short           i;
double          itogo;
class iceb_tu_str kpos("");
short		tipz;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str naim("");
double		cena;
class iceb_tu_str ei("");
short		dnn,mnn,gnn; //Дата налоговой накладной
class iceb_tu_str kodop("");
float pnds=0.;
short lnds=0;


/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tipz,osnov,forop,nomnn,datpnn,kodop,nds,pn,vd \
from Uosdok where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(readkey(strsql,&row,&curr) != 1)
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

kpos.new_plus(row[0]);
tipz=atoi(row[1]);
nomnn.new_plus(row[4]);
rsdat(&dnn,&mnn,&gnn,row[5],2);
kodop.new_plus(row[6]);
lnds=atoi(row[7]);
pnds=atof(row[8]);

class iceb_tu_str kodkontr00("00");
if(tipz == 2)
 kodkontr00.new_plus(iceb_t_getk00(2,kodop.ravno()));

if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");

//Печатаем шапку документа
if(iceb_t_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1) != 0)
 return(1);

iceb_t_insfil("nn9_hap.alx",f1);


sprintf(strsql,"select innom,cena from Uosdok1 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d order by innom asc",gd,md,dd,podr,nomdok,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


itogo=0.;
i=0;
ei.new_plus(gettext("Шт"));
int nomer_str=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;

  cena=atof(row[1]);
  cena=okrug(cena,okrcn);

  //Читаем наименование
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
    naim.new_plus(row1[0]);
  else
    naim.new_plus("");    

  /*Разпечатка строки*/
  iceb_t_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),1,cena,cena,"",0,"","",f1);

  itogo+=cena;


 }
fprintf(f1,"ICEB_LST_END\n");
//Печатаем концовку налоговой накладной
iceb_t_nn9_end(tipz,lnds,"uosnast.alx",kodop.ravno(),kpos.ravno(),f1);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(itogo)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(itogo)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=itogo+suma_nds;
/*записываем в шапку суммы*/
iceb_t_nn9_start_s(imaf,lnds,suma_dok,suma_nds,0);

iceb_t_ustpeh(imaf,3,&kolstr);
iceb_t_rnl(imaf,kolstr,"",&iceb_t_nn9_hap);
return(0);

}
