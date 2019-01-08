/* $Id: copdoku.c,v 5.29 2014/07/31 07:09:15 sasa Exp $ */
/*13.07.2015    29.05.1997      Белых А.И.      copdoku.c
Получение новой накладной на основание списка ранее полученных
накладных
*/
#include        <math.h>
#include        <errno.h>
#include	"buhg.h"

extern double	nds1;
extern double   okrg1;

void copdoku(short dnk,short mnk,short gnk,const char *nomdok,
int podr,short tz)
{
static class iceb_tu_str dat1(""),dat2("");
short           dn,mn,gn,dk,mk,gk;
short		d,m,g;
static class iceb_tu_str kopz("");
static class iceb_tu_str kodoz("");
static class iceb_tu_str prra("");
static class iceb_tu_str nomdokp("");
class iceb_tu_str nomdok1("");
short           kom;
int             tipzz,tp;
double          kolih,cena,cenaz;   /*Цена*/
SQL_str         row,row1,row2;
int		kolstr,kolstr1,kolstr2;
char		strsql[1024];
int		podr1;
class iceb_tu_str ei("");
short		mud; /*1-удалить документы после копирования 2-нет*/
int		kolsz; /*Количество скопированных записей*/
int		koldok; /*Количество скопированных документов*/
double		sumkor;
int		metka;
int		kodzap;
class iceb_tu_str shetu("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Копировать записи из других документов"));
VV.VVOD_SPISOK_add_ZAG(gettext("По не введенным реквизитам поиск не выполняется"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............"));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции.(-,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(-,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Прих./расход (+/-)....."));
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.(,,*).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(prra.ravno(),2);
VV.VVOD_SPISOK_add_data(nomdokp.ravno(),128);

naz:;


clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("копировать"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
kopz.new_plus(VV.VVOD_SPISOK_return_data(2));
kodoz.new_plus(VV.VVOD_SPISOK_return_data(3));
prra.new_plus(VV.VVOD_SPISOK_return_data(4));
nomdokp.new_plus(VV.VVOD_SPISOK_return_data(5));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
/*
   prosf(strsql);
   clear();
*/
   goto naz;
   
  case FK2:
  case PLU:
    break;

  case FK10:
  case ESC:
   return;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

tipzz=0;
if(prra.ravno()[0] == '+')
  tipzz=1;
if(prra.ravno()[0] == '-')
  tipzz=2;
  
if(POLE(VV.VVOD_SPISOK_return_data(5),strsql,1,'*') == 0)
 {
  nomdokp.new_plus(strsql);
 }  
else
  nomdokp.new_plus(VV.VVOD_SPISOK_return_data(5));



VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить документы после копирования ?"));

mud=danet(&DANET,2,stdscr);
 
clear();
GDITE();

move(0,0);
printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

printw("%d.%d.%d %s %d\n",dnk,mnk,gnk,nomdok,podr);
refresh();

sprintf(strsql,"select tp,datd,podr,kontr,nomd,kodop,sumkor \
from Usldokum where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа !"));
  return;
 }
//printw("nomdokp=%s\n",nomdokp);

koldok=kolsz=kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
/*
  printw("%s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5]);
  refresh();
*/  
  tp=atoi(row[0]);
  if(tipzz != 0 && tipzz != tp)
      continue;

  if(proverka(kodoz.ravno(),row[3],0,0) != 0)
   continue;        

  if(proverka(kopz.ravno(),row[5],0,0) != 0)
   continue;        

  if(proverka(nomdokp.ravno(),row[4],0,0) != 0)
   continue;        

  nomdok1.new_plus(row[4]);
  rsdat(&d,&m,&g,row[1],2);  
  podr1=atoi(row[2]);

  /*Не копировать себя самого*/
  if(podr1 == podr && SRAV(nomdok1.ravno(),nomdok,0) == 0 && gnk == g && 
  mnk == m && dnk == d && tp == tz)
    continue;

  memset(strsql,'\0',sizeof(strsql));
  if(tp == 1)
   strcpy(strsql,"+");
  if(tp == 2)
   strcpy(strsql,"-");

  printw("%s %02d.%02d.%d %-3s %-4s %-5s %s\n",
  strsql,d,m,g,row[6],row[3],row[4],row[2]);
  refresh();
  
  /*Читаем сумму корректировки*/

  sumkor=atof(row[6]);

  if(sumkor != 0)
   {

    sprintf(strsql,"update Usldokum set sumkor=sumkor+%.2f where \
datd='%s' and nomd='%s' and podr=%s and tp=%s",
    sumkor,row[1],row[4],row[2],row[0]);

    sql_zapis(strsql,1,0); 
    
   }  


  sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,shsp from Usldokum1 \
