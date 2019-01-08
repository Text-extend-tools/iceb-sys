/*$Id:$ */
/*22.03.2017	05.03.2001	Белых А.И.	doocr.c
Программа дооценки полученных материалов
Если вернули 0- дооценка сделана
             1-нет
*/
#include        <errno.h>
#include        "buhg.h"

extern double	okrcn;
extern short	vtara; /*Код группы возвратная тара*/

int doocr(short dd,short md,short gd,const char *nomdok,int skl,
short dr,short mr,short gr,double sumadoo,const char *osnov,const char *kodmat,const char *kodopdooc)
{
int		*nomkar; //Номера карточек
int		*kodm;  //Коды материалов
double		*kolih; //Количество материалла в документе
double		*kolihok; //Количество материалла в остатке на карточке
double		*suma;  //Сумма на количество остатка
double		sumai=0.;
int		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		nomstr=0;
double		kolihdok=0;
double		kolihost=0;
int		prov1,prov2;
class iceb_tu_str nomd1(""); //Номер приходного документа
class iceb_tu_str nomd2(""); //Номер расходного документа
time_t		tmm;
class iceb_tu_str kontr("");
class iceb_tu_str rnd("");
class iceb_tu_str nomz(""); //номер заказа
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR cur;
SQLCURSOR cur1;

GDITE();
clear();

if(iceb_t_pbpds(mr,gr) != 0)
 {
  sprintf(strsql,"%s %d.%d %s!",gettext("Дата"),mr,gr,gettext("заблокирована"));
  iceb_t_soob(strsql);
  return(1);
 }

//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select prov from Prihod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций приходов !"),kodopdooc);
  iceb_t_soob(strsql);
  return(1);

 }
prov1=atoi(row[0]);
sprintf(strsql,"select prov from Rashod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций расходов !"),kodopdooc);
  iceb_t_soob(strsql);
  return(1);

 }

prov2=atoi(row[0]);

clear();
GDITE();

sprintf(strsql,"select nomkar,kodm,kolih,cena from Dokummat1 \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tipz=1 \
and nomkar != 0 and voztar=0  order by kodm asc",gd,md,dd,skl,nomdok);
//printw("\n%s\n",strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

nomkar=new int[kolstr];
kodm=new int[kolstr];
kolih=new double[kolstr];
kolihok=new double[kolstr];
suma=new double[kolstr];
class ostatokt ost;
sprintf(strsql,"LOCK TABLES Kart READ,Zkart READ,Material READ,icebuser READ");
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kodmat,row[1],0,0) != 0)
    continue;    

  sprintf(strsql,"select kodgr from Material where kodm=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   if(vtara != 0 && vtara == atoi(row1[0]))
     continue;

  nomkar[nomstr]=atoi(row[0]);
  kodm[nomstr]=atoi(row[1]);
  kolihdok+=kolih[nomstr]=atof(row[2]);
  sumai+=suma[nomstr]=kolih[nomstr]*atof(row[3]);
  ostkar(1,1,gt,dt,mt,gt,skl,nomkar[nomstr],&ost);
  kolihost+=kolihok[nomstr]=ost.ostg[3];  
  
  nomstr++;
 }

if(kolihdok - kolihost > 0.009)
 {
  clearstr(LINES-1,0);

  VVOD DANET(1);

  DANET.VVOD_SPISOK_add_MD(gettext("Остаки на карточках не равны количеству в документе !"));

  sprintf(strsql,"%s:%d.%d.%d %s:%f != %s:%f",
  gettext("Дата"),
  dt,mt,gt,gettext("Документ"),
  kolihdok,gettext("Карточки"),kolihost);

  DANET.VVOD_SPISOK_add_MD(strsql);

  DANET.VVOD_SPISOK_add_MD(gettext("Сделать дооценку на остатки ?"));

  if(danet(&DANET,2,stdscr) == 2)
   {
    sprintf(strsql,"UNLOCK TABLES");
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql);
    return(1);
   }
 }

sprintf(strsql,"LOCK TABLES Dokummat WRITE,Dokummat1 WRITE,\
Dokummat2 WRITE,Kart WRITE,icebuser READ,Nalog READ");
if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql);

//Определяем номера документов
class iceb_tu_str sklad;
sklad.plus(skl);
iceb_t_nomnak(gr,sklad.ravno(),&nomd1,1,0,0);
iceb_t_nomnak(gr,sklad.ravno(),&nomd2,2,0,0);

sprintf(strsql,"00-%d",skl);
kontr.new_plus(strsql);

time(&tmm);
float pnds=iceb_t_pnds(dr,mr,gr);

if(zap_s_mudok(1,dr,mr,gr,skl,kontr.ravno(),nomd1.ravno(),"",kodopdooc,0,prov1,nomd2.ravno(),0,0,0,1,"00",pnds) != 0)
 return(1);
