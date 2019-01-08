/* $Id: rasoskr.c,v 5.80 2013/09/26 09:43:39 sasa Exp $ */
/*25.04.2015    01.02.1994      Белых А.И.      rasoskr.c
Распечатка перечня карточек с остатками
*/
#include        <errno.h>
#include	"buhg.h"
class rasoskr_poi
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str skladi;
   class iceb_tu_str grupi;
   class iceb_tu_str sheta;
   class iceb_tu_str kodm;
   class iceb_tu_str nds;
   class iceb_tu_str kon_dat_is; //Конечная дата использования
   class iceb_tu_str invnomp;
   class iceb_tu_str naim_mat;

  rasoskr_poi()
   {
    clear_data();
   }
  void clear_data()
   {
    dat1.new_plus("");
    skladi.new_plus("");
    grupi.new_plus("");
    sheta.new_plus("");
    kodm.new_plus("");
    nds.new_plus("");
    kon_dat_is.new_plus("");
    invnomp.new_plus("");
    naim_mat.new_plus("");
   }  
 };

extern int      kol_strok_na_liste;

#define		IMAFDPI "inventm.alx"

void 		sapkk1(short,short,short,short,FILE*,int*,int*);
void 		sapkk(short,short,short,short,int,FILE*);
void rasoskr_naim_sap(int nom_str,int *kolstr,FILE *ff);
void rasosrk_rp(class rasoskr_poi *poi,FILE *ff);

extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/

int rasoskr(class spis_oth *oth)
{
static class rasoskr_poi poi;
class ostatokt ost;
class iceb_tu_str bros("");
extern char	*maskmat; /*Массив кодов отмеченных материалов*/
struct  tm      *bf;
time_t          tmm;
FILE            *ff1,*ff2;
char            imaf1[64],imafiv[64];
short		nkp;
class iceb_tu_str naiskl("");
int		skl,skk,sklz;
int		nk;
class iceb_tu_str grup(""),grupz("");
double		itgk,itgs; /*Итоги по группе*/
double		itogshk,itogshs; //Итоги по счёту
double		itok,itos; //Итоги по складу
double		itk=0.,its=0.; /*Итоги по общие*/
double		bb=0.;
int		i,kgr;
char		mnds=' ';
int		srt; 
class iceb_tu_str shet(""),shetz("");
short		dos,mos,gos;
short		d,m,g;
int		par,kom1;
short		mns=0;
int		kolstr,kolstr1;
SQL_str         row,row1,rowtmp;
char		strsql[1024];
int		kgrm;
class iceb_tu_str ei("");
class iceb_tu_str naim("");
class iceb_tu_str naimgr("");
class iceb_tu_str naish("");
int		kolstrlist;
int		kollist=0,kollistiv=0;
class iceb_tu_str mesqc("");
class iceb_tu_str innom("");
class iceb_tu_str regnom("");
short		dv,mv,gv;
class iceb_tu_str dopstr("");
int		metkainnom=0;
int             kolstriv=0;

float pnds=iceb_t_pnds();

if(poi.dat1.ravno()[0] == '\0')
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  poi.dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(poi.skladi.ravno(),128);//0
VV.VVOD_SPISOK_add_data(poi.grupi.ravno(),128);//1
VV.VVOD_SPISOK_add_data(poi.sheta.ravno(),128);//2
VV.VVOD_SPISOK_add_data(poi.kodm.ravno(),128);//3
VV.VVOD_SPISOK_add_data(poi.dat1.ravno(),11);//4
VV.VVOD_SPISOK_add_data(poi.nds.ravno(),5);//5
VV.VVOD_SPISOK_add_data(poi.kon_dat_is.ravno(),11);//6
VV.VVOD_SPISOK_add_data(poi.naim_mat.ravno(),112);//7
VV.VVOD_SPISOK_add_data(poi.invnomp.ravno(),112);//8

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка карточек с остатками"));

VV.VVOD_SPISOK_add_MD(gettext("Склад.(,,)......................"));//0
VV.VVOD_SPISOK_add_MD(gettext("Группа..(,,)...................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт....(,,)...................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Материал.(,,/*)................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата остатка...(д.м.г).........."));//5
sprintf(strsql,gettext("НДС (0,%2.f,+)...................."),pnds);//5
VV.VVOD_SPISOK_add_MD(strsql);
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата использования по.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Наименование материалла...(,,).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.........(,,).."));//8

naz:;

clear();

move(LINES-6,0);
printw(gettext("Если НДС не ввели - все карточки.\n\
Если НДС = 0      - карточки с нулевым НДС.\n\
Если НДС = %2.f     - карточки с %2.f%% НДС.\n\
Если НДС = +      - карточки цена в которых вместе с НДС.\n"),pnds,pnds);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+/-",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("доп.инфор."),
"F6",gettext("расчитать"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

poi.skladi.new_plus(VV.data_ravno(0));
poi.grupi.new_plus(VV.VVOD_SPISOK_return_data(1));
poi.sheta.new_plus(VV.VVOD_SPISOK_return_data(2));
poi.kodm.new_plus(VV.data_ravno(3));
poi.dat1.new_plus(VV.VVOD_SPISOK_return_data(4));
poi.nds.new_plus(VV.data_ravno(5));
poi.kon_dat_is.new_plus(VV.data_ravno(6));
poi.naim_mat.new_plus(VV.VVOD_SPISOK_return_data(7));
poi.invnomp.new_plus(VV.data_ravno(8));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch(par)
 {
  case ESC:
  case FK10:
    return(1);
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu5_1.txt");
   clear();
   goto naz;

  case FK2:
  case PLU:
  case OTN:
    break;

  case FK6: //Только карточки с инвентарными номеррами
    metkainnom=1;
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kod_v,&naim_v) == 0)
         VV.data_z_plus(0,kod_v.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&poi.grupi,&naim);
        VV.VVOD_SPISOK_zapis_data(1,poi.grupi.ravno());
        goto naz;

      case 2 :
        if(dirmatr(&kgr,&i,&naim_v,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(3,i);
         }
        
        goto naz;

      case 3 :
        vibrek("Plansh",&poi.sheta,&naim);
        VV.VVOD_SPISOK_zapis_data(2,poi.sheta.ravno());
        goto naz;
     }

  case FK4: /*Очистка меню*/
    VV.VVOD_clear_data();
    goto naz;    

  case FK5: /*Ввод дополнительной информации в шапку инвентаризационной ведомости*/
    iceb_redfil(IMAFDPI,0);
    goto naz;    

  default:
    goto naz;    
 }


