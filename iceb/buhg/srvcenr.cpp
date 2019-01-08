/*$Id:$ */
/*22.03.2017	23.02.2001	Белых А.И	srvcenr.c	
Программа определения средневзвешенной цены на материал
Необходимо посмотреть на какие материалы был приход в заказанном
месяце. определить ненулевые остатки по карточкам и суммы остатков
Разделить сумму на количество и получить средневзвешенную цену
Записать новую карточку с общим остатком и списать остатки со всех 
остальных карточек

Для каких материалов был приход в карточки
Определяем все карточки на этот материал с не нулевым остатком
вычисляем средне-взвешеннуюцену

*/

#include        <errno.h>
#include        "buhg.h"

void zapiskart(short dd,short md,short gd,const char *sklad,const char *nomd,const char *nomdo,const char *kodm,double *kolih,double *suma,const char *nds,const char *ei,const char *shbm,const char *mnds,const char *nomkar,int *kolzom);

extern char	*maskmat; /*Массив кодов отмеченных материалов*/
extern double	okrcn;
extern short	vtara; /*Код группы возвратная тара*/

void srvcenr(short mr,short gr,const char *shet,const char *gruppa,const char *kodmat,const char *sklz,
const char *kodopsrvc) //Код операции расчёта средневзвешенной цены
{
class ostatokt ost;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str skladz(""),sklad("");
class iceb_tu_str kodmz(""),kodm("");
class iceb_tu_str cena1("");
short		dr;
class iceb_tu_str nomd(""),nomdo("");
time_t		tmm=0;
double		kolihi=0.;
double  	suma=0.;
class iceb_tu_str ei(""),eiz("");
class iceb_tu_str mnds(""),mndsz("");
class iceb_tu_str nds(""),ndsz("");
class iceb_tu_str shetuz(""),shetu("");
class iceb_tu_str nomkar(""),nomkarz("");
int		kolzom=0; //Количество записей одного материалла
class iceb_tu_str kontr("");
SQLCURSOR cur;

clear();
printw(gettext("Расчёт средневзвешенных цен на материалы"));
printw("\n%d.%d\n",mr,gr);
GDITE();

int prov1=0;
int prov2=0;
//Смотрим нужно ли делать проводки

//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select prov from Prihod where kod='%s'",kodopsrvc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций приходов !"),kodopsrvc);
  iceb_t_soob(strsql);
  return;

 }
prov1=atoi(row[0]);
sprintf(strsql,"select prov from Rashod where kod='%s'",kodopsrvc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций расходов !"),kodopsrvc);
  iceb_t_soob(strsql);
  return;
 }
prov2=atoi(row[0]);

dpm(&dr,&mr,&gr,5);

sprintf(strsql,"select distinct sklad,nomk from Zkart where \
datdp >= '%04d-%d-01' and datdp <= '%04d-%d-%d' and tipz=1 order by \
nomk asc",gr,mr,gr,mr,dr);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не было ни одного прихода !"));
  return;
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"srvcenr"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
shu char(32) not null,\
ei char(32) not null,\
kodm int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return;
 }  
int kolstr1=0; 
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {  
//  printw("***%s %s\n",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(sklz,row[0],0,0) != 0)
   continue;
  sklad.new_plus(row[0]);

    
  ostkar(1,mr,gr,31,mr,gr,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostg[3] <= 0.) //Если нет остатка
     continue;
  if(ost.ostm[1] <= 0.) //Если не было прихода
     continue;

  //Читаем код материалла
  sprintf(strsql,"select shetu,ei,nds,mnds,kodm,innom,nomz from Kart where sklad=%s and nomk=%s",
  row[0],row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printw("Не найдена карточка %s на складе %s !\n",row[1],row[0]);
    OSTANOV();
    continue;
   } 
  if(row1[5][0] != '\0' || row1[6][0] != '\0')
    continue;
  shetu.new_plus(row1[0]);
  ei.new_plus(row1[1]);
  kodm.new_plus(row1[4]);
  if(proverka(shet,shetu.ravno(),0,0) != 0)
    continue;

  if(kodmat[0] != '\0')
   {
   if(kodmat[0] == '*' && maskmat != NULL)
    {
     if(proverka(maskmat,kodm.ravno(),0,0) != 0)
      continue;
    }
   else
    if(proverka(kodmat,kodm.ravno(),0,0) != 0)
      continue;
   }
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",kodm.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден код материалла"),kodm.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  if(vtara == atoi(row1[0]))
    continue;
    
  if(proverka(gruppa,row1[0],0,0) != 0)
     continue;

  sprintf(strsql,"insert into %s values (%s,'%s','%s',%s)",
  imatmptab,
  sklad.ravno(),shetu.ravno(),ei.ravno(),kodm.ravno());
  
 } 
