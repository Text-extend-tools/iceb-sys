/*$Id: ree_imp.c,v 5.54 2014/07/31 07:09:17 sasa Exp $*/
/*16.02.2017	30.08.2005	Белых А.И.	ree_imp.c
Импорт документов в реест налоговых накладных из подсистем бухгалтерского учёта.
*/
#include "buhg.h"
#include "rnnovd.h"
class ree_imp_rp
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str inn; /*индивидуальный налоговый номер*/
   class iceb_tu_str nnn; /*номер налоговой накладной*/

  ree_imp_rp()
   {
    clear();
   }
  void clear()
   {
    dat1.new_plus("");
    dat2.new_plus("");
    inn.new_plus("");
    nnn.new_plus("");
   }
 };
 
void ree_imp1(class ree_imp_rp *rkimp);
void ree_imp2(class ree_imp_rp *rkimp);
int rnnovd(int metka,class rnnovd_rek *nvd);
void ree_imp_ovd(class iceb_tu_str *viddok,const char *kodop,class SPISOK *spvd,class SPISOK *spko);


void ree_imp(int metka_tp) //1-импорт полученных 2-выданных
{
static class ree_imp_rp rkimp;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int kom=0;

VVOD VV(0);
if(metka_tp == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Импорт полученных документов"));
if(metka_tp == 2)
  VV.VVOD_SPISOK_add_ZAG(gettext("Импорт выданных документов"));

VV.VVOD_SPISOK_add_ZAG(gettext("Введите период за который нужно импортировать документы"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.............(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер.."));
VV.VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной......."));

VV.VVOD_SPISOK_add_data(rkimp.dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(rkimp.dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(rkimp.inn.ravno(),20); 
VV.VVOD_SPISOK_add_data(rkimp.nnn.ravno(),20); 

naz:;

 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("контрагент"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

rkimp.dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
rkimp.dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
rkimp.inn.new_plus(VV.VVOD_SPISOK_return_data(2));
rkimp.nnn.new_plus(VV.VVOD_SPISOK_return_data(3));

class iceb_tu_str kod("");
class iceb_tu_str naim("");
switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK3:
   if(dirkontr(&kod,&naim,1) == 0)
    {
     sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno());
     if(readkey(strsql,&row,&cur) == 1)
      {
       VV.data_plus(2,row[0]);
      }
    }
   clear();
   goto naz;
   
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,rkimp.dat1.ravno(),&dk,&mk,&gk,rkimp.dat2.ravno()) != 0)
 goto naz;

short d=dn;
short m=mn;
short g=gn;

//Проверяем блокировку дат
while(sravmydat(1,m,g,1,mk,gk) <= 0)
 {
  if(iceb_t_pbpds(m,g) != 0)
   goto naz;
  dpm(&d,&m,&g,3);  

 }

if(metka_tp == 1)
  ree_imp1(&rkimp);
if(metka_tp == 2)
  ree_imp2(&rkimp);

}
/***********************************/
/*импорт полученных налоговых накладных*/
/*****************************************/

void ree_imp1(class ree_imp_rp *rkimp)
{
class iceb_tu_str viddok("");
class rnnovd_rek ovd;
char strsql[2048];
int vt=0;

clear();
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,rkimp->dat1.ravno(),&dk,&mk,&gk,rkimp->dat2.ravno()) != 0)
 return;

rnnovd(1,&ovd); /*Читаем настройки для определения вида документа*/
iceb_t_poldan("Код группы возвратная тара",&vt,"matnast.alx");

class iceb_tu_str kodop_storn; /*Коды операций сторнирования*/
iceb_t_poldan("Коды операций сторнирования для приходов",&kodop_storn,"matnast.alx");

SQLCURSOR cur;
SQL_str row;

/*составляем список накладных имеющих код группы*/
sprintf(strsql,"select datd,nnn,mi,gr,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and gr <> ''",gn,mn,dn,gk,mk,dk);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SPISOK spdok;
class masiv_din_int gr_dok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok.plus(strsql);
  gr_dok.plus(row[3]);
 }

/*составляем список накладных имеющих вид документа*/
sprintf(strsql,"select datd,nnn,mi,vd,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and vd <> ''",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SPISOK spdok_vd;
class SPISOK sviddok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok_vd.plus(strsql);
  sviddok.plus(row[3]);
 }
class iceb_tu_str sql_zapros;
//Удаляем все накладные загруженные за этот период
sprintf(strsql,"delete from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0",gn,mn,dn,gk,mk,dk);

sql_zapros.plus(strsql);

if(rkimp->inn.getdlinna() > 1)
 {
  sprintf(strsql," and inn='%s'",rkimp->inn.ravno());
  sql_zapros.plus(strsql);
 }
if(rkimp->nnn.getdlinna() > 1)
 {
  sprintf(strsql," and nnn='%s'",rkimp->nnn.ravno());
  sql_zapros.plus(strsql);
 }

sql_zapis(sql_zapros.ravno(),0,0);

printw("%s\n",gettext("Материальный учёт"));
refresh();

sprintf(strsql,"select datpnn,kontr,nomnn,sklad,nomd,datd,pn,kodop from Dokummat \
where datpnn >= '%d-%d-%d' and datpnn <= '%d-%d-%d' and tip=1 and \
nomnn <> '%s' order by datpnn asc",gn,mn,dn,gk,mk,dk,"\0");
//printw("%s\n",strsql);
//refresh();
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row1;
SQL_str row2;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  printw("%s\n",gettext("В \"Материальном учете\" не найдено ни одного документа"));
 }

int kolstr1=0;
class iceb_tu_str metka(ICEB_MP_MATU);
short d,m,g;
int mnds=0;
double nds=0;
double suma,sumabn,sumkor;
double sum;
double summ[4];
double ndsm[4];

class iceb_tu_str inn("");
class iceb_tu_str nkontr("");
float pnds=0.;
short dv,mv,gv;
int storn=1;
class iceb_tu_str koment("");
class iceb_tu_str kod_gr("");
int nomer_dok=0;
double kolih=0.;
//int nomer_kol=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[2],0,0) != 0)
   continue;
     
  rsdat(&d,&m,&g,row[5],2);
  pnds=atof(row[6]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {

    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;

    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  printw("%s %-6s %-6s %s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);
  
  int lnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    lnds=atoi(row1[0]);
    if(lnds == 4)
     pnds=7.;
   }  

  /*Узнаем основание для того чтобы записать его в комментарий*/
  koment.new_plus("");
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=3",g,row[4],row[3]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    koment.new_plus(row1[0]);
  
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    mnds=atoi(row1[0]);

  nds=0.;

  if(mnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      nds=atof(row1[0]);
   }

  /*Условие продажи - нужен вид документа*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=7",g,row[4],row[3]);
  viddok.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    if(pole2(row1[0],':') == 0)
     {
//      if(rnnvd(1,row1[0]) > 0)
        viddok.new_plus(row1[0]);
     }
    else
     polen(row1[0],&viddok,4,':');
   }

  dv=mv=gv=0;  
  /*Узнаём дату выписки документа*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=14",g,row[4],row[3]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   rsdat(&dv,&mv,&gv,row1[0],1);
       
  

  sumzap1(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih);
  
  storn=1;
  if(proverka(kodop_storn.ravno(),row[7],0,1) == 0)
   storn=-1;
  
  if(mnds == 0 && nds == 0.)
    nds=(suma+sumkor)*pnds/100.;

  rsdat(&d,&m,&g,row[0],2);

  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     VVOD SOOB(1);
     SOOB.VVOD_SPISOK_add_MD(gettext("Материальный учёт"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     SOOB.VVOD_SPISOK_add_MD(strsql);

     SOOB.VVOD_SPISOK_add_MD(gettext("Не ввели индивидуальный налоговый номер!"));
     soobshw(&SOOB,stdscr,-1,-1,0,1);
    
   }
   
  sum=suma+sumkor+nds+sumabn;
  sum*=storn;
  
  if(nds != 0)
   nds*=storn;
   
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));

  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  if(mnds == 4) /* 7% на мед препараты */
   {
    summ[1]=sum;
    ndsm[2]=nds;

   }
#if 0
###########################################################3
  if(mnds < 3) 
   {
    nomer_kol=0;
    if(proverka(mu11_12.ravno(),row[7],0,1) == 0)
     nomer_kol=1;
    if(proverka(mu13_14.ravno(),row[7],0,1) == 0)
     nomer_kol=2;
    if(proverka(mu15_16.ravno(),row[7],0,1) == 0)
     nomer_kol=3;
      
    if(nomer_kol != 1)
     {
      summ[nomer_kol]=sum;
      ndsm[nomer_kol]=nds;
     }
    else
     {
      double sum12=sum*procent_med_pr/100.;
      sum12=okrug(sum12,0.01);
      double sum11=sum-sum12;
      double nds12=nds*procent_med_pr/100.;
      nds12=okrug(nds12,0.01);
      double nds11=nds-nds12;
      
      summ[0]=sum11;
      ndsm[0]=nds11;
      summ[1]=sum12;
      ndsm[1]=nds12;
     }
   }
  else
   {
    if(SRAV("400000000000",inn.ravno(),0) == 0)/*не даёт налоговых накладных*/
     {
      summ[1]=sum;
      ndsm[1]=0.;
     }    
    else
     {
      summ[0]=sum;
      ndsm[0]=nds;
     }
   }
##############################################
#endif  
  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
     kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
     kod_gr.new_plus("");

  /*определяем вид документа*/
  if(viddok.getdlinna() <= 1)
    ree_imp_ovd(&viddok,row[7],&ovd.muvd,&ovd.muso);

  /*Смотрим вид документа если был введён*/      
  if(viddok.getdlinna() <= 1)
   if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));

  zapvree1(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),1,row[5],row[4],koment.ravno(),kod_gr.ravno(),row[7],dv,mv,gv,viddok.ravno());


 }

