/*$Id: sprozn.c,v 5.52 2014/07/31 07:09:17 sasa Exp $*/
/*11.06.2013	07.04.2003	Белых А.И.	sprozn.c
Списание товаров проданных с помощью программы rozn с карточек
*/
#include        <errno.h>
#include        "buhg.h"

int sproznd(short,short,short,short,short,short,int,const char*,int,FILE*);
int rizdel(const char *sklad,const char *kodm,double kolih,SPISOK *ZAPIS,masiv_din_double *KOLIH,masiv_din_double *SUMA);

extern double   okrcn;  /*Округление цены*/
extern double   okrg1;  /*Округление суммы*/

void		sprozn(int metkasp, //0-списание из xmu 1-автомномное списание
FILE *ff)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
short		dn,mn,gn;
short		dk,mk,gk;
char		strsql[512];
int		kom=0;
class iceb_tu_str kodop("");
SQL_str		row;
int		koldok=0;
struct  tm      *bf;
time_t          tmm;
char		imaf[64];
short		metkazf=0;


VVOD VV(0);

time(&tmm);
bf=localtime(&tmm);

if(metkasp == 0)
 {

  if(dat1.ravno()[0] == '\0')
   {
    sprintf(strsql,"%d.%d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
    dat1.new_plus(strsql);
    dat2.new_plus(strsql);
   }

  VV.VVOD_SPISOK_add_ZAG(gettext("Списание розничной продажи"));

  VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
  VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

  VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
  VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

  naz:;

  clear();

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("расчитать"),
  "F4",gettext("очистить"),
  "F10",gettext("выход"),NULL);

  kom=VV.vvod(0,1,1,-1,-1);

  move(LINES-1,0);

  dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
  dat2.new_plus(VV.VVOD_SPISOK_return_data(1));

  switch(kom)
   {
    case FK1:   /*Помощь*/
     GDITE();
     iceb_t_pdoc("matu2_6.txt");
     goto naz;

    case ESC:
    case FK10:
     return;

    case FK2:
    case PLU:
       break;

    case FK4: /*Очистить меню*/
      VV.VVOD_clear_data();  
      goto naz;

    default:
       goto naz;
   }


  if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
   goto naz;
 }
else
 {
  dn=0; mn=0; gn=0;
  dk=bf->tm_mday;
  mk=bf->tm_mon+1;
  dk=bf->tm_year+1900;
  
 }

sprintf(imaf,"%s%siceb_sr%02d%d.pr",g_get_tmp_dir(),G_DIR_SEPARATOR_S,bf->tm_mon+1,bf->tm_year+1900);
if(ff == NULL)
 {
 if((ff = fopen(imaf,"a")) == NULL)
  fprintf(stderr,"\nПрограмма xmu.\nОшибка открытия файла %s:%d\n",imaf,errno);
 else
  metkazf=1;
 }  
time(&tmm);
bf=localtime(&tmm);

if(metkasp == 0 && ff != NULL)
 {
  fprintf(ff,"----------------------------------------------\n");
  fprintf(ff,"Списание из xmu. Запущена %d %s Дата:%d.%d.%dг. Время:%02d:%02d:%02d\n",
  iceb_t_getuid(),iceb_t_getfioop(),bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,bf->tm_sec);

  fprintf(ff,"%s%d.%d.%d\n",gettext("Дата начала.(д.м.г).."),dn,mn,gn);
  fprintf(ff,"%s%d.%d.%d\n",gettext("Дата конца..(д.м.г).."),dk,mk,gk);

 }


if(iceb_t_poldan("Код операции списания розничной продажи",&kodop,"matnast.alx") != 0)
 {
  if(ff != NULL)
   {
    fprintf(ff,"Не найден \"Код операции списания розничной продажи\"\n");
    fclose(ff);
   }

  if(metkasp == 0)
   {
    iceb_t_soob("Не найден \"Код операции списания розничной продажи\"");
    goto naz;
   }

  return;
 }
SQLCURSOR cur;
//Проверяем есть ли этот код операции в расходах
sprintf(strsql,"select kod from Rashod where kod='%s'",kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  if(ff != NULL)
   {
    sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
    fprintf(ff,"%s\n",strsql);
    fclose(ff);
   }

  if(metkasp == 0)
   {
    sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }

  return;
  
 }

if(metkasp == 0)
 {
  clear();
  printw("%s %d.%d.%d => %d.%d.%d\n",
  gettext("Списание розничной продажи."),
  dn,mn,gn,dk,mk,gk);
  GDITE();
 }

koldok=sproznd(dn,mn,gn,dk,mk,gk,0,kodop.ravno(),metkasp,ff); //Списать товары с НДС
koldok+=sproznd(dn,mn,gn,dk,mk,gk,1,kodop.ravno(),metkasp,ff); //Списать товары без НДС
if(koldok == 0)
 {
  if(metkasp == 0)
   {
    iceb_t_soob(gettext("Не найдено ни одной записи !"));
   }
  if(ff != NULL)
    fprintf(ff,"%s\n",gettext("Не найдено ни одной записи !"));
  
 }