GDITE();
SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
sprintf(strsql,"select * from %s order by skl asc,shu asc,ei asc,kodm asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

class iceb_t_tmptab tabtmp1;
const char *imatmptab1={"srvcenr1"};

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
shu char(32) not null,\
ei char(32) not null,\
kodm int not null,\
nds int not null,\
mnds int not null,\
cena double (16,6) not null,\
ost double (16,6) not null,\
nk int not null) ENGINE = MYISAM",imatmptab1);


if(tabtmp1.create_tab(imatmptab1,zaprostmp) != 0)
 {
  return;
 }  

kolstr1=0;
while(cur.read_cursor(&rowtmp) != 0)
 {  
  strzag(LINES-1,0,kolstrtmp,kolstr1);
  sklad.new_plus(rowtmp[0]);
  shetu.new_plus(rowtmp[1]);
  ei.new_plus(rowtmp[2]);
  kodm.new_plus(rowtmp[3]);
  
  //Ищем все карточки с этим материалом
  
  sprintf(strsql,"select shetu,ei,nds,mnds,kodm,cena,nomk from Kart \
where sklad=%s and kodm=%s and shetu='%s' and ei='%s'",
  sklad.ravno(),kodm.ravno(),shetu.ravno(),ei.ravno());

//  printw("%s\n",strsql);
  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    printw("%s\n",sql_error(&bd));
    OSTANOV();
   }

  if(kolstr == 0)
   {
    continue;
   }

  while(cur.read_cursor(&row) != 0)
   {
    ostkar(1,mr,gr,31,mr,gr,sklad.ravno_atoi(),atoi(row[6]),&ost);

//  printw("%f %f %f %f\n",ost.ostg[0],ost.ostg[1],ost.ostg[1],ost.ostg[1]);
//  printw("%f %f %f %f\n",ost.ostm[0],ost.ostm[1],ost.ostm[1],ost.ostm[1]);


    if(ost.ostm[3] <= 0.) //Если нет остатка
     continue;

    shetu.new_plus(row[0]);
    ei.new_plus(row[1]);
    nds.new_plus(row[2]);
    mnds.new_plus(row[3]);
    kodm.new_plus(row[4]);
    cena1.new_plus(row[5]);
    nomkar.new_plus(row[6]);

  sprintf(strsql,"insert into %s values (%s,'%s','%s',%s,%s,%s,%s,%.10g,%s)",
  imatmptab1,
  sklad.ravno(),shetu.ravno(),ei.ravno(),kodm.ravno(),nds.ravno(),mnds.ravno(),cena1.ravno(),ost.ostm[3],nomkar.ravno());

   }
  
/*************
1 - склад
2 - счёт учета
3 - единица измерения
4 - НДС
5 - 0-цена без НДС 1-цена с НДС
6 - код материалла
7 - цена
8 - количество 
9 - номер карточки
 ****************/
 }
GDITE();

sprintf(strsql,"select * from %s order by skl asc,shu asc,ei asc,kodm asc",imatmptab1);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


