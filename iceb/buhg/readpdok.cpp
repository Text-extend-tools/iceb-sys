/* $Id: readpdok.c,v 5.28 2013/08/13 05:49:54 sasa Exp $ */
/*15.07.2015	07.05.1998	Белых А.И.	readpdok.c
Чтение реквизитов платежного документа
Вернули 0- если нашли
        1- не найден документ
        2- не найдены свои реквизиты
        3- не найдены чужие реквизиты
        4- не найден комментарий
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;
extern char     *imafkomtmp; //Имя файла куда выгружается комментарий для редактирования

short readpdok(const char *tabl,short god,const char *npp)
{
FILE		*ff;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
char		strsql[1024];
//printw("readpdok-%s %d %s\n",tabl,god,npp);
//OSTANOV();
rec.clear();

if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  rec.tipz=2;
if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
  rec.tipz=1;
/*Читаем платежку*/
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0)
 {
   sprintf(strsql,"select * from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",
    tabl,god,god,npp);    
 }
else
 {
  sprintf(strsql,"select * from %s where nomd='%s'",tabl,npp);    
 }

if(sql_readkey(&bd,strsql,&row,&cur) == 0)
 {
/****************
  sprintf(strsql,gettext("Не найден документ %s %d в таблице %s !"),npp,god,tabl);
  iceb_t_soob(strsql);
*******************/
  return(1);   
 }
rsdat(&rec.dd,&rec.md,&rec.gd,row[0],2);
rec.ddi=rec.dd;
rec.mdi=rec.md;
rec.gdi=rec.gd;

rec.nomdk.new_plus(row[1]);
rec.nomdk_i.new_plus(row[1]);

rec.kodop.new_plus(row[2]);
rec.kodop_i.new_plus(row[2]);

rec.sumd=atof(row[3]);
rec.nds=atof(row[4]);
rec.uslb=atof(row[5]);

rec.shbzu.new_plus(row[6]);
if(row[14][0] != '\0')
 rsdat(&rec.dv,&rec.mv,&rec.gv,row[14],2);
rec.kodnr.new_plus(row[15]);



/*Читаем реквизиты свои*/
polen(row[7],&rec.kodor,1,'#');
polen(row[7],&rec.naior,2,'#');
polen(row[7],&rec.gorod,3,'#');
polen(row[7],&rec.kod,4,'#');
polen(row[7],&rec.naiban,5,'#');
polen(row[7],&rec.mfo,6,'#');
polen(row[7],&rec.nsh,7,'#');

/*Читаем реквизиты чужие*/
polen(row[8],&rec.kodor1,1,'#');
polen(row[8],&rec.naior1,2,'#');
polen(row[8],&rec.gorod1,3,'#');
polen(row[8],&rec.kod1,4,'#');
polen(row[8],&rec.naiban1,5,'#');
polen(row[8],&rec.mfo1,6,'#');
polen(row[8],&rec.nsh1,7,'#');

rec.podt=atoi(row[9]);
rec.prov=atoi(row[10]);

rec.ktoz=atoi(row[12]);
rec.vremz=atol(row[13]);

/*Пишем комментарий*/
//sprintf(imaf,"pl%d.tmp",getpid());
if((ff = fopen(imafkomtmp,"w")) == NULL)
   {
    error_op_nfil(imafkomtmp,errno,"");
    return(1);
   }

sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
/*printw("\n%s",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(4);
 }

short i=0;

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
/*  printw("%s\n",row[0]);*/
  i++;
  fprintf(ff,"%s\n",row[0]);
 }

for(; i < 5 ; i++)
    fprintf(ff,"\n");

fprintf(ff,"\
#----------------------------------------------------------------------------\n\
#%s\n",
gettext("Введите назначение платежа и другую необходимую информацию."));

fclose(ff);

return(0);   

}
