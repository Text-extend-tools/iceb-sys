/*$Id: rasdok1.c,v 5.44 2013/11/05 10:50:49 sasa Exp $*/
/*15.07.2015	09.07.2003	Белых А.И.	rasdok1.c
Распечатка накладных с учетной ценой
*/
#include        <errno.h>
#include	"buhg.h"
#define		KOLSTROK 77

void	vztr1(short,short,short,int,const char*,double*,FILE*,int*,int*,int);
void    srasdok1(int,int*,int*,FILE*);
void    shrasdok1(int,int*,int*,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/

void rasdok1(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
short lnds,  //Льготы по НДС*
double ves) //Вес
{
class iceb_tu_str kontr("");
class iceb_tu_str kodop("");
int		tipz=0;
class iceb_tu_str naimpr("");
char		strsql[1024];
class iceb_tu_str bros("");
class iceb_tu_str bros1("");
SQL_str		row,row1,rowtmp;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str nn1(""); /*Номер встречного документа*/
short		mtpr=0; /*Метка первого и второго прохода*/
short		mtar=0; /*Метка наличия тары в накладной*/
class iceb_tu_str kdtr(""); /*Коды тары*/
short		kgrm=0; /*код группы материалла*/
class iceb_tu_str naim("");
int             kodm,nk;
double		kratn,cenan=0.;
short		vtar=0; /*Метка возвратной тары*/
short		mnt=0; /*Метка наличия тары*/
double		mest=0.;
int		i1;
int		kollist=0,kolstrok=0;
class iceb_tu_str ein("");
double		kolih=0.;
double		cenareal=0.; //Цена реализации
double		suma=0.;
double		itogo=0.;
double		itogoreal=0.;
int		kolstr=0;
double		sumt=0.;
int		nomer=0;
class iceb_tu_str artikul("");
class iceb_tu_str kodkontr00("00");
SQLCURSOR cur,cur1,curtmp;

GDITE();

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dg,mg,gg,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  skl);

  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
float pnds=atof(row[13]);
kontr.new_plus(row[3]);
kodop.new_plus(row[6]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
nn1.new_plus(row[11]);
kodkontr00.new_plus(row[15]);
int mvnpp=0;
/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Prihod");
if(tipz == 2)
 bros.new_plus("Rashod");

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros.ravno(),kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  move(20,0);
  beep();
  printw(gettext("Не найден код операции %s !"),kodop.ravno());
  printw("\n");
  OSTANOV();
 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }

int kor=0;
iceb_t_poldan("Корректор межстрочного растояния",&kor,"matnast.alx");
//kor=(short)ATOFM(bros);

int ots=0;
iceb_t_poldan("Отступ от левого края",&ots,"matnast.alx");
//ots=(short)ATOFM(bros);

iceb_t_poldan("Код тары",&kdtr,"matnast.alx");

if(tipz == 2)
 {
  if(SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_t_getk00(0,kodop.ravno()));
 }

//sortnak(dg,mg,gg,skl,nomdok);
class iceb_t_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dg,mg,gg,skl,nomdok,imatmptab,&tabtmp,NULL) != 0)
 return;
sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return;
 }


char imafdok[64];
FILE *ffdok;


sprintf(imafdok,"nsuc%d.lst",getpid());
if((ffdok = fopen(imafdok,"w")) == NULL)
 {
  error_op_nfil(imafdok,errno,"");
  return;
 }

rashap(tipz,&kolstrok,kodkontr00.ravno(),kontr.ravno(),ots,kor,ffdok); //печать реквизитов контагентов

class iceb_tu_str naiskl("");
class iceb_tu_str fmoll("");

/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  move(20,0);
  beep();
  printw(gettext("Не найден склад %d в списке складов !"),skl);
  printw("\n");
  OSTANOV();
 }
else
 {
  naiskl.new_plus(row[0]);
  fmoll.new_plus(row[1]);
 }