if(poi.dat1.ravno()[0] == '\0' || rsdat(&dos,&mos,&gos,poi.dat1.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }
short dki=0,mki=0,gki=0;

if(poi.kon_dat_is.ravno()[0] != '\0')
if(rsdat(&dki,&mki,&gki,poi.kon_dat_is.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата конечного использования !"));
  goto naz;
 }

skl=0;
skl=(int)ATOFM(VV.VVOD_SPISOK_return_data(0));

SQLCURSOR cur,curtmp;

SQLCURSOR cur1;

class iceb_tu_str matot("");
if(skl != 0)
 {
  sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
  skl);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl);
    iceb_t_soob(strsql);
    goto naz;
   }

  naiskl.new_plus(row[0]);
  matot.new_plus(row[1]);
 }

/*Определяем количество складов в строке*/
mns=pole2(poi.skladi.ravno(),',');
if(poi.skladi.ravno()[0] != '\0' && mns == 0)
  mns=1; /*Только один склад в списке*/
else
  mns=0;
    
if(poi.kodm.ravno()[0] == '*' && maskmat == NULL)
 { 
  iceb_t_soob(gettext("Не создан список материалов !"));
  goto naz;
 } 

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать группа материалла-склад"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счёт учета материалла-склад"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счет-группа материалла"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
srt=0;

while(menu3w(stdscr,&MENU,&srt,-1,-1,0) != 0);

if(srt == -1 || srt == 3 )
  return(1);

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Карточки с нулевыми остатками распечатывать ?"));

nkp=danet(&DANET,2,stdscr);

clear();
GDITE();





sprintf(strsql,"select * from Kart");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной карточки !"));
  return(1);
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"rasoskr"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kgrm int not null,\
skl int not null,\
nk int not null,\
km int not null,\
naim char(112) not null,\
cena double(15,6) not null,\
nds char(32) not null,\
ei char(24) not null,\
ost double(16,6) not null,\
suma double(16,6) not null,\
datki char(10) not null,\
innom char(56) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 return(-1);
time(&tmm);
bf=localtime(&tmm);