printw("%s\n",gettext("Учёт услуг"));
GDITE();

sprintf(strsql,"select datpnn,kontr,nomnn,podr,nomd,sumkor,\
nds,datd,sumnds,pn,kodop,datdp,osnov,uslpr from Usldokum where datpnn >= '%d-%d-%d' and \
datpnn <= '%d-%d-%d' and tp=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  printw("%s\n",gettext("В \"Учете услуг\" не найдено ни одного документа"));
 }
kolstr1=0;
metka.new_plus(ICEB_MP_USLUGI);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  strzag(lines-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[2],0,0) != 0)
   continue;

  pnds=atof(row[9]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");
  printw("%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);


  sumkor=atof(row[5]);
  mnds=atoi(row[6]);
  nds=0.;
  if(mnds == 0)
   nds=atof(row[8]);
   
  rsdat(&d,&m,&g,row[7],2);
  sumzap1u(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,1);

  sum=suma+sumkor+nds+sumabn;
//  printw("sum=%f\n",sum);
//  OSTANOV();
  if(mnds == 0)
   {
    if(nds == 0.)
     {
      nds=(suma+sumkor)*pnds/100.;
      sum=suma+sumkor+nds+sumabn;
     }     
    else
     {
      //Пересчитываем обратным счетом базу налогообложения
      sum=nds*((100.+pnds)/pnds);
     }
   }

  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     VVOD SOOB(1);
     SOOB.VVOD_SPISOK_add_MD(gettext("Учёт услуг"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     SOOB.VVOD_SPISOK_add_MD(strsql);

     SOOB.VVOD_SPISOK_add_MD(gettext("Не ввели индивидуальный налоговый номер!"));
     soobshw(&SOOB,stdscr,-1,-1,0,1);
    
   }

  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));
  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  /*mnds == 4 7% на мед препараты в услугах не может быть*/


  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
     kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
     kod_gr.new_plus("");
  rsdat(&d,&m,&g,row[0],2);
  rsdat(&dv,&mv,&gv,row[11],2);

  /*определяем вид документа*/
  viddok.new_plus("");
  if(pole2(row[13],':') == 0 && row[13][0] != '\0')
   {
//    if(rnnvd(1,row[13]) > 0)
      viddok.new_plus(row[13]);
   }
  else
   polen(row[13],&viddok,4,':');
  
  if(viddok.getdlinna() <= 1) 
   {
    ree_imp_ovd(&viddok,row[10],&ovd.uuvd,&ovd.uuso);
    /*Смотрим вид документа если был введён*/      
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }  
   }
  zapvree1(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),2,row[7],row[4],row[12],kod_gr.ravno(),row[10],dv,mv,gv,viddok.ravno());

 }

