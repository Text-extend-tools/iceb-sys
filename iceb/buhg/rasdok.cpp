/* $Id: rasdok.c,v 5.117 2013/11/24 08:23:03 sasa Exp $ */
/*26.04.2018    15.04.1997      Белых А.И.      rasdok.c
Распечатка накладных, счета, Акта приемки
*/
#define		KLST1 52
#define		KLST2 70
#include        <errno.h>
#include        <math.h>
#include	"buhg.h"

void 		sapnak(short,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
class SPISOK sp_fio_klad; /*выбранные фамилии кладовщиков по складам*/
class masiv_din_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков*/

int rasdok(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
const char *imaf,   // имя файла
short lnds,  //Льготы по НДС*
short vr,  //0-накладная 1-счет 2-акт приемки 3-акт списания 4-акт выполненых работ  */
FILE *f1,double ves) //Вес
{
class iceb_tu_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_tu_str gor(""),gor1(""); /*Адрес*/
class iceb_tu_str pkod(""),pkod1(""); /*Почтовый код*/
class iceb_tu_str nmb(""),nmb1("");  /*Наименование банка*/
class iceb_tu_str mfo(""),mfo1("");  /*МФО*/
class iceb_tu_str nsh(""),nsh1("");  /*Номер счета*/
class iceb_tu_str inn(""),inn1("");  /*Индивидуальный налоговый номер*/
class iceb_tu_str grb(""),grb1("");  /*Город банка*/
class iceb_tu_str npnds(""),npnds1(""); /*Номер плательщика НДС*/
class iceb_tu_str tel(""),tel1("");    /*Номер телефона*/
class iceb_tu_str gor_kontr("");
class iceb_tu_str gor_kontr1("");
class iceb_tu_str regnom(""); /*Регистрационный номер частного предпринимателя*/
char            bros1[1024];
int             i;
class iceb_tu_str nomn("");  /*Номенклатурный номер*/
double          itogo;
double          itogo1;
double          bb=0.,bb1=0.;
short           d,m,g;
short           dd,md,gd;
short           nnds=0; //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа
short		mnt; /*Метка наличия тары*/
double		sumt; /*Сумма по таре*/
class iceb_tu_str kdtr(""); /*Коды тары*/
short		mtpr; /*Метка первого и второго прохода*/
short		mppu; /*Метка первого прохода услуг*/
double		mest;
class iceb_tu_str naimpr("");
int		skl1;
short		kor,ots=0;
short           mvnpp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
int             kodm,kodmz,nk;
double		kol,sum;
double		kolt=0.;
double		kratn,kratz,cenan,cenaz;
class iceb_tu_str ein(""),eiz("");
class iceb_tu_str naim(""),naimz("");
short		klst,klli;
SQL_str         row,row1,rowtmp;
SQLCURSOR cur,cur1,curtmp;
char		strsql[1024];
long		kolstr;
class iceb_tu_str kpos("");
class iceb_tu_str kop(""); /*Код операции*/
int		tipz=0;
class iceb_tu_str naiskl("");
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str nn1(""); /*Номер встречного документа*/
char		bros[1024];
char		str[1024];
short		vtar; /*Метка возвратной тары*/
short		kgrm; /*код группы материалла*/
class iceb_tu_str shu(""); /*счет учета материалла*/
double		kolih,cena;
class iceb_tu_str fmoll(""); //Фамилия материально-ответственного
class iceb_tu_str nomz("");
class iceb_tu_str nomzz("");
class iceb_tu_str mesqc("");
class iceb_tu_str kodkontr00("00");
short m_tara=0; //Не возвратная тара- обычная
double itogo_mest=0.;
mest=mnt=nnds=0;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(readkey(strsql,&row,&cur) != 1)
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
  return(1);
 }
float pnds=atof(row[13]);
dd=dg;
md=mg;
gd=gg;
kpos.new_plus(row[3]);
kop.new_plus(row[6]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
nn1.new_plus(row[11]);

kodkontr00.new_plus(row[15]);

/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Prihod");
if(tipz == 2)
 strcpy(bros,"Rashod");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",bros,kop.ravno());
if(readkey(strsql,&row,&cur) != 1)
 {

  sprintf(strsql,gettext("Не найден код операции %s !"),kop.ravno());
  iceb_t_soob(strsql);

 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }

kor=0;
memset(bros,'\0',sizeof(bros)); 
iceb_t_poldan("Корректор межстрочного растояния",bros,"matnast.alx");
kor=(short)ATOFM(bros);
class iceb_tu_str nadpis("");
iceb_t_poldan("Надпись на расходной накладной",&nadpis,"matnast.alx");
  
ots=0;
memset(bros,'\0',sizeof(bros)); 
iceb_t_poldan("Отступ от левого края",bros,"matnast.alx");
ots=(short)ATOFM(bros);

nnds=0;
memset(bros,'\0',sizeof(bros));
iceb_t_poldan("Код операции торговли",bros,"matnast.alx");
if(proverka(bros,kop.ravno(),0,1) == 0)
 {
  nnds=1;
 }
else
 {
  if(tipz == 2)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_t_poldan("Форма документов с номером заказа",bros,"matnast.alx");
    if(SRAV(bros,"Вкл",1) == 0)
      nnds=2;
   }
 }

iceb_t_poldan("Код тары",&kdtr,"matnast.alx");

if(vr == 0)
  GDITE();

if(tipz == 2)
 {
  if(SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_t_getk00(0,kop.ravno()));
 }

/*Читаем реквизиты организации свои */


sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(readkey(bros,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  if(row[16][0] == '\0')
   {
    if(polen(row[1],&nmo,1,'(') != 0)
     nmo.new_plus(row[1]);
   }
  else
    nmo.new_plus(row[16]);
    
  gor.new_plus(row[3]);
  pkod.new_plus(row[5]);
  nmb.new_plus(row[2]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
  inn.new_plus(row[8]);
  grb.new_plus(row[4]);
  npnds.new_plus(row[9]);
  tel.new_plus(row[10]);
  gor_kontr.new_plus(row[17]);
 } 


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(ATOFM(kpos.ravno()) != 0. || kpos.ravno()[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos.ravno());
  if(readkey(bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos.ravno());
    iceb_t_soob(strsql);
   }
  else
   {
    if(row[16][0] == '\0')
     nmo1.new_plus(row[1]);
    else
     nmo1.new_plus(row[16]);
    
    gor1.new_plus(row[3]);
    pkod1.new_plus(row[5]);
    nmb1.new_plus(row[2]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    inn1.new_plus(row[8]);
    grb1.new_plus(row[4]);
    npnds1.new_plus(row[9]);
    tel1.new_plus(row[10]);
    regnom.new_plus(row[15]);
    gor_kontr1.new_plus(row[17]);    
   }
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dg,mg,gg,skl,nomdok,imatmptab,&tabtmp,NULL) != 0)
 return(1);


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return(1);
 }

int metka_open_fil=0;
if(f1 == NULL)
 {
  
  if((f1 = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }
  metka_open_fil=1;
 }
if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(f1,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
fprintf(f1,"\x12"); /*Отмена ужатого режима*/
fprintf(f1,"\x1B\x4D"); /*12-знаков*/
if(vr == 1)
  fprintf(f1,"\x1B\x45"); /*Включение режима выделенной печати*/


if(vr == 1)
 {
  if(nadpis.ravno()[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis.ravno());
  
  if(tipz == 2)
   fprintf(f1,gettext("\
        Поставщик                             Плательщик\n"));
  if(tipz == 1)
   fprintf(f1,gettext("\
        Плательщик                            Поставщик\n"));

  fprintf(f1,"%-*.*s N%s %-*.*s\n",
  iceb_tu_kolbait(40,nmo.ravno()),iceb_tu_kolbait(40,nmo.ravno()),nmo.ravno(),
  kpos.ravno(),
  iceb_tu_kolbait(40,nmo1.ravno()),iceb_tu_kolbait(40,nmo1.ravno()),nmo1.ravno());
  
  for(int nom=40; nom < iceb_tu_strlen(nmo.ravno()) || nom < iceb_tu_strlen(nmo1.ravno()); nom+=40)  
   {
    if(nom < iceb_tu_strlen(nmo.ravno()))
     {  
      fprintf(f1,"%-*.*s",
      iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo.ravno())),
      iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo.ravno())),
      iceb_tu_adrsimv(nom,nmo.ravno()));

     }
    else
      fprintf(f1,"%40s "," ");
      
    if(nom < iceb_tu_strlen(nmo1.ravno()))
     {
      fprintf(f1,"%-*.*s",
      iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo1.ravno())),
      iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo1.ravno())),
      iceb_tu_adrsimv(nom,nmo1.ravno()));
     }
    fprintf(f1,"\n");
   }   

 }