if(skl != 0 && mns == 1)
 {
  printw("%s\n\n%s N%d %s\n",iceb_t_get_pnk("00",0),gettext("Перечень карточек по складу"),skl,naiskl.ravno());
 }
if(skl == 0)
 {
  printw("%s\n\n%s\n",iceb_t_get_pnk("00",0),gettext("Перечень карточек по всем складам"));
 }

char data_k_is[64];
class iceb_tu_str invnom("");
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("mt4.mz=%d mt4.sttr-%s\n",mt4.mz,mt4.sttr);
  refresh();
  */
  strzag(LINES-1,0,kolstr,++kolstr1);  

  if(proverka(poi.skladi.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(poi.invnomp.ravno(),row[15],0,0) != 0)
    continue;

  if(poi.kodm.ravno()[0] != '\0')
   {
    if(poi.kodm.ravno()[0] == '*' && maskmat != NULL)
     {
      if(proverka(maskmat,row[2],0,0) != 0)
        continue;
     }
    else
      if(proverka(poi.kodm.ravno(),row[2],0,0) != 0)
        continue;
   }

  if(provnds(poi.nds.ravno(),row) != 0)
    continue;

  if(proverka(poi.sheta.ravno(),row[5],0,0) != 0)
    continue;
  if(metkainnom == 1)
   if(row[15][0] == '\0')
     continue;

  naim.new_plus("");
  kgrm=0;
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) != 1)
    {
     beep();
     printw("Не знайшли найменування матеріалу по коду %s\n",row[2]);
     OSTANOV();
    }
  else
   {
    naim.new_plus(row1[2]);
    kgrm=atoi(row1[1]);
    if(proverka(poi.naim_mat.ravno(),row1[2],4,0) != 0)
     continue;
   }


  if(poi.grupi.getdlinna() > 1)
   {
    if(proverka(poi.grupi.ravno(),kgrm,0,0) != 0)
     continue;
   }

  rsdat(&d,&m,&g,row[18],2);
  memset(data_k_is,'\0',sizeof(data_k_is));
  if(dki != 0 )
   {
    if(d == 0)
     continue;
    if(sravmydat(d,m,g,dki,mki,gki) > 0)
     continue;    
   }
  if(d != 0)
   sprintf(data_k_is,"%02d.%02d.%04d",d,m,g);
  nk=atoi(row[1]);

  ei.new_plus(row[4]);
  ostkar(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {

    VVOD SOOB(1);
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    SOOB.VVOD_SPISOK_add_MD(gettext("Отрицательный остаток"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

   }

  if(nkp == 1 || (nkp == 2 && (ost.ostg[3] > 0.0000001 || ost.ostg[3] < -0.0000001 )))
   {
    if(row[3][0] == '0')
     mnds=' ';    
    if(row[3][0] == '1')
     mnds='+';    

    bb=ost.ostg[3]*atof(row[6]);
    bb=okrug(bb,okrg1);

    invnom.new_plus("");
    if(row[15][0] != '\0')
     {
      invnom.new_plus(row[15]);
      if(row[14][0] != '\0')
       {
        short d,m,g;
        rsdat(&d,&m,&g,row[14],2);
        sprintf(strsql," %d.%d.%d",d,m,g);
        invnom.plus(" ",strsql);
       }
     }

    sprintf(strsql,"insert into %s values ('%s',%d,%s,%d,%s,'%s',%s,'%c%2.2g','%s',%.10g,%.10g,'%s','%s')",
    imatmptab,
    row[5],kgrm,row[0],nk,row[2],
    naim.ravno_filtr(),
    row[6],
    mnds,
    atof(row[9]),
    ei.ravno(),
    ost.ostg[3],
    bb,
    data_k_is,
    invnom.ravno_filtr());

    sql_zapis(strsql,1,0);    

   }

 }

GDITE();
//fclose(ff);


printw(gettext("Сортируем записи"));
printw("\n");
refresh();

/*Сортировка по группе материала*/
if(srt == 0)
  sprintf(strsql,"select * from %s order by kgrm asc,skl asc,naim asc",imatmptab);

/*Сортировка по счёту учета материала*/
if(srt == 1)
  sprintf(strsql,"select * from %s order by shu asc,skl asc,naim asc",imatmptab);
//Сортировка счет, группа
if(srt == 2) //Сортировка счет, группа
  sprintf(strsql,"select * from %s order by shu asc,kgrm asc,skl asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }



printw(gettext("Распечатываем\n"));
refresh();

sprintf(imaf1,"sk%d_%d.lst",skl,getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

sprintf(imafiv,"inv%d_%d.lst",skl,getpid());

if((ff2 = fopen(imafiv,"w")) == NULL)
 {
  error_op_nfil(imafiv,errno,"");
  return(1);
 }



d=dos; m=mos; g=gos;
dpm(&d,&m,&g,1); //Увеличить на день
mesc(m,1,&mesqc);

kolstriv=0;
sapvin(matot.ravno(),mesqc.ravno(),g,gos,&kolstriv,&kollistiv,"",ff2);

kolstrlist=0;
if(poi.skladi.ravno()[0] != '\0')
 {
  if(skl != 0 && mns == 1)
   {
    fprintf(ff1,"%s\n\n%s %s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
    iceb_t_get_pnk("00",0),
    gettext("Перечень карточек по складу"),
    poi.skladi.ravno(),naiskl.ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    kolstrlist=4;
   }
  else
   {
    fprintf(ff1,"%s\n\n%s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
    iceb_t_get_pnk("00",0),
    gettext("Перечень карточек по складу"),
    poi.skladi.ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    kolstrlist=4;
   }
 }

if(poi.skladi.ravno()[0] == '\0')
 {
  fprintf(ff1,"%s\n\n%s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
  iceb_t_get_pnk("00",0),
  gettext("Перечень карточек по всем складам"),
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  kolstrlist=4;
 }

if(poi.grupi.getdlinna() > 1)
 {
  fprintf(ff1,gettext("Групы: %s\n"),poi.grupi.ravno());
  kolstrlist++;
 }
if(poi.sheta.getdlinna() > 1)
 {
  fprintf(ff1,gettext("Счета учета: %s\n"),poi.sheta.ravno());
  kolstrlist++;
 }
if(poi.kodm.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Код материалла: %s\n"),poi.kodm.ravno());
  kolstrlist++;
 }
if(poi.naim_mat.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Наименование материалла"),poi.naim_mat.ravno());
  kolstrlist++;
 }
if(invnom.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Инвентарный номер"),invnom.ravno());
  kolstrlist++;
 }
if(poi.nds.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("НДС"),poi.nds.ravno());
  kolstrlist++;
 }

if(poi.kon_dat_is.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Конечная дата использования"),poi.kon_dat_is.ravno());
  kolstrlist++;
 }
 
if(nkp == 2)
 {
  fprintf(ff1,gettext("Карточки с нулевыми остатками не показаны !\n"));
  kolstrlist++;
 }
if(srt == 0)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по группам материалов"));
  kolstrlist++;
 }
if(srt == 1)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам учета материалов"));
  kolstrlist++;
 }
if(srt == 2)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам, группам"));
  kolstrlist++;
 }
if(metkainnom == 1)
 {
  fprintf(ff1,"%s\n",gettext("Только карточки с инвентарными номерами !"));
  kolstrlist++;
 }

itogshk=itogshs=0.;
itk=its=itgk=itgs=itok=itos=0.; 
kolstr1=sklz=0;

kolstrlist+=4;
kollist=1;
sapkk(dos,mos,gos,par,kollist,ff1);

int nomerstr=0;
int kodmat=0;
double cena=0.;
double kolih=0.,summa=0.;
class iceb_tu_str nds_str("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,++kolstr1);  

  shet.new_plus(rowtmp[0]);
  grup.new_plus(rowtmp[1]);
  skk=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  kodmat=atoi(rowtmp[4]);
  naim.new_plus(rowtmp[5]);
  cena=atof(rowtmp[6]);
  nds_str.new_plus(rowtmp[7]);
  ei.new_plus(rowtmp[8]);
  kolih=atof(rowtmp[9]);
  summa=atof(rowtmp[10]);
  strncpy(data_k_is,rowtmp[11],sizeof(data_k_is)-1);
  invnom.new_plus(rowtmp[12]);





  if((srt == 1 || srt == 2) && SRAV(shetz.ravno(),shet.ravno(),0) != 0)
   {
    
    if(shetz.ravno()[0] != '\0')
     {
      if(srt == 1 && par != OTN)
       { 
        if(mns != 1)
         {
          sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
          fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
          sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
          fprintf(ff1,"%-*s %10.10g  %10.2f\n",
          iceb_tu_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);
         }
        itk+=itok;
        its+=itos;
        itok=itos=0.;
       }

      if(par != OTN)
       {
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_tu_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
        itogshk,itogshs);

        sapkk1iv(&kolstriv,&kollistiv,ff2);
        fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

        sapkk1iv(&kolstriv,&kollistiv,ff2);
        fprintf(ff2,"%-*s %10.10g %10.2f\n",
        iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
        itogshk,itogshs);
       }

      itogshk=itogshs=0.;

     }
    /*Определяем наименование счета*/
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден счёт %s в плане счетов !"),shet.ravno());         
      printw("\n");
      OSTANOV();
      naish.new_plus("");
     }
    else
     naish.new_plus(row1[0]);

    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"%s: %s %s\n",gettext("Счёт"),shet.ravno(),naish.ravno());

    sapkk1iv(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1iv(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Счёт"),shet.ravno(),naish.ravno());

    shetz.new_plus(shet.ravno());    
    sklz=0;
   }
      
  if((srt == 0 || srt == 2) && SRAV(grupz.ravno(),grup.ravno(),0) != 0)
   {
    
    if(grupz.ravno()[0] != '\0')
     {

      if(par != OTN)
       {
        if(itok != 0. || itos != 0.)
         {
          sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
          fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
          sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
          fprintf(ff1,"%-*s %10.10g  %10.2f\n",
          iceb_tu_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);
          itk+=itok;
          its+=itos;
          itok=itos=0.;
         }
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_tu_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

        sapkk1iv(&kolstriv,&kollistiv,ff2);
        fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1iv(&kolstriv,&kollistiv,ff2);
        fprintf(ff2,"%-*s %10.10g %10.2f\n",
        iceb_tu_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
       }
      itgk=itgs=0.; 


     }
    /*Узнаем наименование группы*/
    sprintf(strsql,"select naik from Grup where kod=%s",grup.ravno());
    if(readkey(strsql,&row1,&cur1) != 1)
     {
       beep();
       printw("%s %s!\n",gettext("Не найдена группа"),grup.ravno());
       refresh();
       naimgr.new_plus(gettext("Не найдена группа!"));
     }
    else
     naimgr.new_plus(row1[0]);
     
    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"%s: %s %s\n",gettext("Группа"),grup.ravno(),naimgr.ravno());

    sapkk1iv(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1iv(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Группа"),grup.ravno(),naimgr.ravno());

    grupz.new_plus(grup.ravno());
    sklz=0;
   }


  if(sklz != skk)
   {
    if(sklz != 0 && par != OTN)
     {

      if(mns != 1)
       {
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_tu_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

       }      
      itk+=itok;
      its+=itos;
      itok=itos=0.;

      
     }
    /*Узнаем наименование Склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skk);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
       beep();
       printw("%s %d!\n",gettext("Не найден склад"),skk);
       refresh();
       naimgr.new_plus(gettext("Не найден склад!"));
     }
    else
     {
      naimgr.new_plus(row1[0]);
     }

    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"%s: %d %s\n",gettext("Склад"),skk,naimgr.ravno());
    sklz=skk;    


   }
  itgk+=kolih;
  itgs+=summa;

  itogshk+=kolih;
  itogshs+=summa;

  itok+=kolih;
  itos+=summa;
   

  if(par == OTN)
   {

    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"%-*s|",iceb_tu_kolbait(7,shet.ravno()),shet.ravno());
    fprintf(ff1,"%-2s|",grup.ravno());

    fprintf(ff1,"%-2d|",skk);

    fprintf(ff1,"%-5d|",nk);
    fprintf(ff1,"%-6d|",kodmat);
    fprintf(ff1,"%-*.*s|",iceb_tu_kolbait(37,naim.ravno()),iceb_tu_kolbait(37,naim.ravno()),naim.ravno());

    fprintf(ff1," %-*s|",iceb_tu_kolbait(6,ei.ravno()),ei.ravno());

    fprintf(ff1,"%10.10g |",kolih);

    /*Остаток по документам*/
    class ostatokt ost;
    ostdok(1,1,gos,dos,mos,gos,skk,nk,&ost);
    fprintf(ff1,"%10.10g |",ost.ostg[3]);
    
    fprintf(ff1,"          |          |          |\n");

    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);
    fprintf(ff1,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

   }
  else
   {
    //Читаем инвентарный номер
    dopstr.new_plus("");
    

    sprintf(strsql,"select datv,innom,rnd from Kart where sklad=%d and nomk=%d",skk,nk);
    if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
     {
      rsdat(&dv,&mv,&gv,row[0],2);
      innom.new_plus(row[1]);
      regnom.new_plus(row[2]);
     }   
    else
     {
      regnom.new_plus("");
      dv=mv=gv=0;
      innom.new_plus("");
     }
    if(innom.ravno()[0] != '\0')
     {
      if( regnom.ravno()[0] == '\0')
       {
//        sprintf(dopstr,"%s:%s",gettext("Инвентарный номер"),innom.ravno());
        dopstr.new_plus(gettext("Инвентарный номер"));
        dopstr.plus(":",innom.ravno());
        if(dv != 0)
         {
          sprintf(strsql," %s:%d.%d.%d%s",
          gettext("Дата ввода в эксплуатацию"),
          dv,mv,gv,gettext("г."));
          dopstr.plus(strsql);
         }
       }        
      else
       {
//        sprintf(dopstr,"%s:%s",gettext("Регистрационный номер"),regnom.ravno());
        dopstr.new_plus(gettext("Регистрационный номер"));
        dopstr.plus(":",regnom.ravno());
        sprintf(strsql," %s:%s",gettext("Серия"),innom.ravno());
        dopstr.plus(strsql);
        if(dv != 0)
         {
          sprintf(strsql," %s:%d.%d.%d%s",
          gettext("Дата регистрации"),
          dv,mv,gv,gettext("г."));
          dopstr.plus(strsql);
         }
       }
     }    


    sapkk1(dos,mos,gos,par,ff1,&kolstrlist,&kollist);

    fprintf(ff1,"%-*s |%2d|%-2d|%-5d|%-6d|%-*.*s|%10s ",
    iceb_tu_kolbait(6,shet.ravno()),shet.ravno(),
    grup.ravno_atoi(),skk,nk,kodmat,
    iceb_tu_kolbait(37,naim.ravno()),iceb_tu_kolbait(37,naim.ravno()),naim.ravno(),
    prcn(cena));

    fprintf(ff1,"|%-3s| %-*s|%10.10g |%10.2f|%10s|%s\n",
    nds_str.ravno(),
    iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
    kolih,summa,data_k_is,invnom.ravno());


    sapkk1iv(&kolstriv,&kollistiv,ff2);

    fprintf(ff2,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
    ++nomerstr,
    iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
    cena,kolih,summa," "," "," "," ");
    if(dopstr.ravno()[0] != '\0')
     {
      sapkk1iv(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"%s\n",dopstr.ravno());
     }
    sapkk1iv(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
   }


 }

itk+=itok;
its+=itos;

if(par != OTN)
 {
  if(mns == 1)
   {
    if(srt == 0)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
     }

    if(srt == 1)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-62s %10.10g %10.2f\n",gettext("Общий итог"),itk,its);
     }

    if(srt == 2)
     {

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),itgk,itgs);

     }
   }

  if(mns != 1)
   {
     fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

    if(srt == 0 || srt == 2)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
     }
    if(srt == 1 || srt == 2)
     {
      fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);
     }
    fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_tu_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),itk,its);
    fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
   }
}

