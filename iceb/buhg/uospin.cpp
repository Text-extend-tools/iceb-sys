/*$Id:$*/
/*22.03.2017	10.02.2010	Белых А.И.	uospin.c
Перенос основных срeдств из материального учёта в учёт основных средств
*/
#include        <errno.h>
#include "buhg.h"
class uosvibo_poi
 {
  public:
  class iceb_tu_str datadok;
  class iceb_tu_str shetu_mu;
  class iceb_tu_str sklad;
  class iceb_tu_str ei;
  class iceb_tu_str kodop_mu;
  class iceb_tu_str kodop_uos;
  class iceb_tu_str podr; /*Код подразделения*/
  class iceb_tu_str matot; /*Коды ответственных лиц*/
  class iceb_tu_str grup_nu;
  class iceb_tu_str grup_bu;
  class iceb_tu_str hauz;
  class iceb_tu_str shetu_uos;
  class iceb_tu_str hztz;

  uosvibo_poi()
   {
    clear();
   }
  void clear()
   {
    datadok.new_plus("");
    sklad.new_plus("");
    shetu_mu.new_plus("");
    kodop_mu.new_plus("");
    kodop_uos.new_plus("");
    podr.new_plus("");
    matot.new_plus("");
    grup_nu.new_plus("");
    grup_bu.new_plus("");
    hauz.new_plus("");
    shetu_uos.new_plus("");
    hztz.new_plus("");
    ei.new_plus("");
   }
 };

extern short    mfnn; /*Метка формирования номера накладной для материального учёта*/

