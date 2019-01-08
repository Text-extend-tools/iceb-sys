/* $Id: dvtmcf2.c,v 5.65 2013/09/26 09:43:32 sasa Exp $ */
/*01.12.2016    04.07.1994      Белых А.И.      dvtmcf2.c
Движение товарно-материальных ценностей в развернутом виде
для сверки поступлений или расходов за определенный период времени
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

class dvtmcf2_rr
 {
  public:
  
  class iceb_tu_str datan;
  class iceb_tu_str datak;
  class iceb_tu_str shet;
  class iceb_tu_str sklad;
  class iceb_tu_str grupa;
  class iceb_tu_str kodmat;
  class iceb_tu_str nds;
  class iceb_tu_str kontr;
  class iceb_tu_str kodop;
  class iceb_tu_str pr; /*0-всё 1-приходы 2-расходы*/
  class iceb_tu_str nomdok;
//  class iceb_tu_str naborop;  
  int metka_sort;  
  int metka_cena; /*0-в ценах документа 1-в учётных ценах*/
    
  
  dvtmcf2_rr()
   {
    clear_data();
   }
   
  void clear_data()
   {
    metka_sort=0;
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa.new_plus("");
    kodmat.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    pr.new_plus("");
    nomdok.new_plus("");
    nds.new_plus("");
    metka_cena=0;
//    naborop.new_plus("");
   }
  
 };


void  dvtmcf2ot(const char *imatmptab,class dvtmcf2_rr *rp,FILE *ff,FILE *ff_l);

void sozshgr(masiv_din_int*,SPISOK*,int*,int*);
void rasgrsh(char *imaf,class dvtmcf2_rr *rp,short dn,short mn,short gn,short dk,short mk,short gk,masiv_din_int *GRUP,SPISOK *SHET,int kolgrup,
int kolshet,class masiv_din_double *sumshgr);

void rasgshshk(class dvtmcf2_rr *rp,
short dn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,
class SPISOK *GSHSHK,
class masiv_din_double *SUMGSHSHK);

void dvtmcf2_sap(int *sl,int *kst,FILE *ff);

extern char	*maskmat; /*Массив кодов отмеченных материалов*/
extern double	okrg1;  /*Округление суммы*/
extern int kol_strok_na_liste; //Количество строк на листе в ориентации портрет
extern int kol_strok_na_liste_l; //Количесво строк на стандартном листе в ориентации ландшафт

int dvtmcf2(class spis_oth *oth)
{
static class dvtmcf2_rr rp;
class iceb_tu_str bros("");
double		cena;
class iceb_tu_str ei("");
int             kom,kom1;
short           dn,mn,gn;
short           dk,mk,gk;
class iceb_tu_str kop("");
char            imaf[64];
int             skl,nk;
int		kgr,kodm;
long		kolstr=0,kolstr1=0;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kor("");
class iceb_tu_str shu("");
class iceb_tu_str naim("");
short		kgrm;
double		nds;
short		nomgrup,nomshet;
class iceb_tu_str shetsp(""); //Счёт списания
int		nomel;

float pnds=iceb_t_pnds();


VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Движение материальных ценностей (форма 2)"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

VV.VVOD_SPISOK_add_data(rp.datan.ravno(),11);//0
VV.VVOD_SPISOK_add_data(rp.datak.ravno(),11);//1
VV.VVOD_SPISOK_add_data(rp.sklad.ravno(),128);//2
VV.VVOD_SPISOK_add_data(rp.kontr.ravno(),128);//3
VV.VVOD_SPISOK_add_data(rp.grupa.ravno(),128);//4
VV.VVOD_SPISOK_add_data(rp.kodop.ravno(),128);//5
VV.VVOD_SPISOK_add_data(rp.kodmat.ravno(),128);//6
VV.VVOD_SPISOK_add_data(rp.pr.ravno(),2);//7
VV.VVOD_SPISOK_add_data(rp.shet.ravno(),128);//9
VV.VVOD_SPISOK_add_data(rp.nomdok.ravno(),128);//10
VV.VVOD_SPISOK_add_data(rp.nds.ravno(),5);//11

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад.........(,,)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Группа........(,,)..."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)..."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код материалла.(,,*)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.(+/-).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета.....(,,).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.(,,)."));//9
sprintf(strsql,gettext("НДС (0,%2.f,+)........."),pnds);//10
VV.VVOD_SPISOK_add_MD(strsql);

naz:;

clear();

move(LINES-5,0);
printw(gettext("\
Если НДС не ввели - все карточки.\n\
Если НДС = 0      - карточки с нулевым НДС.\n\
Если НДС = %2.f     - карточки с %2.f%% НДС.\n\
Если НДС = +      - карточки с НДС в том числе.\n"),pnds,pnds);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

rp.datan.new_plus(VV.data_ravno(0));
rp.datak.new_plus(VV.data_ravno(1));
rp.sklad.new_plus(VV.data_ravno(2));
rp.kontr.new_plus(VV.data_ravno(3));
rp.grupa.new_plus(VV.data_ravno(4));
rp.kodop.new_plus(VV.VVOD_SPISOK_return_data(5));
rp.kodmat.new_plus(VV.data_ravno(6));
rp.pr.new_plus(VV.data_ravno(7));
rp.shet.new_plus(VV.data_ravno(8));
rp.nomdok.new_plus(VV.data_ravno(9));
rp.nds.new_plus(VV.data_ravno(10));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_5.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&rp.kontr,&naims);
        VV.VVOD_SPISOK_zapis_data(3,rp.kontr.ravno());
        goto naz;

      case 2 :
        vibrek("Grup",&rp.grupa,&naims);
        VV.VVOD_SPISOK_zapis_data(4,rp.grupa.ravno());
        goto naz;

      case 3 :
