/*$Id:$*/
/*06.14.2018	14.02.2000	Белых А.И.	buhl.h
*/
#include <glib.h>
#include	<util.h>
#include	<scr.h>
#include	<libsql.h>
#include        <lock_tables.h>
#include "icebt_sql_flag.h"
#include "iceb_t_mstr.h"
#include "iceb_t_fioruk.h"
#include "vvod.h"
#include "iceb_t_avp.h"
#include "iceb_t_tmptab.h"
#include "iceb_t_rnfxml.h"
#include "dbfhead.h"
#include "iceb_t_fopen.h"
#include "iceb_t_getkue.h"
#include "iceb_t_imp_xml_nn.h"
#include "iceb_t_vstavfil.h"
#include "iceb_t_vrvr.h"
#include "iceb_t_rnl.h"

#define ICEBEMAIL "sasa@ukrpost.ua"
#define ICEBSAIT "http://iceb.net.ua"
/*Права доступа к создаваемым программой файлам*/
//#define ICEB_UMASK 0117 /*хозяину и группе читать и писать*/
#define ICEB_UMASK 0111 /*Маска для каталогов всем читать и писать*/
#define ICEB_KAT_ALX      ".iceB"             /*Имя каталога который создаётся в домашнем каталоге пользователя для настроечных файлов*/

#define ICEB_DATA_BAZ "27.04.2018" /*Дата последней модификации базы данных*/

const short ICEB_PS_GK =  1; /*Код подсистемы "Главная книга"*/
const short ICEB_PS_MU =  2; /*Код подсистемы "Материальный учёт"*/
const short ICEB_PS_PD =  3; /*Код подсистемы "Платёжные документы"*/
const short ICEB_PS_ZP =  4; /*Код подсистемы "Заработная плата"*/
const short ICEB_PS_UOS=  5; /*Код подсистемы "Учёт основных средств"*/
const short ICEB_PS_UU =  6; /*Код подсистемы "Учёт услуг"*/
const short ICEB_PS_UKO=  7; /*Код подсистемы "Учёт кассовых ордеров"*/
const short ICEB_PS_UKR=  8; /*Код подсистемы "Учёт командировочных расходов"*/
const short ICEB_PS_UPL=  9; /*Код подсистемы "Учёт путевых листов"*/
const short ICEB_PS_RNN= 10; /*Код подсистемы "Реестр налоговых накладных"*/
const short ICEB_PS_UD = 11; /*Код подсистемы "Учёт доверенностей"*/
const short ICEB_PS_ABD= 12; /*Код подсистемы "Администрирование баз данных"*/

const short ICEB_KOD_OP_PROG=99; /*Код оператора для действий выполненных программой без участия оператора*/
#define ICEB_LOGIN_OP_PROG "l99"

#define ICEB_MP_ZARP "ЗП" /*Метка проводки выполненной из подсистемы "Заработная плата"*/
#define ICEB_MP_MATU "МУ" /*Метка проводки выполненной из подсистемы "Материальный учёт"*/
#define ICEB_MP_UOS "УОС" /*Метка проводки выполненной из подсистемы "Учёт основных средств"*/
#define ICEB_MP_PPOR "ПЛТ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных поручений*/
#define ICEB_MP_PTRE "ТРЕ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных требований*/
#define ICEB_MP_KASA "КО" /*Метка проводки выполненной из подсистемы "Учёт кассовых ордеров"*/
#define ICEB_MP_USLUGI "УСЛ" /*Метка проводки выполненной из подсистемы "Учёт услуг"*/
#define ICEB_MP_UKR "УКР" /*Метка проводки выполненной из подсистемы "Учёт командировочных расходов"*/

#define ICEB_MP_GK "ГК" /*Обозначение подсистемы "Главная книга" в реестре налоговых накладных*/

#define ICEB_ZAR_PKTN "тн" /*приставка к табельному номеру*/

extern SQL_baza	bd;

