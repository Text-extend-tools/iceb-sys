/* $Id: rozkor.c,v 5.29 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	22.10.1998	Белых А.И.	rozkor.c
Расчёт корректировки количественных показателей к налоговой
накладной
Расчёт выполняется для возвратных накладных в которых.
Необходимо знать номер накладной на отпуск материалла

*/
#include        <errno.h>
#include        "buhg.h"


void rozkor(const char *nn, //Номер документа
short d,short m,short g,int skl)
{
class iceb_tu_str str("");
short		i;
char		imaf[64];
short		kom;
short		god;
class iceb_tu_str nnn("");
int             kodm; /*Код материалла*/
int             nk;  /*Номер карточки*/
short		di,mi,gi; /*Дата исходной накладной*/
class iceb_tu_str nin(""); /*Номер исходной накладной*/
double		kolih,kolih1;
FILE		*ff;
class iceb_tu_str nai(""),nai1("");
class iceb_tu_str inn(""),inn1("");
class iceb_tu_str adr(""),adr1("");
class iceb_tu_str telef(""),telef1("");
class iceb_tu_str nspdv(""),nspdv1("");
class iceb_tu_str uslpr(""); /*Условие продажи*/
class iceb_tu_str forop(""); /*Форма оплаты*/
class iceb_tu_str nomr(""); /*Номер расчёта*/
class iceb_tu_str fam("");
class iceb_tu_str prish("");
class iceb_tu_str nomdog("");
class iceb_tu_str datdog("");
short		shet; /*Счетчик строк*/
double		bb,bb1;
double		it1,it2;
double		cenaz;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kontr("");
class iceb_tu_str naimat("");
class iceb_tu_str ei("");
int		lnds1=0;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

clear();
/*
printw("rozkor-%d.%d.%d %s %d\n",d,m,g,nn,skl);
refresh();
*/
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG("Расчёт корректировки количественных показателей !");
sprintf(strsql,"Накладная N %s",nn);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_data(12);
VV.VVOD_SPISOK_add_data(12);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(12);
VV.VVOD_SPISOK_add_data(11);
sprintf(strsql,"%d",gt);
VV.VVOD_SPISOK_add_data(strsql,5);

VV.VVOD_SPISOK_add_MD("Номер расчёта................");
VV.VVOD_SPISOK_add_MD("Номер налоговой накладной....");
VV.VVOD_SPISOK_add_MD("Кто выполняет расчёт.........");
VV.VVOD_SPISOK_add_MD("Причина корректировки........");
VV.VVOD_SPISOK_add_MD("Номер договора...............");
VV.VVOD_SPISOK_add_MD("Дата договора.....(д.м.г)....");
VV.VVOD_SPISOK_add_MD("Год налоговой накладной......");

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("расчитать"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

nomr.new_plus(VV.VVOD_SPISOK_return_data(0));
nnn.new_plus(VV.VVOD_SPISOK_return_data(1));
fam.new_plus(VV.VVOD_SPISOK_return_data(2));
prish.new_plus(VV.VVOD_SPISOK_return_data(3));
nomdog.new_plus(VV.VVOD_SPISOK_return_data(4));
datdog.new_plus(VV.VVOD_SPISOK_return_data(5));
god=(short)ATOFM(VV.VVOD_SPISOK_return_data(6));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu2_1_2c.txt");
   clear();
   goto naz;
   
  case FK10 :
   return;

  case FK2 :
  case PLU :
   break;
  default:
   goto naz;
 }       

SQLCURSOR cur;

/*Проверяем есть ли такая накладная*/
sprintf(strsql,"select datd,sklad,nomd,kontr,pn from Dokummat where \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",god,god,nnn.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден документ с номером налоговой накладной %s !",nnn.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }
float pnds=atof(row[4]);

rsdat(&di,&mi,&gi,row[0],2);
if(SRAV1(g,m,d,gi,mi,di) > 0)
 {
  sprintf(strsql,"Дата налоговой накладной %d.%d.%d больше даты возврата %d.%d.%d !",di,mi,gi,d,m,g);
  iceb_t_soob(strsql);
  goto naz;

 }

nin.new_plus(row[2]);
kontr.new_plus(row[3]);


//Узнаем с каким НДС
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and \
nomd='%s' and nomerz=11",god,row[1],row[2]);

if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   lnds1=atoi(row[0]);

/*Читаем условие продажи*/

sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=7",god,nin.ravno(),skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  uslpr.new_plus(row[0]);


/*Читаем форму оплаты*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=8",god,nin.ravno(),skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  forop.new_plus(row[0]);
if(forop.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naik from Foroplat where kod='%s'",forop.ravno());

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    forop.new_plus(row[0]);
  else
    forop.new_plus("");
 }
 
/*Читаем реквизиты организации свои */
sprintf(strsql,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найден код организации 00 !"));
  goto naz;
 }

nai.new_plus(row[0]);
adr.new_plus(row[1]);
inn.new_plus(row[2]);
nspdv.new_plus(row[3]);
telef.new_plus(row[4]);

sprintf(strsql,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='%s'",kontr.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента !"),kontr.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

nai1.new_plus(row[0]);
adr1.new_plus(row[1]);
inn1.new_plus(row[2]);
nspdv1.new_plus(row[3]);
telef1.new_plus(row[4]);

/*Читаем документ*/
sprintf(strsql,"select kodm,nomkar,kolih,ei from Dokummat1 where sklad=%d and nomd='%s' \
and datd='%d-%d-%d'",skl,nn,g,m,d);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }       

SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='dokrozr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

sprintf(imaf,"rz%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
/*Начинаем формирование нового документа*/
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);


cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
sprintf(strsql,"%s / %s",nomr.ravno(),nnn.ravno());
iceb_tu_vstav(&str,strsql,121,144,1);
fprintf(ff,"%s",str.ravno());


cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);

