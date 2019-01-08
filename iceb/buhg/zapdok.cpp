/* $Id: zapdok.c,v 5.43 2014/07/31 07:09:18 sasa Exp $ */
/*17.07.2014    14.04.1997      Белых А.И. zapdok.c
Запись накладной
Если вернули 0 порядок
	     1 такой документ уже есть
*/
#include	"buhg.h"

int zapdok(VVOD *VV,
int tipz,   //1-приход 2 расход
const char *kprr, //Код операции 
int skl1,  //Код склада для встречного документа
int lnds,  //Метка освобождения от ндс
const char *dataz,  //Запомненная дата документа до исправления
const char *nomdokz,  //Номер документа до исправления
const char *kontrz,   //Код контрагента до исправления
int mk,         //0-ввод новой записи 1-корректировка
double sumkor,  //Сумма корректировки
int loginrash, //Логин распечатавшего чек
int metkapr, //0-xmu 1-xpro
const char *nom_pd, /*Номер парного документа*/
const char *k00) /*Код контрагента реквизиты которого берутся в распечатку*/
{
char		bros[1024];
char		strsql[1024];
time_t          tmm;
short           d,m,g;
short           dz,mz,gz;
short		dpnn=0,mpnn=0,gpnn=0; //Дата получения налоговой накладной
short           mz0;
short		pro,pod;
SQL_str		row;
int	ktorab=iceb_t_getuid();

clear();
printw(gettext("Выполняем запись шапки документа\n"));
GDITE();


/*Блокируем таблицу*/
class lock_tables blokt("LOCK TABLES Dokummat WRITE, Prihod READ, Rashod READ, Prov READ,Alx READ,icebuser READ,Nalog READ,Blok READ");

if(SRAV(nomdokz,VV->VVOD_SPISOK_return_data(3),0) != 0)
 {  

  rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),1);

  /*Проверяем есть ли такой номер документа*/
  sprintf(strsql,"select nomd from Dokummat where \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
  g,g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3));
  if(readkey(strsql) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(3));
    iceb_t_soob(strsql);
    return(1);
   }
 }
/*

  printw("tipz=%d VV->VVOD_SPISOK_return_data(7)=%s skl1=%d\n",tipz,VV->VVOD_SPISOK_return_data(7),skl1);
  OSTANOV();
*/
  /*Проверяем номер встречного документа*/
if(tipz == 2 && SRAV(nom_pd,VV->VVOD_SPISOK_return_data(7),0) != 0 && VV->VVOD_SPISOK_return_data(7)[0] != '\0')
 {  
    sprintf(strsql,"select nomd from Dokummat where tip=%d and \
 datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%d and nomd='%s'",
    1,g,g,skl1,VV->VVOD_SPISOK_return_data(7));
    if(readkey(strsql) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(7));
      iceb_t_soob(strsql);
      return(1);
     }
 }



rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),0);
rsdat(&dz,&mz,&gz,dataz,0);
if(tipz == 1)
 rsdat(&dpnn,&mpnn,&gpnn,VV->VVOD_SPISOK_return_data(10),0);
if(tipz == 2)
 rsdat(&dpnn,&mpnn,&gpnn,VV->VVOD_SPISOK_return_data(15),0);

mz0=0;
pro=pod=0;
//if(mvnp != 0 && tipz == 1)
//  pro=1;
int metka_opl=0;
//Узнаём нужно ли для этой операции делать проводки
if(tipz == 1)
 sprintf(strsql,"select prov,vido from Prihod where kod='%s'",kprr);
if(tipz == 2)
 sprintf(strsql,"select prov,vido from Rashod where kod='%s'",kprr);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  if(row[0][0] == '1')
   pro=1;
  int mvnp=atoi(row[1]);
  if(mvnp != 0 && tipz == 1)
    pro=1;
 
  if(atoi(row[1]) != 0)
   metka_opl=1;  
}

time(&tmm);