printw("%*s %8.8g %8.2f\n",iceb_tu_kolbait(47,gettext("Общий итог")),gettext("Общий итог"),itk,its);

koncinv(mesqc.ravno(),gos,ff2);

fclose(ff1);

fclose(ff2);


GDITE();

/*распечатка остатков отсортированных по наименованию*/

sprintf(strsql,"select * from %s order by naim asc",imatmptab);

kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
char imaf_naim[64];
sprintf(imaf_naim,"ostnm%d.lst",getpid());

class iceb_fopen fil_naim;
if(fil_naim.start(imaf_naim,"w") != 0)
 return(1);
 
iceb_t_zagolov(gettext("Остатки на карточках"),fil_naim.ff);

fprintf(fil_naim.ff,"%s %d.%d.%d %s %s\n",
gettext("Остатки вычислены на"),
dos,mos,gos,gettext("г."),
gettext("включительно"));

rasosrk_rp(&poi,fil_naim.ff);


rasoskr_naim_sap(1,NULL,fil_naim.ff);

kolstr1=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,++kolstr1);  
  fprintf(fil_naim.ff,"%-*s %2s %3s %-5s %-6s %-*.*s %11.11g %-3.3s %-*s %11.11g %10.2f %s %s\n",
  iceb_tu_kolbait(7,rowtmp[0]),
  rowtmp[0],
  rowtmp[1],
  rowtmp[2],
  rowtmp[3],
  rowtmp[4],
  iceb_tu_kolbait(37,rowtmp[5]),
  iceb_tu_kolbait(37,rowtmp[5]),
  rowtmp[5],
  atof(rowtmp[6]),
  rowtmp[7],
  iceb_tu_kolbait(7,rowtmp[8]),
  rowtmp[8],    
  atof(rowtmp[9]),
  atof(rowtmp[10]),
  iceb_tu_datzap(rowtmp[11]),
  rowtmp[12]);
 }
