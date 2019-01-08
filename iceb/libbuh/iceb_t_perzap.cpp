/* $Id: iceb_t_perzap.c,v 5.13 2014/07/31 07:09:28 sasa Exp $ */
/*20.05.2016	05.01.2000	Белых А.И.	perzap.c
Определение первого запуска программы в текущем дне
Если файла нет или дата в нем не равна текущей дате - значит
первый запуск,удаляем все файлы *.lst, и в файл записываем текущюю дату
Если вернули 0- всё впорядке
             1- нет
*/
#include        <errno.h>
#include        "buhl.h"

int iceb_t_perzap1(short d,short m,short g,const char *imaf,int metka_udf);

extern char *imabaz;

int iceb_t_perzap(int metka) //0-удалять всегда 1-проверять включено удаление или нет
{
FILE		*ff;
char		imaf[56];
char		str[1024];
char strsql[1024];
short		d,m,g;
class iceb_tu_str bros("");
int metka_udf=0; /*0-удалять файлы 1-нет*/
int kod_voz=0;
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select str from Alx where fil='iceb_data_baz'");

if((kod_voz=readkey(strsql,&row,&cur)) > 0)
 {
  short db,mb,gb;
  short d,m,g;
  rsdat(&db,&mb,&gb,ICEB_DATA_BAZ,1);
  rsdat(&d,&m,&g,row[0],1);
  int voz=0;
  if((voz=sravmydat(d,m,g,db,mb,gb)) < 0)
   {
    beep();
    iceb_t_soob(gettext("Внимание !!! Базу данных нужно преобразовать для текущей версии системы!"));
   }

  if(voz > 0)
   {
    beep();
    sprintf(strsql,"%s\n%d.%d.%d != %d.%d.%d",gettext("Внимание !!!\nДля нормальной работы с базой нужно обновить программы системы iceB!"),d,m,g,db,mb,gb);
    iceb_t_soob(strsql);
   }
 }

if(kod_voz == 0)
 {
  beep();
  iceb_t_soob(gettext("Внимание !!! Базу данных нужно преобразовать для текущей версии системы!"));
 }


if(metka == 1)
 {
  iceb_t_poldan("Удаление распечаток",&bros,"nastsys.alx");
  if(SRAV(bros.ravno(),"Вкл",1) != 0)
   {
    poldan("Удаление распечаток",&bros,".iceb_nastr.alx");
    if(SRAV(bros.ravno(),"Вкл",1) != 0)
      metka_udf=1;
   }
 }

strcpy(imaf,".iceb.dat");
if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(imaf,errno,gettext("Аварийное завершение работы программы !"));
    return(1);
   }
  else
   return(iceb_t_perzap1(0,0,0,imaf,metka_udf));
 }

if(fgets(str,sizeof(str),ff) == NULL)
 {
  printw("\n%s-%s\n",__FUNCTION__,gettext("Ошибка чтения"));
  OSTANOV();
 } 
rsdat(&d,&m,&g,str,1);
fclose(ff);

return(iceb_t_perzap1(d,m,g,imaf,metka_udf));
}
/************************/
/*************************/
int iceb_t_perzap1(short d,short m,short g,const char *imaf,int metka_udf)
{
struct  tm      *bf;
FILE *ff;

time_t tmm=time(&tmm);
bf=localtime(&tmm);

if((ff = fopen(imaf,"w")) == NULL)
 {
  if(errno == EACCES) /*Permission denied*/
   {
    error_op_nfil(imaf,errno,"Программа должна запускаться из домашнего каталога оператора!");
   }
  else
    error_op_nfil(imaf,errno,"");
  return(1);
 }
fprintf(ff,"%d.%d.%d\n%d:%d:%d\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
bf->tm_hour,bf->tm_min,bf->tm_sec);

fprintf(ff,"NAMEBAZ|%s\n",imabaz);

fclose(ff);

if(d != bf->tm_mday || m != bf->tm_mon+1 || g != bf->tm_year+1900)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Здравcтвуйте!"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Удачной вам работы и хорошего настроения!"));

  if(bf->tm_mday == 16 && bf->tm_mon+1 == 6)
   {
    SOOB.VVOD_SPISOK_add_MD(gettext("Можете поздравить с Днём рождения моего разработчика: Белых Александрa Ивановича"));
   }

  if(bf->tm_mday == 31 && bf->tm_mon+1 == 12)
   {
    SOOB.VVOD_SPISOK_add_MD(gettext("С наступающим новым годом!"));
   }

  if(bf->tm_mday == 16 && bf->tm_mon+1 == 7)
   {
    SOOB.VVOD_SPISOK_add_MD(gettext("С днём бухгалтера Украины!"));
   }

  soobshw(&SOOB,stdscr,-1,-1,0,1);

  if(metka_udf == 0)
    iceb_t_delfil(".","lst,lst~,tmp,tmp~");
 }
return(0);
}

