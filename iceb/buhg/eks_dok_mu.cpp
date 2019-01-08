/*$Id: eks_dok_mu.c,v 5.1 2013/10/04 07:25:02 sasa Exp $*/
/*17.02.2016	29.09.2013	Белых А.И.	eks_dok_mu.c
Экспрот документов из подсистемы "Материальный учёт"
*/
#include "errno.h"
#include "buhg.h"

int eks_dok_mu(short dd,short md,short gd,const char *nomdok,int sklad,class iceb_tu_str *imaf,FILE *ff)
{
char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int metka_fopen=0;

if(imaf->getdlinna() <= 1)
 {
  
  imaf->new_plus("eksdokmu");
  imaf->plus(getpid());
  imaf->plus(".csv");

  if((ff = fopen(imaf->ravno(),"w")) == NULL)
   {
    error_op_nfil(imaf->ravno(),errno,"");
    return(1);
   }

  iceb_t_zageks(gettext("Экспорт документов из подсистемы \"Материальный учёт\""),ff);  

  metka_fopen=1;
 }

sprintf(strsql,"select tip,kontr,nomnn,kodop,datpnn,nomon from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,sklad,nomdok);

if(readkey(strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  sklad);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
int tipz=atoi(row[0]);
class iceb_tu_str tipz_char("+");
if(tipz == 2)
 tipz_char.new_plus("-");
 
class iceb_tu_str kod_kontr(row[1]);
class iceb_tu_str nom_nn(row[2]);
class iceb_tu_str kodop(row[3]);
class iceb_tu_str data_pol_nn(row[4]);
if(row[4][0] != '0')
 data_pol_nn.new_plus(iceb_tu_datzap(row[4]));
 
class iceb_tu_str nom_dok_post(row[5]);

/**************************
1  Код контрагента
2  Наименование контрагента
3  Наименование банка
4  Адрес контрагента
5  Населённый пунк где находится банк
6  Код ЕГРПОУ
7  МФО банка
8  Номер счета в банке
9  Индивидуальный налоговый номер
10  Номер свидетельства плательщика НДС
11  Телефон
12 Код группы контрагента
13 Налоговый адрес контрагента
14 Регистрационный номер частного предпринимателя
15 Полное наименование контрагента
16 Населённый пункт контрагента
17 Система налогообложения 0-общая система налогообложения 1-единый налог
******************/
sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_kontr.ravno_filtr());
class iceb_tu_str naim_kontr("");
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_kontr.new_plus(row[1]);  
  fprintf(ff,"KON|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18]);
 }




/***************
DOC|+|1.1.2000|3|345|1+234|4567|ПП|5678|1.1.2000|Госстантарт
 |  |    |     |  |    |    |    |  |     |         |
 |  |    |     |  |    |    |    |  |     |         -> Наименование контрагента
 |  |    |     |  |    |    |    |  |     |      
 |  |    |     |  |    |    |    |  |     |      
 |  |    |     |  |    |    |    |  |     ->Дата выдачи/получения
 |  |    |     |  |    |    |    |  |        налоговой накладной (не обязательный для заполнения реквизит)
 |  |    |     |  |    |    |    |  ->номер документа поставщика  (не обязательный для заполнения реквизит
 |  |    |     |  |    |    |    -> код операции
 |  |    |     |  |    |    -> номер налоговой накладной 
 |  |    |     |  |    |       поставщика (не обязательный для заполнения реквизит)
 |  |    |     |  |    -> номер документа (не обязательный реквизит - если поле не заполнено программа назначит номер документа сама)
 |  |    |     |  -> Код контрагента поставщика/получателя
 |  |    |     -> Код склада получателя/поставщика
 |  |    -> Дата документа
 |  -> "+" приход, "-" расход
 -> Код записи 
**************/


fprintf(ff,"\nDOC|%s|%d.%d.%d|%d|%s|%s|%s|%s|%s|%s|%s|\n",
tipz_char.ravno(),
dd,md,gd,
sklad,
kod_kontr.ravno(),
nomdok,
nom_nn.ravno(),
kodop.ravno(),  
nom_dok_post.ravno(),
data_pol_nn.ravno(),
naim_kontr.ravno());

/*********************** 
ZP3|1|12345#1.1.2001
ZP3|2|Иванов А.Т.
ZP3|3|Распоряжение руководителя
    |          |
    |          -> Содержимое записи
    -> Код содержимого записи
**************************/
sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s'",gd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"ZP3|%s|%s\n",row[0],row[1]);
 }

/***************************
ZP1|123|1|234.89|Шт|0|20|0|281|5678|Бумага в пачках Classik формае А4
 |   |  |   |    |  | |  |  |   |              |
 |   |  |   |    |  | |  |  |   |              -> Наименование материалла
 |   |  |   |    |  | |  |  |   |
 |   |  |   |    |  | |  |  |   -> Номер заказа (не обязательный реквизит)
 |   |  |   |    |  | |  |  -> Счет учета
 |   |  |   |    |  | |  |   (обязателен только для приходных документов)
 |   |  |   |    |  | |  |
 |   |  |   |    |  | |  -> 0 в цену ндс не включен 1-включен
 |   |  |   |    |  | -> Процент НДС
 |   |  |   |    |  -> если равно 1 то это возвратная тара
 |   |  |   |    -> Код единицы измерения
 |   |  |   -> Цена 
 |   |  -> Количество
 |   -> Код материала из справочника материaлов (если введено наименование то это не обязательный реквизит)
 -> Код записи
********************************/

sprintf(strsql,"select kodm,kolih,cena,ei,voztar,nds,mnds,nomz,nomkar from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' order by kodm asc",gd,md,dd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class iceb_tu_str naim("");
class iceb_tu_str shetu("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");
  
  shetu.new_plus("");
    
  if(atoi(row[8]) != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",sklad,atoi(row[8]));
    if(readkey(strsql,&row1,&cur1) == 1)
     shetu.new_plus(row1[0]);
   }      

  fprintf(ff,"ZP1|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],
  row[1],
  row[2],
  row[3],
  row[4],
  row[5],
  row[6],
  shetu.ravno(),
  row[7],
  naim.ravno());
 }

/********************************
ZP2|Усл|1|23400.2|20|Транспортные услуги
 |   |  |   |      |       |
 |   |  |   |      |       -> Наименование услуги
 |   |  |   |      -> Процент НДС
 |   |  |   -> цена
 |   |  -> Количество
 |   -> Код единицы измерения
 -> Код записи
*************************/
sprintf(strsql,"select ei,kolih,cena,nds,naimu from Dokummat3 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,sklad,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"ZP2|%s|%s|%s|%s|%s|\n",
  row[0],
  row[1],
  row[2],
  row[3],
  row[4]);  
 }









if(metka_fopen != 0)
 fclose(ff);





return(0);
}