printw("%s\n",gettext("Учёт основных средств"));
GDITE();

sprintf(strsql,"select datpnn,kontr,podr,nomnn,nomd,\
nds,datd,pn,kodop,dvnn,osnov from Uosdok where datpnn >= '%04d-%d-%d' and \
datpnn <= '%04d-%d-%d' and tipz=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
  printw("%s\n",gettext("В \"Учете основных средств\" не найдено ни одного документа"));

kolstr1=0;
metka.new_plus(ICEB_MP_UOS);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  strzag(lines-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[3],0,0) != 0)
   continue;

  pnds=atof(row[7]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");
  printw("%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);

  mnds=atoi(row[5]);
  rsdat(&d,&m,&g,row[6],2);
  sumzapuos(d,m,g,row[4],&suma,&sumabn,mnds,1);
  nds=0.;
  if(mnds == 0)  
    nds=suma*pnds/100.;

  rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     VVOD SOOB(1);
     SOOB.VVOD_SPISOK_add_MD(gettext("Учёт основных средств"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[3]);
     SOOB.VVOD_SPISOK_add_MD(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     SOOB.VVOD_SPISOK_add_MD(strsql);

     SOOB.VVOD_SPISOK_add_MD(gettext("Не ввели индивидуальный налоговый номер!"));
     soobshw(&SOOB,stdscr,-1,-1,0,1);
    
   }

  sum=suma+nds+sumabn;
  rsdat(&d,&m,&g,row[0],2);

  rsdat(&dv,&mv,&gv,row[9],2);
  
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));
  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  /*mnds == 4 7% на мед препараты не может быть*/

#if 0
######################################################333
  if(mnds < 3)
   {
    nomer_kol=0;
    if(proverka(uos11_12.ravno(),row[8],0,1) == 0)
     nomer_kol=1;
    if(proverka(uos13_14.ravno(),row[8],0,1) == 0)
     nomer_kol=2;
    if(proverka(uos15_16.ravno(),row[8],0,1) == 0)
     nomer_kol=3;
//    summ[nomer_kol]=sum;
//    ndsm[nomer_kol]=nds;

    if(nomer_kol != 1)
     {
      summ[nomer_kol]=sum;
      ndsm[nomer_kol]=nds;
     }
    else
     {
      double sum12=sum*procent_med_pr/100.;
      sum12=okrug(sum12,0.01);
      double sum11=sum-sum12;
      double nds12=nds*procent_med_pr/100.;
      nds12=okrug(nds12,0.01);
      double nds11=nds-nds12;
      
      summ[0]=sum11;
      ndsm[0]=nds11;
      summ[1]=sum12;
      ndsm[1]=nds12;
     }
   }
  else
   {
    if(SRAV("400000000000",inn.ravno(),0) == 0)/*не даёт налоговых накладных*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }
############################3
#endif

  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
     kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
     kod_gr.new_plus("");
  /*определяем вид документа*/
  ree_imp_ovd(&viddok,row[8],&ovd.uosvd,&ovd.uosso);

  /*Смотрим вид документа если был введён*/      
  if(viddok.getdlinna() <= 1)
   {
    sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn.ravno());
    if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));
   }  
  zapvree1(d,m,g,row[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),3,row[6],row[4],row[10],kod_gr.ravno(),row[8],dv,mv,gv,viddok.ravno());
 }

