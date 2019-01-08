/*$Id: crtabl.c,v 5.71 2014/08/31 06:18:17 sasa Exp $*/
/*27.04.2018	07.02.2008	Белых А.И.	crtabl.c
Подпрограмма создания таблиц в базе данных
*/
#include "buhg.h"

int crtabl_crtb(const char *strsql);

void crtabl(const char *imabaz)
{
char strsql[4096];

printw("%s\n",gettext("Открываем базу данных"));
if(mysql_select_db(&bd,imabaz) < 0)
 { 
  printw("%s\n",sql_error(&bd));
  OSTANOV();
  endwin();
  exit(10);
 } 

/*****************************/
/*Подсистема Главная книга*/
/****************************/

clear();

printw("%s Plansh\n",gettext("Создание таблицы плана счетов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plansh (\
ns	CHAR(20) NOT NULL PRIMARY KEY DEFAULT '',\
nais    VARCHAR(255) NOT NULL DEFAULT '',\
tips    SMALLINT NOT NULL DEFAULT 0,\
vids	SMALLINT NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
saldo   SMALLINT NOT NULL DEFAULT 0,\
stat    SMALLINT NOT NULL DEFAULT 0,\
val     SMALLINT NOT NULL DEFAULT 0,\
ksb     SMALLINT NOT NULL DEFAULT 0,\
bs TINYINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
 0 ns	Номер счета
 1 nais   Наименование счета
 2 tips  признак синтетического счёта  0-активный 1-пассивный 2-активно-пассивный 3-контрактивный 4-контрпасивный
 3 vids   0-счет 1-субсчет
 4 ktoi  Кто записал
 5 vrem  Время записи
 6 saldo 0-свернутое 3-развернутое
 7 stat  0-балансовый счёт 1-вне балансовый счет
 8 val   0-национальная валюта или номер валюты из справочника валют (пока не задействовано)
 9 ksb   код суббаланса
10 bs    0-счёт можно применять 1-счёт заблокирован
*/


crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Glksubbal\n",gettext("Создание таблицы суббалансов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glksubbal (\
kod    SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код cуббаланса
1 naik  наименование суббаланса
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Glksval\n",gettext("Создание таблицы валют"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glksval (\
kod  CHAR(16) NOT NULL PRIMARY KEY DEFAULT '',\
naik VARCHAR(128) NOT NULL DEFAULT '',\
kom  VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код валюты
1 naik  наименование валюты
2 kom   коментарий
3 ktoz  кто записал
4 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Glkkv\n",gettext("Создание таблицы курсов валют"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glkkv (\
kod  CHAR(16) NOT NULL key DEFAULT '',\
datk DATE NOT NULL DEFAULT '0000-00-00',\
kurs DOUBLE(14,6) NOT NULL DEFAULT 0.,\
kom  VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kod,datk)) ENGINE = MYISAM");

/*
0 kod   код валюты
1 datk  дата 
2 kurs  курс валюты
3 kom   коментарий
4 ktoz  кто записал
5 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Gkont\n",gettext("Создание таблицы групп контрагентов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Gkont (\
kod    SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код группы
1 naik  наименование группы контрагентов
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Kontragent\n",gettext("Создание таблицы контрагентов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent (\
kodkon CHAR(20) not NULL PRIMARY KEY DEFAULT '',\
naikon VARCHAR(255) NOT NULL DEFAULT '',\
naiban VARCHAR(80) NOT NULL DEFAULT '',\
adres  VARCHAR(100) NOT NULL DEFAULT '',\
adresb VARCHAR(100) NOT NULL DEFAULT '',\
kod    VARCHAR(20) NOT NULL DEFAULT '',\
mfo    VARCHAR(20) NOT NULL DEFAULT '',\
nomsh  VARCHAR(30) NOT NULL DEFAULT '',\
innn   VARCHAR(20) NOT NULL DEFAULT '',\
nspnds VARCHAR(20) NOT NULL DEFAULT '',\
telef  VARCHAR(80) NOT NULL DEFAULT '',\
grup   VARCHAR(10) NOT NULL DEFAULT '',\
ktoz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
na  VARCHAR(255) NOT NULL DEFAULT '',\
regnom VARCHAR(30) NOT NULL DEFAULT '',\
pnaim  VARCHAR(255) NOT NULL DEFAULT '',\
gk VARCHAR(255) NOT NULL DEFAULT '',\
en TINYINT NOT NULL DEFAULT 0,\
UNIQUE(naikon),\
INDEX(pnaim),\
INDEX(kod)) ENGINE = MYISAM");


/*

0  kodkon Код контрагента
1  naikon Короткое Наименование контрагента
2  naiban Наименование банка
3  adres  Адрес контрагента
4  adresb Населённый пунк где находится банк
5  kod    Код ЕГРПОУ
6  mfo    МФО
7  nomsh  Номер счета
8  innn   Индивидуальный налоговый номер
9  nspnds Номер свидетельства плательщика НДС
10 telef  Телефон
11 grup   Группа
12 ktoz   Кто записал
13 vrem   Время записи
14 na     Налоговый адрес
15 regnom регистрационный номер частного предпринимателя
16 pnaim  полное наименование контрагента
17 gk     город контрагента
18 en     0-общая система налогообложения 1-единый налог
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kontragent1\n",gettext("Создание таблицы дополнительной информации"));
refresh();
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent1 (\
kodkon VARCHAR(20) NOT NULL DEFAULT '',\
nomz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
zapis  VARCHAR(255) NOT NULL DEFAULT '',\
UNIQUE(kodkon,nomz)) ENGINE = MYISAM");

/*
0 kodkon  код контрагента
1 nomz    номер записи
2 zapis   содержимое записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Kontragent2\n",gettext("Создание таблицы списка договоров по контрагенту"));
refresh();
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent2 (\
nz INT UNSIGNED PRIMARY KEY AUTO_INCREMENT NOT NULL ,\
kodkon VARCHAR(32) NOT NULL DEFAULT '',\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nomd VARCHAR (64) NOT NULL DEFAULT '',\
vidd VARCHAR (255) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vnn VARCHAR(16) NOT NULL DEFAULT '',\
pu TINYINT NOT NULL DEFAULT 0,\
INDEX(kodkon)) ENGINE = MYISAM");

/*
0 nz      уникальный номер записи
1 kodkon  код контрагента
2 datd    дата договора
3 nomd    номер договора
4 vidd    вид договора
5 ktoz    Кто записал
6 vrem    Время записи
7 vnn     Вид налоговой накладной - ПН
8 pu      Если равно 1-то это договор по умолчанию
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Skontr\n",gettext("Создание таблицы списка контрагентов для счетов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Skontr (\
ns	CHAR(20) NOT NULL DEFAULT '',\
kodkon  CHAR(20) NOT NULL DEFAULT '',\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(ns,kodkon)) ENGINE = MYISAM");

/*
0 ns	номер счета
1 kodkon  Код контрагента
2 ktoi    кто записал
3 vrem    время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Saldo\n",gettext("Создание таблицы сальдо для счетов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Saldo \(\
kkk 	CHAR(2) NOT NULL DEFAULT '',\
gs      SMALLINT NOT NULL DEFAULT 0,\
ns	CHAR(20) NOT NULL DEFAULT '',\
kodkon  CHAR(20) NOT NULL DEFAULT '',\
deb     DOUBLE(14,2) NOT NULL DEFAULT 0,\
kre     DOUBLE(14,2) NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(kkk,gs,ns)) ENGINE = MYISAM");
/*
0 kkk     0- счета обычные 1- с развернутым сальдо
1 gs      Год сальдо
2 ns	Номер счета
3 kodkon  Код контрагента
4 deb     Дебет
5 kre     Кредит
6 ktoi    Кто записал
7 vrem    Время записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Prov\n",gettext("Создание таблицы проводок"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Prov (\
val SMALLINT NOT NULL DEFAULT 0,\
datp DATE NOT NULL DEFAULT '0000-00-00',\
sh CHAR(20) NOT NULL DEFAULT '',\
shk CHAR(20) NOT NULL DEFAULT '',\
kodkon CHAR(20) NOT NULL DEFAULT '',\
kto CHAR(8) NOT NULL DEFAULT '',\
nomd CHAR(20) NOT NULL DEFAULT '',\
pod INT NOT NULL DEFAULT 0,\
oper CHAR(8) NOT NULL DEFAULT '',\
deb DOUBLE(14,2) NOT NULL DEFAULT 0,\
kre DOUBLE(14,2) NOT NULL DEFAULT 0,\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
komen VARCHAR(100) NOT NULL DEFAULT '',\
datd DATE NOT NULL DEFAULT '0000-00-00',\
tz TINYINT NOT NULL DEFAULT 0,\
kekv SMALLINT NOT NULL DEFAULT 0,\
INDEX(datp,sh,shk),\
INDEX(kto,pod,nomd,datd),\
INDEX(vrem)) ENGINE = MYISAM");

/*
0  val    0-балансовые проводки -1 не балансовые
1  datp   дата проводки
2  sh     счет
3  shk    счёт корреспондент
4  kodkon код контрагента для счетов с развернутым сальдо
5  kto    из какой подсистемы сделана проводка
6  nomd   Номер документа к которому относится проводка
7  pod    подразделение
8  oper   код операции
9  deb    сумма по дебету
10 kre    сумма по кредиту
11 ktoi   номер логина того кто записал проводку
12 vrem   время записи проводки
13 komen  комментарий
14 datd   дата документа к которому относится проводка
15 tz     1-приходный документ 2-расходеый 0-не определено
16 kekv   код экономической классификации расходов
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/******************************************/
/*Подсистема "Материальный учёт*/
/*****************************************/

printw("%s Musnrm\n",gettext("Создание таблицы норм расхода материалов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Musnrm \
(nz INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
kd   INT NOT NULL DEFAULT 0,\
eid  VARCHAR(10) NOT NULL DEFAULT '',\
km    INT NOT NULL DEFAULT 0,\
kol   DOUBLE(20,10) NOT NULL DEFAULT 0,\
eim   VARCHAR(10) NOT NULL DEFAULT '',\
vs    TINYINT NOT NULL DEFAULT 0,\
vz    TINYINT NOT NULL DEFAULT 0,\
kom   VARCHAR(60) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(kd,eid,km)) ENGINE = MYISAM");

/*
 0 nz    уникальный номер записи
 1 kd    код детали
 2 eid   единица измерения детали
 3 km    код материала заготовки
 4 kol   количество материала заготовки на единицу детали
 5 eim   единица измерения материала заготовки
 6 vs    вид списания 0-основной 1-альтернативный
 7 vz    вид заготовки 0-материал 1-покупная заготовка 
 8 kom   комментарий
 9 ktoz  кто записал
10 vrem	 время записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Grup\n",gettext("Создание таблицы перечня групп материалов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Grup \
(kod    SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Edizmer\n",gettext("Создание таблицы перечня единиц измерения"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Edizmer \
(kod    CHAR(10) NOT NULL PRIMARY KEY DEFAULT '',\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kei CHAR(16) NOT NULL DEFAULT '') ENGINE = MYISAM");
/*
0 kod    Короткое наименоваие единицы измерения
1 naik   Полное наименование единицы измерения
2 ktoz   Кто записал
3 vrem   Время записи
4 kei    Код единиці измерения
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Material\n",gettext("Создание таблицы перечня материалов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Material (\
kodm INT not NULL PRIMARY KEY DEFAULT 0,\
kodgr SMALLINT NOT NULL DEFAULT 0,\
naimat VARCHAR(100) NOT NULL DEFAULT '',\
strihkod VARCHAR(50) NOT NULL DEFAULT '',\
ei CHAR(8) NOT NULL DEFAULT '',\
kriost DOUBLE NOT NULL DEFAULT 0,\
cenapr DOUBLE (15,6) NOT NULL DEFAULT 0,\
krat DOUBLE NOT NULL DEFAULT 0,\
fasv DOUBLE NOT NULL DEFAULT 0,\
kodt INT NOT NULL DEFAULT 0,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
nds  real NOT NULL DEFAULT 0,\
msp SMALLINT NOT NULL DEFAULT 0,\
artikul VARCHAR(20) NOT NULL DEFAULT '',\
ku CHAR(32) NOT NULL DEFAULT '',\
UNIQUE (naimat),\
INDEX (strihkod)) ENGINE = MYISAM");

/*

0  kodm      Код материалла
1  kodgr     Код группы материалла
2  naimat    Наименование материалла
3  strihkod  Штрих-код
4  ei        Единица измерения
5  kriost    Критический остаток
6  cenapr    Цена реализации
7  krat      Кратность
8  fasv      Фасовка
9  kodt      Код тары
10 ktoz      Кто записал
11 vrem      Время записи
12 nds       1-цена введена без НДС 0-цена с НДС
13 msp       Метка скоропортящегося материалла 1-скоропортящийся товар 0-нет
14 artikul   артикул
15 ku        код товара согласно УКТ ЗЕД
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Prihod\n",gettext("Создание таблицы перечня операций приходов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Prihod \
(kod    CHAR(10) NOT NULL PRIMARY KEY DEFAULT '',\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
prov    SMALLINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido вид операции 0 - внешняя 1-внутренняя 2-изменения учётных данных
5 prov 0 проводки нужно делать 1 - не нужно
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Rashod\n",gettext("Создание таблицы перечня операций расходов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Rashod \
(kod    CHAR(10) NOT NULL PRIMARY KEY DEFAULT '',\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
sheta   VARCHAR(100) NOT NULL DEFAULT '',\
prov    SMALLINT NOT NULL DEFAULT 0,\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido  вид операции 0-внешняя 1-внутренняя 2-изменения учётных данных
5 sheta возможные счета учета для операции
6 prov 0 проводки нужно делать 1 - не нужно
7 kk   код контрагента реквизиты которого нужно брать при печати документов
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Froplat\n",gettext("Создание таблицы перечня форм оплаты"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Foroplat \
(kod    CHAR(10) NOT NULL PRIMARY KEY DEFAULT '',\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod    Код формы оплаты
1 naik   Наименование формы оплаты
2 ktoz   Кто записал
3 vrem   Время записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Sklad\n",gettext("Создание таблицы перечня складов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Sklad \
(kod    SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik    VARCHAR(124) NOT NULL DEFAULT '',\
fmol    VARCHAR(255) NOT NULL DEFAULT '',\
dolg    VARCHAR(124) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
mi TINYINT NOT NULL DEFAULT 0,\
np VARCHAR(255) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod     Код склада
1 naik    Наименование склада
2 fmol    Фамилия материальноответственного лица
3 dolg    Должность
4 ktoz 	  кто записал
5 vrem    время записи
6 mi      метка использования склада 0-используется 1-нет
7 np      населённый пункт
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Roznica\n",gettext("Создание таблицы розничной торговли"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Roznica (\
datrp  DATE NOT NULL DEFAULT '0000-00-00',\
kassa  SMALLINT NOT NULL DEFAULT 0,\
nomer  SMALLINT NOT NULL DEFAULT 0,\
metka  SMALLINT NOT NULL DEFAULT 0,\
kodm   INT NOT NULL DEFAULT 0,\
ei CHAR(8) NOT NULL DEFAULT '',\
kolih  DOUBLE (14,4) NOT NULL DEFAULT 0,\
cena   DOUBLE (9,2) NOT NULL DEFAULT 0,\
nds    float(2) NOT NULL DEFAULT 0,\
sklad  SMALLINT NOT NULL DEFAULT 0,\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE (datrp,kassa,nomer,metka,kodm),\
INDEX (metka,kodm,sklad)) ENGINE = MYISAM");
/*
0 datrp  дата реализации
1 kassa  номер рабочего места реализации
2 nomer  Номер продажи
3 metka  1-товар 2- услуга
4 kodm   код товара или материалла
5 ei     единица измерения
6 kolih  количество
7 cena   цена
8 nds    1-цена введена без НДС 0-цена с НДС
9 sklad  отдел
10 ktoi   кто продал
11 vrem   время продажи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Dokummat\n",gettext("Создание таблицы заголовков документов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat (\
tip    SMALLINT NOT NULL DEFAULT 0,\
datd   DATE NOT NULL DEFAULT '0000-00-00',\
sklad  SMALLINT NOT NULL DEFAULT 0,\
kontr  CHAR(20) NOT NULL DEFAULT '',\
nomd   CHAR(16) NOT NULL DEFAULT '',\
nomnn  CHAR(16) NOT NULL DEFAULT '',\
kodop  CHAR(8) NOT NULL DEFAULT '',\
pod    SMALLINT NOT NULL DEFAULT 0,\
pro    SMALLINT NOT NULL DEFAULT 0,\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
nomon  CHAR(16) NOT NULL DEFAULT '',\
datpnn DATE NOT NULL DEFAULT '0000-00-00',\
pn float(5,2) NOT NULL DEFAULT 0,\
mo TINYINT NOT NULL DEFAULT 0,\
k00 CHAR(16) NOT NULL DEFAULT '00',\
UNIQUE (datd,sklad,nomd),\
INDEX (nomnn)\
) ENGINE = MYISAM");

/*
0 tip    1-приход 2-расход
1 datd   дата
2 sklad  код склада
3 kontr  код контрагента
4 nomd   номер документа
5 nomnn  номер налоговой накладной
6 kodop  код операции
7 pod    0-не подтверждена 1-подтверждена
8 pro    0-не выполнены проводки 1-выполнены
9 ktoi   кто записал
10 vrem   мремя записи
11 nomon  Номер парной накладной при внутреннем перемещении или номер документа поставщика для приходного документа
12 datpnn дата получения/выдачи налоговой накладной
13 pn     процент НДС действовавший на момент создания документа
14 mo     метка оплаты 0-неоплачено 1-оплачено
15 k00    код контрагента реквизиты которого берутся в распечатку
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Dokummat1\n",gettext("Создание таблицы записей в документе"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat1 (\
datd DATE NOT NULL DEFAULT '0000-00-00',\
sklad  SMALLINT NOT NULL  DEFAULT 0 references Sklad (kod),\
nomd   CHAR(16) NOT NULL DEFAULT '',\
nomkar INT NOT NULL DEFAULT 0,\
kodm   INT NOT NULL  DEFAULT 0 references Material (kodm),\
kolih  DOUBLE(17,7) NOT NULL DEFAULT 0,\
cena   DOUBLE(17,7) NOT NULL DEFAULT 0,\
ei     VARCHAR(8) NOT NULL DEFAULT '' references Edizmer (kod),\
voztar SMALLINT NOT NULL DEFAULT 0,\
nds    float(2) NOT NULL DEFAULT 0,\
mnds   SMALLINT NOT NULL DEFAULT 0,\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
tipz   SMALLINT NOT NULL DEFAULT 0,\
nomkarp INT NOT NULL DEFAULT 0,\
shet   VARCHAR(12) NOT NULL DEFAULT '',\
dnaim  VARCHAR(70) NOT NULL DEFAULT '',\
nomz   VARCHAR(20) NOT NULL DEFAULT '',\
mi TINYINT NOT NULL DEFAULT 0,\
kdstv VARCHAR(32) NOT NULL DEFAULT '',\
INDEX (sklad,nomd,kodm,nomkar),\
INDEX (sklad,nomkar),\
INDEX (kodm,sklad),\
INDEX (datd,sklad,nomd)) ENGINE = MYISAM");

/*
0  datd   Дата документа
1  sklad  Склад
2  nomd   Номер документа
3  nomkar Номер карточки
4  kodm   Код материала
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  voztar 1-Метка возвратной тары
9  nds    НДС
10 mnds   0- цена без НДС 1-цена с НДС в том числе
11 ktoi   Кто записал
12 vrem   Время записи
13 tipz   1-приход 2-расход
14 nomkarp номер карточки парного документа
15 shet   счёт списания/приобретения
16 dnaim  Дополнительное наименование материалла
17 nomz   Номер заказа
18 mi     метка импортного товара 0-не импорт 1-импорт
19 kdstv  код діяльності сільськогосподарського товаровиробника
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Dokummat2\n",gettext("Создание таблицы дополнительных записей к документу"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Dokummat2 (\
god    SMALLINT NOT NULL DEFAULT 0,\
sklad  SMALLINT NOT NULL DEFAULT 0 references Sklad (kod),\
nomd   CHAR(16) NOT NULL DEFAULT '',\
nomerz SMALLINT NOT NULL DEFAULT 0,\
sodz   VARCHAR(255) NOT NULL DEFAULT '',\
UNIQUE (god,nomd,sklad,nomerz)\
) ENGINE = MYISAM");
/*
0 god    
1 sklad
2 nomd
3 nomerz=1 - доверенность#дата выдачи
         2 - через кого
         3 - основание для расходного документа / комментарий для приходного документа
         4 - документ заблокирован для программы xpro если есть эта запись
         5 - login распечатавшего чек
         6 - Сумма НДС для приходного документа введенная вручную
         7 - условие продажи  c 10.1.2011 сюда записывается через двоеточие - вид договора:дата договора:номер договора:вид документа (в реестре нал. нак.)
         8 - код формы оплаты
         9 - дата оплаты или отсрочки платежа
        10 - государственный номер машины
        11 - НДС
             0 20% НДС.
             1 0% НДС реализация на територии Украины.
             2 0% НДС экспорт.
             3 0% Освобождение от НДС статья 5.
             4 7% НДС на мед.препараты
        12 - Кто заблокировал документ
        13 - Сумма корректировки по документу +/-
        14 - дата накладной поставщика - дата выписки налоговой накладной
        15 - Номер путевого листа
        16 - Марка автомобиля
        17 - Марка и номер прицепа
        18 - Перевозчик груза
        19 - Заказчик (плательщик)
        20 - Фамилия водителя
        21 - Пункт загрузки
        22 - Пункт разгрузки
        23 - Акцизный сбор        
        24 - Лицензия на алкоголь|период действия лицензии для контрагента 00 в формате дд.мм.гг-дд.мм.гг
        25 - Лицензия на алкоголь|период действия лицензии для контрагента получателя в формате дд.мм.гг-дд.мм.гг
        26 - Вид договора|Дата договора|номер договора
4 sodz  Содержимое записи
 */

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Dokummat3\n",gettext("Создание таблицы услуг для документа"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Dokummat3 (\
datd  DATE NOT NULL DEFAULT '0000-00-00',\
sklad  SMALLINT NOT NULL DEFAULT 0 references Sklad (kod),\
nomd   CHAR(16) NOT NULL DEFAULT '',\
ei     CHAR(12) NOT NULL DEFAULT '',\
kolih DOUBLE NOT NULL DEFAULT 0,\
cena DOUBLE NOT NULL DEFAULT 0,\
nds DOUBLE NOT NULL DEFAULT 0,\
naimu VARCHAR(100) NOT NULL DEFAULT '',\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
ku INT NOT NULL DEFAULT 0,\
nz INT UNSIGNED PRIMARY KEY AUTO_INCREMENT NOT NULL,\
INDEX(datd,sklad,nomd)) ENGINE = MYISAM");

/*
 0 datd  дата документа
 1 sklad код склада
 2 nomd  номер документа
 3 ei    единица измерения
 4 kolih количество
 5 cena  цена/сумма
 6 nds   НДС (процент)
 7 naimu наименование услуги
 8 ktoi  кто записал
 9 vrem  время записи
10 ku    код услуги
11 nz    уникальный номер записи устанавливаемый базой данных автоматически
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Dokummat4\n",gettext("Создание таблицы списания материалов на детали"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat4 \
(datd  DATE NOT NULL DEFAULT '0000-00-00',\
skl    SMALLINT NOT NULL DEFAULT 0,\
nomd   CHAR(16) NOT NULL DEFAULT '',\
km     INT NOT NULL DEFAULT 0,\
nk     INT NOT NULL DEFAULT 0,\
kd     INT NOT NULL DEFAULT 0,\
kol    DOUBLE(16,6) NOT NULL DEFAULT 0,\
kom    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
nz INT UNSIGNED PRIMARY KEY AUTO_INCREMENT NOT NULL,\
INDEX(datd,skl,nomd)) ENGINE = MYISAM");

/*
 0 datd дата документа
 1 skl  код склада
 2 nomd номер документа
 3 km   код материала в документе
 4 nk   номер карточки
 5 kd   код детали на изготовление которой будет использоваться списанный материал
 6 kol  количество материала на указанный код детали (может быть списание на несколько деталей)
 7 kom  коментарий 
 8 ktoz	кто записал
 9 vrem	время записи
10 nz   уникальный номер записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Opldok\n",gettext("Создание таблицы оплат по документам"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Opldok (\
ps    TINYINT NOT NULL DEFAULT 0,\
datd  DATE NOT NULL DEFAULT '0000-00-00',\
pd    INT NOT NULL DEFAULT 0,\
nomd  CHAR(20) NOT NULL DEFAULT '',\
dato  DATE NOT NULL DEFAULT '0000-00-00',\
shet  CHAR(20) NOT NULL DEFAULT '',\
suma  DOUBLE(10,2) NOT NULL DEFAULT 0,\
kekv  SMALLINT NOT NULL DEFAULT 0,\
nomp  CHAR(20) NOT NULL DEFAULT '',\
kom   VARCHAR(100) NOT NULL DEFAULT '',\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(ps,datd,pd,nomd)) ENGINE = MYISAM");

crtabl_crtb(strsql); /*создаём таблицу*/


/*
0 ps    подсистема 1-материальный учёт 2- учёт услуг 3-учёт основных средств
1 datd  дата документа
2 pd    подразделение (для УОС pd=0 так как нумерация документов не зависит от подразделения)
3 nomd  номер документа
4 dato  дата оплаты
5 shet  счёт 
6 suma  сумма закрытая на документ
7 kekv  код экономической классификации затрат
8 nomp  номер платёжного поручения
9 kom   комментарий
10 ktoi  кто записал
11 vrem  время записи
*/

printw("%s Kart\n",gettext("Создание таблицы карточек материалов"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kart (\
sklad  SMALLINT NOT NULL DEFAULT 0 references Sklad (kod),\
nomk    INT NOT NULL DEFAULT 0,\
kodm   INT NOT NULL DEFAULT 0 references Material (kodm),\
mnds    SMALLINT NOT NULL DEFAULT 0,\
ei CHAR(8) NOT NULL references Edizmer (kod),\
shetu   CHAR(12) NOT NULL DEFAULT '' references Plansh (ns),\
cena    DOUBLE(17,7) NOT NULL DEFAULT 0,\
cenap   DOUBLE(17,7) NOT NULL DEFAULT 0,\
krat    decimal(10,2) NOT NULL DEFAULT 0,\
nds	float(2) NOT NULL DEFAULT 0,\
fas     float(2) NOT NULL DEFAULT 0,\
kodt    INT NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
datv    DATE NOT NULL DEFAULT '0000-00-00',\
innom   VARCHAR(16) NOT NULL DEFAULT '',\
rnd     VARCHAR(20) NOT NULL DEFAULT '',\
nomz    VARCHAR(20) NOT NULL DEFAULT '',\
dat_god DATE NOT NULL DEFAULT '0000-00-00',\
UNIQUE (sklad,nomk),\
INDEX (kodm,sklad,shetu,cena,ei,nds,cenap,krat,mnds),\
INDEX(innom),\
INDEX(nomz)\
) ENGINE = MYISAM");

/*
0  sklad    Код склада
1  nomk     Номер карточки
2  kodm     Код материалла
3  mnds     0-цена c НДС 1-цена без НДС
4  ei       Единица измерения
5  shetu    Счет учета
6  cena     Цена учета
7  cenap    Цена продажи для розничной торговли
8  krat     Кратность
9  nds      Н.Д.С
10 fas      Фасовка
11 kodt     Код тары
12 ktoi     Кто записал
13 vrem     Время записи
14 datv     Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов
15 innom    Инвентарный номер (для малоценки) / Серия для медикаментов
16 rnd      Регистрационный номер документа для мед. препарата/дата ввода в эксплуатацию для инвентарного номера
17 nomz     Номер заказа
18 dat_god  Конечная дата годности товара
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zkart\n",gettext("Создание таблицы записей в карточках"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zkart (\
sklad  SMALLINT NOT NULL DEFAULT 0 references Sklad (kod),\
nomk    INT NOT NULL DEFAULT 0,\
nomd    CHAR(16) NOT NULL DEFAULT '',\
datdp	DATE NOT NULL DEFAULT '0000-00-00',\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
tipz	SMALLINT NOT NULL DEFAULT 0,\
kolih   DOUBLE(16,6) NOT NULL DEFAULT 0,\
cena    DOUBLE(17,7) NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE (sklad,nomk,nomd,datdp), \
INDEX (datd,nomd,sklad,nomk),\
INDEX (datdp)\
) ENGINE = MYISAM");

/*
0 sklad	Склад
1 nomk 	Номер карточки
2 nomd 	Номер документа
3 datdp	дата получения-отпуска (дата подтверждения документа)
4 datd  Дата выписки документа
5 tipz 	1 - приход 2 - расход
6 kolih	Количество
7 cena 	Цена
8 ktoi 	Кто записал
9 vrem 	Время записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/

/********************************/
/* Таблицы для заработной платы */
/*********************************/

printw("%s Kartb\n",gettext("Создание таблицы карточки работника"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kartb (\
tabn INT NOT NULL PRIMARY KEY DEFAULT 0,\
fio  VARCHAR(60) NOT NULL DEFAULT '',\
dolg VARCHAR(60) NOT NULL DEFAULT '',\
zvan SMALLINT NOT NULL DEFAULT 0,\
podr INT NOT NULL DEFAULT 0,\
kateg SMALLINT NOT NULL DEFAULT 0,\
datn DATE NOT NULL DEFAULT '0000-00-00',\
datk DATE NOT NULL DEFAULT '0000-00-00',\
shet CHAR(12) NOT NULL DEFAULT '' references Plansh (ns),\
sovm SMALLINT NOT NULL DEFAULT 0,\
inn  CHAR(15) NOT NULL DEFAULT '',\
adres VARCHAR(100) NOT NULL DEFAULT '',\
nomp VARCHAR(30) NOT NULL DEFAULT '',\
vidan VARCHAR(100) NOT NULL DEFAULT '',\
telef VARCHAR(50) NOT NULL DEFAULT '',\
kodg SMALLINT NOT NULL DEFAULT 0,\
harac VARCHAR(50) NOT NULL DEFAULT '',\
lgoti VARCHAR(30) NOT NULL DEFAULT '',\
bankshet VARCHAR(30) NOT NULL DEFAULT '',\
datvd DATE NOT NULL DEFAULT '0000-00-00',\
tipkk VARCHAR(30) NOT NULL DEFAULT '',\
denrog DATE NOT NULL DEFAULT '0000-00-00',\
pl TINYINT NOT NULL DEFAULT 0,\
dppz DATE NOT NULL DEFAULT '0000-00-00',\
kb INT NOT NULL DEFAULT 0,\
kss VARCHAR(16) NOT NULL DEFAULT '',\
INDEX(fio),\
INDEX(inn)) ENGINE = MYISAM");

/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss код основания для учёта специального стажа
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Kartb1\n",gettext("Создание таблицы дополнительной информации к карточке работника"));
refresh();
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kartb1 (\
tabn INT NOT NULL DEFAULT 0,\
nomz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
zapis  VARCHAR(255) NOT NULL DEFAULT '',\
UNIQUE(tabn,nomz)) ENGINE = MYISAM");

/*
0 tabn табельный номер
1 nomz    номер записи
2 zapis   содержимое записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarsdo\n",gettext("Создание таблицы должностных окладов почасовых ставок и тарифных разрядов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarsdo \
(tn INT NOT NULL DEFAULT 0,\
dt DATE NOT NULL DEFAULT '0000-00-00',\
kn INT NOT NULL DEFAULT 0,\
sm DOUBLE(12,2) NOT NULL DEFAULT 0.,\
mt SMALLINT NOT NULL DEFAULT 0,\
kh FLOAT(6,2) NOT NULL DEFAULT 0.,\
ks FLOAT(6,2) NOT NULL DEFAULT 1.,\
sh CHAR(32) NOT NULL DEFAULT '',\
km CHAR(100) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(tn,dt,kn)) ENGINE = MYISAM");

/*
0  tn   табельный номер
1  dt   дата - день всегда 1
2  kn   код начисления
3  sm   сумма 
4  mt   метка суммы
        0 - оплата за месяц расчётная по отработанным дням
        1 - оплата за месяц расчётная по отработанным часам
        2 - оплата за месяц постоянная
        3 - оплата за час
        4 - оплата за день
        5 - оплата по тарифу (минимальная зарплата умноженная на тарифный коэффициент)
5  kh   количество часов в рабочем дне
6  ks   коэффициент ставки по умолчанию 1.
7  sh   бухгалтерский счёт 
8  km   коментарий
9  ktoz	Кто записал
10 vrem	Время записи

*/
crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarpr\n",gettext("Создание таблицы протоколов расчёта зарплаты"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarpr \
(tn INT NOT NULL DEFAULT 0,\
tz TINYINT NOT NULL DEFAULT 0,\
dr DATE NOT NULL DEFAULT '0000-00-00',\
nz INT NOT NULL DEFAULT 0,\
sz TEXT NOT NULL DEFAULT '',\
UNIQUE(tn,tz,dr,nz)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 tz  1-протокол расчёта начислений 2-удержаний 3-начислений на фонд зарплаты
2 dr  Дата расчёта год и месяц день всегда 1
3 nz   номер записи
4 sd  содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarkh\n",gettext("Создание таблицы списка картсчетов работников"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarkh (\
tn INT NOT NULL DEFAULT 0,\
kb  INT NOT NULL DEFAULT 0,\
nks VARCHAR(40) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(tn,kb)) ENGINE = MYISAM");
/****
0 tn - табельный номер
1 kb - код банка
2 nks - номер карт-счёта
3 ktoz кто записал
4 vrem время записи
****/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Zarrud\n",gettext("Создание таблицы справочника регулярных удержаний с работника"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarrud \
(tn INT NOT NULL DEFAULT 0 PRIMARY KEY,\
ku INT  NOT NULL DEFAULT 0,\
sp DOUBLE(10,2) NOT NULL DEFAULT 0.,\
mu TINYINT NOT NULL DEFAULT 0,\
dd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 tn  табельный номер
1 ku  код удержания
2 sp  сумма/процент
3 mu  метка 0-сумма 1-процент
4 sd  дата действия
5 kom коментарий
6 ktoz   Кто записал
7 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Zaresv\n",gettext("Создание таблицы настроек начисления единого взноса"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zaresv \
(kf INT NOT NULL DEFAULT 0,\
datnd DATE NOT NULL DEFAULT '0000-00-00',\
pr float NOT NULL DEFAULT 0.,\
pr1 float NOT NULL DEFAULT 0.,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kf,datnd)) ENGINE = MYISAM");

/********
0 kf  - код фонда
1 datnd- дата начала действия
2 pr   - процент на фонд оплаты
3 prr  - процент с работника
4 ktoz - кто записал
5 vrem - время записи
************/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zarskrdh\n",gettext("Создание таблицы справочника количества рабочих дней и часов"));
memset(strsql,'\0',sizeof(strsql));


strcpy(strsql,"CREATE TABLE Zarskrdh \
(data DATE NOT NULL DEFAULT '0000-00-00' PRIMARY KEY,\
krd FLOAT NOT NULL DEFAULT 0.,\
krh FLOAT NOT NULL DEFAULT 0.,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kpd INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 data   месяц и god день всегда равен 1
1 krd    количество рабочих дней
2 krh    количество рабочих часов
3 ktoz   Кто записал
4 vrem   Время записи
5 kpd    Количество праздничных дней (применяется для расчёта отпускных
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s %s\n",gettext("Создание таблицы справочника выходных дней"),"Zarsvd");
memset(strsql,'\0',sizeof(strsql));


strcpy(strsql,"CREATE TABLE Zarsvd \
(data DATE NOT NULL DEFAULT '0000-00-00' PRIMARY KEY,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 data   дата выходного дня
1 kom    коментарий
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s %s\n",gettext("Создание таблицы справочника мнимальной зарплаты и прожиточного минимума"),"Zarmzpm");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarmzpm \
(data DATE NOT NULL DEFAULT '0000-00-00' PRIMARY KEY,\
mz FLOAT NOT NULL DEFAULT 0.,\
pm FLOAT NOT NULL DEFAULT 0.,\
ms FLOAT NOT NULL DEFAULT 0.,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 data   дата
1 mz     минимальная зарплата
2 pm     прожиточный минимум
3 ms     максимальная сумма с которой берутся соц-отчисления
4 ktoz   Кто записал
5 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Nash\n",gettext("Создание таблицы начислений"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Nash (\
kod	SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
shet    CHAR(12)  NOT NULL DEFAULT '' references Plansh (ns),\
kodt    SMALLINT NOT NULL DEFAULT 0,\
vidn    CHAR(2) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
prov	SMALLINT NOT NULL DEFAULT 0,\
fz      SMALLINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod	код начисления
1 naik    наименование начисления
2 shet    счёт на который ложится начисление
3 kodt    код табеля к которому привязано начисление
4 vidn    0-основная 1-дополнительная 2-прочая    
5 ktoz    кто записал
6 vrem    время записи
7 prov    0-делать проводки 1-не делать
8 fz      0-входит в фонд зарплаты 1-не входит
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Uder\n",gettext("Создание таблицы удержаний"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uder (\
kod SMALLINT NOT NULL PRIMARY KEY DEFAULT 0,\
naik VARCHAR(60) NOT NULL DEFAULT '',\
shet VARCHAR(12) NOT NULL DEFAULT '' references Plansh (ns),\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
prov SMALLINT NOT NULL DEFAULT 0,\
shetb VARCHAR(32) NOT NULL DEFAULT '') ENGINE = MYISAM");
/*
0 kod	код удержание
1 naik  наименование удержания
2 shet  хозрасчётный счёт
3 ktoz  кто записал
4 vrem  время записи
5 prov    0-делать проводки 1-не делать
6 shetb бюджетный счёт
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zargrupp\n",gettext("Создание таблицы групп подразделений"));
refresh();
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargrupp \
(kod   CHAR(10) NOT NULL PRIMARY KEY DEFAULT '',\
naik   VARCHAR(100) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod    код группы
1 naik   наименование группы
2 ktoz	кто записал
3 vrem	время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zargr\n",gettext("Создание таблицы групп работников"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargr \
(kg INT NOT NULL DEFAULT 0,\
tn INT NOT NULL DEFAULT 0,\
dnd DATE NOT NULL DEFAULT '0000-00-00',\
dkd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kg,tn,dnd)) ENGINE = MYISAM");
/*
0 kg   код группы работкика 1-пенсионер 2-инвалид 3-работающий по договору подряда
1 tn   табельный номер
2 dnd  Дата начала действия
3 dkd  дата конца действия
4 kom  коментарий
5 ktoz Кто записал
6 vrem Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Zarsb\n",gettext("Создание таблицы списка банков"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsb \
(kod    INT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod   код банка
1 naik  наименование банка
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Podr\n",gettext("Создание таблицы подразделений"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Podr (\
kod	SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
grup    VARCHAR(10) NOT NULL DEFAULT '') ENGINE = MYISAM");
/*
0 kod	код подразделения
1 naik    наименование подразделения
2 ktoz  Кто записал
3 vrem  Время записи
4 grup  код группы
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kateg\n",gettext("Создание таблицы категорий"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kateg (\
kod	SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod	код категории
1 naik    наименование категории
2 ktoz  Кто записал
3 vrem  Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s %s\n",gettext("Создание таблицы званий"),"Zvan");
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zvan (\
kod	SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
sdzz DOUBLE(12,2) NOT NULL DEFAULT 0.) ENGINE = MYISAM");
/*
0 kod	код звания
1 naik    наименование звания
2 ktoz  Кто записал
3 vrem  Время записи
4 sdzz   доплата за звание
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Gnali\n",gettext("Создание таблицы городов налоговых инспекций"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Gnali (\
kod	SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
kontr   CHAR(20) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod	код города
1 naik    наименование города
2 kontr   код контрагента налоговой инспекции
3 ktoz  кто записал
4 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Tabel\n",gettext("Создание таблицы табелей"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tabel (\
kod	SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod	код табеля
1 naik    наименование табеля
2 ktoz  Кто записал
3 vrem  Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Zarsoc\n",gettext("Создание таблицы справочника социальных начислений"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsoc (\
kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
shet	VARCHAR(30) NOT NULL DEFAULT '',\
proc    float(6,2) NOT NULL DEFAULT 0,\
kodn    VARCHAR(255) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
shetb	VARCHAR(32) NOT NULL DEFAULT '') ENGINE = MYISAM");
/*
0 kod   код фонда
1 naik  наименование фонда
2 shet  счет
3 proc  процент отчисления
4 kodn  коды начислений не входящие в расчёт этого отчисления
5 ktoz  кто записал
6 vrem  время записи
7 shetb бюждетный счёт
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarsocz\n",gettext("Создание таблицы сумм социальных отчислений"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsocz (\
datz    DATE NOT NULL DEFAULT '0000-00-00',\
tabn    INT UNSIGNED NOT NULL DEFAULT 0,\
kodz    SMALLINT NOT NULL DEFAULT 0,\
shet	VARCHAR(30) NOT NULL DEFAULT '',\
sumas   DOUBLE(8,2) NOT NULL DEFAULT 0,\
sumap   DOUBLE(8,2) NOT NULL DEFAULT 0,\
proc    float(6,2) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
sumapb  DOUBLE(8,2) NOT NULL DEFAULT 0,\
sumasb  DOUBLE(8,2) NOT NULL DEFAULT 0, \
nz INT UNSIGNED PRIMARY KEY AUTO_INCREMENT NOT NULL,\
INDEX(datz,tabn,kodz)) ENGINE = MYISAM");

/*
0  datz дата записи, день всегда равен 1
1  tabn табельный номер
2  kodz код социального отчисления
3  shet счёт социального отчисления хозрасчётный через запятую бюджетный (если он есть)
4  sumas сумма с которой расчитано отчисление (общая вместе с бюджетом)
5  sumap сумма отчисления (общая вместе с бюджетом)
6  proc  процент отчисления
7  ktoz  кто записал
8  vrem  время записи
9  sumapb сумма только с бюджетных счетов
10 sumasb бюджетная сумма с кототорой расчитано отчисление
11 nz  уникальный номер записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zsal\n",gettext("Создание таблицы сальдо по зарплате"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zsal (\
god	SMALLINT NOT NULL DEFAULT 0,\
tabn    INT NOT NULL DEFAULT 0,\
saldo   DOUBLE(12,2) NOT NULL DEFAULT 0,\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
saldob  DOUBLE(12,2) NOT NULL DEFAULT 0,\
UNIQUE(god,tabn)) ENGINE = MYISAM");
/*
0 god	год
1 tabn	табельный номер
2 saldo   значение сальдо
3 ktoz  кто записал
4 vrem  время записи
5 saldob  сальдо по бюджетным счетам
*/
crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Ztab\n",gettext("Создание таблицы записей табелей"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ztab (\
god	SMALLINT NOT NULL DEFAULT 0,\
mes     SMALLINT NOT NULL DEFAULT 0,\
tabn    INT NOT NULL DEFAULT 0,\
kodt    SMALLINT NOT NULL DEFAULT 0,\
dnei    float NOT NULL DEFAULT 0,\
has     float NOT NULL DEFAULT 0,\
kdnei   float NOT NULL DEFAULT 0,\
datn    DATE NOT NULL DEFAULT '0000-00-00',\
datk    DATE NOT NULL DEFAULT '0000-00-00',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
kolrd float(5,2) NOT NULL DEFAULT 0,\
kolrh float(5,2) NOT NULL DEFAULT 0,\
kom  VARCHAR(40) NOT NULL DEFAULT '',\
nomz SMALLINT NOT NULL DEFAULT 0,\
UNIQUE(tabn,god,mes,kodt,nomz),\
INDEX(god,mes,tabn)) ENGINE = MYISAM");

/*
0  god	год
1  mes     месяц
2  tabn	табельный номер
3  kodt    код табеля
4  dnei    отработано дней
5  has     отработано часов
6  kdnei   отработано календарных дней
7  datn    дата начала
8  datk    дата конца
9  ktoz    кто записал
10 vrem    время модификации
11 kolrd   количество рабочих дней в месяце по данному табелю для конкретного работника
12 kolrh   скольки часовой рабочий день
13 kom     комментарий
14 nomz    номер записи
*/
crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarlgot\n",gettext("Создание таблицы льгот"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarlgot \
(tn INT NOT NULL DEFAULT 0,\
dndl DATE NOT NULL DEFAULT '0000-00-00',\
pl FLOAT(7,2) NOT NULL DEFAULT 0.,\
koll INT NOT NULL DEFAULT 0,\
kodl CHAR(16) NOT NULL DEFAULT '',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(tn,dndl)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 dndl Дата начала действия льготы
2 pl   процент льготы
3 koll количество льгот
4 kodl код льготы
5 kom  коментарий
6 ktoz Кто записал
7 vrem Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/




printw("%s Zarp\n",gettext("Создание таблицы записей зарплаты"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarp (\
datz	DATE NOT NULL DEFAULT '0000-00-00',\
tabn    INT NOT NULL DEFAULT 0,\
prn     CHAR(2) NOT NULL DEFAULT '',\
knah    SMALLINT NOT NULL DEFAULT 0,\
suma    DOUBLE (14,2) NOT NULL DEFAULT 0,\
shet    VARCHAR(12) NOT NULL DEFAULT '' references Plansh (ns),\
godn    SMALLINT NOT NULL DEFAULT 0,\
mesn    SMALLINT NOT NULL DEFAULT 0,\
kdn     SMALLINT NOT NULL DEFAULT 0,\
nomz    SMALLINT NOT NULL DEFAULT 0,\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
kom	VARCHAR(40) NOT NULL DEFAULT '',\
podr	INT NOT NULL DEFAULT 0,\
nd      VARCHAR(20) NOT NULL DEFAULT '',\
UNIQUE (datz,tabn,prn,knah,godn,mesn,podr,shet,nomz),\
INDEX (tabn,datz),\
INDEX tabn1 (tabn,godn,mesn),\
INDEX (nd,datz)) ENGINE = MYISAM");

/*
0  datz    дата начисления/удержания
1  tabn	табельный номер
2  prn	1- начисление 2 удержание
3  knah	код начисления
4  suma	сумма начисления/удержания
5  shet    счет
6  godn    год в счёт которого начисления/удержания
7  mesn    месяц в счёт которого начисления/удержания
8  kdn     количество дней для больничного
9  nomz    номер записи
10 ktoz    кто записал
11 vrem    дата записи
12 kom     комментарий
13 podr    код подразделения
14 nd      номер документа
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Zarn\n",gettext("Создание таблицы записей настройки карточки по месяцам"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarn (\
god  SMALLINT NOT NULL DEFAULT 0,\
mes  SMALLINT NOT NULL DEFAULT 0,\
podr INT NOT NULL DEFAULT 0,\
tabn INT NOT NULL DEFAULT 0,\
kateg SMALLINT NOT NULL DEFAULT 0,\
sovm SMALLINT NOT NULL DEFAULT 0,\
zvan SMALLINT NOT NULL DEFAULT 0,\
shet VARCHAR(20) NOT NULL references Plansh (ns),\
lgot VARCHAR(60) NOT NULL,\
prov SMALLINT NOT NULL DEFAULT 0, \
datn DATE NOT NULL DEFAULT '0000-00-00',\
datk DATE NOT NULL DEFAULT '0000-00-00',\
dolg VARCHAR(60) NOT NULL DEFAULT '',\
blok SMALLINT NOT NULL DEFAULT 0,\
UNIQUE(tabn,god,mes),\
INDEX(god,mes,podr)) ENGINE = MYISAM");

/*
0  god	   год
1  mes	   месяц
2  podr    подразделение
3  kateg   категория
4  tabn	   табельный номер
5  sovm	   0-нет 1-совместитель
6  zvan	   код звания
7  shet	   счёт 
8  lgot	   льготы
9  prov    0-проводки не сделаны 1-сделаны
10 datn    Дата приема на работу
11 datk    Дата увольнения с работы
12 dolg    Должность
13 blok    0- не заблокирована или логин заблокировавшего карточку
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zarn1\n",gettext("Создание таблицы записей текущей настройки начислений/удержаний"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarn1 (\
tabn	INT NOT NULL DEFAULT 0,\
prn	SMALLINT NOT NULL DEFAULT 0,\
knah    SMALLINT NOT NULL DEFAULT 0,\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
shet	VARCHAR(20)  NOT NULL DEFAULT '' references Plansh (ns),\
UNIQUE(tabn,prn,knah)) ENGINE = MYISAM");
/*
0 tabn	табельный номер
1 prn	1-начисление 2 удержание
2 knah    код начисления/удержания
3 datd    дата до которой действует начисление/удержание
4 shet	счет
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/*Создаем таблицы для формы 8др*/

printw("%s F8dr\n",gettext("Создание таблицы заголовка документа формы 1дф"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr (\
god    SMALLINT NOT NULL DEFAULT 0,\
kvrt   SMALLINT NOT NULL DEFAULT 0,\
nomd   VARCHAR(20) NOT NULL PRIMARY KEY,\
vidd   SMALLINT NOT NULL DEFAULT 0,\
nomdn  VARCHAR(20) NOT NULL DEFAULT '',\
datad  DATE NOT NULL DEFAULT '0000-00-00',\
datan  DATE NOT NULL DEFAULT '0000-00-00',\
metbl  SMALLINT NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kr INT NOT NULL DEFAULT 0,\
ks INT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 god    год
1 kvrt   квартал
2 nomd   номер документа
3 vidd   вид документа 0-отчётный 1-новый отчётный 2-уточняющий
4 nomdn  номер под которым документ зарегистрирован в налоговой
5 datad  дата подачи документа
6 datan  дата когда документ зарегистрирован в налоговой
7 metbl  0-документ не заблокирован или логин того кто заблокировал
8 ktoz  кто записал
9 vrem  время записи
10 kr   количество работающих
11 ks   количество совместителей
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s F8dr1\n",gettext("Создание таблицы записей в документе формы 1дф"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr1 (\
nomd   VARCHAR(20) NOT NULL DEFAULT '',\
inn    CHAR(10)  NOT NULL DEFAULT '',\
sumad  DOUBLE(10,2) NOT NULL DEFAULT 0,\
suman  DOUBLE(8,2) NOT NULL DEFAULT 0,\
priz   CHAR(4) NOT NULL DEFAULT '',\
datap  DATE NOT NULL DEFAULT '0000-00-00',\
datau  DATE NOT NULL DEFAULT '0000-00-00',\
lgota  CHAR(4) NOT NULL DEFAULT '',\
ktoz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
fio    VARCHAR(60) NOT NULL DEFAULT '',\
sumadn  DOUBLE(10,2) NOT NULL DEFAULT 0,\
sumann  DOUBLE(8,2) NOT NULL DEFAULT 0,\
pr     SMALLINT NOT NULL DEFAULT 0,\
UNIQUE(nomd,inn,priz,lgota,pr)) ENGINE = MYISAM");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 sumad  сумма выплаченного дохода
3 suman  суммы выплаченного подоходного налога
4 priz   признак дохода
5 datap  дата приема на работу
6 datau  дата увольнения
7 lgota  признак льготы по подоходному налогу
8 ktoz  кто записал
9 vrem  время записи
10 fio фамилия для произвольного ввода записи
11 sumadn  сумма начисленного дохода
12 sumann  суммы начисленного подоходного налога
13 pr    0-новая запись 1-удалить запись
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s F1dfvs\n",gettext("Создание таблицы записей военного сбора формы 1дф"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F1dfvs (\
nomd   VARCHAR(20) NOT NULL DEFAULT '',\
inn    CHAR(10)  NOT NULL DEFAULT '',\
fio    VARCHAR(60) NOT NULL DEFAULT '',\
sfr  DOUBLE(10,2) NOT NULL DEFAULT 0,\
svs  DOUBLE(8,2) NOT NULL DEFAULT 0,\
sfrv  DOUBLE(10,2) NOT NULL DEFAULT 0,\
svsv  DOUBLE(8,2) NOT NULL DEFAULT 0,\
ktoz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(nomd,inn)) ENGINE = MYISAM");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 fio    фамилия для произвольного ввода записи
3 sfr    сумма с которой был сделан расчёт
4 svs    сумма военного сбора
5 sfrv   сумма с коророй был сделан расчёт выплаченная
6 svsv   сумма военного сбора выплаченная
7 ktoz  кто записал
8 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Zardok\n",gettext("Создание таблицы заголовков нарядов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zardok (\
god SMALLINT NOT NULL DEFAULT 0,\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nomd VARCHAR(20) NOT NULL DEFAULT '',\
pd INT NOT NULL DEFAULT 0,\
koment VARCHAR(100) NOT NULL DEFAULT '',\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
td TINYINT NOT NULL DEFAULT 0,\
datsd DATE NOT NULL DEFAULT '0000-00-00',\
UNIQUE(nomd,god),\
INDEX(datd,nomd)) ENGINE = MYISAM");

/************
0  god     год документа
1  datd    дата начисления
2  nomd      номер документа
3  pd      код подразделения
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  td      1-приходы 2-расходы
8  datsd   дата создания документа
***************/


crtabl_crtb(strsql); /*создаём таблицу*/




/***********************************************************/
/*Таблицы для программы работы с бухгалтерскими документами*/
/************************************************************/

printw("%s Pltp\n",gettext("Создание таблицы заголовков платежных поручений"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltp (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
oper    VARCHAR(12) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
nds     DOUBLE NOT NULL DEFAULT 0,\
uslb    DOUBLE NOT NULL DEFAULT 0,\
shet    VARCHAR(30) NOT NULL DEFAULT '',\
plat	VARCHAR(255) NOT NULL DEFAULT '',\
polu    VARCHAR(255) NOT NULL DEFAULT '',\
podt    CHAR(2) NOT NULL DEFAULT '',\
prov    CHAR(2) NOT NULL DEFAULT '',\
vidpl   CHAR(2) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
datv    DATE NOT NULL DEFAULT '0000-00-00',\
kodnr   VARCHAR(10) NOT NULL DEFAULT '',\
UNIQUE(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	   дата документа
1  nomd    номер документа
2  oper    код операции
3  suma    сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	   реквизиты плательщика 1-код контрагента 2-наименование контрагента 3-город банка 4-код ЕГРПОУ 5-наименование банка 6-МФО 7-номер р/с
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   раньше это был вид платежа О,В,К  - в настоящий момент это метка для передачи платёжки в банк если 1 передавать
12 ktoz    кто записал
13 vrem    время записи
14 datv    Дата валютирования
15 kodnr   Код не резидента
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Pltpz\n",gettext("Создание таблицы записей к платежному поручению"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltpz (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
mz      SMALLINT NOT NULL DEFAULT 0,\
nz      SMALLINT NOT NULL DEFAULT 0,\
zapis   VARCHAR(255) NOT NULL DEFAULT '',\
INDEX(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Pltt\n",gettext("Создание таблицы заголовков платежных требований"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltt (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
oper    VARCHAR(12) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2)NOT NULL DEFAULT 0,\
nds     DOUBLE NOT NULL DEFAULT 0,\
uslb    DOUBLE NOT NULL DEFAULT 0,\
shet    VARCHAR(30) NOT NULL DEFAULT '',\
plat	VARCHAR(255) NOT NULL DEFAULT '',\
polu    VARCHAR(255) NOT NULL DEFAULT '',\
podt    CHAR(2) NOT NULL DEFAULT '',\
prov    CHAR(2) NOT NULL DEFAULT '',\
vidpl   CHAR(2) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
datv    DATE NOT NULL DEFAULT '0000-00-00',\
kodnr   VARCHAR(10) NOT NULL DEFAULT '',\
INDEX(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа (здесь пока не применяется)
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Plttz\n",gettext("Создание таблицы записей к платежному требованию"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plttz (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
mz      SMALLINT NOT NULL DEFAULT 0,\
nz      SMALLINT NOT NULL DEFAULT 0,\
zapis   VARCHAR(255) NOT NULL DEFAULT '',\
INDEX(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Tpltt\n",gettext("Создание таблицы заголовков типовых платежных требований"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltt (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
oper    VARCHAR(12) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
nds     DOUBLE NOT NULL DEFAULT 0,\
uslb    DOUBLE NOT NULL DEFAULT 0,\
shet    VARCHAR(30) NOT NULL DEFAULT '',\
plat	VARCHAR(255) NOT NULL DEFAULT '',\
polu    VARCHAR(255) NOT NULL DEFAULT '',\
podt    CHAR(2) NOT NULL DEFAULT '',\
prov    CHAR(2) NOT NULL DEFAULT '',\
vidpl   CHAR(2) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
datv    DATE NOT NULL DEFAULT '0000-00-00',\
kodnr   VARCHAR(10) NOT NULL DEFAULT '',\
UNIQUE(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   здесть не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Tplttz\n",gettext("Создание таблицы записей к типовому платежному требованию"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tplttz (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
mz      SMALLINT NOT NULL DEFAULT 0,\
nz      SMALLINT NOT NULL DEFAULT 0,\
zapis   VARCHAR(255) NOT NULL DEFAULT '',\
INDEX(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Tpltp\n",gettext("Создание таблицы заголовков типовых платежных поручений"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltp (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
oper    VARCHAR(12) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
nds     DOUBLE NOT NULL DEFAULT 0,\
uslb    DOUBLE NOT NULL DEFAULT 0,\
shet    VARCHAR(30) NOT NULL DEFAULT '',\
plat	VARCHAR(255) NOT NULL DEFAULT '',\
polu    VARCHAR(255) NOT NULL DEFAULT '',\
podt    CHAR(2) NOT NULL DEFAULT '',\
prov    CHAR(2) NOT NULL DEFAULT '',\
vidpl   CHAR(2) NOT NULL DEFAULT '',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
datv    DATE NOT NULL DEFAULT '0000-00-00',\
kodnr   VARCHAR(10) NOT NULL DEFAULT '',\
UNIQUE(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа О,В,К - в настоящий момент не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Tpltpz\n",gettext("Создание таблицы записей к типовому платежному поручению"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltpz (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
mz      SMALLINT NOT NULL DEFAULT 0,\
nz      SMALLINT NOT NULL DEFAULT 0,\
zapis   VARCHAR(255) NOT NULL DEFAULT '',\
INDEX(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Opltp\n",gettext("Создание таблицы перечня операций платежных поручений"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltp \
(kod    CHAR(10) NOT NULL  DEFAULT ''PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код операции платежного поручения
1 naik   Наименование операции платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Opltt\n",gettext("Создание таблицы перечня операций платежных требований"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltt \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код операции платежного требования
1 naik   Наименование операции платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Obqvka\n",gettext("Создание таблицы объявок cдачи налички в банк"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Obqvka (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
suma    DOUBLE (14,2) NOT NULL DEFAULT 0,\
kodop   VARCHAR(8) NOT NULL DEFAULT '',\
fio	VARCHAR(40) NOT NULL DEFAULT '',\
prov    CHAR(2) NOT NULL DEFAULT '',\
podt    CHAR(2) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd	номер документа
2 suma    сумма документа
3 kodop   код операции
4 fio	фамилия сдающего деньги
5 prov    0-проводки не выполнены 1-выполнены
6 podt    0-не подтвержден 1-подтвержден
7 ktoz  
8 vrem
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Oznvb\n",gettext("Создание таблицы перечня операций cдачи налички в банк"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Oznvb \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код операции
1 naik   Наименование операции
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Ktnp\n",gettext("Создание таблицы перечня типовых платежек"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnp \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код типового платежного поручения
1 naik   Наименование типового платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Ktnt\n",gettext("Создание таблицы перечня типовых требований"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnt \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код типового платежного требования
1 naik   Наименование типового платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Ksn\n",gettext("Создание таблицы перечня сдающих в банк наличку"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ksn \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код сдающего наличку в банк
1 naik   Фамилия сдающего наличку в банк
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Pzpd\n",gettext("Создание таблицы подтверждающих записей документов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Pzpd (\
tp	SMALLINT NOT NULL DEFAULT 0,\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
datp	DATE NOT NULL DEFAULT '0000-00-00',\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(tp,nomd)) ENGINE = MYISAM");

/*
0 tp	0-платежкка 1-требование
1 datd    дата документа
2 datp	дата подтверждения
3 nomd	номер документа
4 suma	сумма подтверждения
5 ktoz  кто записал
6 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/************************************************/
/***********Учет основных средств****************/
/************************************************/
printw("%s Uospri\n",gettext("Создание таблицы перечня операций приходов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospri \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
prov    SMALLINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosras\n",gettext("Создание таблицы перечня операций расходов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosras \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
prov    SMALLINT NOT NULL DEFAULT 0,\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
6 kk   код контрагента реквизиты которого нужно брать в расходные документы
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosls\n",gettext("Создание таблицы ликвидационной стоимости"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosls (\
inn INT NOT NULL DEFAULT 0,\
data DATE NOT NULL DEFAULT '0000-00-00',\
lsnu DOUBLE NOT NULL DEFAULT 0.,\
lsbu DOUBLE NOT NULL DEFAULT 0.,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(inn,data)) ENGINE = MYISAM");
/*
0 inn    инвентарный номер
1 data  дата 
2 lsnu    ликвидационная стоимость для налогового учёта
3 lsbu ликвидационная стоимость для бух.учёта
4 kom   коментарий
5 ktoz  Кто записал
6 vrem  Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uospod\n",gettext("Создание таблицы перечня подразделений"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospod \
(kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
mi TINYINT NOT NULL) ENGINE = MYISAM");
/*
0 kod    Код подразделения
1 naik   Наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (остатки по нему нулевые)
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uosdm\n",gettext("Создание таблицы перечня драг-металлов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdm \
(kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(100) NOT NULL DEFAULT '',\
ei      VARCHAR(20) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код драгоценного металла
1 naik   Наименование металла
2 ei     Единица измерения
3 ktoz   Кто записал
4 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosindm\n",gettext("Создание таблицы содержимого драг-металлов в инвентарном номере"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosindm \
(innom INT NOT NULL,\
kod    SMALLINT NOT NULL DEFAULT 0,\
ei     VARCHAR(20) NOT NULL DEFAULT '',\
ves    DOUBLE(13,8) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(innom,kod)) ENGINE = MYISAM");
/*
0 innom    инвентарный номер
1 kod    Код драгоценного металла
2 ei     Единица измерения
3 ves    вес материалла
4 ktoz   Кто записал
5 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosgrup\n",gettext("Создание таблицы перечня груп для налогового учёта"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
kof     float NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
ta TINYINT NOT NULL,\
ar TINYINT NOT NULL) ENGINE = MYISAM");
/*
0 kod    код
1 naik   наименование
2 kof    годовой процент амортизационного отчисления
3 ktoz   Кто записал
4 vrem   Время записи
5 ta     Тип амортизации 0-по объектная 1-в целом по группе
6 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/


crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uosgrup1\n",gettext("Создание таблицы перечня груп для бухгалтерского учёта"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup1 \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
kof     float NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
ar TINYINT NOT NULL) ENGINE = MYISAM");

/*
0 kod    код
1 naik   наименование
2 kof    коэффициент нормы амортотчисления
3 ktoz
4 vrem
5 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("Создание таблицы шифров аналитического учета Uoshau !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshau \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код шифра аналитического учета
1 naik   Наименование шифра аналитического учета
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uoshz\n",gettext("Создание таблицы шифров затрат"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshz \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код шифра затрат
1 naik   Наименование шифра затрат
2 ktoz   Кто записал
3 vrem   Время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uosol\n",gettext("Создание таблицы ответственных"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosol \
(kod    INT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
mi TINYINT NOT NULL,\
dolg VARCHAR(100) NOT NULL DEFAULT '') ENGINE = MYISAM");
/*
0 kod    Код ответственного лица
1 naik   Фамилия ответственного лица
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (уволен и остатки по нему нулевые)
5 dolg   должность
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosin\n",gettext("Создание таблицы инвентарных номеров"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosin (\
innom   INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,\
god     SMALLINT NOT NULL DEFAULT 0,\
naim	VARCHAR(80) NOT NULL DEFAULT '',\
zaviz   VARCHAR(60) NOT NULL DEFAULT '',\
pasp	VARCHAR(60) NOT NULL DEFAULT '',\
model   VARCHAR(60) NOT NULL DEFAULT '',\
zavnom  VARCHAR(60) NOT NULL DEFAULT '',\
datvv   DATE NOT NULL DEFAULT '0000-00-00',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 innom   инвентарный номер
1 god     год изготовления
2 naim	наименование
3 zaviz   завод изготовитель
4 pasp	паспорт
5 model   модель
6 zavnom  заводской номер
7 datvv   дата ввода в эксплуатацию
8 ktoz  кто записал
9 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uosin1\n",gettext("Создание таблицы дополнительной информации по инвентарному номеру"));
refresh();
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uosin1 (\
innom INT UNSIGNED NOT NULL DEFAULT 0,\
nomz   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
zapis  TEXT NOT NULL DEFAULT '',\
md TINYINT NOT NULL DEFAULT 0,\
UNIQUE(innom,md,nomz)) ENGINE = MYISAM");

/*
0 innom инвентарный номер
1 nomz    номер записи
2 zapis   содержимое записи
3 md      метка данных 
          0-дополнительная информация 
          1-оборотная сторона карточки для печати 
          2-оборотная сторона карточки приказ министерства финансов 818 13.09.2016 г.
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosinp\n",gettext("Создание таблицы переменных данных инвентарных номеров"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosinp (\
innom   INT UNSIGNED NOT NULL DEFAULT 0,\
mes	SMALLINT NOT NULL DEFAULT 0,\
god     SMALLINT NOT NULL DEFAULT 0,\
shetu   VARCHAR(20) NOT NULL DEFAULT '' references Plansh (ns),\
hzt     VARCHAR(20) NOT NULL DEFAULT '',\
hau     VARCHAR(20) NOT NULL DEFAULT '',\
hna     VARCHAR(20) NOT NULL DEFAULT '',\
popkof  DOUBLE(12,6) NOT NULL DEFAULT 0.,\
soso    SMALLINT NOT NULL DEFAULT 0,\
nomz	VARCHAR(40) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
hnaby   VARCHAR(20) NOT NULL DEFAULT '',\
popkofby DOUBLE(12,6) NOT NULL DEFAULT 0.,\
UNIQUE(innom,god,mes)) ENGINE = MYISAM");
/*
0  innom   инвентарный номер
1  mes	   месяц записи
2  god     год записи,\
3  shetu   счёт учета
4  hzt     шифр затрат
5  hau     шифр аналитического учета
6  hna     шифр нормы амотротчислений (группа) для налогового учёта
7  popkof  поправочный коэффициент для налогового учёта
8  soso    состояние объекта 0-амортизация считается для бухучета и налогового учёта
                             1-амортизация не считается для бух.учета и налогового учёта
                             2-считается для бух.учета и не считается для налогового
                             3-не считается для бух учета и считается для налогового
9  nomz	   номерной знак (для автомобилей)
10 ktoz    кто записал
11 vrem    время записи
12 hnaby   шифр нормы амотротчислений (группа) для бух. учета
13 popkofby  поправочный коэффициент для бух. учета
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosdok\n",gettext("Создание таблицы заголовков документов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdok (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
tipz	SMALLINT NOT NULL DEFAULT 0,\
kodop   VARCHAR(20) NOT NULL DEFAULT '',\
kontr   VARCHAR(20) NOT NULL DEFAULT '',\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
podr	SMALLINT NOT NULL DEFAULT 0,\
kodol   INT NOT NULL DEFAULT 0,\
podrv	SMALLINT NOT NULL DEFAULT 0,\
nomdv	VARCHAR(20) NOT NULL DEFAULT '',\
kodolv  INT NOT NULL DEFAULT 0,\
prov	SMALLINT NOT NULL DEFAULT 0,\
podt	SMALLINT NOT NULL DEFAULT 0,\
osnov   VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
dover VARCHAR(30) NOT NULL DEFAULT '',\
datvd DATE NOT NULL DEFAULT '0000-00-00',\
forop VARCHAR(10) NOT NULL DEFAULT '',\
datpnn DATE NOT NULL DEFAULT '0000-00-00',\
nomnn VARCHAR(16) NOT NULL DEFAULT '',\
sherez VARCHAR(40) NOT NULL DEFAULT '',\
datop DATE NOT NULL DEFAULT '0000-00-00',\
nds SMALLINT NOT NULL DEFAULT 0,\
pn float(5,2) NOT NULL DEFAULT 0,\
dvnn DATE NOT NULL DEFAULT '0000-00-00',\
mo TINYINT NOT NULL DEFAULT 0,\
vd VARCHAR(255) NOT NULL DEFAULT '',\
UNIQUE(datd,nomd),\
INDEX(nomnn)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  tipz	1-приход 2-расход
2  kodop   код операции
3  kontr   код контрагента
4  nomd	номер документа
5  podr	подразделение
6  kodol  код ответственного лица
7  podrv  подразделение встречное
8  nomdv  номер парного документа для внутреннего перемещения или номер документа поставщика для приходного документа
9  kodolv  код встречного ответственного лица
10 prov	  0-провоки выполнены 1-нет
11 podt	  0-неподтвержден 1-подтвержден
12 osnov  основание
13 ktoz	 кто записал
14 vrem	 время записи
15 dover доверенность
16 datvd дата выдачи доверенности
17 forop код формы оплаты
18 datpnn дата получения налоговой накладной
19 nomnn номер налоговой накладной
20 sherez через кого
21 datop дата оплаты
22 nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 Освобождение от НДС статья 5.
23 pn - процент НДС дейстованший на момент создания документа
24 dvnn - дата выписки налоговой накладной
25 mo - метка оплаты 0- неоплачено 1-оплачено
26 vd - вид договора (для налоговой накладной)
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosdok1\n",gettext("Создание таблицы записей в документе"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdok1 (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
tipz	SMALLINT NOT NULL DEFAULT 0,\
podt	SMALLINT NOT NULL DEFAULT 0,\
innom   INT UNSIGNED NOT NULL DEFAULT 0,\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
podr	SMALLINT NOT NULL DEFAULT 0,\
kodol   INT NOT NULL DEFAULT 0,\
kol     SMALLINT NOT NULL DEFAULT 0,\
bs	DOUBLE(12,2) NOT NULL DEFAULT 0,\
iz	DOUBLE(12,2) NOT NULL DEFAULT 0,\
kind	float NOT NULL DEFAULT 0,\
kodop   VARCHAR(20) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
bsby	DOUBLE(12,2) NOT NULL DEFAULT 0,\
izby    DOUBLE(12,2) NOT NULL DEFAULT 0,\
cena	DOUBLE(14,2) NOT NULL DEFAULT 0,\
shs     VARCHAR(20) NOT NULL DEFAULT '',\
UNIQUE(datd,nomd,innom),\
INDEX(innom,datd)) ENGINE = MYISAM");

/*
0  datd	  дата документа
1  tipz	  1-приход 2-расход
2  podt	  0-не подтверждена 1-подтверждена
3  innom  инвентарный номер
4  nomd	  номер документа
5  podr   подразделение
6  kodol  код ответственного лица
7  kol    количество : +1 приход -1 расход 0 изменение стоимости
8  bs	  балансовая стоимость для налогового учёта
9  iz	  износ для налогового учёта
10 kind	  коэффициент индексации
11 kodop  код операции
12 ktoz   кто записал
13 vrem   время записи
14 bsby	  балансовая стоимость для бух-учета
15 izby	  износ для бух-учета
16 cena   цена продажи
17 shs    счёт получения/списания
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosamor\n",gettext("Создание таблицы амортизации для налогового учёта"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor (\
innom   INT NOT NULL DEFAULT 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	SMALLINT NOT NULL DEFAULT 0,\
hzt	VARCHAR(20) NOT NULL DEFAULT '',\
hna	VARCHAR(20) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kmo INT NOT NULL DEFAULT 0,\
UNIQUE(innom,da)) ENGINE = MYISAM");

/*
0 innom   инвентарный номер
1 da    дата амортизации
2 podr	подразделение
3 hzt	шифр затрат
4 hna	шифр нормы аморт отчислений (группа)
5 suma	сумма
6 ktoz	кто записал
7 vrem	дата записи
8 kmo   код материально-ответсвенного
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uosamor1\n",gettext("Создание таблицы амортизации для бух учета"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor1 (\
innom   INT NOT NULL DEFAULT 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	SMALLINT NOT NULL DEFAULT 0,\
hzt	VARCHAR(20) NOT NULL DEFAULT '',\
hna	VARCHAR(20) NOT NULL DEFAULT '',\
suma	DOUBLE (14,2) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kmo INT NOT NULL DEFAULT 0,\
UNIQUE(innom,da)) ENGINE = MYISAM");

/*
0 innom   инвентарный номер
1 da    дата амортизации
2 podr	подразделение
3 hzt	шифр затрат
4 hna	шифр нормы аморт отчислений (группа)
5 suma	сумма
6 ktoz	кто записал
7 vrem	дата записи
8 kmo   код материально-ответсвенного
*/

crtabl_crtb(strsql); /*создаём таблицу*/

/*********Учет услуг*************************/

printw("%s Uslgrup\n",gettext("Создание таблицы перечня групп услуг"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslgrup \
(kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod    Код группы услуг
1 naik   Наименование группы услуг
2 ktoz   Кто записал
3 vrem   Время записи
*/


crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Usloper1\n",gettext("Создание таблицы перечня операций услуг на приход"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper1 \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
prov    SMALLINT NOT NULL DEFAULT 0,\
shet	VARCHAR(20) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz
3 vrem
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счёт учета операции
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Usloper2\n",gettext("Создание таблицы перечня операций услуг на расход"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper2 \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
vido    SMALLINT NOT NULL DEFAULT 0,\
prov    SMALLINT NOT NULL DEFAULT 0,\
shet	VARCHAR(20) NOT NULL DEFAULT '',\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи в секундах
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счёт учета операции
7 kk   код контрагента реквизиты которого нужно брать в расходные документы
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uslpodr\n",gettext("Создание таблицы перечня подразделений"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslpodr \
(kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
np VARCHAR(255) NOT NULL DEFAULT '',\
fo VARCHAR(128) NOT NULL DEFAULT '',\
dol VARCHAR(255) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod  код подразделения
1 naik наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
4 np   населённый пункт
5 fo   фамилия ответственного
6 dol  должность ответсвенного
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uslugi\n",gettext("Создание таблицы перечня услуг"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslugi (\
kodus   INT NOT NULL DEFAULT 0 PRIMARY KEY,\
kodgr   SMALLINT NOT NULL DEFAULT 0,\
naius   VARCHAR(80) NOT NULL DEFAULT '',\
ei      VARCHAR(20) NOT NULL DEFAULT '',\
shetu   VARCHAR(20) NOT NULL DEFAULT '' references Plansh (ns),\
cena    DOUBLE(10,2) NOT NULL DEFAULT 0,\
nds	real NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	int UNSIGNED NOT NULL DEFAULT 0,\
art VARCHAR(64) NOT NULL DEFAULT '',\
ku CHAR(32) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0  kodus   код услуги
1  kodgr   код группы
2  naius   наименование услуги
3  ei      единица измерения
4  shetu   счёт учета
5  cena    цена услуги
6  nds	0-цена без НДС 1-цена с НДС
7  ktoz	кто записал
8  vrem	время записи
9  art   артикул
10 ku   код "Держаного класифікатора продукції та послуг"
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Usldokum\n",gettext("Создание таблицы заголовков документов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum (\
tp     SMALLINT NOT NULL DEFAULT 0,\
datd   DATE NOT NULL DEFAULT '0000-00-00',\
podr   SMALLINT NOT NULL DEFAULT 0 references Uslpod (kod),\
kontr  VARCHAR(20) NOT NULL DEFAULT '' references Kontragent (kontr),\
nomd   VARCHAR(20) NOT NULL DEFAULT '',\
nomnn  VARCHAR(20) NOT NULL DEFAULT '',\
kodop  VARCHAR(10) NOT NULL DEFAULT '' references Usloper (kod),\
nds    SMALLINT NOT NULL DEFAULT 0,\
forop  VARCHAR(20) NOT NULL DEFAULT '',\
datop  DATE NOT NULL DEFAULT '0000-00-00',\
uslpr  VARCHAR(80) NOT NULL DEFAULT '',\
sumkor DOUBLE(10,2) NOT NULL DEFAULT 0,\
pod    SMALLINT NOT NULL DEFAULT 0,\
pro    SMALLINT NOT NULL DEFAULT 0,\
oplata SMALLINT NOT NULL DEFAULT 0,\
blokir SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
nomdp  VARCHAR(20) NOT NULL DEFAULT '',\
datdp  DATE NOT NULL DEFAULT '0000-00-00',\
dover  VARCHAR(20) NOT NULL DEFAULT '',\
datdov DATE NOT NULL DEFAULT '0000-00-00',\
sherez VARCHAR(40) NOT NULL DEFAULT '',\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
osnov  VARCHAR(80) NOT NULL DEFAULT '',\
datpnn DATE NOT NULL DEFAULT '0000-00-00',\
sumnds DOUBLE(10,2) NOT NULL DEFAULT 0,\
pn float(5,2) NOT NULL DEFAULT 0,\
mo TINYINT NOT NULL DEFAULT 0,\
k00 CHAR(16) NOT NULL DEFAULT '00',\
UNIQUE (datd,podr,nomd,tp),\
INDEX (nomnn)\
) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   дата
2  podr   код подразделения
3  kontr  код контрагента
4  nomd   номер документа
5  nomnn  номер налоговой накладной
6  kodop  код операции
7  nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 0% Освобождение от НДС статья 5.
8  forop  код формы оплаты
9  datop  дата оплаты или отсрочки
10 uslpr  условие продажи (вид договора)
11 sumkor сумма корректировки к документу
12 pod    0-не подтверждена 1-подтверждена
13 pro    0-не выполнены проводки 1-выполнены
14 oplata 0-не оплачена 1-оплачена
15 blokir код того кто заблокировал документ
16 nomdp  номер документа поставщика
17 datdp  Дата виписки документа поставщика - дата выписки налоговой накладной
18 dover  довереность
19 datdov дата доверенности
20 sherez через кого
21 ktoi   кто записал
22 vrem   время записи
23 osnov  основание
24 datpnn дата получения налоговой накладной
25 sumnds Сумма НДС для приходных документов введенная вручную
26 pn - процент НДС действовавший на момент создания документа
27 mo - метка оплаты 0-неоплачено 1-оплачено
28 k00 - код контрагента реквизиты которого нужно брать в распечатку
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Usldokum1\n",gettext("Создание таблицы записей к документам"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum1 (\
tp     SMALLINT NOT NULL DEFAULT 0,\
datd   DATE NOT NULL DEFAULT '0000-00-00',\
nomd   VARCHAR(20) NOT NULL DEFAULT '',\
metka  SMALLINT NOT NULL DEFAULT 0,\
kodzap INT NOT NULL DEFAULT 0,\
kolih  DOUBLE(16,6) NOT NULL DEFAULT 0,\
cena   DOUBLE(16,6) NOT NULL DEFAULT 0,\
ei     VARCHAR(20) NOT NULL DEFAULT '' references Edizmer (kod),\
shetu  VARCHAR(20) NOT NULL DEFAULT '' references Plansh (ns),\
podr   SMALLINT NOT NULL DEFAULT 0 references Uslpod (kod),\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
dnaim  VARCHAR(70) NOT NULL DEFAULT '',\
nz     SMALLINT NOT NULL DEFAULT 0,\
shsp   VARCHAR(20) NOT NULL DEFAULT '',\
UNIQUE (datd,podr,tp,nomd,metka,kodzap,nz)) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  nomd   Номер документа
3  metka  0-материал 1-услуга
4  kodzap Код материалла/услуги
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  shetu  Счет учета
9  podr   Код подразделения
10 ktoi   Кто записал
11 vrem   Время записи
12 dnaim  Дополнительное наименование услуги
13 nz     номер записи
14 shsp   счёт списания/приобретения
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Usldokum2\n",gettext("Создание таблицы подтверждающих записей к документам"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum2 (\
tp     SMALLINT NOT NULL DEFAULT 0,\
datd   DATE NOT NULL DEFAULT '0000-00-00',\
datp   DATE NOT NULL DEFAULT '0000-00-00',\
nomd   VARCHAR(20) NOT NULL DEFAULT '',\
metka  SMALLINT NOT NULL DEFAULT 0,\
kodzap INT NOT NULL DEFAULT 0,\
kolih  DOUBLE(16,6) NOT NULL DEFAULT 0,\
cena   DOUBLE(16,6) NOT NULL DEFAULT 0,\
ei     VARCHAR(20) NOT NULL DEFAULT '' references Edizmer (kod),\
shetu  VARCHAR(20) NOT NULL DEFAULT '' references Plansh (ns),\
podr   SMALLINT NOT NULL DEFAULT 0 references Uslpod (kod),\
ktoi   SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem   int UNSIGNED NOT NULL DEFAULT 0,\
nz     SMALLINT NOT NULL DEFAULT 0,\
UNIQUE (datd,datp,podr,tp,nomd,metka,kodzap,nz),\
INDEX (datp)) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  datp   Дата подтверждения документа
3  nomd   Номер документа
4  metka  0-материал 1-услуга
5  kodzap Код услуги
6  kolih  Количество
7  cena   Цена
8  ei     Единица измерения
9  shetu  Счет учета
10 podr   Код подразделения
11 ktoi   Кто записал
12 vrem   Время записи
13 nz     номер записи в документе
*/

crtabl_crtb(strsql); /*создаём таблицу*/


//Записывается произвольный текст
printw("%s Usldokum3\n",gettext("Создание таблицы приложения к счету"));
refresh();
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Usldokum3 (\
podr    INT NOT NULL DEFAULT 0,\
god     SMALLINT NOT NULL DEFAULT 0,\
tp      SMALLINT NOT NULL DEFAULT 0,\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
nomz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
zapis   VARCHAR(255) NOT NULL DEFAULT '',\
UNIQUE(podr,god,tp,nomd,nomz)) ENGINE = MYISAM");

/*
0 podr    подразделение
1 god     год
2 tp      1-приход 2-расход
3 nomd    номер документа
4 nomz    номер записи
5 zapis   содержимое записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/***********************/
/*Учет кассовых ордеров*/
/***********************/

printw("%s Kas\n",gettext("Создание таблицы справочкика касс"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kas \
(kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    CHAR(60) NOT NULL DEFAULT '',\
shet    CHAR(20) NOT NULL DEFAULT '',\
fio     CHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kv CHAR(16) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod     Код кассы
1 naik    Наименование кассы
2 shet   Счёт кассы
3 fio     Фамилия кассира
4 ktoz   Кто записал
5 vrem   Время записи
6 kv     Код валюты
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kasop1\n",gettext("Создание таблицы операций на приход"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop1 \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    CHAR(250) NOT NULL DEFAULT '',\
shetkas CHAR(20) NOT NULL DEFAULT '',\
shetkor CHAR(20) NOT NULL DEFAULT '',\
metkapr SMALLINT NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kcn     SMALLINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счёт кассы
3 shetkor счёт корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kasop2\n",gettext("Создание таблицы операций на расход"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop2 \
(kod    CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    CHAR(250) NOT NULL DEFAULT '',\
shetkas CHAR(20) NOT NULL DEFAULT '',\
shetkor CHAR(20) NOT NULL DEFAULT '',\
metkapr SMALLINT NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kcn     SMALLINT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счёт кассы
3 shetkor счёт корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kasord\n",gettext("Создание таблицы кассовых ордеров"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasord (\
kassa   SMALLINT NOT NULL DEFAULT 0,\
tp	SMALLINT NOT NULL DEFAULT 0,\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
shetk   VARCHAR(20) NOT NULL DEFAULT '',\
kodop   VARCHAR(10) NOT NULL DEFAULT '',\
prov    SMALLINT NOT NULL DEFAULT 0,\
podt    SMALLINT NOT NULL DEFAULT 0,\
osnov   VARCHAR(250) NOT NULL DEFAULT '',\
dopol   VARCHAR(80) NOT NULL DEFAULT '',\
fio     VARCHAR(60) NOT NULL DEFAULT '',\
dokum   VARCHAR(255) NOT NULL DEFAULT '',\
god     SMALLINT NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
nb      VARCHAR(20) NOT NULL DEFAULT '',\
nomv    VARCHAR(20) NOT NULL DEFAULT '',\
UNIQUE(nomd,kassa,tp,god),\
INDEX(datd,kassa),\
INDEX(nb)) ENGINE = MYISAM");

/*
0  kassa   номер кассы
1  tp	   1-приход 2-расход
2  datd    дата документа
3  nomd    номер документа
4  shetk   счёт корреспондент
5  kodop   код операции
6  prov    0 провоки не сделаны 1-сделаны
7  podt    0 не подтвержден 1-подтвержден
8  osnov   Основание 
9  dopol   дополнение
10 fio     фамилия имя отчество кому выданы деньги если есть ведомоcть к ордеру
11 dokum   наименование дата и номер документа
12 god     год
13 ktoi    Кто записал
14 vrem    Время записи
15 nb      номер бланка
16 nomv    номер ведомости на выдачу средств корескондируется с Kasnoved
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kasord1\n",gettext("Создание таблицы записей кассовых ордеров"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasord1 (\
kassa   SMALLINT NOT NULL DEFAULT 0,\
tp	SMALLINT NOT NULL DEFAULT 0,\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
datp    DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
kontr   VARCHAR(20) NOT NULL DEFAULT '',\
suma    DOUBLE(14,2) NOT NULL DEFAULT 0,\
god     SMALLINT NOT NULL DEFAULT 0,\
metka   SMALLINT NOT NULL DEFAULT 0,\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
koment  VARCHAR(100) NOT NULL DEFAULT '',\
UNIQUE(kassa,god,tp,nomd,kontr),\
INDEX(datd),\
INDEX(datp)) ENGINE = MYISAM");

/*
0  kassa   номер кассы
1  tp	  1-приход 2-расход
2  datd    дата документа
3  datp    дата подтверждения записи
4  nomd    номер документа
5  kontr   код контрагента
6  suma    сумма
7  god     год
8  metka   0 запись не перегружена 1-перегружена (В расчёт зарплаты)
9  ktoi    Кто записал
10 vrem    Время записи
11 koment  комментрарий
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kasnomved\n",gettext("Создание таблицы журнала ведомостей"));
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasnomved (\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
god     SMALLINT NOT NULL DEFAULT 0,\
nomv    VARCHAR(10) NOT NULL DEFAULT '',\
suma    DOUBLE(14,2) NOT NULL DEFAULT 0,\
koment  VARCHAR(100) NOT NULL DEFAULT '',\
ktoi    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem    INT UNSIGNED NOT NULL DEFAULT 0,\
nomd VARCHAR(20) NOT NULL DEFAULT '',\
UNIQUE(god,nomv),\
INDEX(datd,nomv)) ENGINE = MYISAM");

/*
0  datd    дата документа
1  god     год документа
2  nomv    номер ведомости
3  suma    сумма по документа
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  nomd    номер кассового ордера из таблицы Kasord
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Kascn\n",gettext("Создание таблицы целевых назначений"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascn (\
kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
kod   код целевого назначения
naik  наименование целевого назначения
ktoz  кто записал
vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Kascnsl\n",gettext("Создание таблицы сальдо по целевым назначениям"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascnsl (\
god   SMALLINT NOT NULL DEFAULT 0,\
kod   SMALLINT NOT NULL DEFAULT 0,\
ks    INT NOT NULL DEFAULT 0,\
saldo DOUBLE(10,2) NOT NULL DEFAULT 0,\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(god,kod,ks)) ENGINE = MYISAM");

/*
0 god   год
1 kod   код целевого назначения
2 ks    код кассы
3 saldo сальдо
4 ktoz  кто записал
5 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

//************************************************************************
//Подсистема "Учёт командировочных расходов"
//************************************************************************



printw("%s Ukrvkr\n",gettext("Создание таблицы видов командировок"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrvkr (\
kod	CHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код вида командировок
1 naik  наименование вида командировок
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Ukrgrup\n",gettext("Создание таблицы групп видов командировок"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrgrup (\
kod    SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код группы вида командировок
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Ukrkras\n",gettext("Создание таблицы видов расходов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrkras (\
kod     INT NOT NULL DEFAULT 0 PRIMARY KEY,\
naim    VARCHAR(80) NOT NULL DEFAULT '',\
cena    DOUBLE(10,2) NOT NULL DEFAULT 0,\
shet    VARCHAR(20) NOT NULL DEFAULT '',\
kgr     INT NOT NULL DEFAULT 0,\
eiz     VARCHAR(10) NOT NULL DEFAULT '',\
mnds    SMALLINT NOT NULL DEFAULT 0,\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(naim)) ENGINE = MYISAM");

/*
0 kod   код вида расходов
1 naim  наименование вида расхода
2 cena  цена
3 shet  счёт учета
4 kgr   код группы
5 eiz   единица измерения
6 mnds  0-цена введена с НДС 1-без НДС
7 ktoz  кто записал
8 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Ukrdok\n",gettext("Создание таблицы шапок документов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok (\
god     SMALLINT NOT NULL DEFAULT 0,\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
datd    DATE NOT NULL DEFAULT '0000-00-00',\
kont	VARCHAR(20) NOT NULL DEFAULT '',\
pnaz    VARCHAR(80) NOT NULL DEFAULT '',\
organ   VARCHAR(80) NOT NULL DEFAULT '',\
celk    VARCHAR(80) NOT NULL DEFAULT '',\
datn    DATE NOT NULL DEFAULT '0000-00-00',\
datk    DATE NOT NULL DEFAULT '0000-00-00',\
nompr   VARCHAR(20) NOT NULL DEFAULT '',\
datp    DATE NOT NULL,\
mprov   SMALLINT NOT NULL DEFAULT 0,\
mpodt   SMALLINT NOT NULL DEFAULT 0,\
vkom    VARCHAR(10) NOT NULL DEFAULT '',\
nomao   VARCHAR(10) NOT NULL DEFAULT '',\
datao   DATE NOT NULL DEFAULT '0000-00-00',\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(god,nomd),\
INDEX(datd,nomd),\
INDEX(datao,nomao)) ENGINE = MYISAM");

/*
 0 god   год документа
 1 nomd  Номер документа
 2 datd  Дата документа
 3 kont  код контрагента
 4 pnaz  пунк назначения
 5 organ наименование предприятия или организации
 6 celk  Цель командировки
 7 datn  Дата начала командировки
 8 datk  Дата конца командировки
 9 nompr Номер приказа
10 datp  Дата приказа
11 mprov Метка выполнения проводок 0-не выполнены 1-выполнены
12 mpodt Метка присутствия записей в документе 0-нет записей 1-есть
13 vkom  Вид командировки
14 nomao Номер авансового отчёта
15 datao Дата авансового отчёта
16 ktoz  кто записал
17 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Ukrdok1\n",gettext("Создание таблицы записей в документах"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok1 (\
datd	DATE NOT NULL DEFAULT '0000-00-00',\
nomd	VARCHAR(20) NOT NULL DEFAULT '',\
kodr    INT NOT NULL DEFAULT 0,\
shet	VARCHAR(20) NOT NULL DEFAULT '',\
kolih   DOUBLE(15,4) NOT NULL DEFAULT 0,\
cena    DOUBLE(15,2) NOT NULL DEFAULT 0,\
ediz    VARCHAR(10) NOT NULL DEFAULT '',\
snds    DOUBLE(10,2) NOT NULL DEFAULT 0,\
ktoz    SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
nomz	SMALLINT NOT NULL DEFAULT 0,\
kontr   VARCHAR(20) NOT NULL DEFAULT '',\
kdrnn   VARCHAR(20) NOT NULL DEFAULT '',\
ndrnn   VARCHAR(30) NOT NULL DEFAULT '',\
sn      VARCHAR(10) NOT NULL DEFAULT '',\
dvnn DATE NOT NULL DEFAULT '0000-00-00',\
ss DOUBLE(10,2) NOT NULL DEFAULT 0,\
UNIQUE(datd,nomd,kodr,shet,nomz))" );

/*
 0 datd  Дата документа
 1 nomd  Номер документа
 2 kodr  Код расхода
 3 shet  Счет
 4 kolih Количество
 5 cena  Сумма без НДС
 6 ediz  Единица измерения
 7 snds  Сумма НДС
 8 ktoz  Кто записал
 9 vrem  Время записи
10 nomz  Номер записи
11 kontr Код контрагента от кого получена услуга или товар за наличные(вводится чтобы была сделана дополнительная проводка)
12 kdrnn Код контрагента для реестра налоговых накладных
13 ndrnn Номер документа, который в реестре нал. накладных будет записан как номер налоговой накладной
14 sn    счёт НДС (Иногда нужно чтобы в одном документе разные записи шли на разные счета)
15 dvnn  дата выписки налоговой накладной
16 ss    Страховой сбор
*/

crtabl_crtb(strsql); /*создаём таблицу*/



//*************************************************************
//Подсистема "Учёт путевых листов"
//************************************************************

/************31.07.2003******************/

printw("%s Uplpodr\n",gettext("Создание таблицы подразделений"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplpodr (\
kod   SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
naik  VARCHAR(80) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED DEFAULT 0,\
vrem  INT UNSIGNED DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код подразделения
1 naik  наименование подразделения
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uplmt\n",gettext("Создание таблицы марок топлива"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmt (\
kodt  CHAR(20) NOT NULL DEFAULT '' PRIMARY KEY,\
kodm  INT NOT NULL DEFAULT 0,\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kodt  код марки топлива
1 kodm  код материалла в списке материалов
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uplouot\n",gettext("Создание таблицы объектов учета остатков топлива"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplouot (\
kod   SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
mt    SMALLINT NOT NULL DEFAULT 0,\
skl   SMALLINT NOT NULL DEFAULT 0,\
naik  VARCHAR(80) NOT NULL DEFAULT '',\
kontr VARCHAR(20) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 kod   код объекта
1 mt    0-заправка 1-водитель
2 skl   код склада в материальном учете
3 naik  наименование
4 kontr код контрагента
5 ktoz  кто записал
6 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Uplavt\n",gettext("Создание таблицы списка автомобилей"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplavt (\
kod   SMALLINT NOT NULL DEFAULT 0 PRIMARY KEY,\
nomz  VARCHAR(20) NOT NULL DEFAULT '',\
kzv   SMALLINT NOT NULL DEFAULT 0,\
naik  VARCHAR(80) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(nomz)) ENGINE = MYISAM");

/*
0  kod   код автомобиля
1  nomz  номерной знак
2  kzv   код закрепленного водителя
3  naik  наименование
4  ktoz  кто записал
5  vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Uplnst\n",gettext("Создание таблицы норм списания топлива"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplnst (\
kod   CHAR(16) NOT NULL DEFAULT '' PRIMARY KEY,\
naik  CHAR(124) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0  kod   код нормы списания топлива
1  naik  наименование нормы списания топлива
2  ktoz  кто записал
3  vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Uplavt1\n",gettext("Создание таблицы норм списания топлива к автомобилю"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplavt1 (\
kod   SMALLINT NOT NULL DEFAULT 0,\
knst  CHAR(16) NOT NULL DEFAULT '',\
nst   float(8,3) NOT NULL DEFAULT 0.,\
ei    VARCHAR(16) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kod,knst)) ENGINE = MYISAM");

/*
0  kod   код автомобиля
1  knst  код нормы списания топлива
2  nst   норма списания топлива
3  ei    единица изменения
4  ktoz  кто записал
5  vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Upldok",gettext("Создание таблицы шапки путевых листов"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok (\
datd DATE NOT NULL,\
kp SMALLINT NOT NULL DEFAULT 0,\
nomd VARCHAR(20) NOT NULL DEFAULT '',\
god SMALLINT NOT NULL DEFAULT 0,\
nomb VARCHAR(20) NOT NULL DEFAULT '',\
ka SMALLINT NOT NULL DEFAULT 0,\
kv SMALLINT NOT NULL DEFAULT 0,\
psv INT UNSIGNED NOT NULL DEFAULT 0,\
psz INT UNSIGNED NOT NULL DEFAULT 0,\
denn DATE NOT NULL DEFAULT '0000-00-00',\
denk DATE NOT NULL DEFAULT '0000-00-00',\
vremn TIME NOT NULL DEFAULT '00:00:00',\
vremk TIME NOT NULL DEFAULT '00:00:00',\
mt SMALLINT NOT NULL DEFAULT 0,\
mp SMALLINT NOT NULL DEFAULT 0,\
otmet VARCHAR(250) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
nz SMALLINT NOT NULL DEFAULT 0,\
k00 CHAR(16) NOT NULL DEFAULT '00',\
UNIQUE(god,kp,nomd),\
UNIQUE(ka,datd,nz),\
INDEX(datd),\
INDEX(nomd)) ENGINE = MYISAM");

/*
0  datd  дата документа
1  kp    код подразделения (если равно 0 - акт списания)
2  nomd  номер документа
3  god   год документа
4  nomb  номер бланка документа строгой отчётности
5  ka    код автомобиля
6  kv    код водителя
7  psv   показания спидометра при выезде
8  psz   показания спидометра при возвращении
9 12 denn  Дата выезда
10 13 denk  Дата возвращения
11 14 vremn время выезда
12 15 vremk время возвращения
13 16 mt    0-не списано (или не полностью списано) топливо пообъектно 1-списано
14 17 mp    0-проводки не сделаны 1-сделаны
15 18 otmet Особенные отметки
16 20 ktoz  кто записал
17 21 vrem  время записи
18 nz     номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
19 k00   код контрагента реквизиты которого берутся в распечатку
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Upldok1\n",gettext("Создание таблицы записей движения топлива"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok1 (\
datd  DATE NOT NULL DEFAULT '0000-00-00',\
kp SMALLINT NOT NULL DEFAULT 0,\
nomd VARCHAR(10) NOT NULL DEFAULT '',\
nzap SMALLINT NOT NULL DEFAULT 0,\
ka SMALLINT NOT NULL DEFAULT 0,\
kv SMALLINT NOT NULL DEFAULT 0,\
kodsp SMALLINT NOT NULL DEFAULT 0,\
tz SMALLINT NOT NULL DEFAULT 0,\
kodtp VARCHAR(20) NOT NULL DEFAULT '',\
kolih DOUBLE(10,3) NOT NULL DEFAULT 0,\
kom VARCHAR(80) NOT NULL DEFAULT '',\
kap SMALLINT NOT NULL DEFAULT 0,\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
nz SMALLINT NOT NULL DEFAULT 0,\
UNIQUE(datd,kp,nomd,nzap),\
INDEX(datd,ka)) ENGINE = MYISAM");

/*
 0 datd  дата документа
 1 kp    код подразделения (если равно 0 - акт списания)
 2 nomd  номер документа
 3 nzap  номер записи в документе
 4 ka    код автомобиля
 5 kv    код водителя
 6 kodsp для приходной записи код поставщика топлива(заправка или водитель) / для расходной записи 0
 7 tz    1-приход 2-расход
 8 kodtp код топлива
 9 kolih количество
10 kom   комментарий
11 kap   код автомобиля с которого передали топливо (Если kodsp это код водителя, иначе 0)
12 ktoz  кто записал
13 vrem  время записи
14 nz    номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Upldok2a\n",gettext("Создание таблицы записей списания топлива по счетам"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok2a (\
datd  DATE NOT NULL DEFAULT '0000-00-00',\
kp SMALLINT NOT NULL DEFAULT 0,\
nomd VARCHAR(32) NOT NULL DEFAULT '',\
kt VARCHAR(32) NOT NULL DEFAULT '',\
shet VARCHAR(10) NOT NULL DEFAULT '',\
zt float(8,3) NOT NULL DEFAULT 0,\
zpn float(8,3) NOT NULL DEFAULT 0.,\
koment VARCHAR(255) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(datd,kp,nomd,kt,shet)) ENGINE = MYISAM");

/*
0 datd  дата документа
1 kp    код подразделения
2 nomd  номер документа
3 kt    код топлива
4 shet  счёт списания
5 zt    затраты топлива по факту
6 zpn  затраты по норме
7 koment коментарий
8 ktoz  кто записал
9 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Upldok3\n",gettext("Создание таблицы списания топлива к путевому листу"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok3 (\
datd  DATE NOT NULL DEFAULT '0000-00-00',\
kp SMALLINT NOT NULL DEFAULT 0,\
nomd VARCHAR(16) NOT NULL DEFAULT '',\
knst VARCHAR(16) NOT NULL DEFAULT '',\
kl   DOUBLE(10,3) NOT NULL DEFAULT 0.,\
nst  float(8,3) NOT NULL DEFAULT 0.,\
zn   float(8,3) NOT NULL DEFAULT 0.,\
zf   float(8,3) NOT NULL DEFAULT 0.,\
ei    VARCHAR(16) NOT NULL DEFAULT '',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(datd,kp,nomd,knst)) ENGINE = MYISAM");

/*
0  datd  дата документа
1  kp    код подразделения (если равно 0 - акт списания)
2  nomd  номер документа
3  knst  код нормы списания топлива
4  kl    количество
5  nst   норма списания топлива
6  zn    затраты по норме
7  zf    затраты по факту
8  ei    единица измерения
9 kom   коментарий
10  ktoz  кто записал
11 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Uplsal\n",gettext("Создание таблицы сальдо по топливу"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplsal (\
god SMALLINT NOT NULL DEFAULT 0,\
ka SMALLINT NOT NULL DEFAULT 0,\
kv SMALLINT NOT NULL DEFAULT 0,\
kmt VARCHAR(20) NOT NULL DEFAULT '',\
sal DOUBLE(10,2) NOT NULL DEFAULT 0,\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(god,ka,kv,kmt)) ENGINE = MYISAM");

/*
0 god   год
1 ka    код автомобиля
2 kv    код водителя
3 kmt   код марки топлива
4 sal   сальдо
5 ktoz  кто записал
6 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Uplmtsc\n",gettext("Создание таблицы средних цен на топливо"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmtsc (\
datsc DATE NOT NULL DEFAULT '0000-00-00',\
kodt  CHAR(20) NOT NULL DEFAULT '',\
srcn  float(7,2) NOT NULL DEFAULT 0,\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
kp    float(7,3) NOT NULL DEFAULT 0,\
UNIQUE (datsc,kodt)) ENGINE = MYISAM");

/*
0 datsc дата начала действия настройки
1 kodt  код топлива
2 srcn  средняя цена топлива
3 ktoz  кто записал
4 vrem  время записи
5 kp    коэффициент перевода литры => киллограммы
*/

crtabl_crtb(strsql); /*создаём таблицу*/

/***********************************************/

printw("%s Specif\n",gettext("Создание таблицы спецификаций"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Specif \
(kodi   INT NOT NULL DEFAULT 0,\
kodd    INT NOT NULL DEFAULT 0,\
kolih   DOUBLE(20,10) NOT NULL DEFAULT 0,\
koment  VARCHAR(60) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
kz      SMALLINT NOT NULL DEFAULT 0,\
ei      VARCHAR(10) NOT NULL DEFAULT '',\
UNIQUE(kodi,kodd)) ENGINE = MYISAM");

/*
0 kodi    код изделия
1 kodd    код детали
2 kolih   количество
3 koment  комментарий
4 ktoz	кто записал
5 vrem	время записи
6 kz    код записи 0-материалы 1-услуги
7 ei    единица измерения
*/


crtabl_crtb(strsql); /*создаём таблицу*/










/****************************************/


printw("%s Reenn\n",gettext("Создание таблицы реестра выданных налоговых накладных"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn (\
nz INT UNSIGNED AUTO_INCREMENT NOT NULL,\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nnn VARCHAR(20) NOT NULL DEFAULT '',\
sumd DOUBLE(10,2) NOT NULL DEFAULT 0,\
snds DOUBLE(10,2) NOT NULL DEFAULT 0,\
mt TINYINT NOT NULL DEFAULT 0,\
inn VARCHAR(30) NOT NULL DEFAULT '',\
nkontr VARCHAR(255) NOT NULL DEFAULT '',\
mi TINYINT NOT NULL DEFAULT 0,\
datdi DATE NOT NULL DEFAULT '0000-00-00',\
nomdi VARCHAR(20) NOT NULL DEFAULT '',\
koment VARCHAR(100) NOT NULL DEFAULT '',\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
gr VARCHAR(10) NOT NULL DEFAULT '',\
ko VARCHAR(10) NOT NULL DEFAULT '',\
vd CHAR(32) NOT NULL DEFAULT '',\
datv DATE NOT NULL DEFAULT '0000-00-00',\
dnz INT NOT NULL DEFAULT 0,\
nnni INT NOT NULL DEFAULT 0,\
PRIMARY KEY (nz),\
INDEX(datd,nnn)) ENGINE = MYISAM");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата документа
2  nnn     номер налоговой накладной
3  sumd    общая сумма по документа
4  snds    сумма НДС
5  mt      метка документа (каким НДС обкладывается ) 0-20% 1-0% 2-0% 3-0% 4-7%
6  inn     индивидуальный налоговый номер
7  nkontr  наименование контрагента
8  mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
9 datdi   дата документа импорта
10 nomdi   номер документа импорта
11 koment  комментарий
12 ktoi    Кто записал
13 vrem    Время записи
14 gr      код группы документа
15 ko      код операции в импортируемом документе
16 vd      вид документа
17 datv    дата выписки документа
18 dnz     дневной номер записи
19 nnni    номер налоговой накладной в цифровом виде - для правильной сортировки по номеру
***************/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Reenn1\n",gettext("Создание таблицы реестра полученных налоговых накладных"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn1 (\
nz INT UNSIGNED AUTO_INCREMENT NOT NULL,\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nnn VARCHAR(20) NOT NULL DEFAULT '',\
sumd1 DOUBLE(10,2) NOT NULL DEFAULT 0,\
snds1 DOUBLE(10,2) NOT NULL DEFAULT 0,\
sumd2 DOUBLE(10,2) NOT NULL DEFAULT 0,\
snds2 DOUBLE(10,2) NOT NULL DEFAULT 0,\
sumd3 DOUBLE(10,2) NOT NULL DEFAULT 0,\
snds3 DOUBLE(10,2) NOT NULL DEFAULT 0,\
sumd4 DOUBLE(10,2) NOT NULL DEFAULT 0,\
snds4 DOUBLE(10,2) NOT NULL DEFAULT 0,\
inn VARCHAR(30) NOT NULL DEFAULT '',\
nkontr VARCHAR(255) NOT NULL DEFAULT '',\
mi TINYINT NOT NULL DEFAULT 0,\
datdi DATE NOT NULL DEFAULT '0000-00-00',\
nomdi VARCHAR(20) NOT NULL DEFAULT '',\
koment VARCHAR(100) NOT NULL DEFAULT '',\
gr VARCHAR(10) NOT NULL DEFAULT '',\
ktoi SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
ko VARCHAR(10) NOT NULL DEFAULT '',\
dvd DATE NOT NULL DEFAULT '0000-00-00',\
vd CHAR(32) NOT NULL DEFAULT '',\
PRIMARY KEY (nz),\
INDEX(datd,nnn)) ENGINE = MYISAM");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата получения налоговой накладной
2  nnn     номер налоговой накладной
3  sumd1    общая сумма по документу по основной ставке
4  snds1    сумма НДС                по основной ставке
5  sumd2    общая сумма по документу по ставке 7%
6  snds2    сумма НДС                по ставке 7%
7  sumd3    общая сумма по документу по ставке 0%
8  snds3    сумма НДС                по ставке 0%
9  sumd4    общая сумма по документу освобождены от налогообложения
10 snds4    сумма НДС                освобождены от налогообложения
11 inn     индивидуальный налоговый номер
12 nkontr  наименование контрагента
13 mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
14 datdi   дата документа импорта
15 nomdi   номер документа импорта
16 koment  комментарий
17 gr      группа документа
18 ktoi    Кто записал
19 vrem    Время записи
20 ko      код операции в импортируемом документе
21 dvd     Дата выписки налоговой накладной
22 vd      Вид документа
***************/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Reegrup1\n",gettext("Создание таблицы групп полученных налоговых накладных"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup1 \
(kod    VARCHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Reegrup2\n",gettext("Создание таблицы групп выданных налоговых накладных"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup2 \
(kod    VARCHAR(10) NOT NULL DEFAULT '' PRIMARY KEY,\
naik    VARCHAR(80) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/*****************************************/
/*Учёт доверенностей*/
/*************************************/
printw("%s Uddok\n",gettext("Создание таблицы заголовков доверенностей"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok \
(datd   DATE NOT NULL DEFAULT '0000-00-00',\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
datds   DATE NOT NULL DEFAULT '0000-00-00',\
komu    VARCHAR(80) NOT NULL DEFAULT '',\
post    VARCHAR(255) NOT NULL DEFAULT '',\
podok   VARCHAR(50) NOT NULL DEFAULT '',\
otis    VARCHAR(100) NOT NULL DEFAULT '',\
datn    DATE NOT NULL DEFAULT '0000-00-00',\
nomn    VARCHAR(100) NOT NULL DEFAULT '',\
seriq VARCHAR(20) NOT NULL DEFAULT '',\
nomerd VARCHAR(20) NOT NULL DEFAULT '',\
datavd DATE NOT NULL DEFAULT '0000-00-00',\
vidan  VARCHAR(200) NOT NULL DEFAULT '',\
dolg   VARCHAR(50) NOT NULL DEFAULT '',\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(datd,nomd),\
INDEX(nomd)) ENGINE = MYISAM");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  datds действительна до
3  komu  кому выдана доверенность
4  post  поставщик
5  podok по документу
6  otis  отметка об использовании
7  datn  дата наряда
8  nomn  номер наряда
9 seriq серия
10 nomerd номер документа
11 datavd дата выдачи документа
12 vidan  кем выдан
13 dolg   должность
14  ktoz  кто записал
15 vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/



printw("%s Uddok1\n",gettext("Создание таблицы записей в доверенности"));
refresh();

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok1 \
(datd  DATE NOT NULL,\
nomd    VARCHAR(20) NOT NULL DEFAULT '',\
nz      SMALLINT NOT NULL DEFAULT 0,\
zapis   VARCHAR(200) NOT NULL DEFAULT '',\
ei      VARCHAR(20) NOT NULL DEFAULT '',\
kol     DOUBLE(16,4) NOT NULL DEFAULT 0,\
ktoz	SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem	INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(datd,nomd,nz)) ENGINE = MYISAM");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  nz    уникальный номер записи
3  zapis содержимое записи
4  ei    единица измерения
5  kol   количество
6  ktoz  кто записал
7  vrem  время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s icebuser\n",gettext("Создание таблицы операторов"));
refresh();
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE icebuser \
(login CHAR(32) NOT NULL PRIMARY KEY DEFAULT '',\
fio VARCHAR(255) NOT NULL DEFAULT '',\
un  INT UNSIGNED NOT NULL DEFAULT 0,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
logz   VARCHAR(32) NOT NULL DEFAULT '',\
vrem   INT UNSIGNED NOT NULL DEFAULT 0,\
gkd TINYINT NOT NULL DEFAULT 0,\
gkb TINYINT NOT NULL DEFAULT 0,\
gkn TINYINT NOT NULL DEFAULT 0,\
mud TINYINT NOT NULL DEFAULT 0,\
mub TINYINT NOT NULL DEFAULT 0,\
mun TINYINT NOT NULL DEFAULT 0,\
pdd TINYINT NOT NULL DEFAULT 0,\
pdb TINYINT NOT NULL DEFAULT 0,\
pdn TINYINT NOT NULL DEFAULT 0,\
zpd TINYINT NOT NULL DEFAULT 0,\
zpb TINYINT NOT NULL DEFAULT 0,\
zpn TINYINT NOT NULL DEFAULT 0,\
osd TINYINT NOT NULL DEFAULT 0,\
osb TINYINT NOT NULL DEFAULT 0,\
osn TINYINT NOT NULL DEFAULT 0,\
uud TINYINT NOT NULL DEFAULT 0,\
uub TINYINT NOT NULL DEFAULT 0,\
uun TINYINT NOT NULL DEFAULT 0,\
kod TINYINT NOT NULL DEFAULT 0,\
kob TINYINT NOT NULL DEFAULT 0,\
kon TINYINT NOT NULL DEFAULT 0,\
krd TINYINT NOT NULL DEFAULT 0,\
krb TINYINT NOT NULL DEFAULT 0,\
krn TINYINT NOT NULL DEFAULT 0,\
pld TINYINT NOT NULL DEFAULT 0,\
plb TINYINT NOT NULL DEFAULT 0,\
pln TINYINT NOT NULL DEFAULT 0,\
nnd TINYINT NOT NULL DEFAULT 0,\
nnb TINYINT NOT NULL DEFAULT 0,\
nnn TINYINT NOT NULL DEFAULT 0,\
udd TINYINT NOT NULL DEFAULT 0,\
udb TINYINT NOT NULL DEFAULT 0,\
udn TINYINT NOT NULL DEFAULT 0,\
vsd TINYINT NOT NULL DEFAULT 0,\
vsb TINYINT NOT NULL DEFAULT 0,\
vsn TINYINT NOT NULL DEFAULT 0,\
vdb TINYINT NOT NULL DEFAULT 0,\
tn  INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(un)) ENGINE = MYISAM");

/*
 0 login  логин оператора
 1 fio    фамилия имя отчество
 2 un     уникальный номер оператора
 3 kom    коментарий
 4 logz   логин сделавшего запись
 5 vrem   Время записи

 6 gkd    если равно 0 то разрешена работа с подсистемой "Главная книга"
 7 gkb    если равно 0 то разрешена блокировка подсистемы "Главная книга"
 8 gkn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Главная книга"

 9 mud    если равно 0 то разрешена работа с подсистемой "Материальный учёт"
10 mub    если равно 0 то разрешена блокировка подсистемы "Материальный учёт"
11 mun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Материальный учёт"

12 pdd    если равно 0 то разрешена работа с подсистемой "Платёжные документы"
13 pdb    если равно 0 то разрешена блокировка подсистемы "Платёжные документы"
14 pdn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Платёжные документы"

15 zpd    если равно 0 то разрешена работа с подсистемой "Заработная плата"
16 zpb    если равно 0 то разрешена блокировка подсистемы "Заработная плата"
17 zpn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Заработная плата"

18 osd    если равно 0 то разрешена работа с подсистемой "Учёт основных средств"
19 osb    если равно 0 то разрешена блокировка подсистемы "Учёт основных средств"
20 osn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт основных средств"

21 uud    если равно 0 то разрешена работа с подсистемой "Учёт услуг"
22 uub    если равно 0 то разрешена блокировка подсистемы "Учёт услуг"
23 uun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт услуг"

24 kod    если равно 0 то разрешена работа с подсистемой "Учёт кассовых ордеров"
25 kob    если равно 0 то разрешена блокировка подсистемы "Учёт кассовых ордеров"
26 kon    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт кассовых ордеров"

27 krd    если равно 0 то разрешена работа с подсистемой "Учёт командировочных расходов"
28 krb    если равно 0 то разрешена блокировка подсистемы "Учёт командировочных расходов"
29 krn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт командироваочных расходов"

30 pld    если равно 0 то разрешена работа с подсистемой "Учёт путевых листов"
31 plb    если равно 0 то разрешена блокировка подсистемы "Учёт путувых листов"
32 pln    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт путевых листов"

33 nnd    если равно 0 то разрешена работа с подсистемой "Реестр налоговых накладных"
34 nnb    если равно 0 то разрешена блокировка подсистемы "Реестр налоговых накладных"
35 nnn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Реестр налоговых накладных"

36 udd    если равно 0 то разрешена работа с подсистемой "Учёт доверенностей"
37 udb    если равно 0 то разрешена блокировка подсистемы "Учёт доверенностей"
38 udn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт доверенностей"

39 vsd    если равно 0 то разрешена работа с подсистемой "Выписка счетов"
40 vsb    если равно 0 то разрешена блокировка подсистемы "Выписка счетов"
41 vsn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Выписка счетов"

42 vdb    если равно 0 то выгрузка дампа базы оператору разрешена
43 tn     табельный номер
*/

crtabl_crtb(strsql); /*создаём таблицу*/


printw("%s Blok\n",gettext("Создание таблицы блокировок"));
refresh();
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Blok \
(kod INT NOT NULL DEFAULT 0,\
god SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
mes SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
log VARCHAR(255) NOT NULL DEFAULT '',\
shet VARCHAR(255) NOT NULL DEFAULT '',\
ktoz INT NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kod,god,mes)) ENGINE = MYISAM");

/*
0 kod  - код подсистемы
1 god  - год
2 mes  - месяц
3 log  - логины операторов которым отменена блокировка
4 shet - счёта которые заблокированы (только для подсистемы "Главная книга*)
5 ktoz - номер оператора сделавшего запись
6 vrem - время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Alx \n",gettext("Создание таблицы настроек"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Alx \
(fil VARCHAR(255) NOT NULL DEFAULT '',\
ns INT NOT NULL DEFAULT 0,\
ktoz INT NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
str TEXT NOT NULL DEFAULT '',\
UNIQUE(fil,ns)) ENGINE = MYISAM");

/*
0 fil  - имя файла
1 ns   - номер строки
2 ktoz - номер оператора сделавшего запись
3 vrem - время записи
4 str  - строка текста
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Doc\n",gettext("Создание таблицы документации"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Doc \
(fil VARCHAR(255) NOT NULL DEFAULT '',\
ns INT NOT NULL DEFAULT 0,\
str TEXT NOT NULL DEFAULT '',\
UNIQUE(fil,ns)) ENGINE = MYISAM");

/*
0 fil  - имя файла
1 ns   - номер строки
2 str  - строка текста
*/

crtabl_crtb(strsql); /*создаём таблицу*/

printw("%s Nalog\n",gettext("Создание таблицы налогов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Nalog \
(vn INT NOT NULL DEFAULT 0,\
dndn DATE NOT NULL DEFAULT '0000-00-00',\
pr DOUBLE NOT NULL DEFAULT 0,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz INT NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(vn,dndn)) ENGINE = MYISAM");

/*
0 vn   - вид налога 0-НДС
1 dndn - Дата начала действия налога
2 pr   - процент
3 kom  - коментарий
4 ktoz - номер оператора сделавшего запись
5 vrem - время записи
*/

crtabl_crtb(strsql); /*создаём таблицу*/


/*Записываем дату версии базы данных*/
sprintf(strsql,"replace into Alx (fil,ns,ktoz,vrem,str) values('iceb_data_baz',1,%d,%ld,'%s')",iceb_t_getuid(),time(NULL),ICEB_DATA_BAZ);
sql_zapis(strsql,0,0);


}
/********************************/
/*создание таблицы*/
/****************************/
int crtabl_crtb(const char *strsql)
{
if(sql_zap(&bd,strsql) != 0)
 { 
  printw("%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
  
  if(sql_nerror(&bd) != ER_TABLE_EXISTS_ERROR) /*Таблица уже существует*/
   {
    printw("%s\n",strsql);
    OSTANOV();
   }
  return(1);
 } 
else
 printw("%s\n",gettext("Таблица создана"));
refresh();
return(0);
}
