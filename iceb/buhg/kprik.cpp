/*$Id: kprik.c,v 5.16 2013/05/17 14:56:04 sasa Exp $*/
/*15.07.2015	13.09.2003	Белых А.И. 	kprik.c
Распечатка командировочных приказов
*/
#include        <errno.h>
#include	"buhg.h"


int kprik(class spis_oth *oth)
{
char   strsql[1024];
static  class iceb_tu_str dat1(""),dat2("");
short		dn,mn,gn,dk,mk,gk;
int		kom=0;
SQLCURSOR curr;
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка приказов на командировки"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("ukr4_3.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

GDITE();

int	kolstr=0;
SQL_str row,row1;
SQLCURSOR cur;
char    imaf[64];
FILE	*ff;
class iceb_tu_str nompr("");
short   d,m,g;
class iceb_tu_str fio("");
int     kolprik=0;
short   dnp,mnp,gnp;
short   dkp,mkp,gkp;
class iceb_tu_str kodruk("");
short   metkaruk=0;
class iceb_tu_str tabnom("");
class iceb_tu_str dolgn("");
class iceb_tu_str tabnomruk("");
class iceb_t_fioruk_rk rekruk;

iceb_t_fioruk(1,&rekruk);



sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' order by nompr asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

iceb_t_poldan("Табельный номер руководителя",&tabnomruk,"zarnast.alx");

kodruk.new_plus(ICEB_ZAR_PKTN);
kodruk.plus(tabnomruk.ravno());

sprintf(imaf,"pkr%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

int nomer=0;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(SRAV(kodruk.ravno(),row[0],0) == 0)
   {
    metkaruk++;
    continue;
   }

  if(SRAV(nompr.ravno(),row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
   {
    if(kolprik != 0)
     {
      fprintf(ff,"\n%10s %s___________________%s\n"," ",rekruk.dolg.ravno(),rekruk.fio.ravno());
      fprintf(ff,"\f");
     }

    fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));
    fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

    rsdat(&d,&m,&g,row[7],2);

    fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
    gettext("г."));

    fprintf(ff,"%s\n",gettext("Про командировку"));
    fprintf(ff,"%s:\n",gettext("Откомандировать"));

    nompr.new_plus(row[6]);
    nomer=0;    
    kolprik++;    
   }  

  rsdat(&dnp,&mnp,&gnp,row[4],2);
  rsdat(&dkp,&mkp,&gkp,row[5],2);

  dolgn.new_plus("");
    
  if(SRAV(ICEB_ZAR_PKTN,row[0],1) == 0)
   {
   
    tabnom.new_plus(iceb_tu_adrsimv(iceb_tu_strlen(ICEB_ZAR_PKTN),row[0]));
/***************
    dlinnakont=strlen(row[0]);
    i2=0;
    for(i=dlinnapr; i < dlinnakont; i++)
      tabnom[i2++]=row[0][i];     
******************/
    
    //Узнаем должность
    sprintf(strsql,"select dolg from Kartb where tabn=%s",tabnom.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      dolgn.new_plus(row1[0]);

   }
  
  fio.new_plus("");
  //Читаем наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    fio.new_plus(row1[0]);
    
  fprintf(ff,"\n%d. %s %s\n",++nomer,fio.ravno(),dolgn.ravno());
  fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
  fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  " ",row[3],
  gettext("с"),dnp,mnp,gnp,gettext("г."),
  gettext("по"),dkp,mkp,gkp,gettext("г."));
  fprintf(ff,"\n%10s%s________________________\n"," ",gettext("с приказом ознакомлен"));
 }

fprintf(ff,"\n%10s %s___________________%s\n"," ",rekruk.dolg.ravno(),rekruk.fio.ravno());

if(metkaruk != 0)
 {
  short kolprik1=0;
  
  sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' and kont='%s' order by nompr asc",
  gn,mn,dn,gk,mk,dk,kodruk.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    if(SRAV(nompr.ravno(),row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
     {
      if(kolprik != 0 || kolprik1 != 0)
        fprintf(ff,"\f");

      if(kolprik1 != 0)
          fprintf(ff,"\n%10s %s___________________%s\n"," ",rekruk.dolg.ravno(),rekruk.fio.ravno());

      fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));
      fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

      rsdat(&d,&m,&g,row[7],2);

      fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
      gettext("г."));

      fprintf(ff,"%s\n",gettext("Про командировку"));
      fprintf(ff,"%s:\n",gettext("Отбываю в командировку"));

      nompr.new_plus(row[6]);
      nomer=0;    
      kolprik1++;    
     }  
    rsdat(&dnp,&mnp,&gnp,row[4],2);
    rsdat(&dkp,&mkp,&gkp,row[5],2);
    
    fio.new_plus("");
    //Читаем наименование контрагента
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      fio.new_plus(row1[0]);
      
    fprintf(ff,"\n%s %s\n",rekruk.fio.ravno(),rekruk.dolg.ravno());
    fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
    fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
    " ",row[3],
    gettext("с"),dnp,mnp,gnp,gettext("г."),
    gettext("по"),dkp,mkp,gkp,gettext("г."));
   }
  fprintf(ff,"\n%10s %s___________________%s\n"," ",rekruk.dolg.ravno(),rekruk.fio.ravno());
   
 }



fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка приказов на командировки"));
iceb_t_ustpeh(oth->spis_imaf.ravno(0),1);
return(0);
}
