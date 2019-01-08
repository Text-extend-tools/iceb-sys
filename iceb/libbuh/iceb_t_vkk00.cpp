/*$Id: iceb_t_vkk00.c,v 5.12 2014/02/28 05:13:55 sasa Exp $*/
/*16.11.2016	17.02.2011	Белых А.И.	iceb_t_vkk00.с
Ввод наименовани кода контрагента с кодом 00 при первичном запуске подсистем
*/
#include "buhl.h"


void iceb_t_vkk00()
{
char strsql[1024];
class VVOD  VVOD1(2);
class iceb_tu_str naim_kontr("");
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите наименование вашей организации"));
//clear();
helstr(LINES-1,0,"F2",gettext("записать"),NULL);

if(vvod1(&naim_kontr,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 {  
 }
else
 {
  sprintf(strsql,"insert into Kontragent (kodkon,naikon,ktoz,vrem) values ('00','%s',%d,%ld)",naim_kontr.ravno(),getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }    


/*вводим нормативно-справочную информация*/

/*подсистема главная кника*/
sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("дол"),gettext("Доллар"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,0,1);
sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("евр"),gettext("Евро"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);
sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("руб"),gettext("Рубли"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

/*Подсистема платёжные документы*/
sprintf(strsql,"insert into Opltp (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("оп"),
gettext("Оплата поставщику"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Opltt (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("пт"),
gettext("Платёжное требование"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

/*подсистема Учёт кассовых ордеров*/
sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("ппл"),
gettext("Получено от подотчётного лица"),
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("прс"),
gettext("Получено с расчётного счёта"),
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("хр"),
gettext("Хозяйственные расходы"),
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("ком"),
gettext("Командировочные"),
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("зп"),
gettext("Заработная плата"),
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

/*подсистема Материальный учёт*/

sprintf(strsql,"insert into Foroplat (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("осс"),
gettext("Оплата с расчётного счёта"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Foroplat (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("он"),
gettext("Оплата наличными"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ОСТ"),
gettext("Стартовый остаток"),
iceb_t_getuid(),
time(NULL),
0,1);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_t_getuid(),
time(NULL),
1,1);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("поп"),
gettext("Приход от поставщика"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("пип"),
gettext("Приход из производства"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_t_getuid(),
time(NULL),
2,1);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_t_getuid(),
time(NULL),
1,0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рп"),
gettext("Реализация потребителю"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рвп"),
gettext("Расход в производство"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_t_getuid(),
time(NULL),
2,0);

sql_zapis(strsql,1,1);
sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("сп"),
gettext("Списание"),
iceb_t_getuid(),
time(NULL),
2,0);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2009')",
gettext("Шт"),
gettext("Штук"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0301')",
gettext("кг"),
gettext("Килограм"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0306')",
gettext("т"),
gettext("Тонна"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0138')",
gettext("л"),
gettext("Литр"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("ус"),
gettext("Услуга"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0102')",
gettext("км"),
gettext("Километр"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0420')",
gettext("квт"),
gettext("Киловатт"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2454')",
gettext("грн"),
gettext("Гривня"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0415')",
gettext("квт*ч"),
gettext("Киловатт час"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0134')",
gettext("м3"),
gettext("Метр кубический"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0171')",
gettext("мec"),
gettext("Месяц"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,0);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2355')",gettext("ткм"),gettext("Тонно-киллометры"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2908')",gettext("мч"),gettext("Моточасы"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Nalog values (0,'1990-1-1',20.,'',%d,%ld)",getuid(),time(NULL));
sql_zapis(strsql,1,0);
/*подсистема Учёт основных средств*/

sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_t_getuid(),
time(NULL),
1,1);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("поп"),
gettext("Приход от поставщика"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_t_getuid(),
time(NULL),
2,1);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_t_getuid(),
time(NULL),
1,0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рп"),
gettext("Реализация потребителю"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("сп"),
gettext("Списание"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ис"),
gettext("Изменение стоимости"),
iceb_t_getuid(),
time(NULL),
2,0);

sql_zapis(strsql,1,1);

/*подсистема Учёт услуг*/
sprintf(strsql,"insert into Usloper1 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("пу"),
gettext("Полученные услуги"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Usloper1 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ту"),
gettext("Транспортные услуги"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Usloper2 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ву"),
gettext("Выполненные услуги"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Usloper2 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ту"),
gettext("Транспортные услуги"),
iceb_t_getuid(),
time(NULL),
0,0);

sql_zapis(strsql,1,1);

/*подсистема Заработная плата*/

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
1,
gettext("Должнострой оклад"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
2,
gettext("Больничный"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код больничного|2\n' where fil='zarnast.alx' and str like 'Код больничного|%%'");
sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Отпускные"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Почасовая оплата"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Аванс"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
1,
gettext("Налог на доходы физических лиц"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код подоходного налога|1\n' where fil='zarnast.alx' and str like 'Код подоходного налога|%%'");
sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
2,
gettext("Получено в кассе"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код получения зарплаты в кассе|2\n' where fil='zarnast.alx' and str like 'Код получения зарплаты в кассе|%%'");
sql_zapis(strsql,1,1);
/*************
sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Единый социальный взнос"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код удержания единого социального взноса|3\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса|%%'");
sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Единый социальный взнос с больничных"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для больничных|4\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для больничных|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Единый социальный взнос для договоров подряда"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для договоров|5\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для договоров|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
6,
gettext("Единый социальный взнос с инвалидов"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для инвалидов|6\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для инвалидов|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
7,
gettext("Единый социальный взнос с военных"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для военных|7\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для военных|%%'");
sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
8,
gettext("Единый социальный взнос с больничных для инвалидов"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса с больничных для инвалидов|8\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса с больничных для инвалидов|%%'");
sql_zapis(strsql,1,1);
***************************************/



sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Перечислено на карт-счёт"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код перечисления зарплаты на счет|3\n' where fil='zarnast.alx' and str like 'Код перечисления зарплаты на счет|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Профсоюзный взнос"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Профсоюзный фонд-код отчисления|4\n' where fil='zarnast.alx' and str like 'Профсоюзный фонд-код отчисления|%%'");
sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Военный сбор"),
iceb_t_getuid(),
time(NULL),
0);

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код военного сбора|5\n' where fil='zarnast.alx' and str like 'Код военного сбора|%%'");
sql_zapis(strsql,1,1);



sprintf(strsql,"update Alx set str='Обязательные удержания|1,4,5\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'");
sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Kateg (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Рабочий"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Kateg (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
2,
gettext("Служащий"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zvan (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Не аттестован"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код звания для неатестованых|1\n' where fil='zarnast.alx' and str like 'Код звания для неатестованых|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Коды званий гражданских|1\n' where fil='zarnast.alx' and str like 'Коды званий гражданских|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Единый социальный взнос"),
iceb_t_getuid(),
time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22,0.,%d,%ld)",
1,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса|1\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса|%%'");
sql_zapis(strsql,1,1);




sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
2,
gettext("Единый социальный взнос для инвалидов"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',8.41,0.,%d,%ld)",
2,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для инвалидов|2\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для инвалидов|%%'");
sql_zapis(strsql,1,1);



sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
3,
gettext("Единый социальный взнос с больничных"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
3,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для больничных|3\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для больничных|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
4,
gettext("Единый социальный взнос для договоров подряда"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
4,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для договоров|4\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для договоров|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
5,
gettext("Единый социальный взнос для военных"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
5,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для военных|5\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для военных|%%'");
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
6,
gettext("Единый социальный взнос с больничных для инвалидов"),
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);


sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',8.41,0.,%d,%ld)",
6,
iceb_t_getuid(),
time(NULL));

sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса с больничных для инвалидов|6\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса с больничных для инвалидов|%%'");
sql_zapis(strsql,1,1);

/*Подсистема Учёт путевых листов*/

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лпг"),gettext("Летняя норма по городу"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лзг"),gettext("Летняя норма за городом"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зпг"),gettext("Зимняя норма по городу"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("ззг"),gettext("Зимняя норма за городом"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пегр"),gettext("Перевозка груза"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("рд"),gettext("Работа двигателя"),iceb_t_getuid(),time(NULL));
sql_zapis(strsql,1,1);


/*Установка стартового года*/
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastrb.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='zarnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='matnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uslnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uosnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastdok.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastud.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='ukrnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uplnast.alx' and str like 'Стартовый год|%%'",gt);
sql_zapis(strsql,1,1);

sprintf(strsql,"update Alx set str='Дата начала просмотра реестра|01.%d.%d\n' where fil='rnn_nast.alx' and str like 'Дата начала просмотра реестра|%%'",mt,gt);
sql_zapis(strsql,1,1);


/*Записываем код оператора для действий выполненных программой австоматически без участия человека*/
sprintf(strsql,"insert into icebuser (login,fio,un,kom,logz,vrem) values('%s','%s',%d,'%s','%s',%ld)",
ICEB_LOGIN_OP_PROG,
gettext("Программа"),
ICEB_KOD_OP_PROG,
gettext("Для действий выполненных без участия оператора"),
iceb_tu_getlogin(),time(NULL));

sql_zapis(strsql,1,1);


}