else
 if(koldok > 0)
  {
   if(metkasp == 0)
    {
     clearstr(LINES-1,0);
     VVOD DANET(1);
     DANET.VVOD_SPISOK_add_MD(gettext("Удалить загруженные записи ?"));

     if(danet(&DANET,2,stdscr) == 2)
      return;
    }

   if(metkasp == 0)
     sprintf(strsql,"delete from Roznica where \
datrp >= '%d-%d-%d' and  datrp <= '%d-%d-%d' and metka=1",
     gn,mn,dn,gk,mk,dk);
   else
     sprintf(strsql,"delete from Roznica where \
datrp <= '%d-%d-%d' and metka=1",
     gk,mk,dk);

   if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,"",strsql);
  } 

if(metkazf == 1)
 {
  time(&tmm);
  bf=localtime(&tmm);
  fprintf(ff,"Списание закончено. Дата:%d.%d.%dг. Время:%02d:%02d:%02d\n",
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,bf->tm_sec);
  fprintf(ff,"----------------------------------------------\n");
  fclose(ff);
 }
}

/*************************************/
/*Формирование документов на списание*/
/**************************************/

int sproznd(short dn,short mn,short gn,
short dk,short mk,short gk,
int nds, //0-с НДС 1-без НДС
const char *kodop,
int metkasp,
FILE *ff)
{
char 		strsql[1024];
int		kolstr=0;
SQL_str		row,row1;
int		skladz=0;
int		sklad=0;
class iceb_tu_str nomd("");
time_t		tmm;
int		metkapropuska=0;
double		cena=0.;
double		kolih=0.;
int		kodm=0;
class iceb_tu_str ei("");
double		ndss=0.;
int		koldok=0;
double		suma=0.;

//iceb_t_poldan("Н.Д.С.",bros,"matnast.alx");
//float pnds=ATOFM(bros);
float pnds=iceb_t_pnds();

if(metkasp == 0)
  sprintf(strsql,"select kodm,ei,kolih,cena,sklad from Roznica where \
datrp >= '%d-%d-%d' and  datrp <= '%d-%d-%d' and metka=1 and nds=%d. order by sklad asc",
  gn,mn,dn,gk,mk,dk,nds);
else
  sprintf(strsql,"select kodm,ei,kolih,cena,sklad from Roznica where \
datrp <= '%d-%d-%d' and metka=1 and nds=%d. order by sklad asc",
  gk,mk,dk,nds);

//printw("%s\n",strsql);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(metkasp == 0)
   msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,ff);

  return(-1);
 }
if(kolstr == 0)
  return(0);
memset(strsql,'\0',sizeof(strsql));  
if(nds == 0)
  sprintf(strsql,"*%02d%02d/1",dk,mk);
if(nds == 1)
  sprintf(strsql,"*%02d%02d/2",dk,mk);
nomd.new_plus(strsql);
int	kolstr1=0;

SPISOK ZAPIS(0);
masiv_din_double KOLIH(0);
masiv_din_double SUMA(0);
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);

  strzag(LINES-1,0,kolstr,++kolstr1);

  kolih=atof(row[2]);
  if(rizdel(row[4],row[0],kolih,&ZAPIS,&KOLIH,&SUMA) != 0)
    continue;

  sprintf(strsql,"%s|%s|%s",
  row[4],row[0],row[1]);
  
  cena=atof(row[3]);
  cena=okrug(cena,okrg1);
  
  suma=kolih*cena;
  suma=okrug(suma,okrg1); //Определяем сумму для того чтобы вычислить среднюю цену если у материалла были разные цены
  
  if((nomer=ZAPIS.find(strsql)) < 0)
    ZAPIS.plus(strsql);  

  
  KOLIH.plus(kolih,nomer);
  SUMA.plus(suma,nomer);

}

int kolzap=ZAPIS.kolih();
SQLCURSOR cur1;