//        vibrek("Prihod",&rp.kodop);
       if(dirprihod(1,&kods,&naims) == 0)
          VV.data_z_plus(5,kods.ravno());
        goto naz;

      case 4 :
        if(dirrashod(1,&rp.kodop,&naims) == 0)
          VV.data_z_plus(5,rp.kodop.ravno());
        goto naz;
        
      case 5 :
        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(6,kodm);
         }
        
        goto naz;

      case 6 :
        vibrek("Plansh",&rp.shet,&naims);
        VV.VVOD_SPISOK_zapis_data(9,rp.shet.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

if(rp.kodmat.ravno()[0] == '*' && maskmat == NULL)
 { 
  iceb_t_soob(gettext("Не сделан список материалов !"));
  goto naz;
 } 

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));

MENU.VVOD_SPISOK_add_MD(gettext("Сортировка:Счёт-склад-контрагент-группа-дата подтверждения"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка:Cклад-контрагент-дата подтверждения"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка:Счёт-cклад-группа"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка:Контрагент-наименование материала"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

rp.metka_sort=0;
while(menu3w(stdscr,&MENU,&rp.metka_sort,-1,-1,0) != 0);

switch (rp.metka_sort)
 {
  case 4 :
  case -1 :
    goto naz;
  
 }

rp.metka_cena=2; //2-по ценам карточек 1-по ценам в документах

if(rp.pr.ravno_pr() == 2 || rp.pr.ravno_pr() == 0)
 {
  VVOD DANET(1);
  DANET.VVOD_SPISOK_add_MD(gettext("В ценах реализации ?"));
  rp.metka_cena=danet(&DANET,2,stdscr);
 }


clear();
GDITE();

sprintf(strsql,"select * from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' order by sklad,nomk,datdp asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

printw("%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("до"),
dk,mk,gk);


class masiv_din_int GRUP(0); //Массив количества групп
class SPISOK SHET(0);        //Список счетов
int	kolgrup=0;
int     kolshet=0;
class SPISOK GSHSHK(0);      //Список группа-счет-счет корреспондент
class masiv_din_double SUMGSHSHK(0); //Суммы к списку  группа-счет-счет корреспондент

//Создаем список счетов и массив групп
sozshgr(&GRUP,&SHET,&kolgrup,&kolshet);

class masiv_din_double sumshgr; //Массив сумм группа-счет
sumshgr.make_class(kolshet*kolgrup);

class iceb_t_tmptab tabtmp;
const char *imatmptab={"dvtmcf2"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
skl int not null,\
kor char(24) not null,\
kgm int not null,\
dp DATE not null,\
naim char(112) not null,\
km int not null,\
ei char(24) not null,\
kop char(24) not null,\
cena double(15,6) not null,\
dv DATE not null,\
kol double(16,6) not null,\
tip int not null,\
nk int not null,\
nomd char(24) not null,\
shs char(24) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  


/*Открываем заранее, чтобы иметь возможность выгрузить ошибки*/
class iceb_fopen fil;
sprintf(imaf,"dv%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return(1);

class iceb_fopen fil_l;
char imaf_l[64];
sprintf(imaf_l,"dv2l%d.lst",getpid());
if(fil_l.start(imaf_l,"w") != 0)
 return(1);

kolstr1=0;
float pnds_dok=0.;
class iceb_tu_str nomdok("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  
  skl=atoi(row[0]);
  nk=atoi(row[1]);
  nomdok.new_plus(row[2]);
  
  if(proverka(rp.sklad.ravno(),row[0],0,0) != 0)
    continue;  

  if(rp.pr.ravno_pr() != 0)
   if(rp.pr.ravno_pr() != atoi(row[5]))
       continue;

  if(proverka(rp.nomdok.ravno(),row[2],0,0) != 0)
    continue;  

  pnds_dok=0.;
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop,pn from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
  row[4],skl,row[2]);
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(__FUNCTION__);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));
    sprintf(strsql,gettext("Дата=%s Документ=%s Склад=%d"),
    row[4],row[2],skl);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    fprintf(fil.ff,gettext("Не найден документ !"));
    fprintf(fil.ff,gettext("Дата=%s Документ=%s Склад=%d"),
    row[4],row[2],skl);
    fprintf(fil.ff,"\n");
    
    fprintf(fil_l.ff,gettext("Не найден документ !"));
    fprintf(fil_l.ff,gettext("Дата=%s Документ=%s Склад=%d"),
    row[4],row[2],skl);
    fprintf(fil_l.ff,"\n");
    
    continue;
   }
  
  kor.new_plus(row1[0]);
  kop.new_plus(row1[1]);
  pnds_dok=atof(row1[2]);
  
  if(proverka(rp.kontr.ravno(),kor.ravno(),0,0) != 0)
    continue;  

  if(proverka(rp.kodop.ravno(),kop.ravno(),0,0) != 0)
    continue;  

  /*Читаем карточку материалла*/
  sprintf(strsql,"select kodm,shetu,ei,cena,mnds,nds from Kart where sklad=%d and nomk=%d",skl,nk);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw(gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
    fprintf(fil.ff,gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
    fprintf(fil_l.ff,gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
    OSTANOV();
    continue;
   }
  nds=atof(row1[5]);

  if(rp.nds.ravno()[0] != '\0')
   {
    //Карточки с нулевым НДС
    if(rp.nds.ravno()[0] == '0' && atof(row1[5]) != 0.)
        continue;
    //Карточки с НДС нужного значения
    if(rp.nds.ravno()[0] != '+' && rp.nds.ravno()[0] != '0' && (fabs(nds - atof(rp.nds.ravno())) > 0.009 || atoi(row1[4]) != 0))
        continue;
    //Карточки в цене которых включено НДС нужного значения
    if(rp.nds.ravno()[0] == '+' && rp.nds.ravno()[1] != '\0' && (fabs(nds - atof(rp.nds.ravno())) > 0.009 || atoi(row1[4]) != 1) )
        continue;
    //Карточки в цене которых НДС любого значения
    if(rp.nds.ravno()[0] == '+' && rp.nds.ravno()[1] == '\0' &&  atoi(row1[4]) != 1 )
        continue;
   }

  kodm=atoi(row1[0]);
  shu.new_plus(row1[1]);
  ei.new_plus(row1[2]);
  if(rp.metka_cena == 2)
    cena=atof(row1[3]);
  else
   {
    cena=0.;
    //Читаем цену в документе
    sprintf(strsql,"select cena from Dokummat1 where datd='%s' and sklad=%d and \
nomd='%s' and nomkar=%d",row[4],skl,nomdok.ravno_filtr(),nk);
    if(readkey(strsql,&row1,&cur1) == 1)
      cena=atof(row1[0]);
    short mnds=0;
    short d,m,g;
    rsdat(&d,&m,&g,row[4],2);

    //читаем метку НДС
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d and nomerz=11",g,nomdok.ravno_filtr(),skl);

    if(readkey(strsql,&row1,&cur1) == 1)
      mnds=atoi(row1[0]);

    if((mnds == 0 || mnds == 4) && pnds_dok != 0. )
     {
      cena=cena+cena*pnds_dok/100.;    
      cena=okrug(cena,okrg1);
     }
   }

  if(rp.kodmat.ravno()[0] != '\0')
   {
    bros.new_plus(kodm); 
    if(rp.kodmat.ravno()[0] == '*' && maskmat != NULL)
     {
      if(proverka(maskmat,bros.ravno(),0,0) != 0)
        continue;  
     }
    else
      if(proverka(rp.kodmat.ravno(),bros.ravno(),0,0) != 0)
        continue;  
   }

  if(proverka(rp.shet.ravno(),shu.ravno(),1,0) != 0)
    continue;  

  /*Узнаем наименование материалла*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",
  kodm);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    fprintf(fil.ff,"%s %d !\n",gettext("Не найден код материалла"),kodm);
    fprintf(fil_l.ff,"%s %d !\n",gettext("Не найден код материалла"),kodm);
    printw("%s %d !\n",gettext("Не найден код материалла"),kodm);
    continue;
   }

  naim.new_plus(row1[0]);
  kgrm=atoi(row1[1]);
  
  if(proverka(rp.grupa.ravno(),row1[1],0,0) != 0)
    continue;  

  shetsp.new_plus("");

  //Читаем дополнение к наименованию материалла
  sprintf(strsql,"select shet,dnaim from Dokummat1 where datd='%s' and sklad=%s \
and nomd='%s' and kodm=%d and nomkar=%s",
  row[4],row[0],row[2],kodm,row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    shetsp.new_plus(row1[0]);
    naim.plus(" ",row1[1]);
   }  
  else
   {
    printw("Не найдена запись !!!\n");
    OSTANOV();
   }


  sprintf(strsql,"insert into %s values ('%s',%d,'%s',%d,'%s','%s',%d,'%s','%s',%.10g,'%s',%.10g,%d,%d,'%s','%s')",
  imatmptab,
  shu.ravno(),skl,kor.ravno(),kgrm,row[3],naim.ravno_filtr(),kodm,ei.ravno(),kop.ravno(),cena,row[4],atof(row[6]),atoi(row[5]),
  nk,row[2],shetsp.ravno());

  sql_zapis(strsql,1,0);    
  
  nomgrup=GRUP.find(kgrm);
  nomshet=SHET.find(shu.ravno());
  sumshgr.plus(cena*atof(row[6]),nomgrup*kolshet+nomshet);
  
  sprintf(strsql,"%d#%s#%s",kgrm,shu.ravno(),shetsp.ravno());
  if((nomel=GSHSHK.find(strsql)) >= 0) //Есть в списке
   {
    SUMGSHSHK.plus(cena*atof(row[6]),nomel);
   }
  else //Нет в списке
   {
    GSHSHK.plus(strsql);
    SUMGSHSHK.plus(cena*atof(row[6]),-1);
   }
 }

GDITE();

dvtmcf2ot(imatmptab,&rp,fil.ff,fil_l.ff);
GDITE();

podpis(fil.ff);
fil.end();
podpis(fil_l.ff);
fil_l.end();


char	imafshgr[64];
sprintf(imafshgr,"grsh%d.lst",getpid());

rasgrsh(imafshgr,&rp,dn,mn,gn,dk,mk,gk,&GRUP,&SHET,kolgrup,kolshet,&sumshgr);

char	imafgshshk[64];

sprintf(imafgshshk,"grshk%d.lst",getpid());

rasgshshk(&rp,dn,mn,gn,dk,mk,gk,imafgshshk,&GSHSHK,&SUMGSHSHK);

sprintf(strsql,"%s %s %s",imaf,imafshgr,imafgshshk);

OSTANOV();

oth->spis_imaf.plus(imaf_l);
oth->spis_naim.plus(gettext("Движение товарно-материальных ценностей"));
iceb_t_ustpeh(oth->spis_imaf.ravno(0),2);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение товарно-материальных ценностей с итогами"));
iceb_t_ustpeh(oth->spis_imaf.ravno(1),1);

oth->spis_imaf.plus(imafshgr);
oth->spis_naim.plus(gettext("Движение материальных ценностей по группам и счетам учёта"));
iceb_t_ustpeh(oth->spis_imaf.ravno(2),3);

oth->spis_imaf.plus(imafgshshk);
oth->spis_naim.plus(gettext("Движение материальных ценностей по группам и счетам списания/получения"));
iceb_t_ustpeh(oth->spis_imaf.ravno(3),3);


return(0);
}
/*********************/
/*вывод реквизитов поиска*/
/*************************/
void dvtmcf2_vrp(class dvtmcf2_rr *rp,int *kolstr,FILE *ff)
{

if(rp->pr.ravno_pr() == 1)
 {
  fprintf(ff,"%s\n",gettext("Только приходные документы"));
  *kolstr+=1;
 }
if(rp->pr.ravno_pr() == 2)
 {
  fprintf(ff,"%s\n",gettext("Только расходные документы"));
  *kolstr+=1;
 }

if(rp->metka_cena == 2)
 {
  fprintf(ff,"%s\n",gettext("Отчёт по учётным ценам"));
  *kolstr+=1;
 }
if(rp->metka_cena == 1)
 {
  fprintf(ff,"%s\n",gettext("Отчёт в ценах реализации"));
  *kolstr+=1;
 }
if(rp->sklad.getdlinna() > 1)
 {
   fprintf(ff,"%s:%s\n",gettext("Склад"),rp->sklad.ravno());
  *kolstr+=1;
 }
if(rp->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),rp->kontr.ravno());
  *kolstr+=1;
 }

if(rp->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),rp->kodop.ravno());
  *kolstr+=1;
 }
if(rp->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),rp->grupa.ravno());
  *kolstr+=1;
 }
