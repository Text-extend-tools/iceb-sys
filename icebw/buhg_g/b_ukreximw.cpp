/*$Id: b_ukreximw.c,v 1.10 2013/08/13 06:09:31 sasa Exp $*/
/*18.03.2012	20.09.2006	Белых А.И.	b_ukreximw.c
Создание файла с платёжками для експорта в подсистему Клиент-банк 
для УкрЕксим банка

"1" "05.01.2000" "ЖФ ЗАТ\"Видавництво \"Блiц-Iнфоpм\"" "21683859" "ВбДДбЛЕННЯ \"УКРЕКСбМБАНК\",М.ЖИТОМИР" "000311324" "2600801524537" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "13.03" "0" "ПЕРЕРАХ 10% ЗА РЕКЛАМ В ПРАЙС-ЛИСТИ РОСП 191 ПДВ HЕТ" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"1" "05.01.2000" "ДФ\"ВИДАВHИЦТВО\"БЛбЦ-бHФОРМ\"ЗАТ" "25001093" "ДНIПРОПЕТРОВСЬКА Ф-Я \"УКРЕКСIМБАНК\"" "000305675" "2600001501232" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "26.09" "0" "ЗА РЕКЛАМHб ПОСЛУГИ В ПРАЙС ЛИСТАХ ПДВ 4.33 ПР-0.11 РОЗПОР 147 4.10.99" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"1" "05.01.2000" "ЧФ ЗАТ \"ВИДАВHИЦТВО \"БЛбЦ-бHФОРМ\"\"" "22848392" "ВбД. ДЕРЖ. ЕКСП.-бМП. БАНКУ ЧЕРНбВЦб" "000356271" "260030171333" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "195" "0" "ЗА РЕАЛбЗ.ЖУРH.HАТАЛб БЕЗ ПДВ" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"3" "05.01.2000" "ЖФ ЗАТ\"Видавництво \"Блiц-Iнфоpм\"" "21683859" "ВбДДбЛЕННЯ \"УКРЕКСбМБАНК\",М.ЖИТОМИР" "000311324" "2600801524537" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "213" "0" "ПЕРЕРАХ КОШТ ЗА Г-ТУ БIЗHЕС РОСП 2 ПДВ HЕТ" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"1" "05.01.2000" "Блиц-Информ Издательство ЗАО ХФ" "25649760" "ХЕРСОНСЬКЕ ВIДДIЛЕННЯ \"УКРЕКСIМБАНКУ\"" "000352639" "2600201691573" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "220" "0" "Оплата за г-ту Бизнес согласно распоряжения 72 от 22. 07.99г. Без НДС." "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"2" "05.01.2000" "ФбЛбАЛ ЗАТ \"ВИДАВНИЦТВО \"БЛбЦ-бНФОРМ\"" "25669036" "ВIД. УКРЕКСIМБАНКУ, М.IВ.-ФРАНКIВСЬК" "000336688" "2600801542456" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "583.62" "0" "перер гол п-ву за газ бвзнес 162.80 журнал наталв 420.82 розпор 5.01.2000 без пдв" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"
"3" "05.01.2000" "ДФ\"ВИДАВHИЦТВО\"БЛбЦ-бHФОРМ\"ЗАТ" "25001093" "ДНIПРОПЕТРОВСЬКА Ф-Я \"УКРЕКСIМБАНК\"" "000305675" "2600001501232" "ЗАО ИЗДАТЕЛЬСТВО БЛИЦ-ИHФОРМ" "20050164" "ДЕРЖ.ЕКСП.-IМПОРТ.БАНК УКРА░НИ, М.КИ░В" "000322313" "2600603276042" "980" "695.91" "0" "ЗА РЕАЛбЗАЦбЮ БбЗHЕСА БЕЗ ПДВ" "05.01.2000" "1" "05.01.2000" "9469.35" "0" "05.01.2000" "11416.00" "0" "0.00" "0" "1946.65" "0"

 -----------------------------------------------------------------------
     File type         dBASE III
     Last Updated      01.03.01
     Header size            577 bytes
     Record size            452 bytes
     Number of fields        17
 ------------- Field information ---------------------------------------
  Name        Type        Length Decimals
 -----------------------------------------------------------------------
  D_NUMBER    Character     10             Номер док-та
  D_DATE      Date           8             Дата док-та                   
  S_NAME      Character     38             Наименование отправителя      
  S_OKPO      Character     12             ОКПО отправителя              
  S_BANK      Character     38             Наименование бана отправителя 
  S_MFO       Character      9             Код МФО банка отправителя     
  S_ACOUNT    Character     14             Счет отправителя              
  R_NAME      Character     38             Наименование получателя       
  R_OKPO      Character     12             ОКПО получателя               
  R_BANK      Character     38             Наименование банка получателя 
  R_MFO       Character      9             Код МФО банка получателя      
  R_ACOUNT    Character     14             Счет получателя               
  CURRENCY    Character      3             Код валюты                    
  SUMMA       Numeric       20      4      Сумма в гривне
  C_SUMMA     Numeric       20      4      Сумма в валюте, если валютный док.
  DIRECT      Character    160             Назначение платежа          
  PAY_DATE    Date           8             Дата оплаты                   

  FLAG_DK    CHARACTER,1                 - флаг дебета или кредита (0=дебет, 1=кт)
  DATEIN     DATE                        - дата входящего остатка
  INREST     Numeric,20,4                - сумма входящего остатка в гривне
  CINREST    Numeric,20,4                - сумма входящего остатка в валюте
  DATEOUT    DATE                        - дата исходящего остатка
  OUTREST    Numeric,20,4                - сумма исходящего остатка в гривне
  COUTREST   Numeric,20,4                - сумма исходящего остатка в валюте
  SUMDEBET   Numeric,20,4                - обороты по дебету в гривне
  CSUMDEBET  Numeric,20,4                - обороты по дебету в валюте
  SUMCREDIT  Numeric,20,4                - обороты по кредиту в гривне
  CSUMCREDIT Numeric,20,4                - обороты по кредиту в валюте

 ------------------------------------------------------------------------
 


*/

