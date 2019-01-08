/* $Id: avtprpld.c,v 5.28 2013-01-02 10:38:36 sasa Exp $ */
/*13.07.2015    28.05.1998      Белых А.И.      avtprpld.c
Автоматическое проведение проводок для платежних документов
*/
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

void avtprpld(const char *tabl,
const char *kop,  //Код операции
const char *npp,  //Номер документа
const char *kor,  //Код организации
const char *shet,
FILE *ff_prot)
{
class iceb_tu_str bros("");
double          bb,deb,kre;
int             i,i1;
class iceb_tu_str kodop("");
double		sump; /*Сумма подтвержденная общая*/
double		sumpp; /*Сумма последнего подтверждения*/
double		sumpr; /*Сумма по проводкам*/
time_t		vrem;
class iceb_tu_str tipd("");
char		strsql[1024];
class iceb_tu_str kontr("");
class iceb_tu_str kontr1("");;
class iceb_tu_str kor1(""),kor2("");
short		metpr;
class iceb_tu_str shet1("");
class iceb_tu_str shet2("");
class iceb_tu_str shet3("");
class iceb_tu_str koresp("");
int		pod;
short		dp,mp,gp;
struct OPSHET	shetv;
int		kolpr=2;
int kekv=0;
class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-tabl=%s kop=%s npp-%s kor-%s shet-%s\n",__FUNCTION__,tabl,kop,npp,kor,shet);

int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_t_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"nastdok.alx") == 0)
 {
   if(SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }


if(shet[0] == '\0')
  return;

/*Узнаем сумму подтвержденных записей*/
sump=sumpzpd(tabl,&dp,&mp,&gp,&sumpp);

if(SRAV(tabl,"Pltp",0) == 0)
 tipd.new_plus(ICEB_MP_PPOR);
if(SRAV(tabl,"Pltt",0) == 0)
 tipd.new_plus(ICEB_MP_PTRE);

/*Суммируем выполненные проводки*/

sumpr=0.;

sumpr=sumprpd(rec.dd,rec.md,rec.gd,npp,0,tipd.ravno(),shet);
/*
printw("sumpr=%f sump=%f\n",sumpr,sump);
OSTANOV();
*/
/*Нужные проводки сделаны*/

if(sumpr == sump)
 {
  iceb_t_soob(gettext("Все проводки сделаны !"));
  return;
 }


kontr.new_plus(kor);

if(provsh(&kontr,shet,&shetv,metka_vkontr,0) != 0)
   return;
//tp1=shetv.saldo;
kor1.new_plus(kontr.ravno());
kor2.new_plus(kontr.ravno());


/*Открываем файл настроек*/
class iceb_tu_str imaf_np;
if(SRAV(tabl,"Pltp",0) == 0)
  imaf_np.new_plus("avtprodok.alx");
if(SRAV(tabl,"Pltt",0) == 0)
  imaf_np.new_plus("avtprodokt.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_np.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_np.ravno());
  return;
 }


shet3.new_plus(shet);

//kp=0;
time(&vrem);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
    
  if(polen(row_alx[0],&kodop,2,'|') != 0)
   continue;
  
  if(SRAV(kodop.ravno(),kop,0) != 0)
   continue;
  
  polen(row_alx[0],&bros,1,'|');
  metpr=0;
  if(strstrm(bros.ravno(),"У") == 1)
      metpr=1;
  if(strstrm(bros.ravno(),"С") == 1)
      metpr=2;

  if(ff_prot != NULL)
   fprintf(ff_prot,"metpr=%d\n",metpr);

  if(metpr == 0)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s",row_alx[0]);
  deb=kre=0.;
  if(metpr == 1)
   {
    deb=okrug(rec.uslb,0.01);
    kre=0.;   
   }

  if(metpr == 2)
   {
    bb=sumpp-rec.uslb;
    deb=okrug(bb,0.01);
    kre=0.;
   }

  if(ff_prot != NULL)
   fprintf(ff_prot,"deb=%f\n",deb);
   
  if(fabs(deb) < 0.01)
    continue;
       
  /*Берем счет*/
  polen(row_alx[0],&bros,3,'|');

  if(polen(bros.ravno(),&shet1,1,':') != 0)
   shet1.new_plus(bros.ravno());
  else
   polen(bros.ravno(),&kontr1,2,':');
  
  /*Берем кореспонденцию*/
  polen(row_alx[0],&koresp,4,'|');

  i1=pole2(koresp.ravno(),',');
  
  for(i=0; i <= i1; i++)
   {
    shet2.new_plus("");
    kontr.new_plus("");

    if(i1 > 0)
     polen(koresp.ravno(),&bros,i+1,',');
    else
     bros.new_plus(koresp.ravno());      

    if(i1 > 0 && i == 0)
     i1--;
     
    /*Проверяем есть ли код контрагента*/
    if(polen(bros.ravno(),&shet2,1,':') != 0)
     shet2.new_plus(bros.ravno());
    else
     polen(bros.ravno(),&kontr,2,':');

    if(shet2.ravno()[0] == '\0')
     continue;

    if(provsh(&kontr,shet2.ravno(),&shetv,metka_vkontr,0) != 0)
       continue;

    if(kontr.getdlinna() > 1)
      kor1.new_plus(kontr.ravno());
    if(kor2.ravno()[0] == '\0' && kontr.getdlinna() > 1)
      kor2.new_plus(kontr.ravno());
//    printw("shet1=%s shet2=%s kor1=%s kor2=%s kontr=%s\n",shet1.ravno(),shet2.ravno(),kor1.ravno(),kor2.ravno(),kontr);
    
    zapmpr(gp,mp,dp,shet2.ravno(),shet3.ravno(),kor1.ravno(),kor2.ravno(),deb,kre,"",kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

    shet3.new_plus(shet2.ravno());
    kor1.new_plus(kontr.ravno());
    kor2.new_plus(kontr.ravno());
   /*
    printw("shet=%s shet1=%s kor1=%s kor2=%s kontr=%s\n",shet,shet1.ravno(),kor1,kor2,kontr.ravno());
    OSTANOV();        
   */
   }


  if(kontr.ravno()[0] == '\0')
     kontr1.new_plus(kor);

   if(ff_prot != NULL)
     fprintf(ff_prot,"shet1=%s kontr1=%s korr=%s\n",shet1.ravno(),kontr1.ravno(),kor);

   if(provsh(&kontr1,shet1.ravno(),&shetv,metka_vkontr,0) != 0)
      continue;

 

   if(kontr1.getdlinna() > 1)
     kor1.new_plus(kontr1.ravno());
   if(kor2.ravno()[0] == '\0' && kontr1.getdlinna() > 1)
     kor2.new_plus(kontr1.ravno());

   if(ff_prot != NULL)
     fprintf(ff_prot,"shet3=%s shet1=%s kor1=%s kor2=%s kontr1=%s deb=%.2f kre=%.2f\n",
     shet3.ravno(),shet1.ravno(),kor1.ravno(),kor2.ravno(),kontr1.ravno(),deb,kre);

   zapmpr(gp,mp,dp,shet1.ravno(),shet3.ravno(),kor1.ravno(),kor2.ravno(),deb,kre,"",kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

 }

/*Запись проводок из памяти в базу*/
pod=0;
time(&vrem);
int tipz=0;
if(SRAV(tabl,"Pltp",0) == 0)
 tipz=2;
if(SRAV(tabl,"Pltt",0) == 0)
 tipz=1;
zapmpr1(npp,kop,pod,vrem,tipd.ravno(),rec.dd,rec.md,rec.gd,tipz,&sp_prov,&sum_prov_dk,ff_prot);

}