if(rp->kodmat.ravno()[0] != 0)
 {
  fprintf(ff,"%s:%s\n",gettext("Материал"),rp->kodmat.ravno());
  *kolstr+=1;
 }
if(rp->shet.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счета учета"),rp->shet.ravno());
  *kolstr+=1;
 }

if(rp->nomdok.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Документ"),rp->nomdok.ravno());
  *kolstr+=1;
 }
if(rp->nds.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),rp->nds.ravno());
  *kolstr+=1;
 }
}
/*******/
/*Шапка*/
/*******/
void gsapp1(int *sl, //Счётчик листов
int *kst, //Счётчик строк
FILE *ff,
int metka_l) //0-портрет 1-ландшафт
{
*kst+=1;

if(metka_l == 0)
 if(*kst <= kol_strok_na_liste)
  return;
if(metka_l == 1)
 if(*kst <= kol_strok_na_liste_l)
  return;

fprintf(ff,"\f");
*kst=1;
 

dvtmcf2_sap(sl,kst,ff);

}

/*******************/
/*Распечатка итогов*/
/********************/

void rasitogo(int metka,//  1-итого по группе
                                   //  2-итого по контр
                                   //  3-итого по склад
                                   //  4-итого по счёту
double mas[2],int *klst,const char *kod,
FILE *ff,short metoth,
int *sli,int metka_l)
{
char		strsql[512];
class iceb_tu_str naim("");
SQL_str         row;
SQLCURSOR       cur;
class iceb_tu_str bros("");

if(mas[0] == 0.)
  return;

if(metoth == 1 && (metka == 1 || metka == 4))
  return;
if(metoth == 2 && metka == 2)
  return;


if(metka == 1)
 {
  sprintf(strsql,"select naik from Grup where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    printw("%s %s !\n",gettext("Не найдена группа"),kod);
    OSTANOV();
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по группе"),kod,naim.ravno());
 }

if(metka == 2)
 {
  if(kod[0] == '0' && polen(kod,&bros,2,'-') == 0)
   {
    sprintf(strsql,"select naik from Sklad where kod=%s",bros.ravno());
   }
  else
   {
    bros.new_plus(kod);
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod);
   }

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден контрагент"),bros.ravno());
    OSTANOV();
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по контрагенту"),kod,naim.ravno());
 }
if(metka == 3)
 {
  sprintf(strsql,"select naik from Sklad where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw(gettext("Не найден склад %d в списке складов !"),atoi(kod));
    printw("\n");
    OSTANOV();
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по складу"),kod,naim.ravno());
 }

if(metka == 4)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw(gettext("Не найден счёт %s в плане счетов !"),kod);
    printw("\n");
    OSTANOV();
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),kod,naim.ravno());
 }

