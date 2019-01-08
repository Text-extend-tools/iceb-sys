/*$Id: buhkom.c,v 5.42 2013/09/26 09:43:29 sasa Exp $*/
/*13.07.2015	29.12.2004	Белых А.И.	buhkom.c
Отчёт по введенным комментариям
*/
#include <errno.h>
#include        "buhg.h"

int prov_kom(class SPISOK *koment,char *row,int kol_koment);
void buhkom_sh(const char *shet,const char *naim,FILE *ff);
void buhkom_sh_deb(const char *shet,const char *naim,FILE *ff);
void buhkom_it(int metka_i,double deb,double kre,FILE *ff);
void buhkom_it_dk(int metka_i,double deb,FILE *ff);
void buhkom_sh_kre(const char *shet,const char *naim,FILE *ff);


int buhkom(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
int kom=0;
short dn,mn,gn;
short dk,mk,gk;
char strsql[1024];

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Отчёт по комментариям"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)..."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_6.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  case FK5:
    iceb_redfil("buhkom.alx",0);
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


/********************************************/
GDITE();

//Запоминаем список комментариев
int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='buhkom.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки buhkom.alx");
  return(1);
 }

class SPISOK koment;
char bros[1024];

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],bros,sizeof(bros),1,'|') != 0) 
   continue;
  koment.plus(bros);
 }
 

int kol_koment=koment.kolih();
if(kol_koment == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной настройки !"));
  goto naz;
 }

sprintf(strsql,"select sh,shk,deb,kre,komen from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and komen <> ''",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено проводок !"));
  goto naz;
 }
int nomer=0;
int kolstr1=0;
class SPISOK sh_shk_kom;
double deb,kre;
masiv_din_double debet;
masiv_din_double kredit;
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(proverka(sheta.ravno(),row[0],0,0) != 0)
    continue;
  
  if((nomer=prov_kom(&koment,row[4],kol_koment)) < 0)
   continue;

  deb=atof(row[2]);
  kre=atof(row[3]);
  
  sprintf(strsql,"%s|%s|%s",row[0],koment.ravno(nomer),row[1]);
  if((nomer=sh_shk_kom.find(strsql)) < 0)
    sh_shk_kom.plus(strsql);
  debet.plus(deb,nomer);
  kredit.plus(kre,nomer);    
 }

int kol_shet=sh_shk_kom.kolih();
if(kol_shet == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной проводки с указанными комментариями !"));
  return(1);
 }
GDITE();