printw("%s\n",gettext("Учёт командировочных расходов"));
GDITE();

sprintf(strsql,"select nomd,datd,datao,vkom from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d' order by datao asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
  printw("%s\n",gettext("В \"Учете командировочных расходов\" не найдено ни одного документа"));

kolstr1=0;
int kolstr2=0;
double strah_sbor=0.;
metka.new_plus(ICEB_MP_UKR);

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
  strzag(lines-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select cena,snds,kdrnn,ndrnn,dvnn,kodr,ss from Ukrdok1 \
where datd='%s' and nomd='%s' and kdrnn <> ''",row[1],row[0]);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }  

  while(cur1.read_cursor(&row1) != 0)
   {
    dv=mv=gv=0;

    if(proverka(rkimp->nnn.ravno(),row1[3],0,0) != 0)
     continue;

    //Узнаём индивидуальный налоговый номер и наименование контрагента
    nkontr.new_plus("");
    sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row1[2]);
    
    if(readkey(strsql,&row2,&cur2) == 1)
     {
      if(proverka(rkimp->inn.ravno(),row2[1],0,0) != 0)
       continue;
      if(row2[2][0] == '\0')
        nkontr.new_plus(row2[0]);
      else
        nkontr.new_plus(row2[2]);
      inn.new_plus(row2[1]);
     }
    else
      inn.new_plus("");
    printw("%s %-6s %-6s %-6s %-6s\n",metka.ravno(),row1[0],row1[1],row1[2],row1[3]);
    nds=atof(row1[1]);
    sum=atof(row1[0])+nds;
    strah_sbor=atof(row1[6]);
    memset(summ,'\0',sizeof(summ));
    memset(ndsm,'\0',sizeof(ndsm));
    
//    if(nds != 0.)
    if(nds > 0.009)
     {
      summ[0]=sum;
      ndsm[0]=nds;
     }
    else
     {
      if(SRAV("400000000000",inn.ravno(),0) == 0)/*не даёт налоговых накладных*/
       {
        summ[0]=sum;
        ndsm[0]=0.;
       }    
      else
        summ[1]=sum;
     }

    /*Ищем код группы*/
    sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn.ravno());
    if((nomer_dok=spdok.find(strsql)) >= 0)
      kod_gr.new_plus(gr_dok.ravno(nomer_dok));
    else
      kod_gr.new_plus("");
    rsdat(&d,&m,&g,row[2],2);

    if(nds != 0. && inn.ravno()[0] == '\0')
     {
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("Учёт командировочных расходов"));
       sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
       SOOB.VVOD_SPISOK_add_MD(strsql);

       sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row1[3]);
       SOOB.VVOD_SPISOK_add_MD(strsql);

       sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row1[2],nkontr.ravno());
       SOOB.VVOD_SPISOK_add_MD(strsql);

       SOOB.VVOD_SPISOK_add_MD(gettext("Не ввели индивидуальный налоговый номер!"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
      
     }

    rsdat(&dv,&mv,&gv,row1[4],2);

    /*определяем вид документа -вместо кода операции код расхода*/
    ree_imp_ovd(&viddok,row1[5],&ovd.ukrvd,&ovd.ukrsr);

    /*Смотрим вид документа если был введён*/      
    sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn.ravno());
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|4",row[1],row[0]);
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
         viddok.new_plus(sviddok.ravno(nomer_dok));
     }    
    zapvree1(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
    inn.ravno(),nkontr.ravno(),4,row[1],row[0],"",kod_gr.ravno(),row[3],dv,mv,gv,viddok.ravno());

    if(strah_sbor != 0.)
     {
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));
      summ[1]=strah_sbor;      
      zapvree1(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn.ravno(),nkontr.ravno(),4,row[1],row[0],"",kod_gr.ravno(),row[3],dv,mv,gv,viddok.ravno());
     }    
   }
 }