fprintf(fil_naim.ff,"ICEB_LST_END\n");
podpis(fil_naim.ff);

fil_naim.end();


OSTANOV();

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Распечатка карточек на складе"));
oth->spis_imaf.plus(imafiv);
oth->spis_naim.plus(gettext("Инвентаризационная ведомость"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

oth->spis_imaf.plus(imaf_naim);
oth->spis_naim.plus(gettext("Распечатка карточек с сортировкой по наименованиям"));
iceb_t_ustpeh(imaf_naim,3,&kolstr);
iceb_t_rnl(imaf_naim,kolstr,"",&rasoskr_naim_sap);

return(0);
}

/***************/
/*Счетчик строк*/
/****************/
void		sapkk1(short dos,short mos,short gos,
short par,FILE *ff1,int *kolstrlist,int *kollist)
{

*kolstrlist+=1;

if(*kolstrlist <= kol_strok_na_liste)
 return;

*kollist+=1;
fprintf(ff1,"\f");

*kolstrlist=4; //Размер шапки
*kolstrlist+=1; //СТРОКА которая должна быть выведена

sapkk(dos,mos,gos,par,*kollist,ff1);


}


/*************/
/*Шапка*/
/*************/

void sapkk(short dos,short mos,short gos, //дата остатка
short par,int kollist,FILE *ff1)
{

 
if(par == OTN)
 {
  fprintf(ff1,"%100s %s%d\n\
----------------------------------------------------------------------------------------------------------------------------------\n",
  " ",gettext("Лист N"),kollist);
  fprintf(ff1,gettext("  Счёт |Гр|Ск| Н/К |  Код |            Наименование             |  Е.и. | Остаток   |Остаток док|          |          |          |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
 }
else
 {

  fprintf(ff1,"%s:%02d.%02d.%04d %s %s %50s %s%d\n\
---------------------------------------------------------------------------------------------------------------------------\n",
  gettext("Остатки вычислены на"),
  dos,mos,gos,gettext("г."),
  gettext("включительно"),
  " ",gettext("Лист N"),kollist);

  fprintf(ff1,gettext(" Счёт  |Гр|Ск| Н/К |  Код |            Наименование             |    Цена   |НДС|  Е.и. | Остаток   |  Сумма   |К.дата исп|\n"));
  fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
 }
}
/**************************************/
/*шапка для отсортированого по наименованию*/
/********************************************/
void rasoskr_naim_sap(int nom_str,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%100s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Счёт  |Гр|Скл| Н/К |  Код |            Наименование             |    Цена   |НДС|  Е.и. | Остаток   |  Сумма   |К.дата исп|\n"));


fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
}
/***************************************************/
/*реквизиты поиска*/
/*****************************************************/
void rasosrk_rp(class rasoskr_poi *poi,FILE *ff)
{

if(poi->skladi.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Склад"),poi->skladi.ravno());

if(poi->grupi.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Группа"),poi->grupi.ravno());

if(poi->sheta.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт учета"),poi->sheta.ravno());
if(poi->kodm.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),poi->kodm.ravno());
if(poi->naim_mat.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Наименование материалла"),poi->naim_mat.ravno());
if(poi->invnomp.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),poi->invnomp.ravno());

if(poi->nds.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("НДС"),poi->nds.ravno());

if(poi->kon_dat_is.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Конечная дата использования"),poi->kon_dat_is.ravno());

}