/*Проверяем на наличие апострофа в строке*/
/********
sqlfiltr(VV->VVOD_SPISOK_return_data(6),VV->VVOD_SPISOK_return_dls(6));
sqlfiltr(VV->VVOD_SPISOK_return_data(8),VV->VVOD_SPISOK_return_dls(8));
sqlfiltr(VV->VVOD_SPISOK_return_data(9),VV->VVOD_SPISOK_return_dls(9));
************/
/**************/
/*Новая запись*/
/***************/
float pnds=iceb_t_pnds(d,m,g);
if(lnds == 4)
 pnds=7.;
if(mk == 0)
 {   
  if(tipz == 1)
    if(zap_s_mudok(tipz,d,m,g,atoi(VV->data_ravno(2)),
    VV->data_ravno(0),VV->data_ravno(3),VV->data_ravno_filtr(8),kprr,pod,pro,
    VV->data_ravno(7),dpnn,mpnn,gpnn,metka_opl,k00,pnds) != 0)
     {
      return(1);
     }
  if(tipz == 2)
    if(zap_s_mudok(tipz,d,m,g,atoi(VV->VVOD_SPISOK_return_data(2)),
    VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(3),
    VV->VVOD_SPISOK_return_data(11),kprr,pod,pro,
    VV->VVOD_SPISOK_return_data(7),dpnn,mpnn,gpnn,metka_opl,k00,pnds) != 0)
     {
      return(1);
     }
  /*Записываем парную накладную*/
  if(skl1 != 0 && tipz == 2)
   {
    pod=0;
    pro=1;
    sprintf(bros,"00-%s",VV->VVOD_SPISOK_return_data(2));

    zap_s_mudok(1,d,m,g,skl1,bros,VV->VVOD_SPISOK_return_data(7),
    "",kprr,pod,pro,
    VV->VVOD_SPISOK_return_data(3),0,0,0,metka_opl,k00,pnds);

   }

  blokt.unlock();
  

naz:;


  /*Номер и дата выдачи доверенности*/
  if(VV->VVOD_SPISOK_return_data(4)[0] != '\0')
   {
    sprintf(bros,"%s#%s",VV->VVOD_SPISOK_return_data(4),VV->VVOD_SPISOK_return_data(5));
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),1,bros);
    if(sql_zap(&bd,strsql) != 0)
     {
      msql_error(&bd,__FUNCTION__,strsql);
     }
   }

  /*Через кого*/
  if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),2,VV->data_ravno_filtr(6));

    sql_zapis(strsql,1,0);

   }

  if(metkapr == 0)
   {
    //Блокировка документа для выписки
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%d')",
    g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),4,ktorab);

    sql_zapis(strsql,1,0);
   }

  if(loginrash != 0)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%d')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),5,loginrash);

    sql_zapis(strsql,1,0);
   }
  /*НДС*/
  if( lnds != 0 )
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,%d)",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),11,lnds);

    sql_zapis(strsql,1,0);

    /*Пишем в ответный документ*/
    if(skl1 != 0 && mz0 == 0)
     {
      sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,%d)",