if(vr == 0 || vr == 2 || vr == 4)
 {

  if(nadpis.ravno()[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis.ravno());
  if(tipz == 1)
   fprintf(f1,gettext("\
    Кому                                 От кого\n"));
  if(tipz == 2)
   fprintf(f1,gettext("\
     От кого                             Кому\n"));

  fprintf(f1,"%-*.*s ",iceb_tu_kolbait(40,nmo.ravno()),iceb_tu_kolbait(40,nmo.ravno()),nmo.ravno());
  fprintf(f1,"\x1B\x45N%s %-.*s\x1B\x46\n",
  kpos.ravno(),
  iceb_tu_kolbait(40,nmo1.ravno()),nmo1.ravno());

  for(int nom=40; nom < iceb_tu_strlen(nmo.ravno()) || nom < iceb_tu_strlen(nmo1.ravno()); nom+=40)
   {
    if(iceb_tu_strlen(nmo.ravno()) > nom)
     fprintf(f1,"%-*.*s ",
     iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo.ravno())),
     iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo.ravno())),
     iceb_tu_adrsimv(nom,nmo.ravno()));
    else
     fprintf(f1,"%40s "," ");
     
    if(iceb_tu_strlen(nmo1.ravno()) > nom)
      fprintf(f1,"\x1B\x45%-.*s\x1B\x46",
      iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,nmo1.ravno())),
      iceb_tu_adrsimv(nom,nmo1.ravno()));

    fprintf(f1,"\n");
   }
 }