gsapp1(sli,klst,ff,metka_l);
fprintf(ff,"%*s:%12.12g %15.2f\n",iceb_tu_kolbait(79,strsql),strsql,mas[0],mas[1]);

mas[0]=0.;
mas[1]=0.;

}                        

/*******************/
/*Распечатка отчёта*/
/*******************/

void  dvtmcf2ot(const char *imatmptab,class dvtmcf2_rr *rp,FILE *ff,FILE *ff_l)
{
class iceb_tu_str bros("");
int		kodm;
short           dp,mp,gp;
short           d,m,g;
double		its,itk,br;
class iceb_tu_str nomdok("");
class iceb_tu_str poi("");
int		i;
double		itgrup[2];
double		itkontr[2];
double		itsklad[2];
double		itshet[2];
short           mvstr; /*Метка выдачи строки*/
int             sli=0,kst=0;
int             sli_l=0,kst_l=0;
char		strsql[1024];
double		kolih,cena;
class iceb_tu_str shu(""),shuz("");
short		kgrm,kgrmz;
int             skl,skll,nk;
class iceb_tu_str kop("");
class iceb_tu_str kor(""),korzz("");
class iceb_tu_str naim("");
char            nomn[128];
SQL_str         row1;
SQLCURSOR       cur1;
class iceb_tu_str org("");
class iceb_tu_str ei("");
int		kolstr1;
class iceb_tu_str shetsp("");
its=itk=0;


if(rp->metka_sort == 0)/*Сортировка Счёт-склад-контрагент-группа-дата подтверждения*/ 
 {
  printw("\n%s\n",gettext("Сортировка:Счёт-склад-контрагент-группа-дата подтверждения"));
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kor asc,kgm asc,dp asc",imatmptab);
 }

if(rp->metka_sort == 1) /*Сортировка склад-контрагент-дата подтверждения*/ 
 {
  printw("\n%s\n",gettext("Сортировка:Cклад-контрагент-дата подтверждения"));
  sprintf(strsql,"select * from %s order by skl asc,kor asc,dp asc",imatmptab);
 }

if(rp->metka_sort == 2) /*Сортировка:Счёт-cклад-группа*/
 {
  printw("\n%s\n",gettext("Сортировка:Счёт-cклад-группа"));
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kgm asc",imatmptab);
 }

if(rp->metka_sort == 3) /*Сортировка:контрагент-наименование материала*/
 {
  printw("\n%s\n",gettext("Сортировка:Контрагент-наименование материала"));

  sprintf(strsql,"select * from %s order by kor asc,naim asc",imatmptab);
 }

int kolstr;
SQL_str row;
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


iceb_t_zagolov(gettext("Движение товарно-материальных ценностей"),rp->datan.ravno(),rp->datak.ravno(),ff);

iceb_t_zagolov(gettext("Движение товарно-материальных ценностей"),rp->datan.ravno(),rp->datak.ravno(),ff_l);
kst_l=kst=4;

dvtmcf2_vrp(rp,&kst,ff);

dvtmcf2_vrp(rp,&kst_l,ff_l);

//шапка документа
dvtmcf2_sap(&sli,&kst,ff);
dvtmcf2_sap(&sli_l,&kst_l,ff_l);

for(i=0; i < 2 ; i++)
 itgrup[i]=itkontr[i]=itsklad[i]=itshet[i]=0.;
  
kgrmz=skl=0;
mvstr=skll=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,kolstr1++);  

  shu.new_plus(row[0]);
  skl=atoi(row[1]);
  kor.new_plus(row[2]);
  kgrm=atoi(row[3]);
  nk=atoi(row[13]);

  if(kgrmz != kgrm)
   {
    if(kgrmz != 0 && (rp->metka_sort == 0 || rp->metka_sort == 2))
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);
     }
    kgrmz=kgrm;
   }

  if(SRAV(korzz.ravno(),kor.ravno(),0) != 0)
   {
    if(korzz.ravno()[0] != '\0')
     {
      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,rp->metka_sort,&sli_l,1);
     }
    korzz.new_plus(kor.ravno());
   }     

  if(skll != skl)
   {
    if(skll != 0)
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);

      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,rp->metka_sort,&sli_l,1);

      bros.new_plus(skll);
      rasitogo(3,itsklad,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);
                  
     } 
    skll=skl;
   } 
  if(SRAV(shuz.ravno(),shu.ravno(),0) != 0 )
   {
    if(shuz.ravno()[0] != '\0' && (rp->metka_sort == 0 || rp->metka_sort == 2))
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);

      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,rp->metka_sort,&sli_l,1);

      bros.new_plus(skll);
      rasitogo(3,itsklad,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);

      rasitogo(4,itshet,&kst,shuz.ravno(),ff,rp->metka_sort,&sli,0);
      rasitogo(4,itshet,&kst_l,shuz.ravno(),ff_l,rp->metka_sort,&sli_l,1);
     }
    shuz.new_plus(shu.ravno());
   }

  naim.new_plus(row[5]);
  kor.new_plus(row[2]);
  kodm=atoi(row[6]);
  ei.new_plus(row[7]);
  rsdat(&dp,&mp,&gp,row[4],2);
  rsdat(&d,&m,&g,row[10],2);
  kolih=atof(row[11]);  
  shetsp.new_plus(row[15]);
  kop.new_plus(row[8]);

  if(atoi(row[12]) == 1)
     kop.plus("+");
  if(atoi(row[12]) == 2)
     kop.plus("-");

  nomdok.new_plus(row[14]); 


  if(rp->metka_cena == 2) /*по учётным ценам*/
   {
    sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
      fprintf(ff,gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
      fprintf(ff_l,gettext("Не нашли карточку !  Склад %d Номер карточки %d\n"),skl,nk);
      OSTANOV();
      continue;
     }
   cena=atof(row1[0]);
   }
  else /*по ценам в документе*/
   {
    float pnds_dok=0.;
    sprintf(strsql,"select pn from Dokummat where datd='%s' and sklad=%d and nomd='%s'",row[4],skl,nomdok.ravno_filtr());
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(__FUNCTION__);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));
      sprintf(strsql,gettext("Дата=%s Документ=%s Склад=%d"),
      row[4],row[2],skl);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      fprintf(ff,gettext("Не найден документ !"));
      fprintf(ff,gettext("Дата=%s Документ=%s Склад=%d"),
      row[4],row[2],skl);
      fprintf(ff,"\n");
      
      fprintf(ff_l,gettext("Не найден документ !"));
      fprintf(ff_l,gettext("Дата=%s Документ=%s Склад=%d"),
      row[4],row[2],skl);
      fprintf(ff_l,"\n");
      
      continue;
     }
    
    pnds_dok=atof(row1[0]);

    cena=0.;
    //Читаем цену в документе
    sprintf(strsql,"select cena from Dokummat1 where datd='%s' and sklad=%d and \
nomd='%s' and nomkar=%d",row[4],skl,nomdok.ravno_filtr(),nk);
    if(readkey(strsql,&row1,&cur1) == 1)
      cena=atof(row1[0]);
    short mnds=0;
    short d,m,g;
    rsdat(&d,&m,&g,row[4],2);

    //читаем метку НДС
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d and nomerz=11",g,nomdok.ravno_filtr(),skl);

    if(readkey(strsql,&row1,&cur1) == 1)
      mnds=atoi(row1[0]);

    if((mnds == 0 || mnds == 4) && pnds_dok != 0. )
     {
      cena=cena+cena*pnds_dok/100.;    
      cena=okrug(cena,okrg1);
     }
   }



       
  mvstr=0;
  itgrup[0]+=kolih;  
  itkontr[0]+=kolih;
  itsklad[0]+=kolih;
  itshet[0]+=kolih;
  itk+=kolih;

  br=cena*kolih;
  br=okrug(br,0.01);
  itgrup[1]+=br;  
  itkontr[1]+=br;
  itsklad[1]+=br;
  itshet[1]+=br;
  its+=br;
  
  /*Узнаем наименование контрагента*/
  if(kor.ravno()[0] == '0' && polen(kor.ravno(),&bros,2,'-') == 0)
   {
    poi.new_plus(bros.ravno());   
    sprintf(strsql,"select naik from Sklad where kod=%s",poi.ravno());
   }
  else
   {
    poi.new_plus(kor.ravno());   
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    poi.ravno());
   }
  org.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
     fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     fprintf(ff_l,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     printw("%s %s %s !\n",gettext("Не найден код контрагента"),kor.ravno(),poi.ravno());
     printw(gettext("Склад %d Ном. кар. %d Док.%s\n"),
     skl,nk,nomdok.ravno());
     fprintf(ff,gettext("Склад %d Ном. кар. %d Док.%s\n"),skl,nk,nomdok.ravno());
     fprintf(ff_l,gettext("Склад %d Ном. кар. %d Док.%s\n"),skl,nk,nomdok.ravno());
     OSTANOV();
   }
  else
    org.new_plus(row1[0]);

 
  gsapp1(&sli,&kst,ff,0);
  gsapp1(&sli_l,&kst_l,ff_l,1);
  if(mvstr == 0)
   {
    sprintf(nomn,"%d.%s.%d.%d",skl,shu.ravno(),kgrm,nk);
     
    fprintf(ff,"\
%-6d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    kodm,
    iceb_tu_kolbait(35,naim.ravno()),iceb_tu_kolbait(35,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(15,nomn),nomn,
    iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_tu_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());

    fprintf(ff_l,"\
%-6d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    kodm,
    iceb_tu_kolbait(35,naim.ravno()),
    iceb_tu_kolbait(35,naim.ravno()),
    naim.ravno(),
    iceb_tu_kolbait(15,nomn),
    nomn,
    iceb_tu_kolbait(5,ei.ravno()),
    ei.ravno(),
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_tu_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());

    if(iceb_tu_strlen(naim.ravno()) > 37)
     {
      gsapp1(&sli,&kst,ff,0);
      fprintf(ff,"%4s %s\n"," ",iceb_tu_adrsimv(37,naim.ravno()));
      gsapp1(&sli_l,&kst_l,ff_l,1);
      fprintf(ff_l,"%4s %s\n"," ",iceb_tu_adrsimv(37,naim.ravno()));
     }
   }
  else
   {
    fprintf(ff,"\
%6s %-35s %-15s %-5s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    " "," "," "," ",
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_tu_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());
    
    fprintf(ff_l,"\
%6s %-35s %-15s %-5s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    " "," "," "," ",
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_tu_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());
   }  
  
  mvstr++;

 }