kolstr1=0;
while(cur.read_cursor(&rowtmp) != 0)
 {  
  strzag(LINES-1,0,kolstrtmp,kolstr1);
  sklad.new_plus(rowtmp[0]);
  shetu.new_plus(rowtmp[1]);
  ei.new_plus(rowtmp[2]);
  kodm.new_plus(rowtmp[3]);
  nds.new_plus(rowtmp[4]);
  mnds.new_plus(rowtmp[5]);
  double cena=atof(rowtmp[6]);
  double kolih=atof(rowtmp[7]);
  nomkar.new_plus(rowtmp[8]);
  
  if(SRAV(skladz.ravno(),sklad.ravno(),0) != 0)
   {
    if(skladz.ravno()[0] != '\0')
      zapiskart(dr,mr,gr,skladz.ravno(),nomd.ravno(),nomdo.ravno(),kodmz.ravno(),&kolihi,&suma,ndsz.ravno(),eiz.ravno(),shetuz.ravno(),mndsz.ravno(),nomkarz.ravno(),&kolzom);

    kodmz.new_plus("");
    shetuz.new_plus("");
    eiz.new_plus("");
    ndsz.new_plus("");
    mndsz.new_plus("");
    kolzom=0;
    
    //Открываем новый документ
    sprintf(strsql,"%s+%d*",sklad.ravno(),mr);
    nomd.new_plus(strsql);
    sprintf(strsql,"%s-%d*",sklad.ravno(),mr);
    nomdo.new_plus(strsql);
    sprintf(strsql,"00-%s",sklad.ravno());
    kontr.new_plus(strsql);
    printw("Создаем документ %s\n",nomd.ravno());

    //Проверяем есть ли документ на приход
    sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%s and nomd='%s'",
    1,gr,gr,sklad.ravno(),nomd.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomd.ravno());
      iceb_t_soob(strsql);
      return;
     }
    //Проверяем есть ли документ на расход
    sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%s and nomd='%s'",
    2,gr,gr,sklad.ravno(),nomdo.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdo.ravno());
      iceb_t_soob(strsql);
      return;
     }

    sprintf(strsql,"insert into Dokummat \
values (%d,'%04d-%02d-%02d',%s,'%s','%s','%s','%s',%d,%d,%d,%ld,'%s',\
'%04d-%d-%d')",
1,gr,mr,dr,sklad.ravno(),kontr.ravno(),nomd.ravno(),"",kodopsrvc,0,prov1,iceb_t_getuid(),tmm,
nomdo.ravno(),0,0,0);
    if(sql_zap(&bd,strsql) != 0)
     {
      msql_error(&bd,"",strsql);
      return;
     }

    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s')",gr,sklad.ravno(),nomd.ravno(),
    gettext("Расчёт средневзвешенных цен на материалы."));
    sql_zap(&bd,strsql);

    printw("Создаем документ %s\n",nomdo.ravno());

    sprintf(strsql,"insert into Dokummat \
values (%d,'%04d-%02d-%02d',%s,'%s','%s','%s','%s',%d,%d,%d,%ld,'%s',\
'%04d-%d-%d')",
2,gr,mr,dr,sklad.ravno(),kontr.ravno(),nomdo.ravno(),"",kodopsrvc,0,prov2,iceb_t_getuid(),tmm,
nomd.ravno(),0,0,0);
    
    if(sql_zap(&bd,strsql) != 0)
     {
      msql_error(&bd,"",strsql);
      return;
     }
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s')",gr,sklad.ravno(),nomdo.ravno(),
    gettext("Расчёт средневзвешенных цен на материалы."));
    sql_zap(&bd,strsql);
    
    skladz.new_plus(sklad.ravno());
   }
  if(SRAV(kodmz.ravno(),kodm.ravno(),0) != 0)
   {
    if(kodmz.ravno()[0] != '\0')
      zapiskart(dr,mr,gr,skladz.ravno(),nomd.ravno(),nomdo.ravno(),kodmz.ravno(),&kolihi,&suma,ndsz.ravno(),eiz.ravno(),shetuz.ravno(),mndsz.ravno(),nomkarz.ravno(),&kolzom);
    
    kodmz.new_plus(kodm.ravno());
   }

  if(SRAV(shetuz.ravno(),shetu.ravno(),0) != 0)
   {
    if(shetuz.ravno()[0] != '\0')
      zapiskart(dr,mr,gr,skladz.ravno(),nomd.ravno(),nomdo.ravno(),kodmz.ravno(),&kolihi,&suma,ndsz.ravno(),eiz.ravno(),shetuz.ravno(),mndsz.ravno(),nomkarz.ravno(),&kolzom);

    shetuz.new_plus(shetu.ravno());
   }  
  if(SRAV(eiz.ravno(),ei.ravno(),0) != 0)
   {
    if(eiz.ravno()[0] != '\0')
      zapiskart(dr,mr,gr,skladz.ravno(),nomd.ravno(),nomdo.ravno(),kodmz.ravno(),&kolihi,&suma,ndsz.ravno(),eiz.ravno(),shetuz.ravno(),mndsz.ravno(),nomkarz.ravno(),&kolzom);

    eiz.new_plus(ei.ravno());
   }  