if(vr == 3)
 {
  fprintf(f1,"%s\n",nmo.ravno());
 }

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
sprintf(bros,"%s %s",gettext("Адрес"),gor.ravno());
sprintf(bros1,"%s %s",gettext("Адрес"),gor1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %-*.*s\n",
  iceb_tu_kolbait(40,bros),iceb_tu_kolbait(40,bros),bros,
  iceb_tu_kolbait(40,bros1),iceb_tu_kolbait(40,bros1),bros1);

  if(iceb_tu_strlen(bros) > 40 || iceb_tu_strlen(bros1) > 40)
   fprintf(f1,"%-*.*s %-*.*s\n",
   iceb_tu_kolbait(40,iceb_tu_adrsimv(40,bros)),
   iceb_tu_kolbait(40,iceb_tu_adrsimv(40,bros)),
   iceb_tu_adrsimv(40,bros),
   iceb_tu_kolbait(40,iceb_tu_adrsimv(40,bros1)),
   iceb_tu_kolbait(40,iceb_tu_adrsimv(40,bros1)),
   iceb_tu_adrsimv(40,bros1));
  
 }
sprintf(bros,"%s %s",gettext("Код ЕГРПОУ"),pkod.ravno());
sprintf(bros1,"%s %s",gettext("Код ЕГРПОУ"),pkod1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 fprintf(f1,"%-*s %s\n",iceb_tu_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1.ravno(),
gettext("МФО"),mfo1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
  fprintf(f1,"%-*s %s\n",iceb_tu_kolbait(40,bros),bros,bros1);


sprintf(bros,"%s %s",gettext("в"),nmb.ravno());
sprintf(bros1,"%s %s",gettext("в"),nmb1.ravno());

if(grb.getdlinna() > 1)
 {
  sprintf(strsql," %s",grb.ravno());
  strcat(bros,strsql);
 }

if(grb1.getdlinna() > 1)
 {
  sprintf(strsql," %s",grb1.ravno());
  strcat(bros1,strsql);
 }

//sprintf(bros,"%s %s %s %s",gettext("в"),nmb.ravno(),gettext("гор."),grb.ravno());
//sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1.ravno(),gettext("гор."),grb1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %s\n",iceb_tu_kolbait(40,bros),iceb_tu_kolbait(40,bros),bros,bros1);
  if(iceb_tu_strlen(bros) > 40)
   fprintf(f1,"%s\n",iceb_tu_adrsimv(40,bros));  
 }
sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn.ravno());

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
  fprintf(f1,"%-*s %s\n",iceb_tu_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
  fprintf(f1,"%-*s %s\n",iceb_tu_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel.ravno());

sprintf(bros1,"%s %s",gettext("Телефон"),tel1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
  fprintf(f1,"%-*s %s\n",iceb_tu_kolbait(40,bros),bros,bros1);

if(nnds == 2)
  fprintf(f1,"\
──────────────────────────────────────────────────────────────────────────────────\n");

fprintf(f1,"\x1B\x50"); /*10-знаков*/



class SPISOK fio_klad;
class iceb_tu_str fio_klad_vibr("");


/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol,np from Sklad where kod=%d",skl);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl);
  iceb_t_soob(strsql);
 }
else
 {


  naiskl.new_plus(row[0]);
  fmoll.new_plus(row[1]);


  if(row[2][0] != '\0')
    gor_kontr.new_plus(row[2]);

  int nom_skl=0;
  if((nom_skl=sp_kod_sklad.find(skl)) < 0)
   {
    iceb_tu_strspisok(row[1],'/',&fio_klad); /*составляем список фамилий кладовщиков*/

    if(fio_klad.kolih() == 1)
     {
      fio_klad_vibr.new_plus(fio_klad.ravno(0));
     }
    if(fio_klad.kolih() > 1)
     {
      int vozv=0;
      int kk=0;

      if((vozv=dirmasiv(&fio_klad,-1,-1,0,gettext("Выберите фамилию кладовщика"),0,&kk)) >= 0)
        fio_klad_vibr.new_plus(fio_klad.ravno(vozv));
        
     }

    sp_kod_sklad.plus(skl);
    sp_fio_klad.plus(fio_klad_vibr.ravno());
   
   }
  else
    fio_klad_vibr.new_plus(sp_fio_klad.ravno(nom_skl));
 }

if(vr == 0 || vr == 2 || vr == 3 )
  fprintf(f1,"\n%s",naiskl.ravno());

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(ATOFM(kpos.ravno()) == 0. && polen(kpos.ravno(),bros,sizeof(bros),2,'-') == 0 && kpos.ravno()[0] == '0')
   {
    polen(kpos.ravno(),bros,sizeof(bros),2,'-');
    skl1=(int)ATOFM(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl1);
    naiskl.new_plus("");
    class iceb_tu_str fmol("");
    if(readkey(strsql,&row,&cur) != 1)
     {
      move(20,0);
      beep();
      printw(gettext("Не найден склад %d в списке складов !"),skl1);
      printw("\n");
      OSTANOV();
     }
    else
     {
      naiskl.new_plus(row[0]);
      fmol.new_plus(row[1]);
     }
    if(tipz == 1)
     {
      
      sprintf(strsql,"%s: %s %d <= %s %d %s",
      naimpr.ravno(),gettext("Склад"),skl,
      gettext("Склад"),skl1,naiskl.ravno());
      
      fprintf(f1,"\n\%.*s\n",iceb_tu_kolbait(70,strsql),strsql);

      sprintf(strsql,"%22s %s <= %s"," ",fmoll.ravno(),fmol.ravno());
      fprintf(f1,"%.*s\n",iceb_tu_kolbait(70,strsql),strsql);
            
     }

    if(tipz == 2)
     {
      sprintf(strsql,"%s: %s %d => %s %d %s",
      naimpr.ravno(),gettext("Склад"),skl,
      gettext("Склад"),skl1,naiskl.ravno());

      fprintf(f1,"\n\%.*s\n",iceb_tu_kolbait(70,strsql),strsql);

      sprintf(strsql,"%22s %s >= %s"," ",fmoll.ravno(),fmol.ravno());
      fprintf(f1,"%.*s\n",iceb_tu_kolbait(70,strsql),strsql);

     }
   }
  else
    fprintf(f1,"\n%s",naimpr.ravno());
 }