printw("%s\n",gettext("Главная книга"));
GDITE();
class iceb_tu_str shet_nds("");
          
if(iceb_t_poldan("Дебеты счетов для импорта в реест полученных накладных",&shet_nds,"rnn_nast.alx") == 0)
 {
  printw("Счета=%s\n",shet_nds.ravno());
  if(shet_nds.getdlinna() > 1)
   {
    
    sprintf(strsql,"select datp,sh,shk,kodkon,deb,komen from Prov where datp >= '%04d-%d-%d' and \
    datp <= '%04d-%d-%d' and deb <> 0. and komen like 'нн%%' order by datp asc",
    gn,mn,dn,gk,mk,dk);
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

    if(kolstr == 0)
      printw("%s\n",gettext("В \"Главной книге \" не найдено нужных проводок"));

    kolstr1=0;
    metka.new_plus(gettext("ГК"));
    class iceb_tu_str nomer_nn("");
    class iceb_tu_str koment;
    while(cur.read_cursor(&row) != 0)
     {
//      printw("%s-%s %-6s %-6s %s\n",__FUNCTION__,metka,row[0],row[1],row[2]);


      strzag(lines-1,0,kolstr,++kolstr1);
      if(proverka(shet_nds.ravno(),row[1],1,0) != 0)
       {
        continue;
       }
      dv=mv=gv=0;
      //Узнаём индивидуальный налоговый номер и наименование контрагента
      nkontr.new_plus("");
      sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[3]);
      
      if(readkey(strsql,&row2,&cur2) == 1)
       {
        if(proverka(rkimp->inn.ravno(),row2[1],0,0) != 0)
         continue;
        if(row2[2][0] == '\0')
          nkontr.new_plus(row2[0]);
        else
          nkontr.new_plus(row2[2]);
        inn.new_plus(row2[1]);
       }
      else
       inn.new_plus("");      
      rsdat(&d,&m,&g,row[0],2);
      nds=atof(row[4]);
      
      /*Номер налоговой накладной*/
      if(polen(row[5],strsql,sizeof(strsql),1,' ') != 0)
       {
        printw("Не правильно введён комментраий!\n");
        OSTANOV();
        continue;
       }
       
      nomer_nn.new_plus(iceb_tu_adrsimv(2,strsql));

      if(proverka(rkimp->nnn.ravno(),nomer_nn.ravno(),0,0) != 0)
       continue;      

      printw("%s %-6s %-6s %-6s %-6s %10s %s\n",metka.ravno(),iceb_tu_datzap(row[0]),row[1],row[2],row[3],row[4],row[5]);

      /*Дата выписки налоговой накладной*/
      if(polen(row[5],strsql,sizeof(strsql),2,' ') != 0)
       {
        printw("В комментарии не найдена дата выписки налоговой накладной !\n");
        OSTANOV();
        continue;
       }
      if(rsdat(&dv,&mv,&gv,strsql,1) != 0)
       {
        
        printw("В комментарии не правильно введена дата выписки налоговой накладной!\n");
        printw("%s\n",strsql);
        OSTANOV();
        continue;
       }     

      if(polen(row[5],&viddok,3,' ') != 0)
       viddok.new_plus("ПН");
      
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));

      sum=nds*100./20.+nds;
      summ[0]=okrug(sum,0.01);
      ndsm[0]=nds;
      
      koment.new_plus(row[1]);
      koment.plus(" ");
      koment.plus(row[2]);
      koment.plus(" ");
      koment.plus(row[5]);

      /*Ищем код группы*/
      sprintf(strsql,"%s|%s|5|%s",row[1],nomer_nn.ravno(),inn.ravno());
      if((nomer_dok=spdok.find(strsql)) >= 0)
         kod_gr.new_plus(gr_dok.ravno(nomer_dok));
      else
         kod_gr.new_plus("");
      zapvree1(d,m,g,nomer_nn.ravno(),summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn.ravno(),nkontr.ravno(),5,"","",koment.ravno(),kod_gr.ravno(),"",dv,mv,gv,viddok.ravno());

     }
   }
 }