class iceb_t_tmptab tabtmp;
const char *imatmptab={"buhkom"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(32) not null,\
kom char(112) not null,\
shk char(32) not null,\
deb double(12,2) not null,\
kre double(12,2) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

class iceb_tu_str sh("");
class iceb_tu_str komentt("");
class iceb_tu_str shk("");
for(int i=0; i < kol_shet ; i++)
 {
  polen(sh_shk_kom.ravno(i),&sh,1,'|');
  polen(sh_shk_kom.ravno(i),&komentt,2,'|');
  polen(sh_shk_kom.ravno(i),&shk,3,'|');
  
  sprintf(strsql,"insert into %s values ('%s','%s','%s',%.2f,%.2f)",
  imatmptab,
  sh.ravno(),
  komentt.ravno_filtr(),
  shk.ravno(),
  debet.ravno(i),
  kredit.ravno(i));
  
  sql_zapis(strsql,1,0);    

 }
SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
sprintf(strsql,"select * from %s order by sh asc,kom asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }




FILE *ff;

char imaf[64];
sprintf(imaf,"kom%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }
iceb_t_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,ff);

if(sheta.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta.ravno());

class iceb_tu_str shet("");
class iceb_tu_str shet_z("");
class iceb_tu_str shetk("");
class iceb_tu_str komentar("");
class iceb_tu_str komentar_z("");
double ideb=0.,ikre=0.;
double idebk=0.,ikrek=0.;
class iceb_tu_str naim("");

while(curtmp.read_cursor(&rowtmp) != 0)
 {

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
  deb=atof(rowtmp[3]);
  kre=atof(rowtmp[4]);
    
  if(SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it(0,idebk,ikrek,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it(0,idebk,ikrek,ff);
        idebk=ikrek=0.;
       }
      buhkom_it(1,ideb,ikre,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh(shet.ravno(),naim.ravno(),ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f %10.2f\n",
  iceb_tu_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_tu_kolbait(7,shetk.ravno()),shetk.ravno(),
  deb,kre);
  
  ideb+=deb;
  ikre+=kre;
  idebk+=deb;
  ikrek+=kre;

 }
buhkom_it(0,idebk,ikrek,ff);
buhkom_it(1,ideb,ikre,ff);

podpis(ff);
fclose(ff);


char imaf_deb[64];
sprintf(imaf_deb,"komd%d.lst",getpid());
if((ff = fopen(imaf_deb,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }
iceb_t_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,ff);

if(sheta.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta.ravno());


shet_z.new_plus("");
komentar_z.new_plus("");
//распечатываем дебет
curtmp.poz_cursor(0);
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  deb=atof(rowtmp[3]);
  if(deb == 0.)
   continue;

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
    
  if(SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it_dk(0,idebk,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it_dk(0,idebk,ff);
        idebk=ikrek=0.;
       }
      buhkom_it_dk(1,ideb,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh_deb(shet.ravno(),naim.ravno(),ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_tu_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_tu_kolbait(7,shetk.ravno()),shetk.ravno(),
  deb);
  
  ideb+=deb;
  idebk+=deb;

 }
buhkom_it_dk(0,idebk,ff);
buhkom_it_dk(1,ideb,ff);


podpis(ff);
fclose(ff);


char imaf_kre[64];
sprintf(imaf_kre,"komk%d.lst",getpid());
if((ff = fopen(imaf_kre,"w")) == NULL)
   {
    error_op_nfil(imaf_kre,errno,"");
    return(1);
   }
iceb_t_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,ff);

if(sheta.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta.ravno());


shet_z.new_plus("");
komentar_z.new_plus("");
ikrek=0.;
ikre=0.;
curtmp.poz_cursor(0);
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  kre=atof(rowtmp[4]);
  if(kre == 0.)
   continue;

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
    
  if(SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it_dk(0,ikrek,ff);
    ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(ikrek != 0.)
       {
        buhkom_it_dk(0,ikrek,ff);
        ikrek=0.;
       }
      buhkom_it_dk(1,ikre,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh_kre(shet.ravno(),naim.ravno(),ff);
     
    ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_tu_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_tu_kolbait(7,shetk.ravno()),shetk.ravno(),
  kre);
  
  ikre+=kre;
  ikrek+=kre;

 }
buhkom_it_dk(0,ikrek,ff);
buhkom_it_dk(1,ikre,ff);


podpis(ff);
fclose(ff);




//Распечатываем проводки


char imaf_prov[64];
sprintf(imaf_prov,"komp%d.lst",getpid());
if((ff = fopen(imaf_prov,"w")) == NULL)
 {
  error_op_nfil(imaf_prov,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk,ff);

if(sheta.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta.ravno());
short d,m,g;
class iceb_tu_str naikon("");
SQL_str row1;
SQLCURSOR cur1; 
kolstr1=0;
ideb=ikre=0.;
shet_z.new_plus("");
komentar_z.new_plus("");
class iceb_tu_str komentar_prov("");
curtmp.poz_cursor(0);

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kol_shet,++kolstr1);
//  iceb_pbar(data->bar,kol_shet,++kolstr1);
  
  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);

  if(SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      raspitog(idebk,ikrek,3,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        raspitog(idebk,ikrek,3,ff);
        idebk=ikrek=0.;
       }
      raspitog(ideb,ikre,4,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    fprintf(ff,"\n%s %s\n",shet.ravno(),naim.ravno());
    rapsap(ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }
    
  sprintf(strsql,"select * from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and sh='%s' and shk='%s'  and komen <> ''",gn,mn,dn,gk,mk,dk,shet.ravno(),shetk.ravno());
  int kolstr;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr == 0)
   continue;

  while(cur.read_cursor(&row) != 0)
   {
    if(row[13][0] == '\0')
     continue;
    if(strstrm(row[13],komentar.ravno()) == 0)
      continue;
    rsdat(&d,&m,&g,row[1],2);
    deb=atof(row[9]);
    kre=atof(row[10]);
    ideb+=deb;
    ikre+=kre;
    idebk+=deb;
    ikrek+=kre;
    
    naikon.new_plus("");
    
    if(row[4][0] != '\0')
     {  
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[4]);
      if(readkey(strsql,&row1,&cur1) != 1)
       {
        printw("%02d.%02d.%d %-5s %-5s %-5s %-3s %-4s %10.2f %10.2f %s%-10.10s\n",      
        d,m,g,row[2],row[3],row[4],row[5],row[6],
        deb,kre,row[8],row[13]);
        beep();
        printw("\n%s %s\n",gettext("Не найден код контрагента"),row[4]);
        OSTANOV();
       }
      else
       naikon.new_plus(row1[0]);
     }       

    sozkom_prov(&komentar_prov,row[13],row[5],row[8]);

    fprintf(ff,"%02d.%02d.%4d %-*s %-*s %-*s %-*s%*s %-*s %10s",
    d,m,g,
    iceb_tu_kolbait(6,row[2]),row[2],
    iceb_tu_kolbait(6,row[3]),row[3],
    iceb_tu_kolbait(7,row[4]),row[4],
    iceb_tu_kolbait(3,row[5]),row[5],
    iceb_tu_kolbait(4,row[11]),row[11],
    iceb_tu_kolbait(10,row[6]),row[6],
    prnbr(deb));

    fprintf(ff," %10s %*s %-*s %-*.*s %s\n",
    prnbr(kre),
    iceb_tu_kolbait(4,row[16]),row[16],
    iceb_tu_kolbait(4,row[8]),row[8],
    iceb_tu_kolbait(32,komentar_prov.ravno()),
    iceb_tu_kolbait(32,komentar_prov.ravno()),
    komentar_prov.ravno(),
    naikon.ravno());

    for(int nom=32; nom < iceb_tu_strlen(komentar_prov.ravno()); nom+=32)
      fprintf(ff,"%82s %-*.*s\n",
      "",
      iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,komentar_prov.ravno())),
      iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,komentar_prov.ravno())),
      iceb_tu_adrsimv(nom,komentar_prov.ravno()));
   }   
 }