else
 { 
  bros[0]='\0';

  if(iceb_t_poldan("Наименование операции в накладной",bros,"matnast.alx") == 0)
   if(SRAV(bros,"Вкл",1) == 0)
    fprintf(f1,"\n%s",naimpr.ravno());
   
 }

if(nnds != 2)
  fprintf(f1,"\n");

sprintf(bros,"N%s",nomdok);  
if(tipz == 2 && iceb_t_poldan("Перенос символа N",bros,"matnast.alx") == 0)
 {
  if(SRAV(bros,"Вкл",1) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    polen(nomdok,bros,sizeof(bros),1,'-');
    polen(nomdok,bros1,sizeof(bros1),2,'-');
    strcat(bros,"-N");  
    strcat(bros,bros1);  
    
   }       
  else
    sprintf(bros,"N%s",nomdok);  

 } 
mesc(md,1,&mesqc);

if(tipz == 1)
 fprintf(f1,"\n%s\n",gor_kontr1.ravno());
if(tipz == 2) 
 fprintf(f1,"\n%s\n",gor_kontr.ravno());
 
if(vr == 0)
 {
  if(tipz == 1)
    fprintf(f1,"\
                     %s  %s\n",
    gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros);
  if(tipz == 2)
    fprintf(f1,"\
                     %s  %s\n",
    gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros);
 }

if(vr == 1)
 {
  if(nnds == 2)
    fprintf(f1,"\
                 %s %s %s %d %s %d%s\n",
    gettext("С ч ё т"),bros,gettext("от"),
    dd,mesqc.ravno(),gd,gettext("г."));
  else
    fprintf(f1,"\
                     %s %s\n",gettext("С ч ё т"),bros);
  }

if(vr == 2)
  fprintf(f1,"\
                     %s  %s\n",
  gettext("АКТ ПРИЁМКИ"),bros);
if(vr == 3)
  fprintf(f1,"\
                     %s  %s\n",
  gettext("АКТ СПИСАНИЯ"),bros);
if(vr == 4)
  fprintf(f1,"\
                     %s  %s\n",
  gettext("АКТ ВЫПОЛНЕНЫХ РАБОТ"),bros);


if(nnds != 2 || vr != 1)
  fprintf(f1,"\
                      %d %s %d%s\n",dd,mesqc.ravno(),gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(readkey(strsql,&row,&cur) == 1)
 {
  fprintf(f1,"%s N",gettext("Доверенность"));
  polen(row[0],bros,sizeof(bros),1,'#');
  fprintf(f1," %s",bros);
  polen(row[0],bros,sizeof(bros),2,'#');
  fprintf(f1," %s %s",gettext("от"),bros);
 }

fprintf(f1,"\n");


/*Читаем через кого*/
class iceb_tu_str sherez_kogo("");
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(readkey(strsql,&row,&cur) == 1)
 {
  fprintf(f1,"%s: %s\n",gettext("Через кого"),row[0]);
  sherez_kogo.new_plus(row[0]);
 }

if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(readkey(strsql,&row,&cur) == 1)
    fprintf(f1,"%s: %s\n",gettext("Основание"),row[0]);

  if(regnom.ravno()[0] != '\0')
    fprintf(f1,"%s: %s\n",gettext("Регистрационный номер"),regnom.ravno());
  
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(readkey(strsql,&row,&cur) == 1)
   {
    rsdat(&d,&m,&g,row[0],0);
//    if(d == 0 || SRAV1(g,m,d,gd,md,dd) <= 0)
    if(d == 0 || sravmydat(d,m,g,dd,md,gd) >= 0)
     {
      if(vr == 0)
       fprintf(f1,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
       gettext("Конечная дата расчёта"),row[0]);
      if(vr == 1)
       fprintf(f1,"\x1B\x45%s %s\x1B\x45\x1b\x46\n",
       gettext("Счёт действителен до"),row[0]);

     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(readkey(strsql,&row,&cur) == 1)

  if(nomnn.ravno()[0] != '\0')
    fprintf(f1,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn.ravno());
    
 }

if(nn1.ravno()[0] != '\0' && tipz == 1)
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());
if(nn1.ravno()[0] != '\0' && tipz == 2 && mvnpp > 0 )
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());

if(vr == 1)
 {
  fprintf(f1,"\x1B\x46"); /*Выключение режима выделенной печати*/
  fprintf(f1,"\x1B\x47"); /*Включение режима двойного удара*/
 }

