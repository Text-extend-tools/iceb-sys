/* $Id: zarnast.c,v 5.84 2011-12-02 09:48:19 sasa Exp $ */
/*27.11.2011    10.12.1993      Белых А.И.      zarnast.c
Читаем настройку на зарплату
*/
#include        <errno.h>
#include        "../buhg/buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif


float  kof_prog_min=0.; //Коэффициент прожиточного минимума
float  ppn=0.; //Процент подоходного налога
float  pomzp=0.; //Процент от минимальной заработной платы

double          okrg; /*Округление*/

short		metkarnb=0; //0-расчет налогов для бюджета пропорциональный 1-последовательный
short		avkvs=0; //0-Не вставлять автоматом код контрагента 1-вставлять
short           *knvr=NULL;/*Коды начислений не входящие в расчет подоходного налога*/
short           *kn_bsl=NULL; //Коды начислений на которые не распространяется льгота по подоходному налогу
short           *kuvr=NULL;/*Коды удержаний входящие в расчет подоходного налога*/
short           *knnf=NULL; //Коды начислений недежными формами 
short           *knnf_nds=NULL; /*Коды начислений неденежными формами на которые начисляется НДС*/
short           kodpn;   /*Код подоходного налога*/
short kodpn_sbol=0;/*Код подоходного налга с больничного*/
class iceb_tu_str kodpn_all; /*Все коды подоходного налога*/
short kodvpn=0; /*Код возврата подоходного налога*/
short           kodperpn;   /*Код перерасчета подоходного налога*/
short		kodindex;  /*Код индексации*/
short		kodindexb;  /*Код индексации больничного*/

//short		kodpenf; /*Код пенсионного фонда*/
//short           kodpen;  /*Код пенсионных отчислений*/
//short		*kodnvpen=NULL; /*Коды не входящие в расчет пенсионного отчисления*/
short           *kuvvr_pens_sr=NULL; //Коды удержаний входящие в расчет при отчислении с работника
short           *kuvvr_pens_sfz=NULL; //Коды удержаний входящие в расчет при отчислении с фонда зарплаты
//float           procpen[2]; /*Процент отчисления в пенсионный фонд*/
char            *p_shet_inv=NULL; //счет для начисления пенсионных отчислений на фонд зарплаты для инвалидов (если для них нужен другой счет)

short		*kodnvrindex=NULL; //Коды не входящие в расчет индексации
//float           p_tarif_inv=0.; //тариф (процент) отчисления в пенсионный фонд инвалидами

//short		kodsoc;   /*Код фонда социального страхования*/
//short           kodsocstr;  /*Код отчисления на соц-страх*/
//float           procsoc=0.; /*Процент отчисления в соц-страх*/
//float           procsoc1=0.; /*Процент отчисления в соц-страх*/
//short		*kodsocstrnv=NULL; //Коды не входящие в расчет соцстраха
char		*shetsoc=NULL; //Счета фонда соц-страха не бюджет/бюджет

//short		kodbezf;  /*Код фонда занятости*/
//short		*kodbzrnv=NULL; //Коды не входящие в расчет отчислений на безработицу
//short           kodbzr;  /*Код отчисления на безработицу*/
//float           procbez; /*Процент отчисления на безработицу*/
//float procent_fb_dog=0; /*Прочент начисления на фонд зарплаты для фонда безработицы для работающий по договору*/


//short		kodsoci;   /*Код социального страхования инвалидов*/
//short		kodfsons;   /*Код фонда страхования от несчасного случая*/

short           *kodmp=NULL;  //Коды благотворительной помощи
short           *kodbl=NULL;  /*Коды начисления больничного*/
short		koddopmin; /*Код доплаты до минимальной зарплаты*/
short           *kodnvmin=NULL; /*Коды не входящие в расчет доплаты до минимальной зарплаты*/
short           *kodotrs=NULL;  /*Коды отдельного рачета доплаты до минимпльной зарплаты*/

short		*obud=NULL; /*Обязательные удержания*/
short		*obnah=NULL; /*Обязательные начисления*/
short		startgodz; /*Стаpтовый год для расчета сальдо*/
short		startgodb; /*Стартовый год просмотров главной книги*/