class spis_oth
 {
  public:
   class SPISOK spis_imaf; /*Список имён файлов с отчётами*/
   class SPISOK spis_naim; /*Список наименований отчётов*/
   class SPISOK spis_ms;   /*список имён файлов у которых метка сохранить*/  
   class SPISOK spis_mk;   /*список имён файлов у которых метка кодировки windows-1251*/
  void clear()
   {
    spis_imaf.free_class();
    spis_naim.free_class();
    spis_ms.free_class();
    spis_mk.free_class();
   }
 };

struct OPSHET
 {
  char	naim[512];//Наименование
  short tips;    // 0-активный 1-пассивный 2-активно-пассивный
  short vids;    // 0-счет 1-субсчет
  short saldo;   // 0-сальдо свернутое 3-сальдо развернутое
  short stat;    // 0-балансовый 1-не балансовый
  short val;     // 0-национальная валюта или номер валюты из справочника валют
  short ksb;     //код суббаланса
 };

class akt_sverki //Для расчёта акта сверки по контрагентам
 {
  public:
   class SPISOK data_nomd; //Дата|номер документа
   class SPISOK koment;    //коментарий
   class masiv_din_double suma_deb; 
   class masiv_din_double suma_kre; 
   double start_saldo_deb;
   double start_saldo_kre;
   class iceb_tu_str shet;  
   short dn,mn,gn;
     
  akt_sverki()
   {
    start_saldo_deb=start_saldo_kre=0.;
    shet.plus("");
    dn=mn=gn=0;
   }
  void clear()
   {
    start_saldo_deb=start_saldo_kre=0.;
    shet.new_plus("");
    suma_deb.free_class();
    suma_kre.free_class();
    data_nomd.free_class();
    koment.free_class();    
    dn=mn=gn=0;
   }
 };



void sozdmas(char *obr,char **maskmat,long *dlmas,char razd);
int runsystem1(const char *comanda,int metka);
int runsystem(const char *comanda);
int	oprsk(const char*);
int	sql_zapis(const char*,int,int);
int sql_zapis(const char *strsql,int metka,int metkasoob,WINDOW *win);
int	provinnom(const char*);
int	invnomer(int);
int   	nomnalnak(short,short);
int	provnomnnak(short,short,const char*);
void     eropbaz(const char*,int,const char *name,int);
void     eropbaz(const char*,int,const char *name,int,FILE*);
int	vibrek(const char*,char*,int,char*);
int      vibrek(const char *tabl,class iceb_tu_str *kodrec,class iceb_tu_str *naim);
int      vibrek(const char *tabl,class iceb_tu_str *kodrec);
int	vvodkod(const char*,class iceb_tu_str*,class iceb_tu_str*,int,int,int);
void	raspprov(SQLCURSOR*,short,short,short,const char*,int,int);
void	kasord1(const char*,const char*,short,short,short,const char*,const char*,double,const char*,const char*,const char*,const char*,FILE*);
void	kasord2(const char*,const char*,short,short,short,const char*,const char*,double,const char*,const char*,const char*,const char*,const char*,FILE*);
int	dirmat1(const char*,class iceb_tu_str*,class iceb_tu_str*,int,int,int,short,int*,int*,const char*,const char*);
int 	dirkontr(class iceb_tu_str*,class iceb_tu_str*,int);
int 	dirshet(class iceb_tu_str*,class iceb_tu_str*,int);
int      dirskont(const char *sht,class iceb_tu_str *kodo,class iceb_tu_str *nai,int met);
int	poivcur(const char*,int,SQLCURSOR*);
int	provsh(class iceb_tu_str *kor,const char*,struct OPSHET*,int,int);
int 	provud(const char*,const char*,int);
int 	prsh(const char*,OPSHET*);
int 	prsh1(const char*,OPSHET*);
int 	prsh1(const char*);
int	salorok(const char*,const char*,class spis_oth*);
int	smenabaz(int);
int smenabaz_mysql(class iceb_tu_str *imabazv,int metka);
int vkontr(const char*,class iceb_tu_str*,int);
int zapprov(int val,int god,int mes,int den,const char *sh,const char *shk,const char *kor1,const char *kor2,const char *kto,const char *nn,const char *kop,double deb,double kre,const char *raz,short kp,int pod,time_t vrem,short gd,short md,short dd,int tipz,int kekv);