if(nnds == 1)
  fprintf(f1,"\x1B\x4D"); //12-знаков
if(nnds == 0)
  fprintf(f1,"\x0F"); //Включение ужатого режима печати

if(nnds == 2)
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/

klst=0;
klli=1;

sapnak(nnds,f1);

itogo=itogo1=0.;
mtpr=0;

naz:;
mnt=0;
i=0;
kol=kolt=sum=0.;
nomn.new_plus("");
kratn=cenan=kratz=cenaz=kodmz=0;

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
  shu.new_plus(rowtmp[8]);
  kolih=atof(rowtmp[9]);
  nomz.new_plus(rowtmp[10]);
   
  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  mest=0;
//  kolt=0.;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  memset(bros,'\0',sizeof(bros));
  sprintf(bros,"%d",kodm);
  if(mtpr == 0)
   if(proverka(kdtr.ravno(),bros,0,1) == 0)
    {  
     m_tara++;
     continue;
    }
  if(mtpr == 1)
   if(proverka(kdtr.ravno(),bros,0,1) != 0)
    continue;
    
  if(obzap == 1)
   {
    if((kodmz != 0 && kodmz != kodm) || (kratz != 0 && kratz != kratn) ||
    (cenaz != 0. && cenaz != cenan) || (eiz.ravno()[0] != '\0' && SRAV(eiz.ravno(),ein.ravno(),0) != 0)\
     || (naimz.ravno()[0] != '\0' && SRAV(naimz.ravno(),naim.ravno(),0) != 0))
     {
      if(nnds == 0)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
        i,
        iceb_tu_kolbait(18,nomn.ravno()),nomn.ravno(),
        iceb_tu_kolbait(46,naimz.ravno()),iceb_tu_kolbait(46,naimz.ravno()),naimz.ravno(),
        iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol,prcn(cenaz));
        
        fprintf(f1," %10s",prcn(sum));
        rasdokkr(kol,kolt,kratz,f1);

        if(iceb_tu_strlen(naimz.ravno()) > 46)
         {
          klst++;
          fprintf(f1,"%3s %15s %s\n"," "," ",iceb_tu_adrsimv(46,naimz.ravno()));
         }
       }
      if(nnds == 1)
       {
        i++;
        klst++;

        fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
        i,
        iceb_tu_kolbait(33,naimz.ravno()),iceb_tu_kolbait(33,naimz.ravno()),naimz.ravno(),
        iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol,bb1,prcn(cenaz));

        fprintf(f1," %8s",prcn(sum));
        rasdokkr(kol,mest,kratn,f1);

        if(iceb_tu_strlen(naimz.ravno()) > 33)
         {
          klst++;
          fprintf(f1,"%3s %s\n"," ",iceb_tu_adrsimv(33,naimz.ravno()));
         }
       }
      if(nnds == 2)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
        i,
        iceb_tu_kolbait(20,nomzz.ravno()),nomzz.ravno(),
        iceb_tu_kolbait(46,naimz.ravno()),iceb_tu_kolbait(46,naimz.ravno()),naimz.ravno(),
        iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol," ",prcn(cenaz));
        
        fprintf(f1," %10s\n",prcn(sum));

        if(iceb_tu_strlen(naimz.ravno()) > 46)
         {
          klst++;
          fprintf(f1,"%3s %20s %s\n"," "," ",iceb_tu_adrsimv(46,naimz.ravno()));
         }
       }
     }
    if(kodmz != kodm || kratz != kratn || cenaz != cenan || 
     SRAV(eiz.ravno(),ein.ravno(),0) != 0 || SRAV(naimz.ravno(),naim.ravno(),0) != 0)
     {
      kol=kolt=sum=0.;
      kodmz=kodm;
      cenaz=cenan;
      kratz=kratn;
      eiz.new_plus(ein.ravno());
      naimz.new_plus(naim.ravno());

     }
   }
  sprintf(strsql,"%d.%s.%d.%d",skl,shu.ravno(),kodm,nk);

  if(obzap == 1 && kol != 0.)
     sprintf(strsql,"%d.%s.%d.***",skl,shu.ravno(),kodm);
  nomn.new_plus(strsql);
  nomzz.new_plus(nomz.ravno());
  bb=cenan*kolih;
  bb=okrug(bb,okrg1);
  bb1=cenan+(cenan*pnds/100.);
  bb1=okrug(bb1,okrg1);

  mest=0;
  if(mtpr == 0 && kratn != 0.)
   {
    mest=kolih/kratn;
    mest=okrug(mest,0.1);
    itogo_mest+=mest;
   }

  if(obzap == 0)
   {
    naimz.new_plus(naim.ravno());
    if(nnds == 1)
     {
      i++;
      klst++;
      fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
      i,
      iceb_tu_kolbait(33,naim.ravno()),iceb_tu_kolbait(33,naim.ravno()),naim.ravno(),
      iceb_tu_kolbait(4,ein.ravno()),iceb_tu_kolbait(4,ein.ravno()),ein.ravno(),
      kolih,bb1,prcn(cenan));

      fprintf(f1," %8s",prcn(bb));
      rasdokkr(kol,mest,kratn,f1);

      if(iceb_tu_strlen(naim.ravno()) > 33)
       {
        klst++;
        fprintf(f1,"%3s %s\n"," ",iceb_tu_adrsimv(33,naim.ravno()));
       }
     }

    if(nnds == 0)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
      i,
      iceb_tu_kolbait(18,nomn.ravno()),nomn.ravno(),
      iceb_tu_kolbait(46,naim.ravno()),iceb_tu_kolbait(46,naim.ravno()),naim.ravno(),
      iceb_tu_kolbait(4,ein.ravno()),iceb_tu_kolbait(4,ein.ravno()),ein.ravno(),
      kolih,prcn(cenan));
      
      fprintf(f1," %10s",prcn(bb));
      rasdokkr(kol,mest,kratn,f1);
      if(iceb_tu_strlen(naim.ravno()) > 46)
       {
        klst++;
        fprintf(f1,"%3s %15s %s\n"," "," ",iceb_tu_adrsimv(46,naim.ravno()));
       }
     }

    if(nnds == 2)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
      i,
      iceb_tu_kolbait(20,nomz.ravno()),nomz.ravno(),
      iceb_tu_kolbait(46,naim.ravno()),iceb_tu_kolbait(46,naim.ravno()),naim.ravno(),
      iceb_tu_kolbait(4,ein.ravno()),iceb_tu_kolbait(4,ein.ravno()),ein.ravno(),
      kolih," ",prcn(cenan));
      fprintf(f1," %10s\n",prcn(bb));
      if(iceb_tu_strlen(naim.ravno()) > 46)
       {
        klst++;
        fprintf(f1,"%3s %20s %s\n"," "," ",iceb_tu_adrsimv(46,naim.ravno()));
       }
     }
   }

  kol+=kolih;
  kolt+=mest;
  sum+=bb;
  itogo+=bb;

  if((klli == 1 && klst >= KLST1 ) || (klli != 1 && klst >= KLST2))
   {
    fprintf(f1,"\f");
    sapnak(nnds,f1);
    klst=0;
    klli++;
   }

 }