//char		shrpz[16]; /*Счет расчетов по зарплате*/
class iceb_tu_str shrpz; /*Счет расчетов по зарплате*/
//char		shrpzbt[16]; /*Счет расчетов по зарплате бюджет*/
class iceb_tu_str shrpzbt; /*Счет расчетов по зарплате бюджет*/
//char		pktn[16]; /*Приставка к табельному номеру*/
class iceb_tu_str pktn; /*Приставка к табельному номеру*/
class SPISOK dop_zar_sheta; //Дополнительные зарплатные счёта
class SPISOK dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/
char		*shpn=NULL; /*Счет подоходного налога*/
char		*shetb=NULL; /*Бюджетные счета начислений*/
char		*shetbu=NULL; /*Бюджетные счета удержаний*/
char		*shpnb=NULL;   /*Счета подоходного налога*/
char		*shetps=NULL; /*Счета профсоюзного сбора*/
char		*shetfb=NULL; /*Счета фонда безработицы*/
char		*shetpf=NULL; /*Счета пенсионного фонда*/

short		kuprof=0; //Код удержания в профсоюзный фонд
float		procprof=0.; //Процент отчисления в профсоюзный фонд
short		*knvrprof=NULL; //Коды не входящие в расчет удержаний в профсоюзный фонд

double          snmpz=0.; /*Сумма не облагаемой материальной помощи заданная*/

short		pehfam=0; //0-фамилия в корешке мелко 1-крупно

short *kodkomand=NULL; //Коды командировочных расходов
short *knvr_komand=NULL; //Коды начислений не входящие в расчёт командировочных
short kodt_b_komand=0; //Код бюджетного табеля командировочных
short *ktnvr_komand=NULL; //Коды табеля не входящие в расчёт командировочных
short koddzn=0; /*Код доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short *kvvrdzn=NULL; /*Коды входящие в расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short kod_komp_z_kom=0; /*Код компенсации за командировку*/
 
short kod_ud_dnev_zar=0; //Код удержания дневного заработка
short *kodnah_nvr_dnev_zar=NULL;  //Коды начислений не входящие в расчёт удержания дневного заработка
short *kodtab_nvr_dnev_zar=NULL;  //Коды табеля не входящие в расчёт удержания однодневного заработка

int kekv_at=0;  //Код экономической классификации для атестованых
int kekv_nat=0; //Код экономической классификации для не атестованых
int kodzv_nat=0;//Код звания неатестованный
int kekv_nfo=0; //Код экономической классификации для начислений на фонд оплаты

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

