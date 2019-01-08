/* $Id: korsdok.c,v 5.22 2013/08/13 05:49:49 sasa Exp $ */
/*26.11.2014	15.02.1999	Белых А.И.	korsdok.c
Режим чтения для последующей корректировки документа
Если вернули 0 - записали шапку документа
             1 - не записали
*/
#include	"buhg.h"


int korsdok(int tipz,short *dd,short *md,short *gd,
int *skl,class iceb_tu_str *nomdok,const char *nomnn,const char *nomdokp,const char *kpos,int lnds,
class iceb_tu_str *kprr,short dpnn,short mpnn,short gpnn)
{
SQL_str         row;
SQLCURSOR       cur;
int		kolstr;
char		strsql[1024];
class iceb_tu_str dat1("");
int		nz;
short		d,m,g;
double		sumkor;
int		loginrash=0;

//printw("\n%s-Режим корректировки\n",__FUNCTION__);
//refresh();

sumkor=0.;

VVOD VV(0);

sozvdnd(tipz,&VV);

VV.VVOD_SPISOK_zapis_data(0,kpos);
sprintf(strsql,"%d.%d.%d",*dd,*md,*gd);
VV.VVOD_SPISOK_zapis_data(1,strsql);
sprintf(strsql,"%d",*skl);
VV.VVOD_SPISOK_zapis_data(2,strsql);
VV.VVOD_SPISOK_zapis_data(3,nomdok->ravno());
VV.VVOD_SPISOK_zapis_data(7,nomdokp);
if(tipz == 1)
 {
  VV.VVOD_SPISOK_zapis_data(8,nomnn);
  if(lnds == 0)
     {
      float pnds=iceb_t_pnds(*dd,*md,*gd);
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV.VVOD_SPISOK_zapis_data(11,strsql);
     }
  if(lnds == 1)
        VV.VVOD_SPISOK_zapis_data(11,gettext("0% НДС реализация на територии Украины"));
  if(lnds == 2)
        VV.VVOD_SPISOK_zapis_data(11,gettext("0% НДС экспорт"));
  if(lnds == 3)
        VV.VVOD_SPISOK_zapis_data(11,gettext("Освобождение от НДС"));
  if(lnds == 4)
        VV.VVOD_SPISOK_zapis_data(11,gettext("7% НДС на мед. препараты"));
  if(dpnn != 0)
   {
    sprintf(strsql,"%d.%d.%d",dpnn,mpnn,gpnn);
    VV.VVOD_SPISOK_zapis_data(10,strsql);
   }
 }

if(tipz == 2)
 {
  VV.VVOD_SPISOK_zapis_data(11,nomnn);
  if(lnds == 0)
     {
      float pnds=iceb_t_pnds(*dd,*md,*gd);
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV.VVOD_SPISOK_zapis_data(14,strsql);
     }
  if(lnds == 1)
        VV.VVOD_SPISOK_zapis_data(14,gettext("0% НДС реализация на територии Украины"));
  if(lnds == 2)
        VV.VVOD_SPISOK_zapis_data(14,gettext("0% НДС экспорт"));
  if(lnds == 3)
        VV.VVOD_SPISOK_zapis_data(14,gettext("Освобождение от НДС"));
  if(lnds == 4)
        VV.VVOD_SPISOK_zapis_data(14,gettext("7% НДС на мед. препараты"));
  if(dpnn != 0)
   {
    sprintf(strsql,"%d.%d.%d",dpnn,mpnn,gpnn);
    VV.VVOD_SPISOK_zapis_data(15,strsql);

   }
 }

sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d \
and sklad=%d and nomd='%s'",*gd,*skl,nomdok->ravno());
//printw("\n%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

while(cur.read_cursor(&row) != 0)
 {
  
  printw("%s %s\n",row[0],row[1]);
  refresh();
  nz=atoi(row[0]);
  
  if(nz == 1)
   {
    class iceb_tu_str rekv("");
    /*Доверенность*/
    polen(row[1],&rekv,1,'#');
    VV.data_plus(4,rekv.ravno());
    /*Дата выдачи доверенности*/
    polen(row[1],&rekv,2,'#');
    VV.data_plus(5,rekv.ravno());
   }    

  /*Через кого*/
  if(nz == 2)
    VV.VVOD_SPISOK_zapis_data(6,row[1]);

  /*Основание*/
  if(nz == 3)
   {
    if(tipz == 1)
     VV.VVOD_SPISOK_zapis_data(14,row[1]);
    if(tipz == 2)
     VV.VVOD_SPISOK_zapis_data(8,row[1]);
   }

  if(nz == 5)
   loginrash=atoi(row[1]);

  /*Регистрационный номер часного предпринимателя*/
/*
  if(nz == 6)
   VV.VVOD_SPISOK_zapis_data(9,row[1]);
*/
  /*Условие продажи*/
  if(nz == 7)
   VV.VVOD_SPISOK_zapis_data(9,row[1]);

  /*Форма оплаты*/
  if(nz == 8)
   {
    if(tipz == 2)
     VV.VVOD_SPISOK_zapis_data(10,row[1]);
    if(tipz == 1)
     VV.VVOD_SPISOK_zapis_data(13,row[1]);
   }
  /*Дата оплаты*/
  if(nz == 9)
   {
    if(tipz == 1)
      VV.data_plus(12,row[1]);
    if(tipz == 2)
      VV.VVOD_SPISOK_zapis_data(12,row[1]);
   }
  /*Номер машины*/
  if(nz == 10)
   VV.VVOD_SPISOK_zapis_data(13,row[1]);

  /*Сумма корректировки*/
  if(nz == 13)
    sumkor=atof(row[1]);

  /*Дата накладной поставщика*/
  if(nz == 14)
    VV.VVOD_SPISOK_zapis_data(9,row[1]);
  

 }
if(tipz == 2 && VV.VVOD_SPISOK_return_data(10)[0] != '\0')
 {
  /*Читаем форму оплаты*/
  sprintf(strsql,"select kod,naik from Foroplat \
where kod='%s'",VV.VVOD_SPISOK_return_data(10));

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найдена форма оплаты %s !"),VV.VVOD_SPISOK_return_data(10));
    iceb_t_soob(strsql);
   }
  else
   {
    sprintf(strsql,"%s|%s",row[0],row[1]);  
    VV.VVOD_SPISOK_zapis_data(10,strsql);
   }
 }
if(tipz == 1 && VV.VVOD_SPISOK_return_data(13)[0] != '\0')
 {
  /*Читаем форму оплаты*/
  sprintf(strsql,"select kod,naik from Foroplat \
where kod='%s'",VV.VVOD_SPISOK_return_data(13));

  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найдена форма оплаты %s !"),VV.VVOD_SPISOK_return_data(13));
    iceb_t_soob(strsql);
   }
  else
   {
    sprintf(strsql,"%s|%s",row[0],row[1]);  
    VV.VVOD_SPISOK_zapis_data(13,strsql);
   }
 }

if(vdnd(&VV,&dat1,tipz,kprr,1,lnds,sumkor,loginrash,0) == 0)
{
  nomdok->new_plus(VV.VVOD_SPISOK_return_data(3));
  rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
  *dd=d;
  *md=m;
  *gd=g;
  *skl=atoi(VV.VVOD_SPISOK_return_data(2));
  return(0);
 }
return(1);
}