double   sumvpr(const char*,const char*,short,short,short,const char*,int,const char*,time_t,int,const char*);
void zapmpr(short g,short m,short d,const char *sh,const char *shk,const char *kor1,const char *kor2,double deb,double kre,const char *kom,int kolpr,int kekv,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE*);
void zapmpr1(const char *nn,const char *kop,int skk,time_t vrem,const char *kto,short dd,short md,short gd,int tipz,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE*);

void     sozvprov1(VVOD*);
void vprov1(VVOD *VV,const char *metkasys,short nin,class iceb_tu_str *dpr,const char *nn,const char *kop,int skk,int dd,int md,int gd,short mt,time_t vremz,const char *kontr,int  ktozap,int pods,int tipz,int val);
void     salork(const char*,const char*,double*,double*,short,char*,char*,const char*,short,short,short,short,short,short,const char*,short,char*,char*);
void     printcod(FILE*,const char*,const char*,const char*,int,const char*,int*);
void	korkod(const char*,const char*,const char*);
int	udprgr(const char*,short,short,short,const char*,int,int);
int    	udprov(int,short,short,short,const char*,const char*,time_t,double,double,const char*,short);
void	poigod(int);
void	smenkodk(const char*,const char*);
int	dikont(char,const char*,const char*,const char*,const char*);
int iceb_redfil(const char*,int metka_r);
int iceb_redfil(const char *imafil,int metka_r,const char *editor);
void     zagacts(short,short,short,short,short,short,short,short,short,const char*,const char*,FILE*);
void	konact(FILE*);
void	zagsh(const char*,FILE*);
void     salorksl(double,double,double,double,double,double,FILE*,FILE*);
int	dirmt(class iceb_tu_str*,int);
void	msql_error(SQL_baza*,const char*,const char*,FILE*);
void	msql_error(SQL_baza*,const char*,const char*);
void	msql_error(SQL_baza*,const char*,const char*,WINDOW *win);
int      dolruk(char*,int,char*,int);
int      readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur,WINDOW *win);
int      readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur);
int      readkey(char *zapros,WINDOW *win);
int      readkey(char *zapros);
void icebstart(int argc,char **argv);
void icebstart_rf();
int exit_iceb(int);
void imafn(const char*,class iceb_tu_str*);
void pehf(const char *imaf,short kk,const char *print);
void dirlst(const char *ima);
int spis_koment(class iceb_tu_str *koment,const char *imaf_s_koment);
int dirf(char *tif,int mp,int y,int x,int kk);
int dirfajl(const char *name,int mi,class SPISOK *mfa);
int dirkatl(class iceb_tu_str *imafil);
void printw_vr(time_t vrem_n);
int iceb_t_spis_komp(const char *imaf);
void rabfil(char *naim,char *imafs,char *klush_peh);
void akt_sv(class akt_sverki *akt_svr,FILE *ff,FILE *ff_bi);
int  vvod1(char *strk,int dlstrk,int kolbiat,VVOD *VV,int (*outvvod)(char*),WINDOW *win1,int PY,int PX);
int  vvod1(class iceb_tu_str *strk,short dlstrk,VVOD *VV,int (*outvvod)(char*),WINDOW *win1,int PY,int PX);
void readkey_par(int argc,char **argv,char **parol,char **imabaz,char **host);
int readkey_par(int argc,char **argv,char **parol,char **imabaz,char **host,FILE *ff);
int	mprintcap(void);
int iceb_t_kolstrfil(const char *imaf);
int iceb_t_rstrf(int nomer_str,const char *imaf,class iceb_tu_str *str_fil);
int dirsubbal(int metkar,class iceb_tu_str *kod_subbal,class iceb_tu_str *naim_subbal);
void iceb_t_spk(short dk,short mk,short gk,const char *shet,const char *naimsh,const char *kontr,const char *naikontr,double suma,FILE *ff_spr);
void  iceb_t_rabfil(class spis_oth *oth,const char *klush_peh);
int iceb_t_system(const char *komanda,int);
int menufil(const char *imafil,int y,int x,int kk,SPISOK *masiv,char *soob);
int menufil1(const char *imafil,SPISOK *masiv);
int     dirmasiv(SPISOK *masivmenu,int y,int x,int kk,const char *soob,int nomras,int *kod_kl);
void iceb_t_ko2(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff_f);
void iceb_t_ko1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,FILE *ff_f);
int iceb_t_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob);
int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob);
int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,int metka_soob);
int iceb_t_pbpds(short mes,short god,int metka_soob);
int iceb_t_pbpds(short mes,short god);
int iceb_t_pbpds(const char *data_str);
int iceb_t_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl);
int iceb_t_delfil(const char *katalog,const char *rashir);
int iceb_t_perzap(int metka);
int iceb_t_fioruk(int metka,class iceb_t_fioruk_rk *rek);
void iceb_t_mpods(int metka_ps,char *kto);
int iceb_t_mpods(char *kto);
int iceb_t_pvkdd(int metka_ps,short dd,short md,short gd,short mk,short gk,int pod,const char *nomdok,int tipz);
int	vibprint(void);
int	nastsys();
void    pros_fil(const char *imaf,const char *klush_s_fil);
void	prosf(const char*);
int vizred(const char*);
int vizred(const char *imaf,const char *editor);