if(obzap == 1)
 {
  i++;
  klst++;
  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
    i,
    iceb_tu_kolbait(33,naimz.ravno()),iceb_tu_kolbait(33,naimz.ravno()),naimz.ravno(),
    iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol,bb1,prcn(cenaz));
    
    fprintf(f1," %8s",prcn(sum));
    rasdokkr(kol,mest,kratn,f1);
    if(iceb_tu_strlen(naimz.ravno()) > 33)
     {
      klst++;
      fprintf(f1,"%3s %s\n"," ",iceb_tu_adrsimv(33,naimz.ravno()));
     }
   }        
  if(nnds == 0)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_tu_kolbait(18,nomn.ravno()),nomn.ravno(),
    iceb_tu_kolbait(46,naimz.ravno()),iceb_tu_kolbait(46,naimz.ravno()),naimz.ravno(),
    iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol,prcn(cenaz));
    fprintf(f1," %10s",prcn(sum));
    rasdokkr(kol,kolt,kratz,f1);
    if(iceb_tu_strlen(naimz.ravno()) > 46)
     {
      klst++;
      fprintf(f1,"%3s %15s %s\n"," "," ",iceb_tu_adrsimv(46,naimz.ravno()));
     }
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    i,
    iceb_tu_kolbait(20,nomz.ravno()),nomz.ravno(),
    iceb_tu_kolbait(46,naimz.ravno()),iceb_tu_kolbait(46,naimz.ravno()),naimz.ravno(),
    iceb_tu_kolbait(4,eiz.ravno()),iceb_tu_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol," ",prcn(cenaz));
    
    fprintf(f1," %10s\n",prcn(sum));
    if(iceb_tu_strlen(naimz.ravno()) > 46)
     {
      klst++;
      fprintf(f1,"%3s %20s %s\n"," "," ",iceb_tu_adrsimv(46,naimz.ravno()));
     }
   }
 }

mtpr++;
/*
printw("mtpr=%d kdtr=%s mtar=%d\n",mtpr,kdtr,mtar);
OSTANOV();
*/
if(mtpr == 1 && m_tara != 0) /*Распечатываем отдельно тару*/
 {
  klst++;
  if(nnds == 1)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 0)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 2)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  curtmp.poz_cursor(0);
  goto naz;
 }


/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