sprintf(strsql,"%d.%d.%d",di,mi,gi);
iceb_tu_vstav(&str,strsql,78,88,1);
iceb_tu_vstav(&str,nnn.ravno(),94,105,1);
iceb_tu_vstav(&str,datdog.ravno(),124,135,1);
iceb_tu_vstav(&str,nomdog.ravno(),140,154,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,nai.ravno(),36,75,1);
iceb_tu_vstav(&str,nai1.ravno(),155,193,1);
fprintf(ff,"%s",str.ravno());

for(i=0; i < 3 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  str.new_plus(row_alx[0]);
  fprintf(ff,"%s",str.ravno());
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,inn.ravno(),13,58,4);

iceb_tu_vstav(&str,inn1.ravno(),139,183,4);
fprintf(ff,"%s",str.ravno());

for(i=0; i < 2 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  str.new_plus(row_alx[0]);
  fprintf(ff,"%s",str.ravno());
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,adr.ravno(),25,64,1);
iceb_tu_vstav(&str,adr1.ravno(),120,159,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,telef.ravno(),17,56,1);
iceb_tu_vstav(&str,telef1.ravno(),150,189,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,nspdv.ravno(),45,64,1);
iceb_tu_vstav(&str,nspdv1.ravno(),138,157,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,uslpr.ravno(),20,59,1);
fprintf(ff,"%s",str.ravno());