int iceb_t_pvglkni(short mes,short god);
int iceb_t_pvglkni(const char *data);
int iceb_t_soob(const char *soob);
int iceb_t_soob(const char *soob,int metka_nlk,int ten);
int iceb_t_soob(const char *soob,int metka_nlk,int ten,WINDOW *win);
int iceb_t_soob(const char *soob,WINDOW *win);
void infosys(int y,int x,const char *host,const char *vess,const char *DV,short ddd,short mmm,short ggg,const char *imabaz,int uid,const char *fioop);
int iceb_t_make_kat(class iceb_tu_str *putnakat,const char *podkat);
void iceb_t_fsystem(const char *imaf_nast,const char *zamena);
int vparol(VVOD *VV,char parol[],int dlstrk,WINDOW *win1,int Y,int X,int TEN);
int parolv(VVOD *PAROL,short d,short m,short g,int metka);
int parolv(const char *text,int metka);
void kzvz(const char *ktozap,const char *vrem,VVOD *VV);
void kzvz(int ktozap,time_t vrem,VVOD *VV);
int   rsdatp(short *dn,short *mn,short *gn,const char *datn,short *dk,short *mk,short *gk,const char *datk);
int rsdatp(const char *datan,class iceb_tu_str *datak);
int iceb_t_rsdatp1(short *mn,short *gn,const char *datan,short *mk,short *gk,const char *datak);
void error_op_nfil(const char *imafil,int koderrno,const char *soob);
int iceb_t_poldan(const char *strpoi,char *find_dat,const char *imaf);
int iceb_t_poldan(const char *strpoi,int *kod,const char *imaf);
int iceb_t_poldan(const char *strpoi,double *kod,const char *imaf);
int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf);
int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf,int metka_ps);
int iceb_t_poldan_vkl(const char *strpoi,const char *imaf);
int vixod(short d,short m,short g);
void podpis(FILE *ff);
int perecod(int metka_kod,const char *imaf);
int danet(VVOD *VV,int N,WINDOW *win1);
int menu3(VVOD *VV,int *N,int mt,int PY,int PX);
int menu3w(WINDOW *win1,VVOD *VV,int *N,int Y,int X,int mt);
int soobsh(VVOD *VV,int PY,int PX,int TEN);
int soobshw(VVOD *VV,WINDOW *win1,int PY0,int PX0,int ost,int TEN);
int iceb_t_menudpr(int metka);
void iceb_t_ndelkod(const char *tablica,const char *kod);
void iceb_t_avp(class iceb_t_avp *avp,const char *imaf_nast,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE *ff_prot);
int iceb_t_ustpeh(const char *imaf,int orient);
int iceb_t_ustpeh(const char *imaf,int orient,int *kolstr);
int iceb_t_sortkk(int metkasort,class SPISOK *skontr,class SPISOK *skontr_sort);
int iceb_t_cat(const char *imaf1,const char *imaf2);
int iceb_t_cp(const char *imaf_out,const char *imaf_in,int metka);
int my_getstr(char *stroka,int dls,int mdls,int kolbait,WINDOW *okno);
int iceb_t_pnnnp(const char *datann,const char *datavnn,const char *kodkontr,const char *nomnn,const char *datadok,const char *nomdok,int metka_ps);
int iceb_t_00_skl(const char *kontr);
void helstr(int py,int px,...);
int iceb_t_getuid();
void iceb_tl_blok();
int iceb_t_pblok(short mes,short god,int kodps);
int iceb_t_prr();
int iceb_t_prr(int podsystem);
const char *iceb_t_getnps();
int iceb_t_alxout(const char *imaf);
int iceb_t_alxout(const char *imaf,const char *imafin);
void iceb_t_alxin(const char *imaf);
const char *iceb_t_getfioop();
int iceb_t_prn();
int iceb_t_prn(int podsystem);
int iceb_t_pdoc(const char *imafil);
const char *iceb_t_kszap(const char *kodop);
const char *iceb_t_kszap(int kodop);
int iceb_t_getnkontr(int nom_start);
int iceb_t_insfil(const char *imaf,FILE *ff);
double iceb_t_pnds(short d,short m,short g);
double iceb_t_pnds(const char *data);
double iceb_t_pnds();
double iceb_t_pndsf(short d,short m,short g);
double iceb_t_pndsf(const char *data);
double iceb_t_pndsf();
void iceb_t_fplus(const char *stroka,class iceb_tu_str *stroka1,class SQLCURSOR *cur);
void iceb_t_fplus(int metka_sp,const char *stroka,class SPISOK *nastr,class SQLCURSOR *cur);
void iceb_t_sozmas(short **kod,char *st1,class SQLCURSOR *cur);