OSTANOV(); 
}
/***********************************/
/*импорт выданных налоговых накладных*/
/*****************************************/

void ree_imp2(class ree_imp_rp *rkimp)
{
class iceb_tu_str viddok("");
class rnnovd_rek ovd;
char strsql[512];

clear();
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,rkimp->dat1.ravno(),&dk,&mk,&gk,rkimp->dat2.ravno()) != 0)
 return;

rnnovd(2,&ovd); /*Читаем настройки для определения вида документа*/

int vt=0;
iceb_t_poldan("Код группы возвратная тара",&vt,"matnast.alx");

class iceb_tu_str kodop_storn; /*Коды операций сторнирования*/
iceb_t_poldan("Коды операций сторнирования для расходов",&kodop_storn,"matnast.alx");

SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row,row1;

/*составляем список накладных имеющих код группы*/
sprintf(strsql,"select datd,nnn,mi,gr,inn from Reenn where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and gr <> ''",gn,mn,dn,gk,mk,dk);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SPISOK spdok;
class masiv_din_int gr_dok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok.plus(strsql);
  gr_dok.plus(row[3]);
 }

/*составляем список накладных имеющих вид документа*/
sprintf(strsql,"select datd,nnn,mi,vd,inn from Reenn where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and vd <> ''",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SPISOK spdok_vd;
class SPISOK sviddok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok_vd.plus(strsql);
  sviddok.plus(row[3]);
 }

class iceb_tu_str sql_zapros;
//Удаляем все накладные загруженные за этот период
sprintf(strsql,"delete from Reenn where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0",gn,mn,dn,gk,mk,dk);

sql_zapros.plus(strsql);

if(rkimp->inn.getdlinna() > 1)
 {
  sprintf(strsql," and inn='%s'",rkimp->inn.ravno());
  sql_zapros.plus(strsql);
 }
if(rkimp->nnn.getdlinna() > 1)
 {
  sprintf(strsql," and nnn='%s'",rkimp->nnn.ravno());
  sql_zapros.plus(strsql);
 }

sql_zapis(sql_zapros.ravno(),0,0);

printw("%s\n",gettext("Материальный учет"));
refresh();

sprintf(strsql,"select datpnn,kontr,nomnn,sklad,nomd,datd,pn,kodop from Dokummat \
where datpnn >= '%04d-%d-%d' and datpnn <= '%04d-%d-%d' and tip=2 and \
nomnn <> '%s' order by datpnn asc,nomnn asc",gn,mn,dn,gk,mk,dk,"\0");
//printw("%s\n",strsql);
//refresh();
float pnds=0.;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 { 
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
  printw("%s\n",gettext("В \"Материальном учете\" не найдено ни одного документа"));

int kolstr1=0;
class iceb_tu_str metka(ICEB_MP_MATU);
short d,m,g;
int lnds=0;
double suma,sumabn,sumkor;
double nds=0.;
double sum=0.;
int mt=0;
class iceb_tu_str inn("");
class iceb_tu_str nkontr("");
int storn=1;
class iceb_tu_str koment("");
class iceb_tu_str kod_gr("");
int nomer_dok=0;
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(lines-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[2],0,0) != 0)
   continue;  

  rsdat(&d,&m,&g,row[5],2);
  pnds=atof(row[6]);
  
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
   inn.new_plus("");
  printw("%s %-6s %-6s %s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);
  
  /*Узнаем основание для того чтобы записать его в комментарий*/
  koment.new_plus("");
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=3",g,row[4],row[3]);

  viddok.new_plus("");
  /*Условие продажи - нужен вид документа*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=7",g,row[4],row[3]);
  viddok.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    if(pole2(row1[0],':') == 0)
     {
//      if(rnnvd(2,row1[0]) > 0)
        viddok.new_plus(row1[0]);
     }
    else
     polen(row1[0],&viddok,4,':');
   }

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    koment.new_plus(row1[0]);

  /*Узнаем НДС документа*/
  lnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    lnds=atoi(row1[0]);
    if(lnds == 4)
     pnds=7.;
   }  

  sumzap1(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,lnds,vt,&sumkor,2,&kolih);

  storn=1;
  if(proverka(kodop_storn.ravno(),row[7],0,1) == 0)
   storn=-1;

  nds=0.;
  if(lnds == 0)
    nds=(suma+sumkor)*pnds/100.;

  sum=(suma+sumkor+sumabn+nds)*storn;

  if(nds != 0)
   nds*=storn;
     
  if(lnds == 0)
   mt=0;
  if(lnds == 1)
   mt=1;
  if(lnds == 2)
   mt=3;
  if(lnds == 3)
   mt=2;

  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
     kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
     kod_gr.new_plus("");
  /*определяем вид документа*/
  if(viddok.getdlinna() <= 1)
    ree_imp_ovd(&viddok,row[7],&ovd.muvd,&ovd.muso);

  /*Смотрим был ли введён вид документа*/
  if(viddok.getdlinna() <= 1)
   {
    sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn.ravno());
    if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));
   }
  rsdat(&d,&m,&g,row[0],2);
  zapvree(d,m,g,row[2],sum,nds,mt,inn.ravno(),nkontr.ravno(),1,row[5],row[4],koment.ravno(),kod_gr.ravno(),row[7],viddok.ravno(),"0000.000.00");

 }