fprintf(ffdok,"\n%s\n",naiskl.ravno());
kolstrok+=2;

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(ATOFM(kontr.ravno()) == 0. && polen(kontr.ravno(),&bros,2,'-') == 0 && kontr.ravno()[0] == '0')
   {
    int skl1=0;
    polen(kontr.ravno(),&skl1,2,'-');
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl1);
    class iceb_tu_str fmol("");
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"%s-%s %d %s!",__FUNCTION__,gettext("Не найден склад"),skl1,gettext("в списке складов"));
      iceb_t_soob(strsql);
     }
    else
     {
      naiskl.new_plus(row[0]);
      fmol.new_plus(row[1]);
     }
    if(tipz == 1)
       fprintf(ffdok,"\n\%s:\n%s %d <= %s %d %s\n\%s <= %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno(),fmoll.ravno(),fmol.ravno());
    if(tipz == 2)
       fprintf(ffdok,"\n\%s:\n%s %d => %s %d %s\n\%s => %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno(),fmoll.ravno(),fmol.ravno());
    kolstrok+=3;

   }
  else
   {
    fprintf(ffdok,"\n%s\n",naimpr.ravno());
    kolstrok+=2;
   }
 }
else
 { 
  if(iceb_t_poldan("Наименование операции в накладной",&bros,"matnast.alx") == 0)
   if(SRAV(bros.ravno(),"Вкл",1) == 0)
    {
     fprintf(ffdok,"\n%s\n",naimpr.ravno());
     kolstrok+=2;
    }   
 }

//sprintf(bros,"N%s",nomdok);  
bros.new_plus("N");
bros.plus(nomdok);
if(tipz == 2 && iceb_t_poldan("Перенос символа N",&bros,"matnast.alx") == 0)
 {
  if(SRAV(bros.ravno(),"Вкл",1) == 0)
   {
    polen(nomdok,&bros,1,'-');
    polen(nomdok,&bros1,2,'-');
    bros.plus("-N");  
    bros.plus(bros1.ravno());  
    
   }       
  else
   {
//    sprintf(bros,"N%s",nomdok);  
    bros.new_plus("N");
    bros.plus(nomdok);
   }
 } 

if(tipz == 1)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros.ravno(),
  gettext("от"),dg,mg,gg,gettext("г."));

if(tipz == 2)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros.ravno(),
  gettext("от"),dg,mg,gg,gettext("г."));
kolstrok+=2;

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,"%s N",gettext("Доверенность"));
  polen(row[0],&bros,1,'#');
  fprintf(ffdok," %s",bros.ravno());
  polen(row[0],&bros,2,'#');
  fprintf(ffdok," %s %s\n",gettext("от"),bros.ravno());
  kolstrok++;
 }

/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,"%s: %s\n",gettext("Через кого"),row[0]);
  kolstrok++;
 }

if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    fprintf(ffdok,"%s: %s\n",gettext("Основание"),row[0]);
    kolstrok++;
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    short d,m,g;
    rsdat(&d,&m,&g,row[0],0);
    if(d == 0 || SRAV1(g,m,d,gg,mg,dg) <= 0)
     {
      fprintf(ffdok,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
      gettext("Конечная дата расчёта"),row[0]);
      kolstrok++;
     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn.ravno()[0] != '\0')
   {
    fprintf(ffdok,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn.ravno());
    kolstrok++;
   }    
 }

if(nn1.ravno()[0] != '\0' && tipz == 1)
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1.ravno());
  kolstrok++;
 }
if(nn1.ravno()[0] != '\0' && tipz == 2 && mvnpp > 0 )
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1.ravno());
  kolstrok++;
 }

fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/
//fprintf(ffdok,"\x0F");  /*Ужатый режим*/

srasdok1(tipz,&kollist,&kolstrok,ffdok);