int iceb_t_getuslp(const char *kodkon,class iceb_tu_str *uslprod);
int iceb_t_getuslp(int un_nom_zap,class iceb_tu_str *uslprod);
int iceb_t_getuslp(const char *kodkon,class iceb_tu_str *uslprod,int metka);
int iceb_t_getuslp(int un_nom_zap,class iceb_tu_str *uslprod,int metka);
int iceb_t_dirdog(int metka_r,const char *kodkon);
void iceb_t_vkk00();
void iceb_t_vk00(class iceb_tu_str *k00);
void GDITE();
void iceb_t_rnfxml(class iceb_rnfxml_data *rek_zag_nn);
int iceb_t_openxml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *tip_dok,const char *pod_tip_dok,const char *nomer_versii_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml);
int iceb_t_openxml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *kod_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml);

const char *iceb_t_get_pnk(const char *kodkontr,int metka);

void iceb_t_locale();

void iceb_t_zagolov(const char *naim,short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff);
void iceb_t_zagolov(const char *naim,FILE *ff);
void iceb_t_zagolov(const char *naim,const char *datan,const char *datak,FILE *ff);

int iceb_t_nn9_start(const char *imaf,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *kpos,int tipz,const char *kodkontr00,FILE **ff);
int iceb_t_nn9_start_v(class iceb_tu_str *zved_pn,class iceb_tu_str *snoz,class iceb_tu_str *nevid,class iceb_tu_str *tippr);
void iceb_t_nn9_str(int nomer_str,int metka_str,const char *naim,const char *ei,double kolih,double cena,double suma,const char *kodtov,int metka_imp,const char *ku,const char *kdstv,FILE *ff);
void iceb_t_nn9_end(int tipz, short lnds,const char *imaf_nastr,const char *kodop,const char *kod_kontr,FILE *ff);
void iceb_t_nn9_hap(int nomstr,int *kolstr,FILE *ff);
void iceb_t_nn9_start_s(const char *imaf,int lnds,double suma_dok,double suma_nds,double suma_vt);