where datd='%s' and  nomd='%s' and podr=%s and tp=%s",
    row[1],row[4],row[2],row[0]);
//  printw("%s\n",strsql);
//  OSTANOV();
  SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    continue;
   }
         
  koldok++;
  while(cur1.read_cursor(&row1) != 0)
   {
    metka=atoi(row1[0]);
    kodzap=atoi(row1[1]);
    kolih=atof(row1[2]);
    cena=atof(row1[3]);
    ei.new_plus(row1[4]);
    shetu.new_plus(row1[5]);
    kolsz++;
       
    /*Проверяем нет ли уже такой записи*/
    sprintf(strsql,"select kolih,cena from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and shetu='%s'",
    gnk,mnk,dnk,podr,nomdok,metka,kodzap,tz,shetu.ravno());
      
    if(sql_readkey(&bd,strsql,&row2,&curr) == 1)
     {
      kolih+=atof(row2[0]);
      cenaz=atof(row2[1]);
/*
      printw("Перезаписываем %d.%d.%d %s %d\n",dnk,mnk,gnk,nomdok,podr);
      refresh();  
*/
      if(fabs(cenaz-cena) > 0.009)
       {
        beep();
        printw("%s %s %s %d.%d.%d, %d. %s %.10g %.10g\n\%s.\n",
        gettext("Документ"),
        nomdok1.ravno(),
        gettext("від"),
        d,m,g,kodzap,
        gettext("Не совпадает цена !"),
        cena,cenaz,
        gettext("Берем большую цену"));
        
        if(cena-cenaz < 0.)
         cena=cenaz;
        OSTANOV();
       }
      
      sprintf(strsql,"update Usldokum1 \
set \
cena=%.10g,\
kolih=%.10g,\
ktoi=%d \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d \
and kodzap=%d and tp=%d",
      cena,kolih,iceb_t_getuid(),gnk,mnk,dnk,podr,nomdok,metka,kodzap,tz);

      sql_zapis(strsql,1,0); 

     }
    else
     {
/*
      printw("Пишем %d.%d.%d %s %d %d\n",dnk,mnk,gnk,nomdok,podr,nk);
      refresh();  
*/
      zapuvdok(tz,dnk,mnk,gnk,nomdok,metka,kodzap,kolih,cena,ei.ravno(),shetu.ravno(),podr,row1[6],"");

     }


   }

  /*Удаление после копирования*/
  if(mud == 1)
   {
//    sprintf(strsql,"%s",iceb_t_imafnsi("uslugbl.alx"));
    if(iceb_t_pblok(m,g,ICEB_PS_UU) != 0)
     {
      beep();
      printw(gettext("Документ %s удалить невозможно. Дата %d.%dг. заблокирована !"),
      nomdok1.ravno(),m,g);
      printw("\n");
      continue;
     }
    if(udprgr(ICEB_MP_USLUGI,d,m,g,nomdok1.ravno(),podr1,tp) != 0)
     continue;
    uduvdok(tp,d,m,g,nomdok1.ravno(),podr1,0,0,0);

   } 


 }

VVOD SOOB(1);

sprintf(strsql,gettext("Количество переписаних документов %d !"),koldok);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,gettext("Количество переписаних записей %d !"),kolsz);
SOOB.VVOD_SPISOK_add_MD(strsql);

soobshw(&SOOB,stdscr,-1,-1,0,1);
}