for(i=0; i < 2 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,forop.ravno(),30,69,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_tu_vstav(&str,fam.ravno(),40,69,1);
fprintf(ff,"%s",str.ravno());

fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал*/
for(i=0; i < 19 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

clear();
printw("Номер накладной %s Номер налоговой накладной %s\n",
nin.ravno(),nnn.ravno());

fprintf(ff,"\x1B\x2D\x31"); /*Включить подчеркивание*/
SQLCURSOR cur1;
it1=it2=0.;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  
  kodm=atoi(row[0]);
  nk=atoi(row[1]);
  kolih=atof(row[2]);
  ei.new_plus(row[3]);
      
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     beep();
     printw(gettext("Не найдено наименование материалла !"));
     printw("\n");
     OSTANOV();
     naimat.new_plus("");
    }
  else
    naimat.new_plus(row1[0]);


  printw("%4d %4d %-*.*s %.8g\n",
  kodm,nk,
  iceb_tu_kolbait(30,naimat.ravno()),iceb_tu_kolbait(30,naimat.ravno()),naimat.ravno(),kolih);
  refresh();  

  /*Проверяем есть ли материал в исходной накладной и если есть то 
  сколько по количеству*/
  sprintf(strsql,"select kolih,cena from Dokummat1 where \
sklad=%d and nomd='%s' and datd='%d-%d-%d' and kodm=%d and nomkar=%d",
skl,nin.ravno(),gi,mi,di,kodm,nk);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("Нет такого материалла в исходной накладной !\n");
    OSTANOV();
    continue;
   }

  kolih1=atof(row1[0]);
  cenaz=atof(row1[1]);

  if(kolih1-kolih < 0.)
   {
    beep();
    printw("В исходной накладной количество меньше чем в приходной (%.8g) !\n",kolih1);
    OSTANOV();
    continue;
   } 

  bb1=kolih*cenaz*(-1);
  bb1=okrug(bb1,0.01);
  it1+=bb1;
  
  bb=kolih*cenaz*pnds/100.*(-1);
  bb=okrug(bb,0.01);
  it2+=bb;     
  if(lnds1 == 0)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10.2f|%10s|%10s|%11.2f|%11.2f|%11s|%11s|\n",
   d,m,g,
   iceb_tu_kolbait(20,prish.ravno()),iceb_tu_kolbait(20,prish.ravno()),prish.ravno(),
   iceb_tu_kolbait(30,naimat.ravno()),iceb_tu_kolbait(30,naimat.ravno()),naimat.ravno(),
   iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
   kolih*(-1),cenaz," "," ",
   bb1," "," ",bb,bb," "," ");

  if(lnds1 == 3)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10s|%10s|%10.2f|%11s|%11s|%11s|%11s|\n",
   d,m,g,
   iceb_tu_kolbait(20,prish.ravno()),iceb_tu_kolbait(20,prish.ravno()),prish.ravno(),
   iceb_tu_kolbait(30,naimat.ravno()),iceb_tu_kolbait(30,naimat.ravno()),naimat.ravno(),
   iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
   kolih*(-1),cenaz," "," ",
   " "," ",bb1," "," "," "," ");


/*
|    1     |          2         |               3              |  4  |    5     |    6     |    7     |    8     |     9    |    10    |    11    |    12     |     13    |     14    |    15     |
*/     
   
 }
if(lnds1 == 0)
  fprintf(ff,"%*s: %10.2f %21s %11.2f %11.2f\n",
  iceb_tu_kolbait(112,gettext("Итого")),gettext("Итого"),it1," ",it2,it2);

if(lnds1 == 3)
  fprintf(ff,"%*s: %10.2f %11s %11s\n",
  iceb_tu_kolbait(134,gettext("Итого")),gettext("Итого"),it1," "," ");

fprintf(ff,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

shet=1;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  str.new_plus(row_alx[0]);
  if(shet == 3)
    iceb_tu_vstav(&str,fam.ravno(),160,189,1);
  if(shet == 8)
   {
    sprintf(strsql,"%d.%d.%d",d,m,g);
    iceb_tu_vstav(&str,strsql,30,41,1);
    iceb_tu_vstav(&str,nomr.ravno(),48,59,1);
    sprintf(strsql,"%d.%d.%d",di,mi,gi);
    iceb_tu_vstav(&str,strsql,89,101,1);
    iceb_tu_vstav(&str,nnn.ravno(),106,117,1);
   }
  fprintf(ff,"%s",str.ravno());
  shet++;
 }  
  
fclose(ff);

printw("\nРаспечатка выгружена в файл %s\n",imaf);

OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Расчёт корректировки налоговой накладной");
iceb_t_ustpeh(imaf,1);
iceb_t_rabfil(&oth,"");


}
