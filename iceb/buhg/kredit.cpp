/* $Id: kredit.c,v 5.22 2013/09/26 09:43:35 sasa Exp $ */
/*20.04.2016    01.12.1992      Белых А.И.      kredit.c
Подпрограмма расчёта удержаний за кредит
Переменные данные лежат в файле кредит
*/
#include        "buhg.h"

extern struct ZAR zar;

void kredit(int tn,short mp,short gp,int podr,const char *uder_only)
{
int             ku=0;   /*Код начисления*/
double          nah=0.;
short		d,m,g;
SQL_str         row;
char		strsql[1024];
class iceb_tu_str shet("");
class ZARP	zp;
class SQLCURSOR cur;

sprintf(strsql,"\n%s-Расчёт удержания по кредиту\n----------------------------------------------\n",__FUNCTION__);
zar_pr_ins(strsql);

sprintf(strsql,"select ku,sp,mu,dd from Zarrud where tn=%d",tn);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-Ненайдено настройки для удержания!\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;
 }

ku=atoi(row[0]);

if(proverka(uder_only,ku,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,ku);
  zar_pr_ins(strsql);
  return;
 }


d=0;
rsdat(&d,&m,&g,row[3],2);
if(d != 0 && mp > m && gp >= g)
 {
  sprintf(strsql,"%s-Закончисля срок действия. Расчёт не производим\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;
 }

if(atoi(row[2]) == 1)
 {
  double procent=atof(row[1]);
  sprintf(strsql,"%s-Процент от суммы начисленной %f\n",__FUNCTION__,procent);
  zar_pr_ins(strsql);
  //Узнаём всю сумму начиления
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and suma <> 0.",
  gp,mp,gp,mp,tn);
  int kolstr=0;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
   }
  double suma=0.;

  if(kolstr > 0)
   while(cur.read_cursor(&row) != 0)
    suma+=atof(row[0]);

  sprintf(strsql,"%s-Сумма всех начислений:%.2f процент=%f\n",__FUNCTION__,suma,procent);
  zar_pr_ins(strsql);

  if(suma != 0.)   
   {
    nah=suma*procent/100.;
    nah=okrug(nah,0.01);
     
   }
 }
else
 {
  sprintf(strsql,"%s-Стабильная сумма\n",__FUNCTION__);
  zar_pr_ins(strsql);
  nah=atof(row[1]);
  
 }


if(nah == 0.)
 {
  sprintf(strsql,"%s-Начисленная сумма равна нолю\n",__FUNCTION__);
  zar_pr_ins(strsql);
   
  return;
 }

sprintf(strsql,"%s-Код удержания %d. Сумма удержания %.2f\n",__FUNCTION__,ku,nah);
zar_pr_ins(strsql);
 
SQLCURSOR curr;
/*Проверяем есть ли код начисления в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,ku); 
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s-Не введён код удержания %d в список удержаний !\n",__FUNCTION__,ku);
  zar_pr_ins(strsql);
  return;
 }
shet.new_plus(row[0]);

nah=nah*(-1);

dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet.ravno());
zapzarp(d,mp,gp,tn,2,ku,nah,shet.ravno(),mp,gp,0,0,"",podr,"",zp); 
}