printw("%s\n",gettext("Учёт услуг"));
GDITE();

sprintf(strsql,"select datpnn,kontr,nomnn,podr,nomd,sumkor,\
nds,datd,pn,kodop,osnov,uslpr from Usldokum where datpnn >= '%04d-%d-%d' and \
datpnn <= '%04d-%d-%d' and tp=2 and nomnn <> '%s' order by datpnn asc,nomnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
  printw("%s\n",gettext("В \"Учете услуг\" не найдено ни одного документа"));

kolstr1=0;
metka.new_plus(ICEB_MP_USLUGI);
while(cur.read_cursor(&row) != 0)
 {
  strzag(lines-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[2],0,0) != 0)
   continue;

  pnds=atof(row[8]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");
  printw("%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);

  sumkor=atof(row[5]);
  lnds=atoi(row[6]);
  rsdat(&d,&m,&g,row[7],2);
  sumzap1u(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,lnds,2);

  nds=0.;
  if(lnds == 0)
     nds=(suma+sumkor)*pnds/100.;

  sum=suma+sumkor+sumabn+nds;

  if(lnds == 0)
   mt=0;
  if(lnds == 1)
   mt=1;
  if(lnds == 2)
   mt=3;
  if(lnds == 3)
   mt=2;

  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
    kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
    kod_gr.new_plus("");
  /*определяем вид документа*/
  viddok.new_plus("");
  if(pole2(row[11],':') == 0 && row[11][0] != '\0')
   {
//    if(rnnvd(2,row[11]) > 0)
     viddok.new_plus(row[11]);
   }
  else
   polen(row[11],&viddok,4,':');

  if(viddok.getdlinna() <= 1) 
   {
      ree_imp_ovd(&viddok,row[9],&ovd.uuvd,&ovd.uuso);
    /*Смотрим был ли введён вид документа*/
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }   
   }
  rsdat(&d,&m,&g,row[0],2);
  zapvree(d,m,g,row[2],sum,nds,mt,inn.ravno(),nkontr.ravno(),2,row[7],row[4],row[10],kod_gr.ravno(),row[9],viddok.ravno(),"0000-00-00");

 }

printw("%s\n",gettext("Учёт основных средств"));
GDITE();

sprintf(strsql,"select datpnn,kontr,nomnn,nomd,\
nds,datd,pn,kodop,osnov from Uosdok where datpnn >= '%04d-%d-%d' and \
datpnn <= '%04d-%d-%d' and tipz=2 and nomnn <> '%s' order by datpnn asc,nomnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
  printw("%s\n",gettext("В \"Учете основных средств\" не найдено ни одного документа"));

kolstr1=0;
metka.new_plus(ICEB_MP_UOS);
while(cur.read_cursor(&row) != 0)
 {
  strzag(lines-1,0,kolstr,++kolstr1);

  if(proverka(rkimp->nnn.ravno(),row[2],0,0) != 0)
   continue;
   
  pnds=atof(row[6]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(rkimp->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");

  printw("%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);

  lnds=atoi(row[4]);

  rsdat(&d,&m,&g,row[5],2);

  sumzapuos(d,m,g,row[3],&suma,&sumabn,lnds,2);

  nds=0.;
  if(lnds == 0)
     nds=suma*pnds/100.;

  sum=suma+sumabn+nds;

  if(lnds == 0)
   mt=0;
  if(lnds == 1)
   mt=1;
  if(lnds == 2)
   mt=3;
  if(lnds == 3)
   mt=2;

  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|3|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
    kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
    kod_gr.new_plus("");

  /*определяем вид документа*/
  ree_imp_ovd(&viddok,row[7],&ovd.uosvd,&ovd.uosso);

  /*Смотрим был ли введён вид документа*/
  if(viddok.getdlinna() <= 1)
   {
    sprintf(strsql,"%s|%s|3|%s",row[0],row[2],inn.ravno());
    if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));
   }
  rsdat(&d,&m,&g,row[0],2);

  zapvree(d,m,g,row[2],sum,nds,mt,inn.ravno(),nkontr.ravno(),3,row[5],row[3],row[8],kod_gr.ravno(),row[7],viddok.ravno(),"0000.000.00");

 }

printw("%s\n",gettext("Главная книга"));
GDITE();
class iceb_tu_str shet_nds("");
short dv,mv,gv;
SQL_str row2;
class SQLCURSOR cur2;
if(iceb_t_poldan("Кредиты счетов для импорта в реестр выданных накладных",&shet_nds,"rnn_nast.alx") == 0)
 {
  printw("Счета=%s\n",shet_nds.ravno());
  if(shet_nds.getdlinna() > 1)
   {
    
    sprintf(strsql,"select datp,sh,shk,kodkon,kre,komen from Prov where datp >= '%04d-%d-%d' and \
    datp <= '%04d-%d-%d' and kre <> 0. and komen like 'нн%%' order by datp asc",
    gn,mn,dn,gk,mk,dk);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

    if(kolstr == 0)
      printw("%s\n",gettext("В \"Главной книге \" не найдено нужных проводок"));
     
    kolstr1=0;
    metka.new_plus(gettext("ГК"));
    char nomer_nn[64];
    class iceb_tu_str koment;
    while(cur.read_cursor(&row) != 0)
     {
    //  printw("%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
      strzag(lines-1,0,kolstr,++kolstr1);
      if(proverka(shet_nds.ravno(),row[1],1,0) != 0)
       continue;
      dv=mv=gv=0;
      //Узнаём индивидуальный налоговый номер и наименование контрагента
      nkontr.new_plus("");
      sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[3]);
      
      if(readkey(strsql,&row2,&cur2) == 1)
       {
        if(proverka(rkimp->inn.ravno(),row2[1],0,0) != 0)
         continue;
        if(row2[2][0] == '\0')
          nkontr.new_plus(row2[0]);
        else
          nkontr.new_plus(row2[2]);
        inn.new_plus(row2[1]);
       }
      else
        inn.new_plus("");      
      rsdat(&d,&m,&g,row[0],2);
      nds=atof(row[4]);
      
      /*Номер налоговой накладной*/
      if(polen(row[5],strsql,sizeof(strsql),1,' ') != 0)
       {
        printw("Не правильно введён комментраий!\n");
        OSTANOV();
        continue;
       }
       
      strncpy(nomer_nn,iceb_tu_adrsimv(2,strsql),sizeof(nomer_nn));

      if(proverka(rkimp->nnn.ravno(),nomer_nn,0,0) != 0)
       continue;      

      printw("%s %-6s %-6s %-6s %-6s %10s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4],row[5]);

      /*Дата выписки налоговой накладной*/
      if(polen(row[5],strsql,sizeof(strsql),2,' ') != 0)
       {
        printw("В комментарии не найдена дата выписки налоговой накладной !\n");
        OSTANOV();
        continue;
       }
      if(rsdat(&dv,&mv,&gv,strsql,1) != 0)
       {
        
        printw("В комментарии не правильно введена дата выписки налоговой накладной!\n");
        printw("%s\n",strsql);
        OSTANOV();
        continue;
       }     

      if(polen(row[5],&viddok,3,' ') != 0)
       viddok.new_plus("ПН");

      sum=nds*100./20.+nds;
      sum=okrug(sum,0.01);
      
      koment.new_plus(row[1]);
      koment.plus(" ");
      koment.plus(row[2]);
      koment.plus(" ");
      koment.plus(row[5]);

      /*Ищем код группы*/
      sprintf(strsql,"%s|%s|5|%s",row[0],nomer_nn,inn.ravno());
      if((nomer_dok=spdok.find(strsql)) >= 0)
         kod_gr.new_plus(gr_dok.ravno(nomer_dok));
      else
         kod_gr.new_plus("");
      zapvree(d,m,g,nomer_nn,sum,nds,0,inn.ravno(),nkontr.ravno(),5,"0000-00-00","",koment.ravno(),kod_gr.ravno(),"",viddok.ravno(),"0000.000.00");

     }
   }
 }

OSTANOV();
}
/******************************/
/*Определение вида документа*/
/*****************************/

void ree_imp_ovd(class iceb_tu_str *viddok,const char *kodop,class SPISOK *spvd,class SPISOK *spko)
{
viddok->new_plus("");
for(int nom=0; nom < spvd->kolih(); nom++)
 if(proverka(spko->ravno(nom),kodop,0,1) == 0)
  {
   viddok->new_plus(spvd->ravno(nom));
   return;
  }
}
