/*$Id: zapzarp.c,v 5.27 2013/09/26 09:43:45 sasa Exp $*/ 
/*11.01.2018	25.05.1999 Белых А.И.  zapzarp.c
Запись строки начисления/удержания
Если вернули 0 - записали
             1 - запись уже есть
             2 - ошибка записи
             3 - у оператора доступ только на чтение
*/
#include        "buhg.h"


int zapzarp(short d,short m,short g, //Дата записи
long tabn,short prn,
short knah,double suma,const char *shet,
short mesn,short godn, //В счёт какого месяца
short kdn,
short nomz,
const char *kom,
int podr,
const char *nomdok,
class ZARP zp)
{
time_t		vrem;
char		strsql[1024];


if(zp.dz != 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
  tabn=%ld and prn='%d' and knah=%d and godn=%d and mesn=%d and \
  podr=%d and shet='%s' and nomz=%d",g,m,zp.dz,tabn,prn,knah,
  zp.godn,zp.mesn,zp.podr,zp.shet,zp.nomz);
/**************
  zar_pr_ins(__FUNCTION__);
  zar_pr_ins("-");
  zar_pr_ins(strsql);
  zar_pr_ins("\n");
*************/
//printw("\n1-%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();        
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return(3);
   else
    {
     msql_error(&bd,__FUNCTION__,strsql);
    }
   }
 }

time(&vrem);
suma=okrug(suma,0.01);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"insert into Zarp \
values ('%04d-%02d-%02d',%ld,'%d',%d,%.2f,'%s',%d,%d,%d,%d,%d,%ld,'%s',%d,'%s')",
g,m,d,tabn,prn,knah,suma,shet,godn,mesn,kdn,nomz,iceb_t_getuid(),vrem,kom,podr,nomdok);

//printw("\n2-%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();        
/***************
  zar_pr_ins(__FUNCTION__);
  zar_pr_ins("-");
  zar_pr_ins(strsql);
  zar_pr_ins("\n");
****************/
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
//    printw("\n%s-%s\n",__FUNCTION__,strsql);
//    OSTANOV();
    return(1);
   }
  else
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(2);
   }
 }

return(0);

}
/*************************************************/
/*************************************************/
int zapzarp(short d,short m,short g, //Дата записи
long tabn,short prn,
short knah,double suma,const char *shet,
short mesn,short godn, //В счёт какого месяца
short kdn,
short nomz,
const char *kom,
int podr,
const char *nomdok,
class ZARP zp,
FILE *ff_prot)
{
time_t		vrem;
char		strsql[512];


if(zp.dz != 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
  tabn=%ld and prn='%d' and knah=%d and godn=%d and mesn=%d and \
  podr=%d and shet='%s' and nomz=%d",g,m,zp.dz,tabn,prn,knah,
  zp.godn,zp.mesn,zp.podr,zp.shet,zp.nomz);

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);
         

  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return(3);
   else
    {
     msql_error(&bd,__FUNCTION__,strsql);
    }
   }
 }

time(&vrem);
suma=okrug(suma,0.01);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"insert into Zarp \
values ('%04d-%02d-%02d',%ld,'%d',%d,%.2f,'%s',%d,%d,%d,%d,%d,%ld,'%s',%d,'%s')",
g,m,d,tabn,prn,knah,suma,shet,godn,mesn,kdn,nomz,iceb_t_getuid(),vrem,kom,podr,nomdok);

if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);

//printw("zapzarp-%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
    return(1);
  else
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(2);
   }
 }

return(0);

}

/***************************************************/
/*************************************************/

int zapzarp(class ZARP *zp,
double suma,
short denz, //день записи
short mv,short gv,  //В счёт какого месяца
short kdn, //Количество дней
const char *shet, //Счет в запись
const char *koment, //Коментарий
short nz, //Номер записи
int podr, //подразделение
const char *nomdok) //номер документа
{
time_t		vrem;
char		strsql[1024];

if(zp->dz != 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and \
podr=%d and shet='%s' and nomz=%d",
  zp->gz,zp->mz,zp->dz,zp->tabnom,zp->prn,zp->knu,
  zp->godn,zp->mesn,zp->podr,zp->shet,zp->nomz);
/***************
  zar_pr_ins(__FUNCTION__);
  zar_pr_ins("-");
  zar_pr_ins(strsql);
  zar_pr_ins("\n");
*************/
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return(3);
   else
    {
     msql_error(&bd,__FUNCTION__,strsql);
    }
   }
 }

time(&vrem);
suma=okrug(suma,0.01);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"insert into Zarp \
values ('%04d-%02d-%02d',%d,'%d',%d,%.2f,'%s',%d,%d,%d,%d,%d,%ld,'%s',%d,'%s')",
zp->gz,zp->mz,denz,zp->tabnom,zp->prn,zp->knu,suma,shet,gv,mv,kdn,nz,iceb_t_getuid(),vrem,koment,podr,nomdok);
/**************
zar_pr_ins(__FUNCTION__);
zar_pr_ins("-");
zar_pr_ins(strsql);
zar_pr_ins("\n");
*************/
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
//    printf("zapzarpw-запись уже есть-%s\n%d %s",strsql,sql_nerror(&bd),sql_error(&bd));
    return(1);
   }
  else
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(2);
   }
 }

return(0);

}