#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int b_ukreximw(const char *tabl,GtkWidget *wpredok)
{
char		imaf1[32];
FILE		*ff1;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }

sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }

char naior[512];
char naior1[512];
char naiban[512];
char naiban1[512];
char koment_f[1024];

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;
  
  memset(naior,'\0',sizeof(naior));
  strncpy(naior,rec.naior.ravno(),sizeof(naior)-1);
  iceb_u_ekrs(naior,sizeof(naior),'\\',"\"");

  memset(naior1,'\0',sizeof(naior1));
  strncpy(naior1,rec.naior1.ravno(),sizeof(naior1)-1);
  iceb_u_ekrs(naior1,sizeof(naior1),'\\',"\"");

  memset(naiban,'\0',sizeof(naiban));
  strncpy(naiban,rec.naiban.ravno(),sizeof(naiban)-1);
  iceb_u_ekrs(naiban,sizeof(naiban),'\\',"\"");

  memset(naiban1,'\0',sizeof(naiban1));
  strncpy(naiban1,rec.naiban1.ravno(),sizeof(naiban1)-1);
  iceb_u_ekrs(naiban1,sizeof(naiban1),'\\',"\"");
  
  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  memset(koment_f,'\0',sizeof(koment_f));
  strncpy(koment_f,koment.ravno(),sizeof(koment_f)-1);
  
  iceb_u_ekrs(koment_f,sizeof(koment_f),'\\',"\"");

  fprintf(ff1,"\"%.10s\" \"%02d.%02d.%d\" \"%.*s\" \"%.12s\"\
 \"%.*s\" \"%09d\" \"%.14s\" \"%.*s\" \"%.12s\" \"%.*s\"\
 \"%09d\" \"%.14s\" \"%s\" \"%.2f\" \"%.2f\" \"%.*s\"\
 \"%02d.%02d.%d\"\n",
  row[1],
  rec.dd,rec.md,rec.gd,
  iceb_u_kolbait(38,naior),naior,
  rec.kod.ravno(),
  iceb_u_kolbait(38,naiban),naiban,
  atoi(rec.mfo.ravno()),
  rec.nsh.ravno(),
  iceb_u_kolbait(38,naior1),naior1,
  rec.kod1.ravno(),
  iceb_u_kolbait(38,naiban1),naiban1,
  atoi(rec.mfo1.ravno()),
  rec.nsh1.ravno(),
  "980",
  rec.sumd,
  0.,
  iceb_u_kolbait(160,koment_f),koment_f,
  rec.dd,rec.md,rec.gd);
  
 }

fclose(ff1);
iceb_perecod(2,imaf1,wpredok);
return(0);
}