for(nomer=0; nomer < kolzap; nomer++)
 {
  polen(ZAPIS.ravno(nomer),strsql,sizeof(strsql),1,'|');
  
  sklad=atoi(strsql);

  if(skladz != sklad)
   {
    if(skladz  > 0 )
     {
      podtdok1(dk,mk,gk,sklad,nomd.ravno(),2,dk,mk,gk,1);
      podvdok(dk,mk,gk,nomd.ravno(),sklad);
     }     

    int nomerdok=0;    
    class iceb_tu_str nomerdokch(nomd.ravno());
//    strcpy(nomerdokch,nomd.ravno());

nazad:;

    //Проверяем может такой документ уже введен
    sprintf(strsql,"select nomd from Dokummat1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and sklad=%d and nomd='%s'",
    gk,1,1,gk,12,31,sklad,nomerdokch.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {

  //    sprintf(nomerdokch,"%s/%d",nomd.ravno(),++nomerdok);
      nomerdokch.plus("/");
      nomerdokch.plus(++nomerdok);
      goto nazad;      
     }

    nomd.new_plus(nomerdokch.ravno());

    time(&tmm);
    if(zap_s_mudok(2,dk,mk,gk,sklad,"00",nomd.ravno(),"",kodop,0,0,"",0,0,0,1,"00",pnds) != 0)
     {
      if(metkasp == 0)
       msql_error(&bd,"",strsql);
      msql_error(&bd,"",strsql,ff);
      return(0);
     }

    koldok++;
    
    if(nds == 0)
     {
      sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%s')",
      gk,sklad,nomd.ravno(),11,"0");
      ndss=pnds;
     }

    if(nds == 1)
     {
      sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%s')",
      gk,sklad,nomd.ravno(),11,"3");
      ndss=0.;
     }
     
    if(sql_zap(&bd,strsql) != 0)
     {
      if(metkasp == 0)
        msql_error(&bd,"",strsql);
      msql_error(&bd,"",strsql,ff);
     }
    sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%d')",
    gk,sklad,nomd.ravno(),4,iceb_t_getuid());

    if(sql_zap(&bd,strsql) != 0)
     {
      if(metkasp == 0)
       msql_error(&bd,"",strsql);
      msql_error(&bd,"",strsql,ff);
     }

    skladz=sklad;
    metkapropuska=0;

    if(metkasp == 0)
     {
      printw("%s %d %s: %s\n",
      gettext("Склад"),sklad,
      gettext("Документ"),
      nomd.ravno());
      refresh();
     }
    if(ff != NULL)
      fprintf(ff,"%s %d %s: %s\n",
      gettext("Склад"),sklad,
      gettext("Документ"),
      nomd.ravno());
    
   }

  if(metkapropuska == 1)
   continue;
  
  polen(ZAPIS.ravno(nomer),strsql,sizeof(strsql),2,'|');
  kodm=atoi(strsql);

  polen(ZAPIS.ravno(nomer),&ei,3,'|');


  kolih=KOLIH.ravno(nomer);
  suma=SUMA.ravno(nomer);
  cena=suma/kolih;
  cena=okrug(cena,okrcn);
  
  if(nds == 0) //Узнаем цену без ндс
     cena=cena-cena*pnds/(100.+pnds);

  cena=okrug(cena,okrcn);
  
  priv_k_kart(dk,mk,gk,nomd.ravno(),sklad,kodm,kolih,cena,"",ei.ravno(),ndss,0,0,"",0,"");

 }

podtdok1(dk,mk,gk,sklad,nomd.ravno(),2,dk,mk,gk,1);
podvdok(dk,mk,gk,nomd.ravno(),sklad);
return(koldok);

}

/**************************************/
/*Разуздование изделия                */
/**************************************/
// 0-если не изделие 1-изделие
int rizdel(const char *sklad,const char *kodm,double kolih,SPISOK *ZAPIS,masiv_din_double *KOLIH,masiv_din_double *SUMA)
{
char strsql[512];
SQL_str  row;
SQLCURSOR cur;

sprintf(strsql,"select distinct kodi from Specif where kodi=%s",kodm);
if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
 return(0);
class iceb_t_razuz_data data;
data.kod_izd=atoi(kodm);
int kolndet=0;

if((kolndet=iceb_t_razuz_kod(&data)) == 0)
  return(0);  

int nomer=0;
int nomer1=0;
double cena,suma;
class iceb_tu_str ei("");
double kolmat;
int kodmat;
kolndet=data.kod_det_ei.kolih();
for(nomer=0; nomer < kolndet;nomer++)
 {
  if(data.metka_mu.ravno(nomer) == 1)
   continue;

  polen(data.kod_det_ei.ravno(nomer),&kodmat,1,'|');
  polen(data.kod_det_ei.ravno(nomer),&ei,2,'|');
  kolmat=data.kolih_det_ei.ravno(nomer);

  cena=suma=0.;

  /*Читаем цену материалла*/
  sprintf(strsql,"select cenapr from Material where kodm=%d",
  kodmat);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    cena=atof(row[0]);

  sprintf(strsql,"%s|%d|%s",sklad,kodmat,ei.ravno());
  
  suma=kolmat*kolih*cena;
  suma=okrug(suma,okrg1); //Определяем сумму для того чтобы вычислить среднюю цену если у материалла были разные цены
  
  if((nomer1=ZAPIS->find(strsql)) < 0)
    ZAPIS->plus(strsql);  

  
  KOLIH->plus(kolmat*kolih,nomer1);
  SUMA->plus(suma,nomer1);
   
 }
//OSTANOV();

return(1);

}