if(zap_s_mudok(2,dr,mr,gr,skl,kontr.ravno(),nomd2.ravno(),"",kodopdooc,0,prov2,nomd1.ravno(),0,0,0,1,"00",pnds) != 0)
 return(1);

sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad.ravno(),nomd1.ravno(),nomdok,osnov);
sql_zap(&bd,strsql);


sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad.ravno(),nomd2.ravno(),nomdok,osnov);
sql_zap(&bd,strsql);

//printw("nomd1=%s nomd2=%s\n",nomd1,nomd2);
//refresh();

double	cena=0.;
double	kof=0.;
kof=(sumai+sumadoo)/sumai;
printw("kof=%f=(%.2f+%.2f)/%.2f\n",kof,sumai,sumadoo,sumai);
class iceb_tu_str shetu("");
class iceb_tu_str ei("");
class iceb_tu_str nds("");
class iceb_tu_str mnds("");
class iceb_tu_str krat("");
int	nomk=0;
double cenakar=0.;
short denv=0,mesv=0,godv=0;
class iceb_tu_str innom("");
short deng,mesg,godg;

//Формируем записи приходного документа
for(int i=0 ; i < kolstr ; i++)
 {
  printw("%4d %4d %8.8g %8.8g %8.8g\n",
  nomkar[i],kodm[i],kolih[i],kolihok[i],suma[i]);
  refresh();
  
  if(kolih[i] == 0. || suma[i] == 0.)
    continue;

  cena=(suma[i]*kof)/kolih[i];
  printw("cena=(%f*%f)/%f=%f\n",suma[i],kof,kolih[i],cena);
  cena=okrug(cena,okrcn);
  
  shetu.new_plus("");  
  ei.new_plus("");
  nds.new_plus("");
  mnds.new_plus("");
  krat.new_plus("");


  //  Читаем реквизиты карточки
  sprintf(strsql,"select shetu,ei,nds,krat,mnds,cena,innom,rnd,nomz,dat_god from \
Kart where sklad=%d and nomk=%d",skl,nomkar[i]);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    printw("Не нашли карточку %d на складе %d !\n",nomkar[i],skl);
    OSTANOV();
    continue;
   }

  shetu.new_plus(row[0]);
  ei.new_plus(row[1]);
  nds.new_plus(row[2]);
  krat.new_plus(row[3]);
  mnds.new_plus(row[4]);
  cenakar=atof(row[5]);
  cenakar=okrug(cenakar,okrcn);
  innom.new_plus(row[6]);
  rnd.new_plus(row[7]);
  nomz.new_plus(row[8]);
  rsdat(&deng,&mesg,&godg,row[9],2);
  
  //Проверяем может карточка с введенными уникальными реквизитами уже есть на складе
  sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d \
  and shetu='%s' and cena=%.10g and ei='%s' and nds=%s and cenap=%.10g \
  and krat=%s and mnds=%s and dat_god='%04d-%02d-%02d'",
  kodm[i],skl,shetu.ravno(),cena,ei.ravno(),nds.ravno(),0.,krat.ravno(),mnds.ravno(),godg,mesg,godg);

  /*
  printw("\n%s\n",strsql);
  OSTANOV();
  */
  if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
   {
    nomk=atoi(row[0]);
    zapvdok(dr,mr,gr,skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
    ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,1,nomkar[i],"",nomz.ravno(),0,"");
   }
  else
   {
    nomk=nomkr(skl);
//    printw("nds=%s shbm=%s\n",nds,shetu.ravno());
//    refresh();
         
    if(zapkar(skl,nomk,kodm[i],cena,ei.ravno(),shetu.ravno(),krat.ravno_atof(),0.,0,
    nds.ravno_atof(),mnds.ravno_atoi(),0.,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),deng,mesg,godg,0) == 0)
      zapvdok(dr,mr,gr,skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
      ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,1,nomkar[i],"",nomz.ravno(),0,"");
   }

  //Формируем запись расходного документа
  zapvdok(dr,mr,gr,skl,nomkar[i],kodm[i],nomd2.ravno(),kolih[i],cenakar,
  ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,2,nomk,"",nomz.ravno(),0,"");

 }

sprintf(strsql,"UNLOCK TABLES");
if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql);

delete [] nomkar;
delete [] kodm;
delete [] kolih;
delete [] kolihok;
delete [] suma;

VVOD SOOB(1);

sprintf(strsql,"%s:%d.%d.%d",gettext("Дата"),dr,mr,gr);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s: %s",gettext("Номер расходного документа"),nomd2.ravno());
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%s: %s",gettext("Номер приходного документа"),nomd1.ravno());
SOOB.VVOD_SPISOK_add_MD(strsql);

soobshw(&SOOB,stdscr,-1,-1,0,1);

return(0);
}