bros.new_plus(kgrmz);
rasitogo(1,itgrup,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);

rasitogo(2,itkontr,&kst,korzz.ravno(),ff,rp->metka_sort,&sli,0);
rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,rp->metka_sort,&sli_l,1);

bros.new_plus(skll);
rasitogo(3,itsklad,&kst,bros.ravno(),ff,rp->metka_sort,&sli,0);
rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,rp->metka_sort,&sli_l,1);

rasitogo(4,itshet,&kst,shuz.ravno(),ff,rp->metka_sort,&sli,0);
rasitogo(4,itshet,&kst_l,shuz.ravno(),ff_l,rp->metka_sort,&sli_l,1);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s:%12.12g %15.2f\n",iceb_tu_kolbait(77,gettext("Общий итог")),gettext("Общий итог"),itk,its);

fprintf(ff_l,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%77s:%12.12g %15.2f\n",gettext("Общий итог"),itk,its);


printw("%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_tu_kolbait(10,gettext("количество")),
gettext("количество"),
itk,
iceb_tu_kolbait(10,gettext("сумма")),
gettext("сумма"),
its,
gettext("грн.")); 


}

/*********************************************/
/*Создание массивов счетов и групп материалов*/
/*********************************************/
void	sozshgr(masiv_din_int *GRUP, //Массив количества групп
SPISOK *SHET,        //Список счетов
int *kolgrup, //Количество групп
int *kolshet) //Количество счетов
{
SQL_str		row;
char		strsql[512];


sprintf(strsql,"select kod from Grup");

SQLCURSOR cur;
if((*kolgrup=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  GRUP->plus(atoi(row[0]),-1);
 }

sprintf(strsql,"select distinct shetu from Kart");

if((*kolshet=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  SHET->plus(row[0]);
 }

}
/******************************************/
/*Распечатка массива группа-счет          */
/******************************************/

void rasgrsh(char *imaf,
class dvtmcf2_rr *rp,
short dn,short mn,short gn,
short dk,short mk,short gk,
class masiv_din_int *GRUP, //Массив количества групп
class SPISOK *SHET, //Список счетов
int kolgrup, //Количество групп
int kolshet, //Количество счетов
class masiv_din_double *sumshgr)
{
int	i,i1,i2;
int	nomgrup;
class iceb_tu_str shet("");
char	strsql[512];
SQL_str	row;
SQLCURSOR cur;
class iceb_tu_str naimgrup("");
class iceb_tu_str stroka("");
double	itogkol=0.;
double  itogo=0.;

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Движение товарно-материалных ценностей по группам и счетам"),dn,mn,gn,dk,mk,gk,fil.ff);

dvtmcf2_vrp(rp,&i1,fil.ff);

stroka.new_plus("-----------------------------------"); //Строка на наименованием

for(i1=0; i1 < kolshet+1;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  stroka.plus("-------------");
 }
fprintf(fil.ff,"%s\n",stroka.ravno());  

fprintf(fil.ff,"Код|  Наименование группы         |");

for(i1=0; i1 < kolshet;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  shet.new_plus(SHET->ravno(i1));
  fprintf(fil.ff,"%*s|",iceb_tu_kolbait(12,shet.ravno()),shet.ravno());
 }
fprintf(fil.ff,"%*s|\n",iceb_tu_kolbait(12,gettext("Итого")),gettext("Итого"));

fprintf(fil.ff,"%s\n",stroka.ravno());  

for(i=0; i < kolgrup; i++)
 {
  itogkol=0.;
  for(i1=0; i1 < kolshet;i1++)
      itogkol+=sumshgr->ravno(i*kolshet+i1);
  if(itogkol == 0.)
    continue;

  nomgrup=GRUP->ravno(i);  

  sprintf(strsql,"select naik from Grup where kod=%d",nomgrup);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naimgrup.new_plus(row[0]);
  else
    naimgrup.new_plus("");
    
  fprintf(fil.ff,"%3d %-*.*s",nomgrup,iceb_tu_kolbait(30,naimgrup.ravno()),iceb_tu_kolbait(30,naimgrup.ravno()),naimgrup.ravno());    
  itogo=0.;
  for(i1=0; i1 < kolshet;i1++)
   {
    itogkol=0.;
    for(i2=0; i2 < kolgrup;i2++)
     {
      itogkol+=sumshgr->ravno(i2*kolshet+i1);
     }
    if(itogkol == 0.)
      continue;
    fprintf(fil.ff," %12.2f",sumshgr->ravno(i*kolshet+i1));
    itogo+=sumshgr->ravno(i*kolshet+i1);
   }

  fprintf(fil.ff," %12.2f\n",itogo);
 }

fprintf(fil.ff,"%s\n",stroka.ravno());  
fprintf(fil.ff,"%*s:",iceb_tu_kolbait(33,gettext("Итого")),gettext("Итого"));
itogo=0.;
for(i1=0; i1 < kolshet; i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  fprintf(fil.ff," %12.2f",itogkol);
  itogo+=itogkol;
 }

fprintf(fil.ff," %12.2f\n",itogo);

podpis(fil.ff);
fil.end();

}

/***********************************************/
/*Распечатка группа-счет-счет корреспондент    */
/***********************************************/
void rasgshshk(class dvtmcf2_rr *rp,
short dn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,
class SPISOK *GSHSHK,      //Список группа-счет-счет корреспондент
class masiv_din_double *SUMGSHSHK) //Суммы к списку  группа-счет-счет корреспондент
{
char	strsql[512];
SQL_str	row;
int	kolstr=0;
int	kolelem=0;
int	i=0;
short   metkavgr=0;
class iceb_tu_str kod("");
class iceb_tu_str shet("");
class iceb_tu_str shetk("");
double	itoggr=0.;

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Движение товарно-материалных ценностей по группам и счетам"),dn,mn,gn,dk,mk,gk,fil.ff);

dvtmcf2_vrp(rp,&i,fil.ff);

sprintf(strsql,"select kod,naik from Grup order by kod asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

kolelem=GSHSHK->kolih();
while(cur.read_cursor(&row) != 0)
 {
  if(itoggr != 0)
   {
    fprintf(fil.ff,"%*s:%10.2f\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);
    itoggr=0.;
   }

  metkavgr=0;  
  for(i=0; i < kolelem; i++)
   {
    polen(GSHSHK->ravno(i),&kod,1,'#');
    if(SRAV(kod.ravno(),row[0],0) == 0)
     {
      if(metkavgr == 0)
       {
        fprintf(fil.ff,"\n%s %s\n",row[0],row[1]);

        fprintf(fil.ff,"\
------------------------------\n");
        fprintf(fil.ff,gettext("\
Дебет |Кредит|    Сумма      |\n"));
        fprintf(fil.ff,"\
------------------------------\n");

        metkavgr=1;
       }
      polen(GSHSHK->ravno(i),&shet,2,'#');
      polen(GSHSHK->ravno(i),&shetk,3,'#');
      if(shetk.ravno()[0] == '\0')
       shetk.new_plus("???");
      fprintf(fil.ff,"%-*s %-*s %10.2f\n",
      iceb_tu_kolbait(6,shet.ravno()),shet.ravno(),
      iceb_tu_kolbait(6,shetk.ravno()),shetk.ravno(),
      SUMGSHSHK->ravno(i));
      itoggr+=SUMGSHSHK->ravno(i);
     }            

   }  

 }

if(itoggr != 0)
  fprintf(fil.ff,"%*s:%10.2f\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);


podpis(fil.ff);
fil.end();

}
/**************************************/
/*Вывод шапки*/
/***************************/
void dvtmcf2_sap(int *sl, //Счетчик листов
int *kst, //Счетчик строк
FILE *ff)
{
*sl+=1;

fprintf(ff,"%*s N%d\n",iceb_tu_kolbait(170,gettext("Лист")),gettext("Лист"),*sl);

 fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код  |        Наименование               |   Номенкла-   |Един.|              |            |               |               | Д а т а  | Д а т а  | Код   |\n\
      |    товара или материалла          | турный номер  |изме-|   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |опера- |         К о н т р а г е н т\n\
      |                                   |               |рения|              |            |               |               |документа | дения    | ции   |\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  *kst+=7; /*больше на 1*/

}