g,skl1,VV->VVOD_SPISOK_return_data(7),11,lnds);

      sql_zapis(strsql,1,0);
     }   
   }

  /*Основание для выписки накладной*/
  if(tipz == 2 && VV->VVOD_SPISOK_return_data(8)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),3,VV->data_ravno_filtr(8));
    sql_zapis(strsql,1,0);
   }

  /*Коментарий для приходной накладной*/
  if(tipz == 1 && VV->VVOD_SPISOK_return_data(14)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),3,VV->VVOD_SPISOK_return_data(14));
    sql_zapis(strsql,1,0);
   }

  /*Условие продажи.*/
  if(tipz == 2 && VV->VVOD_SPISOK_return_data(9)[0] != '\0')
   {
//    sqlfiltr(VV->VVOD_SPISOK_return_data(9),sizeof(VV->VVOD_SPISOK_return_data(9)));
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
    g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),7,VV->data_ravno_filtr(9));

    sql_zapis(strsql,1,0);
   }  

  /*Форма оплаты.*/
  if(tipz == 2 && VV->VVOD_SPISOK_return_data(10)[0] != '\0')
   {
    if(POLE(VV->VVOD_SPISOK_return_data(10),bros,1,'|') == 0)
      VV->data_plus(10,bros);
      
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
    g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),8,VV->VVOD_SPISOK_return_data(10));

    sql_zapis(strsql,1,0);
   }
  /*Форма оплаты.*/
  if(tipz == 1 && VV->VVOD_SPISOK_return_data(13)[0] != '\0')
   {
    if(POLE(VV->VVOD_SPISOK_return_data(13),bros,1,'|') == 0)
      VV->data_plus(13,bros);
      
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
    g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),8,VV->VVOD_SPISOK_return_data(13));

    sql_zapis(strsql,1,0);
   }

  /*Дата оплаты.*/
  if(tipz == 1 && VV->VVOD_SPISOK_return_data(12)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),9,VV->VVOD_SPISOK_return_data(12));

    sql_zapis(strsql,1,0);
   }

  /*Дата оплаты.*/
  if(tipz == 2 && VV->VVOD_SPISOK_return_data(12)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),9,VV->VVOD_SPISOK_return_data(12));

    sql_zapis(strsql,1,0);
   }

  /*Номер машины*/
  if(tipz == 2 && VV->VVOD_SPISOK_return_data(13)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),10,VV->VVOD_SPISOK_return_data(13));

    sql_zapis(strsql,1,0);
   }
  /*Дата выписки налоговой накладной*/
  if(tipz == 1 && VV->VVOD_SPISOK_return_data(9)[0] != '\0')
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),14,VV->VVOD_SPISOK_return_data(9));

    sql_zapis(strsql,1,0);
   }
  if(sumkor != 0.)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%.2f')",
g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3),13,sumkor);

    sql_zapis(strsql,1,0);

   }
  
 } 
/***************/
/*Корректировка*/
/***************/
if(mk == 1 && mz0 == 0)
 {   

  /*Проверяем возможность корректировки даты проводок*/
  
  if(mz != m || gz != g)
   if(iceb_t_pvkdd(1,dz,mz,gz,m,g,VV->data_ravno_atoi(2),nomdokz,tipz) != 0)
    return(1);

   /*узнаём кто сделал запись*/
  sprintf(strsql,"select ktoi from Dokummat where tip=%d and datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
  tipz,gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);
    
  if(readkey(strsql,&row,&cur) == 1)
   {
    if(mu_pvku(atoi(row[0])) != 0) /*проверка возможности корректировки записи*/
     return(1);
   }

  if(lnds == 4)
   pnds=7.;
     
  if(tipz == 1)
    sprintf(strsql,"update Dokummat \
set \
tip=%d,\
datd='%04d-%02d-%02d',\
sklad=%s,\
kontr='%s',\
nomd='%s',\
nomnn='%s',\
kodop='%s',\
pod=%d,\
pro=%d,\
ktoi=%d,\
vrem=%ld,\
nomon='%s',\
datpnn='%04d-%d-%d',\
pn=%.2f,\
k00='%s' \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
tipz,g,m,d,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(8),kprr,pod,pro,ktorab,tmm,VV->VVOD_SPISOK_return_data(7),
gpnn,mpnn,dpnn,pnds,k00,
gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);

  if(tipz == 2)
    sprintf(strsql,"update Dokummat \
set \
tip=%d,\
datd='%04d-%02d-%02d',\
sklad=%s,\
kontr='%s',\
nomd='%s',\
nomnn='%s',\
kodop='%s',\
pod=%d,\
pro=%d,\
ktoi=%d,\
vrem=%ld,\
nomon='%s',\
datpnn='%04d-%d-%d',\
pn=%.2f, \
k00='%s' \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
tipz,g,m,d,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(11),kprr,pod,pro,ktorab,tmm,VV->VVOD_SPISOK_return_data(7),
gpnn,mpnn,dpnn,pnds,k00,
gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"%s %s",__FUNCTION__,gettext("Ошибка корректировки записи !"));
    msql_error(&bd,bros,strsql);

    return(2);
   }

  blokt.unlock();

  /*Удаляем все дополнительные записи*/
  if(metkapr == 0)
    sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%s and nomd='%s'",
    gz,VV->VVOD_SPISOK_return_data(2),nomdokz);

  if(metkapr == 1)
    sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%s and nomd='%s' and nomerz != 4",
    gz,VV->VVOD_SPISOK_return_data(2),nomdokz);