naz:;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  kodm=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  cenan=atof(rowtmp[4]);
  cenan=okrug(cenan,okrcn);
  kratn=atoi(rowtmp[5]);
  ein.new_plus(rowtmp[6]);
  vtar=atoi(rowtmp[7]);
  kolih=atof(rowtmp[9]);
  artikul.new_plus(rowtmp[11]);

  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  mest=0;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  bros.new_plus(kgrm);
  if(kdtr.ravno()[0] != '\0' && mtpr == 0 )
    if(pole1(kdtr.ravno(),bros.ravno(),',',0,&i1) == 0 || SRAV(kdtr.ravno(),bros.ravno(),0) == 0)
     {  
      mtar++;
      continue;
     }


  if(kdtr.ravno()[0] != '\0' && mtpr == 1)
    if(pole1(kdtr.ravno(),bros.ravno(),',',0,&i1) != 0)
     if(SRAV(kdtr.ravno(),bros.ravno(),0) != 0)
       continue;
  mest=0.;
  if(kratn != 0.)
   {
    mest=kolih/kratn;
    mest=okrug(mest,0.1);
   }

  suma=cenan*kolih;
  suma=okrug(suma,okrg1);

  itogo+=suma;
  

  cenareal=0.;

  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    if(mvnpp == 1) /*Внутреннее перемещение*/
     {
      //Читаем учетную цену в карточке
      sprintf(strsql,"select cenap from Kart where sklad=%d and nomk=%d",skl,nk);
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
         cenareal=atof(row[0]);    
     }
    else
     {
      sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
       cenareal=atof(row[0]);    
     }
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena,cenap from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     {
      if(mvnpp == 1) /*Внутреннее перемещение*/
       cenareal=atof(row[1]);    
      else
       cenareal=atof(row[0]);    
     }    
   }
  double suma1=cenareal*kolih;  
  suma1=okrug(suma1,okrg1);

  itogoreal+=suma1;

  if(artikul.ravno()[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul.ravno());
    naim.plus(strsql);
   }

  shrasdok1(tipz,&kollist,&kolstrok,ffdok);

  fprintf(ffdok,"%2d %5d %-*.*s %-*.*s %8s %10.10g",
  ++nomer,kodm,
  iceb_tu_kolbait(25,naim.ravno()),iceb_tu_kolbait(25,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(4,ein.ravno()),iceb_tu_kolbait(4,ein.ravno()),ein.ravno(),
  prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %8.2f",
  prcn(suma),cenareal);

  rasdokkr(kolih,mest,kratn,ffdok);
  if(iceb_tu_strlen(naim.ravno()) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_tu_adrsimv(25,naim.ravno()));
   }  

 }
 
mtpr++;
/*
printw("mtpr=%d kdtr.ravno()=%s mtar=%d\n",mtpr,kdtr.ravno(),mtar);
OSTANOV();
*/
if(mtpr == 1 && kdtr.ravno()[0] != '\0' && mtar != 0) /*Распечатываем отдельно тару*/
 {
  kolstrok++;

  fprintf(ffdok,"\
- - - - - - - - - - - - - - - - - - - %s - -- - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

//  rewind(fftmp);
  curtmp.poz_cursor(0);
  goto naz;
 }


/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gg,mg,dg,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

class iceb_tu_str naiusl("");
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cenan=atof(row[5]);
  cenan=okrug(cenan,okrcn);
  
  if(kolih > 0)
    suma=cenan*kolih;
  else
    suma=cenan;
  
  suma=okrug(suma,okrg1);
  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naiusl.new_plus(row1[0]);
   }  
  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);  
  else
   {
    naiusl.plus(" ",row[7]);
   }

  shrasdok1(tipz,&kollist,&kolstrok,ffdok);

  fprintf(ffdok,"%2d %5s %-*.*s %-*.*s %8s %10.10g %8s\n",
  ++nomer," ",
  iceb_tu_kolbait(25,naiusl.ravno()),iceb_tu_kolbait(25,naiusl.ravno()),naiusl.ravno(),
  iceb_tu_kolbait(4,row[3]),iceb_tu_kolbait(4,row[3]),row[3],
  prcn(cenan),kolih,prcn(suma));
  if(iceb_tu_strlen(naiusl.ravno()) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_tu_adrsimv(25,naiusl.ravno()));
   }  

  itogo+=suma;

 }
 
fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");
fprintf(ffdok,"\
%*s: %10s",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prcn(itogo));
fprintf(ffdok,"(%8s)\n",prcn(itogoreal));

double itogo1=itogo;

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  short           nnds=0; //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа
  int dlinna=58;
  itogo1=dok_end_mu(tipz,dg,mg,gg,nomdok,skl,dlinna,itogo,pnds,lnds,mnt,nnds,ffdok);

 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr1(dg,mg,gg,skl,nomdok,&sumt,ffdok,&kollist,&kolstrok,tipz);
  itogo1+=sumt;
 }

memset(strsql,'\0',sizeof(strsql));
preobr(itogo1,strsql,0);

fprintf(ffdok,"%s:%s\n",gettext("Сумма прописью"),strsql);


fprintf(ffdok,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

podpis(ffdok);

fclose(ffdok);

class spis_oth oth;
oth.spis_imaf.plus(imafdok);
oth.spis_naim.plus(gettext("Распечатка накладных с ценами учета/реализации"));
iceb_t_ustpeh(imafdok,1);
iceb_t_rabfil(&oth,"");

}
/*******************************************/
/* Шапка документа                         */
/*******************************************/

void    srasdok1(int tipz,int *kollist,int *kolstrok,FILE *ffdok)
{

*kollist+=1;
*kolstrok+=5;

fprintf(ffdok,"%75s%s%d\n","",gettext("Лист N"),*kollist);

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

if(tipz == 1)
 {
  fprintf(ffdok,gettext("\
N |Код  |  Наименование товара,   |Ед. |Цена пр-|Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|       материалла        |изм.|бретения|          |          |реализац|м-ст|ность|\n"));
 }

if(tipz == 2)
 {
  fprintf(ffdok,gettext("\
N |Код  |  Наименование товара,   |Ед. |  Цена  |Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|       материалла        |изм.|реализац|          |          |учетная |м-ст|ность|\n"));
 }

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

}
/****************************/
/*Счетчик                   */
/****************************/
void   shrasdok1(int tipz,int *kollist,int *kolstrok,FILE *ffdok)
{
*kolstrok+=1;
if(*kolstrok <= KOLSTROK)
 return;

fprintf(ffdok,"\f");
*kolstrok=0;
srasdok1(tipz,kollist,kolstrok,ffdok);
*kolstrok+=1; //Плюс следующая за шапкой строка

}
/*****************/
/*Распечатка тары*/
/*****************/

void		vztr1(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *ffdok,int *kollist,int *kolstrok,int tipz)
{
double		itogo;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cenan;
int		nk,kgrm;
class iceb_tu_str naim("");
short		vtr;
int		i;
double		cenareal=0.;
int		kodm=0;
double		suma=0.;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 
SQLCURSOR cur1;
itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],
  row[10]);
*/
  
  /*Читаем наименование материалла*/
  naim.new_plus("");
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     beep();
     printw("%s-Не нашли наименование материалла по коду %s\n",__FUNCTION__,row[4]);
     OSTANOV();
    }
  else
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }
  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kodm=atoi(row[4]);
  kolih=atof(row[5]);
  cenan=atof(row[6]);
  cenan=okrug(cenan,okrcn);

/*
  printw("mk2.kodm=%d %.2f vt=%d %d\n",mk2.kodm,mk2.nds,vt,mk8.kgrm);
  refresh();  
*/

  suma=cenan*kolih;
  suma=okrug(suma,okrg1);
  i++;
  if(i == 1)
   {   
    fprintf(ffdok,gettext("Оборотная тара:\n"));
   }

  cenareal=0.;
  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
    
   }

  fprintf(ffdok,"%8d %-*.*s %-*s %10s %10.10g",
  kodm,
  iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(4,row[7]),row[7],
  prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %10.2f\n",
  prcn(suma),cenareal);
  if(iceb_tu_strlen(naim.ravno()) > 40)
   {
    fprintf(ffdok,"%8s %s\n"," ",iceb_tu_adrsimv(40,naim.ravno()));
   }  


  itogo+=suma;

 }
*sumt=itogo;
}