int             zarnast(void)
{
int		i;
char            str[1024];
char            bros[1024];


dop_zar_sheta.free_class(); /*Обнуляем список*/
dop_zar_sheta_spn.free_class(); /*Обнуляем список*/
kod_komp_z_kom=0;
koddzn=0;
kodt_b_komand=0;
kuprof=metkarnb=avkvs=0;
//p_tarif_inv=0.;
snmpz=0.;
shrpz.new_plus("");
shrpzbt.new_plus("");
//memset(shrpz,'\0',sizeof(shrpz));

//memset(shrpzbt,'\0',sizeof(shrpzbt));
pktn.new_plus("");
//memset(pktn,'\0',sizeof(pktn));
kof_prog_min=ppn=pomzp=0.;
kodpn_all.new_plus("");
if(ktnvr_komand != NULL)
 {
  delete [] ktnvr_komand;
  ktnvr_komand=NULL;
 }

if(knvr_komand != NULL)
 {
  delete [] knvr_komand;
  knvr_komand=NULL;
 }

if(knvrprof != NULL)
 {
  delete [] knvrprof;
  knvrprof=NULL;
 }

if(kuvvr_pens_sfz != NULL)
 {
  delete [] kuvvr_pens_sfz;
  kuvvr_pens_sfz=NULL;
 }

if(kuvvr_pens_sr != NULL)
 {
  delete [] kuvvr_pens_sr;
  kuvvr_pens_sr=NULL;
 }
/**********
if(kodsocstrnv != NULL)
 {
  delete [] kodsocstrnv;
  kodsocstrnv=NULL;
 }
if(kodbzrnv != NULL)
 {
  delete [] kodbzrnv;
  kodbzrnv=NULL;
 }
*************/
if(kodnvrindex != NULL)
 {
  delete [] kodnvrindex;
  kodnvrindex=NULL;
 }

if(obud != NULL)
 {
  delete [] obud;
  obud=NULL;
 }
if(obnah != NULL)
 {
  delete [] obnah;
  obnah=NULL;
 }



if(knvr != NULL)
 {
  delete [] knvr;
  knvr=NULL;
 }
if(knnf != NULL)
 {
  delete [] knnf;
  knnf=NULL;
 }
if(knnf_nds != NULL)
 {
  delete [] knnf_nds;
  knnf_nds=NULL;
 }
if(kn_bsl != NULL)
 {
  delete [] kn_bsl;
  kn_bsl=NULL;
 }
/**************
if(kodnvpen != NULL)
 {
  delete [] kodnvpen;
  kodnvpen=NULL;
 }
***************/
if(kodnvmin != NULL)
 {
  delete [] kodnvmin;
  kodnvmin=NULL;
 }
if(kodotrs != NULL)
 {
  delete [] kodotrs;
  kodotrs=NULL;
 }

if(kuvr != NULL)
 {
  delete [] kuvr;
  kuvr=NULL;
 }

if(kodmp != NULL)
 {
  delete [] kodmp;
  kodmp=NULL;
 }


if(shetb != NULL)
 {
  delete [] shetb;
  shetb=NULL;
 }
if(shetbu != NULL)
 {
  delete [] shetbu;
  shetbu=NULL;
 }

if(shpnb != NULL)
 {
  delete [] shpnb;
  shpnb=NULL;
 }
if(shpn != NULL)
 {
  delete [] shpn;
  shpn=NULL;
 }

if(shetfb != NULL)
 {
  delete [] shetfb;
  shetfb=NULL;
 }
if(kodkomand != NULL)
 {
  delete [] kodkomand;
  kodkomand=NULL;
 }

if(kvvrdzn != NULL)
 {
  delete [] kvvrdzn;
  kvvrdzn=NULL;
 }

if(shetps != NULL)
 {
  delete [] shetps;
  shetps=NULL;
 }
if(shetpf != NULL)
 {
  delete [] shetpf;
  shetpf=NULL;
 }
if(shetsoc != NULL)
 {
  delete [] shetsoc;
  shetsoc=NULL;
 }
if(p_shet_inv != NULL)
 {
  delete [] p_shet_inv;
  p_shet_inv=NULL;
 }

if(kodbl != NULL)
 {
  delete [] kodbl;
  kodbl=NULL;
 }

if(kodnah_nvr_dnev_zar != NULL)
 {
  delete [] kodnah_nvr_dnev_zar;
  kodnah_nvr_dnev_zar=NULL;
 }
if(kodtab_nvr_dnev_zar != NULL)
 {
  delete [] kodtab_nvr_dnev_zar;
  kodtab_nvr_dnev_zar=NULL;
 }

//nomin=0.;
procprof=0.;
pehfam=koddopmin=startgodz=0;
kod_ud_dnev_zar=0;
okrg=0.;

kodindex=kodindexb=0;
kodperpn=vplsh=kodpn=kodpn_sbol=kodvpn=0;
kekv_at=kekv_nat=kodzv_nat=kekv_nfo=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],str,sizeof(str),1,'|');

  class iceb_tu_str spisok_kodov("");


  if(SRAV(str,"Бюджетные счета",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetb=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetb !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetb,bros);
      }
           
     continue;

   }
  if(SRAV(str,"Бюджетные счета удержаний",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetbu=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetbu !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetbu,bros);
      }
     continue;

   }

  if(SRAV(str,"Счета подоходного налога",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shpnb=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shpnb !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shpnb,bros);
      }
     continue;

   }
  if(SRAV(str,"Счета пенсионного отчисления",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetpf=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetpf !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
         strcpy(shetpf,bros);
      }
     continue;

   }
  if(SRAV(str,"Счета фонда социального страхования",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetsoc=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetsoc !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetsoc,bros);
      }
     continue;

   }

  if(SRAV(str,"Счет отчисления в пенсионный фонд инвалидами (фонд зарплаты)",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((p_shet_inv=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s p_shet_inv !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(p_shet_inv,bros);
      }
     continue;

   }

  if(SRAV(str,"Дополнительные зарплатные счёта",0) == 0)
   {
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     iceb_t_fplus(0,bros,&dop_zar_sheta,&cur_alx);
     continue;
   }

  if(SRAV(str,"Счета отчисления в фонд безработицы",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetfb=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetfb !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetfb,bros);
      }
     continue;

   }

  if(SRAV(str,"Счета профсоюзного отчисления",0) == 0)
   {
     memset(bros,'\0',sizeof(bros));
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if(i != 1)
      {
       if((shetps=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetps !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetps,bros);
      }
     continue;

   }

  if(SRAV(str,"Код командировочного начисления",0) == 0)
   {
    iceb_t_sozmas(&kodkomand,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Код компенсации за командировку",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kod_komp_z_kom=atoi(str); 
    continue;
   }

  if(SRAV(str,"Коды, которые нужно взять в расчёт для расчёта доплаты до средней зарплаты в командировке",0) == 0)
   {
    iceb_t_sozmas(&kvvrdzn,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Код доплаты до средней зарплаты в командировке c учётом надбавки за награду",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    koddzn=atoi(str); 
    continue;
   }


  if(SRAV(str,"Код бюджетного табеля командировочных",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodt_b_komand=atoi(str); 
    continue;
   }
/************
  if(SRAV(str,"Код пенсионного фонда",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodpenf=(short)ATOFM(str); 
    continue;
   }
*************/
  if(SRAV(str,"Профсоюзный фонд-код отчисления",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kuprof=(short)ATOFM(str); 
    continue;
   }
  if(SRAV(str,"Профсоюзный фонд-процент отчисления",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    procprof=(float)ATOFM(str); 
    continue;
   }
/*****************
  if(SRAV(str,"Код отчисления в соц-страх",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodsocstr=(short)ATOFM(str); 
    continue;
   }
  if(SRAV(str,"Код фонда занятости",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodbezf=(short)ATOFM(str); 
    continue;
   }
  if(SRAV(str,"Код фонда социального страхования",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodsoc=(short)ATOFM(str); 
    continue;
   }
  if(SRAV(str,"Код фонда социальной защиты инвалидов",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodsoci=(short)ATOFM(str); 
    continue;
   }
*******************/
  if(SRAV(str,"Код удержания дневного заработка",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kod_ud_dnev_zar=(short)ATOFM(str); 
    continue;
   }
/*******************
  if(SRAV(str,"Код фонда страхования от несчастного случая",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodfsons=(short)ATOFM(str); 
    continue;
   }
*******************/

  if(SRAV(str,"Стартовый год",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     startgodz=(short)ATOFM(str); 
     continue;
    }

  if(SRAV(str,"Автоматическая вставка кода контрагента в список счета",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     avkvs=0;
     if(SRAV(str,"Вкл",1) == 0)
       avkvs=1;
     continue;
    }

  if(SRAV(str,"Печатать фамилию в корешке крупно",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     pehfam=0;
     if(SRAV(str,"Вкл",1) == 0)
       pehfam=1;
     continue;
    }

  if(SRAV(str,"Расчет налогов последовательный",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     metkarnb=0;
     if(SRAV(str,"Вкл",1) == 0)
       metkarnb=1;
     continue;
    }


  if(SRAV(str,"Счет расчетов по зарплате",0) == 0 )
    {
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     if(polen(bros,&shrpzbt,2,',') == 0)
      {
       polen(bros,&shrpz,1,',');
      }            
     else
       shrpz.new_plus(bros);

     continue;
    }

/*************
  if(SRAV(str,"Коды не входящие в расчет отчислений на безработицу",0) == 0)
   {
    iceb_t_sozmas(&kodbzrnv,row_alx[0],&cur_alx);
    continue;
   }
****************/
  if(SRAV(str,"Коды начислений не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_t_sozmas(&kodnah_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str,"Коды табеля не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_t_sozmas(&kodtab_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды не входящие в расчет индексации",0) == 0)
   {
    iceb_t_sozmas(&kodnvrindex,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды начислений не входящие в расчет командировочных",0) == 0)
   {
    iceb_t_sozmas(&knvr_komand,row_alx[0],&cur_alx);
    continue;
   }
/***************
  if(SRAV(str,"Коды не входящие в расчет отчислений на соцстрах",0) == 0)
   {
    iceb_t_sozmas(&kodsocstrnv,row_alx[0],&cur_alx);
    continue;
   }
******************/
  if(SRAV(str,"Коды табеля не входящие в расчет командировочных",0) == 0)
   {
    iceb_t_sozmas(&ktnvr_komand,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Профсоюзный фонд-коды начислений не входящие в расчет",0) == 0)
    {
     iceb_t_sozmas(&knvrprof,row_alx[0],&cur_alx);
     continue;
    }

  if(SRAV(str,"Профсоюзный фонд-коды начислений не входящие в расчет",0) == 0)
    {
     iceb_t_sozmas(&knvrprof,row_alx[0],&cur_alx);
     continue;
    }

  if(SRAV(str,"Приставка к табельному номеру",0) == 0 )
   {
    polen(row_alx[0],&pktn,2,'|');
    continue;
   }

  if(SRAV(str,"Счет подоходного налога",0) == 0)
    {
     polen(row_alx[0],bros,sizeof(bros),2,'|');
     shpn=new char[strlen(bros)+1];
     strcpy(shpn,bros);     
     continue;
    }

  if(SRAV(str,"Максимальная сумма необлагаемой благотворительной помощи",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     snmpz = ATOFM(str);
     continue;
    }
/**************
  if(SRAV(str,"Тариф отчисления в пенсионный фонд инвалидами",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    p_tarif_inv = ATOFM(str);
    continue;
   }
  *****************/   
  if(SRAV(str,"Коэффициент прожиточного минимума",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kof_prog_min = ATOFM(str);
    continue;
   }

  if(SRAV(str,"Процент подоходного налога",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    ppn = ATOFM(str);
    continue;
   }

  if(SRAV(str,"Процент от минимальной заработной платы",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    pomzp = ATOFM(str);
    continue;
   }

  if(SRAV(str,"Код больничного",0) == 0)
   {
    iceb_t_sozmas(&kodbl,row_alx[0],&cur_alx);
    continue;
   }


  if(SRAV(str,"Обязательные удержания",0) == 0)
   {
    iceb_t_sozmas(&obud,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды удержаний входящие в расчет пенсионного отчисления с работника",0) == 0)
   {
    iceb_t_sozmas(&kuvvr_pens_sr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды удержаний входящие в расчет пенсионного отчисления с фонда зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kuvvr_pens_sfz,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Обязательные начисления",0) == 0)
    {
     iceb_t_sozmas(&obnah,row_alx[0],&cur_alx);
     continue;
    }


  if(SRAV(str,"Код подоходного налога",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodpn=(short)ATOFM(str);
     if(kodpn != 0)
       kodpn_all.z_plus(kodpn);
     continue;
    }

  if(SRAV(str,"Код подоходного налога с больничного",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodpn_sbol=(short)ATOFM(str);
     if(kodpn_sbol != 0)
       kodpn_all.z_plus(kodpn_sbol);
     continue;
    }


  if(SRAV(str,"Код начисления для возврата суммы подоходного налога",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodvpn=(short)ATOFM(str);
     continue;
    }

  if(SRAV(str,"Код перерасчета подоходного налога",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodperpn=(short)ATOFM(str);
     if(kodperpn != 0)
      kodpn_all.z_plus(kodperpn);
    }

  if(SRAV(str,"Код индексации",0) == 0)
   {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodindex=(short)ATOFM(str);
     continue;
   }
  if(SRAV(str,"Код индексации больничного",0) == 0)
   {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodindexb=(short)ATOFM(str);
     continue;
   }
/**************
  if(SRAV(str,"Код пенсионного отчисления",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodpen=(short)ATOFM(str);
     continue;
    }
  if(SRAV(str,"Код отчисления на безработицу",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     kodbzr=(short)ATOFM(str);
     continue;
    }
  if(SRAV(str,"Процент отчисления в пенсионный фонд",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     procpen[0]=(float)ATOFM(str);
     if(polen(row_alx[0],str,sizeof(str),3,'|') == 0)
       procpen[1]=(float)ATOFM(str);
     else 
       procpen[1]=procpen[0];
     
     continue;
    }
  if(SRAV(str,"Процент отчисления в соц-страх",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     procsoc=(float)ATOFM(str);
     polen(row_alx[0],str,sizeof(str),3,'|');
     procsoc1=(float)ATOFM(str);
     continue;
    }
  if(SRAV(str,"Процент отчисления в фонд безработицы",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     procbez=(float)ATOFM(str);
     continue;
    }
**************************/


  if(SRAV(str,"Коды благотворительной помощи",0) == 0)
   {
    iceb_t_sozmas(&kodmp,row_alx[0],&cur_alx);
    continue;
   }


  if(SRAV(str,"Коды начислений не входящие в расчет подоходного налога",0) == 0)
   {
    iceb_t_sozmas(&knvr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды начислений неденежными формами",0) == 0)
   {
    iceb_t_sozmas(&knnf,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str,"Коды начислений неденежными формами на которые начисляется НДС",0) == 0)
   {
    iceb_t_sozmas(&knnf_nds,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str,"Коды начислений, на которые не распространяется социальная льгота",0) == 0)
   {
    iceb_t_sozmas(&kn_bsl,row_alx[0],&cur_alx);
    continue;
   }
/************
  if(SRAV(str,"Коды не входящие в расчет пенсионного отчисления",0) == 0)
   {
    iceb_t_sozmas(&kodnvpen,row_alx[0],&cur_alx);
    continue;
   }
***************/
  if(SRAV(str,"Код доплаты до минимальной зарплаты",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    koddopmin=(short)ATOFM(str);
    continue;
   }

  if(SRAV(str,"Код экономической классификации затрат для атестованных",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_at=atoi(str);
    continue;
   }
  if(SRAV(str,"Код экономической классификации затрат для неатестованных",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_nat=atoi(str);
    continue;
   }
  if(SRAV(str,"Код экономической классификации затрат для начислений на фонд оплаты",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kekv_nfo=atoi(str);
    continue;
   }
  if(SRAV(str,"Код звания для неатестованых",0) == 0)
   {
    polen(row_alx[0],str,sizeof(str),2,'|');
    kodzv_nat=atoi(str);
    continue;
   }

  if(SRAV(str,"Коды не входящие в расчет доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kodnvmin,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды отдельного расчета доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kodotrs,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Коды удержаний входящие в расчет подоходного налога",0) == 0)
   {
    iceb_t_sozmas(&kuvr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str,"Округление",0) == 0)
    {
     polen(row_alx[0],str,sizeof(str),2,'|');
     okrg=ATOFM(str);
    }

 }

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesv();


memset(str,'\0',sizeof(str));
memset(bros,'\0',sizeof(bros));
//sprintf(str,"%s",iceb_t_imafnsi("nastrb.alx"));
iceb_t_poldan("Многопорядковый план счетов",bros,"nastrb.alx");
if(SRAV(bros,"Вкл",1) == 0)
 vplsh=1;


startgodb=0;
if(iceb_t_poldan("Стартовый год",bros,"nastrb.alx") == 0)
 {
  startgodb=(short)ATOFM(bros);
 }

/*Ищем списки начислений для дополнительных зарплатных счетов*/

for(int kkk=0; kkk < dop_zar_sheta.kolih(); kkk++)
 {
  if(dop_zar_sheta.ravno(kkk)[0] == '\0')
   continue;
  sprintf(str,"Начисления, для которых проводки выполняются по дополнительному счёту %s",dop_zar_sheta.ravno(kkk));
  memset(bros,'\0',sizeof(bros));
  iceb_t_poldan(str,bros,"zarnast.alx");
  dop_zar_sheta_spn.plus(bros);

/*  printw("dop_zar_sheta=%s-%s\n",dop_zar_sheta.ravno(kkk),dop_zar_sheta_spn.ravno(kkk));*/
 
 }

/*расчёт по фонду осуществляется в момент выхода из просмотра начисления поэтому обязательно должен быть прочитан здесь*/
/**************
class iceb_tu_str znah("");
if(iceb_t_poldan("Процент начисления для фонда безработицы",&znah,"zardog.alx") == 0)
 {
  
  procent_fb_dog=znah.ravno_atof();
 }
***************/

return(0);
}