raspitog(idebk,ikrek,3,ff);
raspitog(ideb,ikre,4,ff);


podpis(ff);
fclose(ff);



oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Отчёт по комментариям"));

oth->spis_imaf.plus(imaf_deb);
oth->spis_naim.plus(gettext("Отчёт по комментариям (дебет)"));

oth->spis_imaf.plus(imaf_kre);
oth->spis_naim.plus(gettext("Отчёт по комментариям (кредит)"));

oth->spis_imaf.plus(imaf_prov);
oth->spis_naim.plus(gettext("Распечатка проводок"));
/*так захотел клиент из Шепетовки*/
for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
return(0);
}
/**************************/
/*Проверка комментария*/
/***************************/
//Возвращаем номер комментария или -1 
int prov_kom(class SPISOK *koment,char *row,int kol_koment)
{
if(row[0] == '\0')
 return(-1);
for(int i=0; i < kol_koment;i++)
 {
  if(strstrm(row,koment->ravno(i)) != 0)
    return(i);
 }    
return(-1);

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
-----------------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  | Дебет    |  Кредит  |\n"));
fprintf(ff,"\
-----------------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_deb(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  | Дебет    |\n"));
fprintf(ff,"\
------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_kre(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Коментарий| Счёт  |  Кредит  |\n"));
fprintf(ff,"\
------------------------------\n");

}
/************************/
/*выдача итога*/
/*************************/
void buhkom_it(int metka_i,double deb,double kre,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(18,gettext("Итого")),gettext("Итого"),deb,kre);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb,kre);
}

/************************/
/*выдача итога*/
/*************************/
void buhkom_it_dk(int metka_i,double deb,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(18,gettext("Итого")),gettext("Итого"),deb);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb);
}
