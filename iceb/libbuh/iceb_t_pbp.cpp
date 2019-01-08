/*$Id: iceb_t_pbp.c,v 5.20 2013/05/17 14:56:29 sasa Exp $*/
/*13.02.2016	18.03.2008	Белых А.И.	iceb_t_pbp.c
Проверка заблокированы ли даты по которым сделаны проводки к документу
Применяется перед удалением документа
Если вернули 0-проводки не заблокированы
             1-хотябы одна проводка заблокирована
*/
#include        "buhl.h"


int iceb_t_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,
int pod,int tipz,const char *soob)
{
char kto[64];
memset(kto,'\0',sizeof(kto));

iceb_t_mpods(metka_ps,kto); /*получение символьной метки подсистемы*/

return(iceb_t_pbp(kto,dd,md,gd,nomdok,pod,tipz,soob));

}
/************************/
/*******************************/

int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,
int pod,int tipz,const char *soob)
{
return(iceb_t_pbp(kto,dd,md,gd,nomdok,pod,tipz,soob,0));

}

/****************************************/
int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,
int pod,int tipz,const char *soob,int metka_soob) /*0-выдать сообщение 1-не выдавать*/
{
char		strsql[1024];

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select distinct datp,sh,shk,ktoi from Prov where kto='%s' and pod=%d \
and nomd='%s' and datd = '%04d-%02d-%02d' and tz=%d and deb <> 0.",
kto,pod,nomdok,gd,md,dd,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  if(iceb_t_pblok(m,g,ICEB_PS_GK) != 0)
   {
    if(metka_soob != 0)
     {
      sprintf(strsql,gettext("Проводки на дату %d.%d г. заблокированы!"),m,g);
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      if(soob[0] != '\0')
        SOOB.VVOD_SPISOK_add_MD(soob);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
     }
    return(1);  
   }
  if(iceb_t_pbsh(m,g,row[1],row[2],"") != 0)
   return(1);

/***************  
  если проводки сделает тот кто может разблокировать то хозяин не сможет их переделать например в зарпате при 
  выходе из просмотра начислений или удержаний
  ***********/
//  if(SRAV(kto,ICEB_MP_ZARP,0) != 0)
  if(SRAV(kto,ICEB_MP_ZARP,0) != 0)
   if(iceb_t_pvkup(atoi(row[3])) != 0) //удалять может только хозяин или тот кому разрешена блокировк/разблокировка
    return(1);

 }

return(0);
}