/*printw("Удаление доп. зап\nstrsql=%s\n",strsql);*/

  sql_zapis(strsql,1,0);

  mz0=1;
  goto naz;  
 }


if(mk == 1)
 {
  
  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
  printw(gettext("Проверяем проводки...\n"));
  refresh();
 
  
  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%02d-%02d',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s'",
  kprr,VV->VVOD_SPISOK_return_data(3),g,m,d,g,m,d,ktorab,tmm,
  gz,mz,dz,gz,mz,dz,nomdokz,VV->VVOD_SPISOK_return_data(2),ICEB_MP_MATU);


  sql_zapis(strsql,1,0);
  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%s and kto='%s'",
kprr,VV->VVOD_SPISOK_return_data(3),g,m,d,ktorab,tmm,
gz,mz,dz,nomdokz,VV->VVOD_SPISOK_return_data(2),ICEB_MP_MATU);


  sql_zapis(strsql,1,0);

  if(SRAV(kontrz,VV->VVOD_SPISOK_return_data(0),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s' and kodkon='%s'",
    VV->VVOD_SPISOK_return_data(0),
    g,m,d,VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(2),ICEB_MP_MATU,kontrz);


    sql_zapis(strsql,1,0);

   }


/*Корректировку в накладных и карточках выполняем если была 
изменена дата или номер документа, склад менятся не может*/

  if(SRAV(VV->VVOD_SPISOK_return_data(1),dataz,0) == 0 && SRAV(nomdokz,VV->VVOD_SPISOK_return_data(3),0) == 0)
    return(0);

  /*Корректировка в накладной*/
  printw(gettext("Корректировка записей в документе...\n"));
  refresh();

  sprintf(strsql,"update Dokummat1 \
set \
datd='%04d-%02d-%02d',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
g,m,d,VV->VVOD_SPISOK_return_data(3),ktorab,tmm,
gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);

/*printw("strsql=%s\n",strsql);*/


  sql_zapis(strsql,1,0);

  /*Корректировка записей услуг*/
  printw(gettext("Корректировка записей услуг...\n"));
  refresh();

  sprintf(strsql,"update Dokummat3 \
set \
datd='%04d-%02d-%02d',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
g,m,d,VV->VVOD_SPISOK_return_data(3),ktorab,tmm,
gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);

/*printw("strsql=%s\n",strsql);*/


  sql_zapis(strsql,1,0);

  /*Корректировка в карточках*/
  printw(gettext("Корректировка записей в карточках...\n"));
  refresh();

  /*Только записи в которых дата подтверждения совпадает с датой документа
  изменяем дату подтверждения тоже*/

  sprintf(strsql,"update Zkart \
set \
datdp='%04d-%02d-%02d',\
datd='%04d-%02d-%02d',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s' \
and datdp='%04d-%02d-%02d'",
  g,m,d,g,m,d,VV->VVOD_SPISOK_return_data(3),ktorab,tmm,
  gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz,gz,mz,dz);



  sql_zapis(strsql,1,0);


  /*Исправляем остальные записи*/
  sprintf(strsql,"update Zkart \
set \
datd='%04d-%02d-%02d',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%s and nomd='%s'",
  g,m,d,VV->VVOD_SPISOK_return_data(3),ktorab,tmm,
  gz,mz,dz,VV->VVOD_SPISOK_return_data(2),nomdokz);



  sql_zapis(strsql,1,0);


 }

return(0);

}
