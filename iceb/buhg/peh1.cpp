/*$Id: peh1.c,v 5.22 2013/08/13 05:49:50 sasa Exp $*/
/*22.05.2013	06.09.2006	Белых А.И.	peh1.c
Распечатка корешка
*/
#include        "buhg.h"

extern short	pehfam; //0-фамилия в корешке мелко 1-крупно

void    peh1(const char *np,//Наименование подразделения
const char *zap,long tabb,const char *inn,short mr,short gr,
const char *dolg,
int kategor,
FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s,FILE *ff_nuv)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str nait("");
class iceb_tu_str str("");
class iceb_tu_str naim_kateg("");
SQLCURSOR curr;


sprintf(strsql,"select naik from Kateg where kod=%d",kategor);
if(readkey(strsql,&row,&curr) == 1)
  naim_kateg.new_plus(row[0]);


str.new_plus("------------------------------------------------------------");

strfil(str.ravno(),str.ravno(),1,kaw);
strfil(str.ravno(),str.ravno(),1,ff_nuv);
strfil(str.ravno(),str.ravno(),1,ffkor2s);

if(kawk1 != NULL)
 fprintf(kawk1,"\n%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"\n%s\n",str.ravno());

str.new_plus(iceb_t_get_pnk("00",0));

strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());


str.new_plus(np);
strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());



sprintf(strsql,"%s %d.%d%s %s:%s",
gettext("Дата"),mr,gr,
gettext("г."),gettext("Фамилия"),zap);

str.new_plus(strsql);

strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);

if(pehfam == 1)
 sprintf(strsql,"%s %d.%d%s %s:\x12%s\x0F",
 gettext("Дата"),mr,gr,
 gettext("г."),gettext("Фамилия"),zap);
else
 sprintf(strsql,"%s %d.%d%s %s:%s",
 gettext("Дата"),mr,gr,
 gettext("г."),gettext("Фамилия"),zap);

str.new_plus(strsql);


if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());

sprintf(strsql,"%s:%s",gettext("Должность"),dolg);
str.new_plus(strsql);

strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());

sprintf(strsql,"%s:%d %s",gettext("Категория"),kategor,naim_kateg.ravno());
str.new_plus(strsql);

strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());


sprintf(strsql,"%s %ld %s %s",
gettext("Табельный номер"),tabb,
gettext("Иден.ном."),inn);

str.new_plus(strsql);

strfil(str.ravno(),str.ravno(),0,kaw);
strfil(str.ravno(),str.ravno(),0,ff_nuv);
strfil(str.ravno(),str.ravno(),0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str.ravno());
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str.ravno());

int tarif=0; 
double okld=0.;
int kollgot=0;
class iceb_tu_str kod_lgot("");
float lgota=zarlgot(tabb,mr,gr,&kollgot,&kod_lgot);

if((okld=oklad(tabb,mr,gr,&tarif)) != 0.)
 {
  sprintf(strsql,"%s:%.2f %s:%.2f%%/%d",gettext("Оклад"),okld,gettext("Льгота"),lgota,kollgot);
  if(tarif == 1)
    sprintf(strsql,"%s:%.2f %s:%.2f%%/%d",gettext("Тариф"),okld,gettext("Льгота"),lgota,kollgot);
  str.new_plus(strsql);

  strfil(str.ravno(),str.ravno(),0,kaw);
  strfil(str.ravno(),str.ravno(),0,ff_nuv);
  strfil(str.ravno(),str.ravno(),0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str.ravno());
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str.ravno());
 }  
else
 {
  sprintf(strsql,"%s:%.2f%%/%d",gettext("Льгота"),lgota,kollgot);
  if(tarif == 1)
    sprintf(strsql,"%s:%.2f%%/%d",gettext("Льгота"),lgota,kollgot);
  str.new_plus(strsql);

  strfil(str.ravno(),str.ravno(),0,kaw);
  strfil(str.ravno(),str.ravno(),0,ff_nuv);
  strfil(str.ravno(),str.ravno(),0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str.ravno());
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str.ravno());
 }  


/*Проверяем есть ли в списке пенсионеров*/

if(zarprtn(mr,gr,tabb,1) == 1)
 {
  str.new_plus(gettext("Пенсионер"));
  strfil(str.ravno(),str.ravno(),0,kaw);
  strfil(str.ravno(),str.ravno(),0,ff_nuv);
  strfil(str.ravno(),str.ravno(),0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str.ravno());
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str.ravno());

 }

/*Проверяем есть ли в списке инвалидов*/

if(zarprtn(mr,gr,tabb,2) == 1)
 {
  str.new_plus(gettext("Инвалид"));
  strfil(str.ravno(),str.ravno(),0,kaw);
  strfil(str.ravno(),str.ravno(),0,ff_nuv);
  strfil(str.ravno(),str.ravno(),0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str.ravno());
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",str.ravno());

 }




/*Читаем табель*/
sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where \
god=%d and mes=%d and tabn=%ld",gr,mr,tabb);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код табеля"),row[0]);
    iceb_t_soob(strsql);
    nait.new_plus("");
    continue;
   }
  else
    nait.new_plus(row1[0]);

   sprintf(strsql,"%-*.*s: %2.fд. %3.fч. %2.fк.д.",
   iceb_tu_kolbait(40,nait.ravno()),iceb_tu_kolbait(40,nait.ravno()),nait.ravno(),
   atof(row[1]),atof(row[2]),atof(row[3]));
   
   str.new_plus(strsql);
    
   strfil(str.ravno(),str.ravno(),0,kaw);
   strfil(str.ravno(),str.ravno(),0,ff_nuv);
   strfil(str.ravno(),str.ravno(),0,ffkor2s);
 
   if(kawk1 != NULL)
    fprintf(kawk1,"%s\n",str.ravno());
   if(kawk2 != NULL)
    fprintf(kawk2,"%s\n",str.ravno());

 }
printw("%-4ld %-*s",tabb,iceb_tu_kolbait(40,zap),zap);
}