//  Клиент попросил не разделять карточки ндс и без ндс
  if(SRAV(ndsz.ravno(),nds.ravno(),0) != 0)
   {

    ndsz.new_plus(nds.ravno());
   }  
  if(SRAV(mndsz.ravno(),mnds.ravno(),0) != 0)
   {

    mndsz.new_plus(mnds.ravno());
   }  

  kolzom++;
  //Записываем в расходный документ
//  printw("Записываем\n");
  zapvdok(dr,mr,gr,sklad.ravno_atoi(),nomkar.ravno_atoi(),kodm.ravno_atoi(),nomdo.ravno(),kolih,cena,ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,2,0,"","",0,"");

  nomkarz.new_plus(nomkar.ravno());
//  OSTANOV();
  
  suma+=cena*kolih;
  kolihi+=kolih;  

 }

zapiskart(dr,mr,gr,skladz.ravno(),nomd.ravno(),nomdo.ravno(),kodmz.ravno(),&kolihi,&suma,ndsz.ravno(),eiz.ravno(),shetuz.ravno(),mndsz.ravno(),nomkar.ravno(),&kolzom);


OSTANOV();
}

/*************************************/
/*Запись в приходный документ        */
/*************************************/

void zapiskart(short dd,short md,short gd,
const char *sklad,const char *nomd,const char *nomdo,const char *kodm,double *kolih,double *suma,
const char *nds,const char *ei,const char *shbm,const char *mnds,const char *nomkar,int *kolzom)
{
int	nomk=0;
char strsql[1024];

//printw("kolih=%f\n",*kolih);
if(*kolih == 0.)
  return;

if(*kolzom == 1)
 {
  //Удаляем запись из расходного документа
  sprintf(strsql,"delete from Dokummat1 where nomd='%s' and sklad=%s \
and kodm=%s and nomkar=%s",nomdo,sklad,kodm,nomkar);
//  printw("%s\n",strsql);
//  OSTANOV();
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

  *kolih=*suma=0.;

  *kolzom=0;
  
  return; 
 }   

*kolzom=0;

//Определяем средневзвешенную цену

double sredcn;

sredcn=(*suma)/(*kolih);
sredcn=okrug(sredcn,okrcn);

//Проверяем может карточка с введенными уникальными реквизитами уже есть на складе

sprintf(strsql,"select nomk from Kart where kodm=%s and sklad=%s \
and shetu='%s' and cena=%.10g and ei='%s' and nds=%s and cenap=%.10g \
and krat=%.10g and mnds=%s and dat_god='0000-00-00'",kodm,sklad,shbm,sredcn,ei,nds,0.,0.,mnds);


//printw("\n%s\n",strsql);
//OSTANOV();

SQL_str row;
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  nomk=atoi(row[0]);
  zapvdok(dd,md,gd,atoi(sklad),nomk,atoi(kodm),nomd,*kolih,sredcn,ei,atof(nds),atoi(mnds),0,1,0,"","",0,"");
 }
else
 {
  nomk=nomkr(atoi(sklad));
  short denv=0,mesv=0,godv=0;
       
  if(zapkar(atoi(sklad),nomk,atoi(kodm),sredcn,ei,shbm,0.,0.,0,atof(nds),atoi(mnds),0.,denv,mesv,godv,"","","",0,0,0,0) == 0)
    zapvdok(dd,md,gd,atoi(sklad),nomk,atoi(kodm),nomd,*kolih,sredcn,ei,atof(nds),atoi(mnds),0,1,0,"","",0,"");
 }
 
*kolih=*suma=0.;

}