mppu=0;/*метка первого прохода услуг*/
class iceb_tu_str naiusl("");

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=okrug(cena,okrcn);
  
  nomn.new_plus(gettext("**Услуги***"));
  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;
  
  bb=okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=okrug(bb1,okrg1);

  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naiusl.new_plus(row1[0]);
   }  
  if(naiusl.getdlinna() <= 1)
   naiusl.new_plus(row[7]);
  else
   {
//    strcat(naiusl," ");
//    strcat(naiusl,row[7]);
    naiusl.plus(" ",row[7]);
   }

  if(nnds == 1)
   {
/*
    if(mppu == 0)
     fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - П о с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
    ++i,
    iceb_tu_kolbait(33,naiusl.ravno()),iceb_tu_kolbait(33,naiusl.ravno()),naiusl.ravno(),
    iceb_tu_kolbait(4,row[3]),iceb_tu_kolbait(4,row[3]),row[3],
    kolih,bb1,prcn(cena));
    fprintf(f1," %8s\n",prcn(bb));
   }
  if(nnds == 0)
   { 
/*
    if(mppu == 0)
      fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - П о с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    ++i,
    iceb_tu_kolbait(18,nomn.ravno()),nomn.ravno(),
    iceb_tu_kolbait(46,naiusl.ravno()),iceb_tu_kolbait(46,naiusl.ravno()),naiusl.ravno(),
    iceb_tu_kolbait(4,row[3]),iceb_tu_kolbait(4,row[3]),row[3],
    kolih,prcn(cena));
    fprintf(f1," %10s\n",prcn(bb));
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    ++i,
    iceb_tu_kolbait(10,nomn.ravno()),nomn.ravno(),
    iceb_tu_kolbait(46,naiusl.ravno()),iceb_tu_kolbait(46,naiusl.ravno()),naiusl.ravno(),
    iceb_tu_kolbait(4,row[3]),iceb_tu_kolbait(4,row[3]),row[3],
    kolih," ",prcn(cena));
    fprintf(f1," %10s\n",prcn(bb));
   }
  
  mppu++;
  
  itogo+=bb;

 }
int dlinna=0; 
if(nnds == 1)
 {
  dlinna=66;
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
  fprintf(f1,"\
%*s: %10s\n",iceb_tu_kolbait(dlinna,gettext("Итого")),gettext("Итого"),prcn(itogo));
  }

if(nnds == 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  if(itogo_mest > 0.)
   sprintf(strsql,"%.f",itogo_mest);
  dlinna=96;
  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10s %-4s\n",iceb_tu_kolbait(dlinna,gettext("Итого")),gettext("Итого"),prcn(itogo),strsql);
 }

if(nnds == 2)
 {
  dlinna=104;
  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\
%*s: %10s\n",iceb_tu_kolbait(dlinna,gettext("Итого")),gettext("Итого"),prcn(itogo));

 }

itogo1=itogo;

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  
  itogo1=dok_end_mu(tipz,dd,md,gd,nomdok,skl,dlinna,itogo,pnds,lnds,mnt,nnds,f1);

 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr(dd,md,gd,skl,nomdok,&sumt,f1,nnds);
  itogo1+=sumt;
 }
memset(str,'\0',sizeof(str));
preobr(itogo1,str,0);


fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str);
if(lnds != 0 && mvnpp == 0 )
 fprintf(f1,"%s\n",gettext("Без НДС"));
 
fprintf(f1,"\x1B\x50"); /*10-знаков*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glavbuh;
iceb_t_fioruk(1,&rukov);
iceb_t_fioruk(2,&glavbuh);

if(vr == 0)
 {
  if(mvnpp == 0)
   {
    iceb_t_insfil("matnakend_dp.alx",f1);
   }

  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/
/***************
  fprintf(f1,"\n\%s%s\n",gettext("\
Директор______________________        Главный  бухгалтер_______________\n\n"),
  gettext("\
Отпустил______________________        Принял__________________________"));
********************/
  if(tipz == 1)
   {
    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_tu_kolbait(20,rukov.famil_inic.ravno()),rukov.famil_inic.ravno(),
    iceb_tu_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
    glavbuh.famil_inic.ravno());

    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_tu_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
    30,"",
    iceb_tu_kolbait(20,gettext("Принял")),gettext("Принял"),
    fio_klad_vibr.ravno());
    
/***************
    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_tu_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
    iceb_tu_kolbait(20,sherez_kogo.ravno()),sherez_kogo.ravno(),
    iceb_tu_kolbait(20,gettext("Принял")),gettext("Принял"),
    fio_klad_vibr.ravno());
****************/
   }    

  if(tipz == 2)
   {
    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_tu_kolbait(20,rukov.famil_inic.ravno()),rukov.famil_inic.ravno(),
    iceb_tu_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
    glavbuh.famil_inic.ravno());

    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_tu_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
    iceb_tu_kolbait(20,fio_klad_vibr.ravno()),fio_klad_vibr.ravno(),
    iceb_tu_kolbait(20,gettext("Принял")),gettext("Принял"),
    sherez_kogo.ravno());
   }    
   
  if(mvnpp == 0)
   {
    iceb_t_insfil("matnakend.alx",f1);
   }
 }
fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/

if(vr == 1)
 {

  fprintf(f1,"\n%*s________________%s\n\n%*s________________%s\n",
  iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
  rukov.famil_inic.ravno(),
  iceb_tu_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
  glavbuh.famil_inic.ravno());
  
  iceb_t_insfil("matshetend.alx",f1);
  fprintf(f1,"\x1B\x48"); /*Выключение режима двойного удара*/
 }


if(vr == 2)
 {
  fprintf(f1,gettext("\n\
Отпустил _____________________        Принял__________________________\n\n\
Комиссия _____________________\n\
         _____________________\n\
         _____________________\n"));
 }
if(vr == 3)
 {
  fprintf(f1,gettext("\n\
Руководитель _____________________\n\n\
    Комиссия _____________________\n\
             _____________________\n\
             _____________________\n"));
 }