int uospin(class spis_oth *oth)
{
class SQLCURSOR cur,cur1;
SQL_str row,row1;
FILE *ff_prot=NULL;
char imaf_prot[64];
int kolstr=0;
int kolstr1=0;
char strsql[512];
static class uosvibo_poi rp;
int kom=0,kom1=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(rp.datadok.ravno(),11);
VV.VVOD_SPISOK_add_data(rp.shetu_mu.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.kodop_mu.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.sklad.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.ei.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.kodop_uos.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.grup_nu.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.grup_bu.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.podr.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.matot.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.hauz.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.shetu_uos.ravno(),16);
VV.VVOD_SPISOK_add_data(rp.hztz.ravno(),16);

VV.VVOD_SPISOK_add_MD("Дата переноса (д.м.г)....................");//0
VV.VVOD_SPISOK_add_MD("Счёт учёта материалов....................");//1
VV.VVOD_SPISOK_add_MD("Код операции списания в мат.учёте........");//2
VV.VVOD_SPISOK_add_MD("Код склада в материальном учёте..........");//3
VV.VVOD_SPISOK_add_MD("Код единицы измерения..........(,,)......");//4

VV.VVOD_SPISOK_add_MD("Код операции прихода в уч.основ.средств..");//5
VV.VVOD_SPISOK_add_MD("Группа нал.учета.........................");//6
VV.VVOD_SPISOK_add_MD("Группа бух.учета.........................");//7
VV.VVOD_SPISOK_add_MD("Код подразделения........................");//8
VV.VVOD_SPISOK_add_MD("Код мат.ответственного...................");//9
VV.VVOD_SPISOK_add_MD("Шифр анал. учета.........................");//10
VV.VVOD_SPISOK_add_MD("Счёт учета основных средств..............");//11
VV.VVOD_SPISOK_add_MD("Шифр затрат..............................");//12

VV.VVOD_SPISOK_add_ZAG(gettext("Перенос основных средств из материального учёта"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rp.datadok.new_plus(VV.VVOD_SPISOK_return_data(0));
rp.shetu_mu.new_plus(VV.VVOD_SPISOK_return_data(1));
rp.kodop_mu.new_plus(VV.VVOD_SPISOK_return_data(2));
rp.sklad.new_plus(VV.VVOD_SPISOK_return_data(3));
rp.ei.new_plus(VV.VVOD_SPISOK_return_data(4));
rp.kodop_uos.new_plus(VV.VVOD_SPISOK_return_data(5));

rp.grup_nu.new_plus(VV.VVOD_SPISOK_return_data(6));

rp.grup_bu.new_plus(VV.VVOD_SPISOK_return_data(7));

rp.podr.new_plus(VV.VVOD_SPISOK_return_data(8));
rp.matot.new_plus(VV.VVOD_SPISOK_return_data(9));
rp.hauz.new_plus(VV.VVOD_SPISOK_return_data(10));
rp.shetu_uos.new_plus(VV.VVOD_SPISOK_return_data(11));
rp.hztz.new_plus(VV.VVOD_SPISOK_return_data(12));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch(kom)
 {
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:  /*Просмотр реквизитов*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственных"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учета"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций мат. учёта"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций учёта осн. средств"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка единиц измерения"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//10

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case 10 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uosgrup",&rp.grup_nu);
        VV.VVOD_SPISOK_zapis_data(6,rp.grup_nu.ravno());
        goto naz;

      case 1 :
        vibrek("Uosgrup1",&rp.grup_bu);
        VV.VVOD_SPISOK_zapis_data(7,rp.grup_bu.ravno());
        goto naz;

      case 2 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_plus(8,kod_v.ravno());
        goto naz;

      case 3 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_plus(9,kod_v.ravno());
        goto naz;

      case 4 :
        vibrek("Uoshau",&rp.hauz);
        VV.VVOD_SPISOK_zapis_data(10,rp.hauz.ravno());
        goto naz;

      case 5 :
        vibrek("Uoshz",&rp.hztz);
        VV.VVOD_SPISOK_zapis_data(12,rp.hztz.ravno());
        goto naz;

      case 6 :
//        vibrek("Rashod",&rp.kodop_mu);
        if(dirrashod(1,&rp.kodop_mu,&naim_v) == 0)
          VV.data_plus(2,rp.kodop_mu.ravno());
        goto naz;

      case 7 :
        vibrek("Uospri",&rp.kodop_uos);
        VV.VVOD_SPISOK_zapis_data(5,rp.kodop_uos.ravno());
        goto naz;

      case 8 :
        if(dirsklad(1,&kod_v,&naim_v) == 0)
          VV.data_plus(3,kod_v.ravno());
        goto naz;

      case 9 :
        vibrek("Edizmer",&rp.ei);
        VV.VVOD_SPISOK_zapis_data(4,rp.ei.ravno());
        goto naz;
     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

short dos,mos,gos;
if(rsdat(&dos,&mos,&gos,rp.datadok.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата!"));
  goto naz;
 }

class OPSHET reksh;
if(prsh1(rp.shetu_mu.ravno(),&reksh) != 0)
 goto naz;
if(prsh1(rp.shetu_uos.ravno(),&reksh) != 0)
 goto naz;

sprintf(strsql,"select naik from Rashod where kod='%s'",rp.kodop_mu.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s!"),rp.kodop_mu.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select naik from Uospri where kod='%s'",rp.kodop_uos.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s!"),rp.kodop_uos.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }
if(rp.sklad.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Sklad where kod=%d",rp.sklad.ravno_atoi());
  if(readkey(strsql) != 1)
   {
    sprintf(strsql,gettext("Не найден склад %d в списке складов!"),rp.sklad.ravno_atoi());
    iceb_t_soob(strsql);
    goto naz;
   }
 }

sprintf(strsql,"select naik from Uospod where kod=%d",rp.podr.ravno_atoi());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код подразделения"),rp.podr.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }
sprintf(strsql,"select naik from Uosol where kod=%d",rp.matot.ravno_atoi());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден мат.ответственный"),rp.matot.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select naik from Uoshz where kod='%s'",rp.hztz.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден шифр затрат"),rp.hztz.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }
sprintf(strsql,"select naik from Uoshau where kod='%s'",rp.hauz.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден шифр аналитического учёта"),rp.hauz.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select naik from Uosgrup where kod='%s'",rp.grup_nu.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код группы"),rp.grup_nu.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",rp.grup_bu.ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код группы"),rp.grup_bu.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select sklad,nomk,kodm,mnds,ei,shetu,cena,nds,innom,rnd from Kart where shetu='%s' order by sklad asc",
rp.shetu_mu.ravno());

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

int innom=0;
class ostatokt ostkart;
kolstr1=0;
int sklad=0;
int skladz=0;
int nomkart=0;
int kodmat=0;
class iceb_tu_str nomdok_mu("");
class iceb_tu_str nomdok_uos("");
class iceb_tu_str naim_mat("");

sprintf(imaf_prot,"uospin%d.tmp",getpid());
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Перенос основных средств из материального учёта"),0,0,0,dos,mos,gos,ff_prot);
double cena=0.;
float pnds=iceb_t_pnds();

class lock_tables bt("LOCK TABLES Uosdok WRITE,icebuser READ");
uosnd(gos,1,&nomdok_uos); /*uosvdok.c*/

sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,ktoz,vrem,nds,pn) \
values ('%04d-%d-%d',1,'%s','%s','%s',%d,%d,%d,%d,%ld,%d,%.2f)",
gos,mos,dos,
rp.kodop_uos.ravno(),
"00",
nomdok_uos.ravno_filtr(),
rp.podr.ravno_atoi(),
rp.matot.ravno_atoi(),
1,
iceb_t_getuid(),
time(NULL),
3,
pnds);
if(sql_zapis(strsql,1,0) != 0)
   return(1);

bt.unlock();

if(ff_prot != NULL)
 fprintf(ff_prot,"Создали документ %s на приход основных средств\n",nomdok_uos.ravno());

short dv,mv,gv;
char datavvoda[64];

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(rp.sklad.ravno(),row[0],0,1) != 0)
   continue;
  if(proverka(rp.shetu_mu.ravno(),row[5],0,1) != 0)
   continue;
  if(proverka(rp.ei.ravno(),row[4],0,1) != 0)
   continue;

  sklad=atoi(row[0]);
  nomkart=atoi(row[1]);
  kodmat=atoi(row[2]);
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"Склад %d Карточка %d Код материалла=%d Счёт учёта=%s\n",sklad,nomkart,kodmat,row[5]);
  
  ostkar(1,1,gos,dos,mos,gos,sklad,nomkart,&ostkart);


  if(ostkart.ostg[3] <= 0.)
   {
    fprintf(ff_prot,"Нулевой остаток!\n");
    continue;
   }  

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
//  printw("%s %s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
//  refresh();
  cena=atof(row[6]);
  innom=atoi(row[8]);

  /*Читаем наименование материалла*/
  naim_mat.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);

  if(skladz != sklad)
   {
    class lock_tables bt("LOCK TABLES Dokummat WRITE,icebuser READ");
    iceb_t_nomnak(gos,row[0],&nomdok_mu,2,0,0);
    zap_s_mudok(2,dos,mos,gos,sklad,"00",nomdok_mu.ravno(),"",rp.kodop_mu.ravno(),0,0,"",0,0,0,0,"00",pnds);

    if(ff_prot != NULL)
     fprintf(ff_prot,"Номер документа на списание %s\n",nomdok_mu.ravno());
     
    skladz=sklad;
   }

  zapvdok(dos,mos,gos,sklad,nomkart,kodmat,nomdok_mu.ravno(),ostkart.ostg[3],cena,row[4],atof(row[7]),atoi(row[3]),0,2,0,"","",0,"");

  for(int nom=0; nom < ostkart.ostg[3]; nom++)
   {


    class lock_tables buos("LOCK TABLES Uosin WRITE,Kart READ,icebuser READ");

    if(ostkart.ostg[3] == 1 && innom != 0) /*Проверяем есть ли такой инвентарный номер в Учёте основных средств*/
     {
      sprintf(strsql,"select innom from Uosin where innom=%d",innom);
      if(readkey(strsql) >= 1)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Инвентарный номер %d уже есть в Учёте основных средств!\n",innom);
        sprintf(strsql,"Инвентарный номер %d уже есть в Учёте основных средств!\n",innom);
        iceb_t_soob(strsql);        
        continue;
       }
      if(ff_prot != NULL)
       fprintf(ff_prot,"Взяли инвентарный номер из карточкир %d\n",innom);
  //    printw("Взяли инвентарный номер из карточкир %d\n",innom);
     
     }
    else  /*Назначаем инвентарный номер*/
     {
      innom=invnomer(1);
      if(ff_prot != NULL)
       fprintf(ff_prot,"Назначили инвентарный номер %d\n",innom);
  //    printw("Назначили инвентарный номер %d\n",innom);
  //    refresh();
     }
    memset(datavvoda,'\0',sizeof(datavvoda));
    rsdat(&dv,&mv,&gv,row[9],1);
    if(dv != 0)
     sprintf(datavvoda,"%04d-%02d-%02d",gv,mv,dv);
    else
     sprintf(datavvoda,"0000-00-00");
    if(ostkart.ostg[3]- nom < 1)
     {
      naim_mat.plus(" (");
      naim_mat.plus(ostkart.ostg[3]- nom);
      naim_mat.plus(")");
      
     }
    sprintf(strsql,"insert into Uosin (innom,naim,datvv,ktoz,vrem) values (%d,'%s','%s',%d,%ld)",
    innom,naim_mat.ravno(),datavvoda,iceb_t_getuid(),time(NULL));

    if(sql_zapis(strsql,1,0) != 0)
     continue;

    buos.unlock();
       
    sprintf(strsql,"insert into Uosdok1 (datd,tipz,innom,nomd,podr,kodol,kol,bs,iz,kind,kodop,ktoz,vrem,bsby,izby,cena,shs) \
   values ('%04d-%d-%d',1,%d,'%s',%d,%d,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
    gos,mos,dos,
    innom,
    nomdok_uos.ravno_filtr(),
    rp.podr.ravno_atoi(),
    rp.matot.ravno_atoi(),
    1,
    cena,
    0.,
    0.,
    rp.kodop_uos.ravno(),
    iceb_t_getuid(),
    time(NULL),
    cena,
    0.,
    0.,
    "");

    if(sql_zapis(strsql,1,0) != 0)
     continue;


    sprintf(strsql,"insert into Uosinp (innom,mes,god,shetu,hzt,hau,hna,popkof,soso,ktoz,vrem,hnaby,popkofby) \
   values (%d,%d,%d,'%s','%s','%s','%s',%.2f,%d,%d,%ld,'%s',%.2f)",
    innom,mos,gos,
    rp.shetu_uos.ravno(),
    rp.hztz.ravno(),
    rp.hauz.ravno(),
    rp.grup_nu.ravno(),
    1.,
    1,
    iceb_t_getuid(),
    time(NULL),
    rp.grup_bu.ravno(),
    1.);
      
    sql_zapis(strsql,1,0);
   }
    
 }

podpis(ff_prot);
fclose(ff_prot);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus("Протокол хода расчёта");

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);
}