void iceb_t_nn10_str_xml(int nomer,double kolih,double cena,double suma,const char *ei,const char *naim_mat,const char *kodtov,const char *kod_stavki,const char *kod_lgoti,int metka_imp_tov,const char *ku,const char *kdstv,double spnv,FILE *ff_xml);
void iceb_t_nn10_end_xml(const char *kodop,const char *imaf_nastr,int lnds,double sumdok,double suma_nds,double suma_vt,FILE *ff_xml);
void iceb_t_nn10_zag_xml(int tipz,const char *nomdok,short dnp,short mnp,short gnp,short dd,short md,short gd,const char *kontr00,const char *kontr,const char *vid_dok,FILE *ff_xml);

const char *iceb_t_getk00(int metka,const char *kodop);
int iceb_t_menu_import(class iceb_tu_str *imaf_imp,const char *zagolov,const char *imaf_nast,const char *imaf_help);
void iceb_t_rnn_sap_xml(short mn,short gn,short mk,const char *tip_dok,const char *podtip,int type_ver,int pnd,const char *imaf,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);
void iceb_t_rnn_sap_xml(short mn,short gn,short mk,const char *kod_dok,int pnd,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);

void iceb_t_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname);
void iceb_t_blokps();
const char *iceb_t_get_namebase();
void iceb_t_zageks(const char *naim,FILE *ff);
void iceb_t_lnom();
const char *iceb_t_get_printer();
int iceb_t_prov_vdb(const char *namebases);
int iceb_tl_ei(int metkar,class iceb_tu_str *ed_izmer,class iceb_tu_str *naim_kod);
int iceb_t_rnl(const char *imaf,int orient,const char *obr,void (*hapka)(int nom,int *kostr,FILE *FF));
int iceb_t_rnl(const char *imaf,class iceb_rnl_c *rh,void (*hapka)(class iceb_rnl_c *rh,int *nom_str,FILE *ff));

const char *iceb_t_getkoddok(short d,short m,short g,int metka);

int iceb_t_open_fil_nn_xml(char *imaf_xml,int pnd,int tipz,short mn,short gn,FILE **ff_xml);
void str_nn_xml(int nomer,int metka_nds,const char *datop,char *kolih,double cena,double suma,const char *ei,const char *naim_mat,const char *kodtov,FILE *ff_xml);
int iceb_t_imp_xml_nn(const char *imaf_xml,class iceb_imp_xml_nn_r *rk);


void iceb_t_nomnak(const char *data,const char *skll,class iceb_tu_str *ndk,int tz,int metka,int metka_lid_nol);
void iceb_t_nomnak(short g,const char *skll,class iceb_tu_str *ndk,int tz,int metka,int metka_lid_nol);
void iceb_t_nomnak(short g,int skll,class iceb_tu_str *ndk,int tz,int metka,int metka_lid_nol);
void iceb_t_nomnak(short g,const char *skll,char *ndk,int tz,int metka,int metka_lid_nol);

int iceb_t_nomdok(short god,const char *tabl);
int iceb_t_nabor(class iceb_tu_str *nabor,const char *imaf_s_nabor);

int iceb_t_vstavfil(class iceb_vstavfil_rk *rek,const char *naimfil,FILE *ff);

int iceb_t_get_kkfname(const char *naim,const char *kodkon,class iceb_tu_str *kkontr,int *nom_kod_kontr);

int iceb_t_pvkup(int kodop_v_prov);
int iceb_t_zkvsk(const char *strok,int *nom_kod_kontr,FILE *ff_prot);

int iceb_t_get_rtn(int tabnom,class iceb_t_fioruk_rk *rek);




const char *iceb_t_get_kodstavki(int lnds);
const char *iceb_t_get_fiotbn(int tabnom);

int iceb_t_vrvr(const char *imaf,class iceb_vrvr *rv,FILE *ff);
const char *iceb_t_get_edrpou(const char *kod_kontr);


