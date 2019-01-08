/* $Id: zappdok.c,v 5.26 2013/08/13 05:50:01 sasa Exp $ */
/*28.06.2013	07.05.1998	Белых А.И.	zappdok.c
Запись платежного поручения
Если вернули 0-записали
             1-нет
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

int zappdok(const char *tabl)
{
short		i1;
FILE		*ff;
char		imaf[64];
char		plat[1024],polu[1024];
char		strsql[2048];

/*Записываем саму платежку*/

//printw("zappdok-tabl=%s %d.%d.%d npp=%s sum=%.2f smp=%.2f kop=%s rec.ddi=%d\n",
//tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),rec.sumd,rec.uslb,rec.kodop.ravno(),rec.ddi);


sprintf(plat,"%s#%s#%s#%s#%s#%s#%s",
rec.kodor.ravno(),rec.naior.ravno(),rec.gorod.ravno(),rec.kod.ravno(),rec.naiban.ravno(),rec.mfo.ravno(),rec.nsh.ravno());
sqlfiltr(plat,sizeof(plat));

sprintf(polu,"%s#%s#%s#%s#%s#%s#%s",
rec.kodor1.ravno(),rec.naior1.ravno(),rec.gorod1.ravno(),rec.kod1.ravno(),rec.naiban1.ravno(),rec.mfo1.ravno(),rec.nsh1.ravno());
sqlfiltr(polu,sizeof(polu));

sprintf(strsql,"insert into %s \
values ('%04d-%02d-%02d','%s','%s',%.2f,%.2f,%.2f,'%s','%s','%s',0,1,\
'%s',%d,%ld,'%04d-%d-%d','%s')",
tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),rec.kodop.ravno(),rec.sumd,rec.nds,rec.uslb,rec.shbzu.ravno(),plat,polu,"",
iceb_t_getuid(),time(NULL),rec.gv,rec.mv,rec.dv,rec.kodnr.ravno());   

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   return(0); //чтобы не выдавала сообщение что документ уже есть
  else
   {
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    return(1);
   }
 }
/*Записываем текст комментария*/
sprintf(imaf,"pl%d.tmp",getpid());
if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(0);
 }

for(i1=0; i1 < 5 ; i1++)
 {
  memset(plat,'\0',sizeof(plat));
  if(FGETSS(plat,sizeof(plat),ff) == 0)
    break;
  if(plat[0] == '#')
   continue;
  sqlfiltr(plat,sizeof(plat));
/*
  if(plat[0] == '\n' || plat[0] == '\0')
   continue;
*/
  sprintf(strsql,"insert into %sz values ('%04d-%02d-%02d','%s',0,%d,'%s')",
  tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),i1,plat);   

  sql_zapis(strsql,1,0);
 }

fclose(ff);

//Выполняем корректировки в сопутствующих таблицах
if(rec.ddi != 0)
 {
  if(SRAV(rec.nomdk.ravno(),rec.nomdk_i.ravno(),0) != 0 || 
  sravmydat(rec.dd,rec.md,rec.gd,rec.ddi,rec.mdi,rec.gdi) != 0 || \
  SRAV(rec.kodop.ravno(),rec.kodop_i.ravno(),0) != 0)
   {
    char tipd[10];
    memset(tipd,'\0',sizeof(tipd));
    short tp=0;
    if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
     {
      sprintf(tipd,ICEB_MP_PPOR);
      tp=0;
     }
    if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
     {
      sprintf(tipd,ICEB_MP_PTRE);
      tp=1;
     }
     //Исправляем подтверждающие записи если они есть
     sprintf(strsql,"update Pzpd set \
datd='%04d-%d-%d', \
nomd='%s' \
where tp=%d and nomd='%s' and \
datd='%04d-%d-%d'",rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),tp,rec.nomdk_i.ravno(),rec.gdi,rec.mdi,rec.ddi);

  //  printw("%s\n",strsql);
    
    sql_zapis(strsql,1,0);

    sprintf(strsql,"update Pzpd set \
datp='%04d-%d-%d' \
where tp=%d and nomd='%s' and datd='%04d-%d-%d' and datp='%04d-%d-%d'",
    rec.gd,rec.md,rec.dd,rec.tipz,rec.nomdk_i.ravno(),
    rec.gd,rec.md,rec.dd,rec.gdi,rec.mdi,rec.ddi);

    sql_zapis(strsql,1,0);
                        
    //исправляем в проводках дату документа и номер документа, операцию
    sprintf(strsql,"update Prov set \
datd='%04d-%d-%d',\
nomd='%s',\
oper='%s' \
where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%d-%d' and tz=%d",rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),rec.kodop.ravno(),
    tipd,0,rec.nomdk_i.ravno(),rec.gdi,rec.mdi,rec.ddi,rec.tipz);
//По операции не проверяем так как не может быть двух платежк с одним номером

    sql_zapis(strsql,1,0);

    //исправляем в проводках дату проводки
    //с датой документа
    sprintf(strsql,"update Prov set \
datp='%04d-%d-%d' \
where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%d-%d' and datp='%04d-%d-%d' and tz=%d",
    rec.gd,rec.md,rec.dd,tipd,0,rec.nomdk_i.ravno(),
    rec.gd,rec.md,rec.dd,rec.gdi,rec.mdi,rec.ddi,rec.tipz);


    sql_zapis(strsql,1,0);
   }
 
 
 }
rec.ddi=rec.dd;
rec.mdi=rec.md;
rec.gdi=rec.gd;
rec.nomdk_i.new_plus(rec.nomdk.ravno());
rec.kodop_i.new_plus(rec.kodop.ravno());

//OSTANOV();

return(0);
}


