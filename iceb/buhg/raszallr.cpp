/* $Id: raszallr.c,v 5.38 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	15.09.2000	Белых А.И.	raszallr.c
Расчёт начислений и удержаний для всех карточек
*/
#include <math.h>
#include        "buhg.h"

extern float    dnei; /*Количество рабочих дней в текущем месяце*/
extern float    hasov; /*Количество рабочих часов в текущем месяце*/

void	raszallr(short mr,short gr,const char *podr,const char *tabnom,
const char *nah_only,
const char *uder_only,
int metka)  //1-начисления 2-удержания 3-начисления и удержания
            //4-соц-отчисления
            //5-проводки
{
SQL_str		row,row1;
int		kolstr,kolstr1;
char		strsql[512];
int tabn;
int		kpodr;
short		kateg,sovm,zvan;
class iceb_tu_str shet("");
class iceb_tu_str lgot("");
short		dn,mn,gn,dk,mk,gk;
SQLCURSOR curr;
clear();
dk=1; mk=mr; gk=gr;
dpm(&dk,&mk,&gk,4); //Уменьшаем на месяц тоесть все уволенные до этого месяца в расчёт не идут



zarreadn(1,mr,gr,metka);


sprintf(strsql,"select tabn,fio,podr,sovm,kodg,kateg,zvan,\
shet,lgoti,datn,datk,dolg from Kartb where datk='0000-00-00' \
or datk > '%04d-%d-%d'",gk,mk,dk);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr <= 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {

  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  if(proverka(podr,row[2],0,0) != 0)
    continue;
  if(proverka(tabnom,row[0],0,0) != 0)
    continue;

  printw("%5s %s\n",row[0],row[1]);

  //Проверяем если карточка заблокирована то ее пропускаем
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%s",gr,mr,row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   if(atoi(row1[0]) != 0)
    {
     printw("%s !\n",gettext("Карточка заблокирована"));
     continue;
    }

  tabn=atol(row[0]);
  kpodr=atoi(row[2]);
  sovm=atoi(row[3]);
  kateg=atoi(row[5]);
  zvan=atoi(row[6]);
  shet.new_plus(row[7]);
  lgot.new_plus(row[8]);
  
  /*Проверяем есть ли записи начислений, если их нет совсем
    проверяем есть ли набор начислений и удержаний на этот
    табельный номер
    Если набор есть загружаем его и считаем
    Если нет смотрим следующий табельный номер
   */
  if(metka < 4)
   {  
    //Проверяем записана ли стандартная настройка
    provzzag(mr,gr,tabn,3,kpodr);
  //  printw("%ld %d.%d %d %d\n",tabn,mr,gr,sovm,kpodr);
  //  refresh();

//    getyx(stdscr,Y,X);
//     printw("ff=%d\n",ff);
//  OSTANOV();
      
    rasshetz(tabn,mr,gr,kpodr,metka,nah_only,uder_only); /*Расчёт*/

    //Проверяем есть ли промежуточная запись
    sprintf(strsql,"select god from Zarn where tabn=%d and god=%d \
and mes=%d",tabn,gr,mr);
    if(sql_readkey(&bd,strsql,&row1,&curr) < 1)
     {
      /*Проверяем есть ли начисления или удержания*/
      sprintf(strsql,"select tabn from Zarp where tabn=%d \
and suma <> 0. limit 1",tabn);  

      if(readkey(strsql) > 0)
       {
        rsdat(&dn,&mn,&gn,row[9],2);
        rsdat(&dk,&mk,&gk,row[10],2);
        zapzarn(mr,gr,kpodr,tabn,kateg,sovm,zvan,shet.ravno(),lgot.ravno(),dn,mn,gn,dk,mk,gk,0,row[11]);
       }
     }
   }
  if(metka < 5)
    zarsoc(mr,gr,tabn);
  
  zaravpr(tabn,mr,gr,NULL);

  //Проверяем сделаны ли все проводки
  double sald=0.;
  double nah,nahprov,uder,uderprov,nahb,uderb,uderbprov;
  nahud(gr,mr,tabn,&nah,&nahprov,&uder,&uderprov,&nahb,&uderb,&uderbprov,0,sald,0,"");

  sprintf(strsql,"%d-%d",mr,tabn);
  
  double sumd,sumk;
  
  sumprz(mr,gr,strsql,0,&sumd,&sumk);
  int prov;

  if(fabs((sumd+sumk)-(uderprov*(-1)+nahprov)) > 0.009 )
    prov=0;
  else
    prov=1;

  sprintf(strsql,"update Zarn set prov=%d where tabn=%d and god=%d and mes=%d",prov,tabn,gr,mr);
  sql_zapis(strsql,1,0);
    
 }
}