if( vr == 4)
 {
  fprintf(f1,"\n\
%*s________________ %*s______________\n\n\
%*s________________ %*s______________\n",
  iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
  iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
  iceb_tu_kolbait(20,gettext("Головний бухгалтер")),gettext("Головний бухгалтер"),
  iceb_tu_kolbait(20,gettext("Головний бухгалтер")),gettext("Головний бухгалтер"));

  if(tipz == 1)
    fprintf(f1,"\n\
%*s________________ %*s______________\n",
    iceb_tu_kolbait(20,gettext("Заказчик")),gettext("Заказчик"),
    iceb_tu_kolbait(20,gettext("Исполнитель")),gettext("Исполнитель"));
  if(tipz == 2)
    fprintf(f1,"\n\
%*s________________ %*s______________\n",
    iceb_tu_kolbait(20,gettext("Исполнитель")),gettext("Исполнитель"),
    iceb_tu_kolbait(20,gettext("Заказчик")),gettext("Заказчик"));
 } 

if(mvnpp == 0 && tipz == 2 && vr != 3 ) /*Внешнее перемещение*/
 {
  if(nalndoh() == 0)
    fprintf(f1,"%s\n",gettext("Предприятие является плательщиком налога на доход на общих основаниях."));
//  fprintf(f1,"\n");
  if(ves != 0)
    fprintf(f1,"%s: %.2f %s\n",
    gettext("Вес"),ves,gettext("Кг."));

  fprintf(f1,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(f1,gettext("Благодарим за сотрудничество !"));
  fprintf(f1,"\x14"); /*Выключение режима удвоенной ширины*/
 }


podpis(f1);

if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/

if(vr == 0)
 {
  clearstr((short)(LINES-3),0);
  printw("%s \"%s\" %s %d",
  gettext("Распечатка накладной выгружена в файл"),
  imaf,
  gettext("Листов"),
  klli);
 }
if(vr == 1)
 {
  clearstr((short)(LINES-2),0);
  printw("%s \"%s\" %s %d",
  gettext("Распечатка счета выгружена в файл"),
  imaf,
  gettext("Листов"),
  klli);
 }

if(metka_open_fil == 1)
 fclose(f1);
 
return(0);
}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,
short nnds) //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа 3-счёт-фактура
{
double		itogo,bb;
char		nomn[512];
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cena;
int		nk,kgrm;
class iceb_tu_str naim("");
class iceb_tu_str shu("");
short		vtr;
int		i;

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
  return;

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
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[4]);
    iceb_t_soob(strsql);
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
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);

/*
  printw("mk2.kodm=%d %.2f vt=%d %d\n",mk2.kodm,mk2.nds,vt,mk8.kgrm);
  refresh();  
*/
  shu.new_plus("**");
  
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%d",
    row[1],nk);
    if(readkey(strsql,&row1,&cur1) == 1)
     { 
      shu.new_plus(row1[0]);
     }
   }

  sprintf(nomn,"%s.%s.%s.%d",row[1],shu.ravno(),row[4],nk);

  bb=cena*kolih;
  bb=okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"%s:\n",gettext("Оборотная тара"));


  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7s %11s",
    i,
    iceb_tu_kolbait(33,naim.ravno()),iceb_tu_kolbait(33,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(4,row[7]),iceb_tu_kolbait(4,row[7]),row[7],
    kolih," ",prcn(cena));
    fprintf(f1," %8s\n",prcn(bb));
   }
  if(nnds == 0)
   { 
  
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_tu_kolbait(18,nomn),nomn,
    iceb_tu_kolbait(46,naim.ravno()),iceb_tu_kolbait(46,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(4,row[7]),iceb_tu_kolbait(4,row[7]),row[7],
    kolih,prcn(cena));
    fprintf(f1," %10s\n",prcn(bb));
   }
  if(nnds == 3) //счёт-фактура
   { 
  
    fprintf(f1," %3d %-*.*s %6.6g %-6s %8.8g %10.2f\n",
    i,
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    kolih,row[7],cena,bb);
    if(iceb_tu_strlen(naim.ravno()) > 30)
       fprintf(f1,"%3s %s\n"," ",iceb_tu_adrsimv(30,naim.ravno()));
   }

  itogo+=bb;

 }
*sumt=itogo;
}
/************************/
/*Выдача шапки накладной*/
/************************/
void sapnak(short nnds,FILE *f1)
{
if(nnds == 1)
 {
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |    Наименование продукции       |Ед. |Колич-|Ц е н а|  Ц е н а  | Сумма  |Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |     (товаров,работ,услуг)       |изм.|ество | с НДС |  без НДС  |без НДС |м-ст|ность|\n"));
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
 }

if(nnds == 0)
 {

  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |  Номенклатурный  |       Наименование продукции                 |Ед. |Количество|  Ц е н а  | С у м м а|Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |       номер      |       (товаров,работ,услуг)                  |изм.|          |           |          |м-ст|ность|\n"));

  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n");
 }

if(nnds == 2)
 {

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
//-----------------------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |   Номер заказа     |       Наименование продукции                 |Ед. |Количество|Факт.|  Ц е н а  | С у м м а|\n"));
  fprintf(f1,gettext("   |                    |       (товаров,работ,услуг)                  |изм.|          |кол. |           |          |\n"));

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
//-----------------------------------------------------------------------------------------------------------\n");
 }
}
