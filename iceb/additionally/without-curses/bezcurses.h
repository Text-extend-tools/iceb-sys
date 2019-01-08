/*$Id:$*/
/*17.11.2016	09.09.2013	Белых А.И.	bezcurses.h
*/
#include <glib.h>
#include <libsql.h>
#include <util.h>
#include "lock_tablesf.h"
#include "ostatokt.h"
#include "iceb_t_fopenf.h"
#include "iceb_t_getkuef.h"
#include "iceb_t_avpf.h"

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

#define ICEB_MP_ZARP "ЗП" /*Метка проводки выполненной из подсистемы "Заработная плата"*/
#define ICEB_MP_MATU "МУ" /*Метка проводки выполненной из подсистемы "Материальный учёт"*/
#define ICEB_MP_UOS "УОС" /*Метка проводки выполненной из подсистемы "Учёт основных средств"*/
#define ICEB_MP_PPOR "ПЛТ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных поручений*/
#define ICEB_MP_PTRE "ТРЕ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных требований*/
#define ICEB_MP_KASA "КО" /*Метка проводки выполненной из подсистемы "Учёт кассовых ордеров"*/
#define ICEB_MP_USLUGI "УСЛ" /*Метка проводки выполненной из подсистемы "Учёт услуг"*/
#define ICEB_MP_UKR "УКР" /*Метка проводки выполненной из подсистемы "Учёт командировочных расходов"*/

#define ICEB_ZAR_PKTN "тн" /*приставка к табельному номеру*/

#define ICEB_UMASK 0111 /*Маска для каталогов всем читать и писать*/

extern SQL_baza bd;

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

void readkey_par(int argc,char **argv,char **parol,char **imabaz,char **host,int *regim);
void icebstart(int argc,char **argv);
int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf,int metka_ps);
int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf);
int iceb_t_poldan(const char *strpoi,char *find_dat,const char *imaf);
int iceb_t_poldan(const char *strpoi,int *kod,const char *imaf);
int iceb_t_poldan(const char *strpoi,short *kod,const char *imaf);
int iceb_t_poldan(const char *strpoi,double *kod,const char *imaf);
int readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur);
int readkey(char *zapros);
int iceb_t_getnkontr(int nom_start);
int iceb_t_getuid();
double iceb_t_pnds(short d,short m,short g);
double iceb_t_pnds(const char *data);
double iceb_t_pnds();
int prsh(const char *ssh,OPSHET *shetv);
int prsh1(const char *shet,OPSHET *shetv);
int sql_zapis(const char *strsql,int metka,int metkasoob);
void iceb_t_locale();
void	msql_error(SQL_baza *bd,const char *soobsh,const char *zapros);
void readkey_par(int argc,char **argv,char **parol,char **imabaz,char **host,int *regim);

int zap_prihod(short dd,short md,short gd,const char *nomdok,int sklad,int kodmat,double kolih,double cena,
const char *eiz,const char *shetu,double nds,int vtara,int mnds,const char *nomzak,const char *shet_sp);

short zapkar(int skll,int  nkk,int km,double ccn,const char *eii,const char *shu,double krt,double fas,int kodt,double nds,int mnds,double cenav,
short denv,short mesv,short godv,const char *innom,const char *rnd,const char *nomz,short deng,short mesg,short godg,int mv);

int zapvdok(short dd,short md,short gd,int skll,int nkk,int km,const char nnn[],double kol,double cen,const char ei[],double nds,int mnds,int voztar,
int tipz,int nomkarp,const char *shet,const char *nomz);
int             nomkr(int sklad);
void nomnak(short g,const char *skll,class iceb_tu_str *ndk,int tz,int metka,int metka_lid_nol);
void nomnak(short g,const char *skll,char *ndk,int tz,int metka,int metka_lid_nol);
int nomkm(int nom_start);
int nomkm();
int nomkr(int sklad);
int priv_k_kart(short dk,short mk,short gk,const char *nomd,int sklad,int kodm,double kolih,double cena,const char *shetu,const char *ei,
double ndss,int mnds,int voztar,const char *nomzak,int metka_priv,const char *shet_sp);

int findkar(short dd,short md,short gd,int sklad,int kodmat,double kolih,masiv_din_int *nomkart,masiv_din_double *kolihp,
int kodopuc,double cena,const char *shetu);

void ostkar(short dn,short mn,short gn,short dk,short mk,short gk,int skl,int nk,class ostatokt *ost);

short zapkar(int skll,int  nkk,int km,double ccn,const char *eii,const char *shu,double krt,double fas,int kodt,double nds,int mnds,double cenav,
short denv,short mesv,short godv,const char *innom,const char *rnd,const char *nomz,short deng,short mesg,short godg,int mv);

int zapvdok(short dd,short md,short gd,int skll,int nkk,int km,const char nnn[],double kol,double cen,const char ei[],double nds,int mnds,int voztar,int tipz,
int nomkarp,const char *shet, const char *nomz);
int prdpp(int skl,int nk,short *dp,short *mp,short *gp);
double readkolk(int skl,int nk,short d,short m,short g,const char *nn);
int zapvkr(short dp,short mp,short gp,const char *nn,int skl,int nk,short d,short m,short g,int tp,double kolih,double cena);
int iceb_t_pblok(short mes,short god,int kodps);
int iceb_t_pbpds(short mes,short god,int metka_soob);
int iceb_t_pbpds(short mes,short god);
int iceb_t_pbpds(const char *data_str);
int udprgr(const char *kto,short dd,short md,short gd,const char *nn,int pod,int tipz);
int iceb_t_pvglkni(const char *data);
int iceb_t_pvglkni(short mes,short god);

int zapprov(int val,int god,int mes,int den,const char *sh,const char *shk,const char *kor1,const char *kor2,const char *kto,
const char *nn,const char *kop,double deb,double kre,const char *raz,short kp,int pod,time_t vrem,short gd,short md,short dd,
int tipz,int kekv);

void error_op_nfil(const char *imafil,int koderrno,const char *soob);
int iceb_t_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob);
int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob);
int iceb_t_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,int metka_soob);
int iceb_t_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl);
void iceb_t_mpods(int nomer_ps,char *kto);
int iceb_t_mpods(char *kto);
int sumkasorue(const char *kassa,short tipz,const char *nomd,short god,double *sumd,double *sumpod);

int provsh(class iceb_tu_str *kor,const char *shp4,struct OPSHET *shetv,int metka,int metkanbs);
void avtpromu1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot);
void iceb_t_avp(class iceb_t_avp *avp,const char *imaf_nast,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE *ff_prot);
void zapmpr1(const char *nn,const char *kop,int skk,time_t vrem,const char *kto,short dd,short md,short gd,int tipz,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE *ff_prot);
void zapmpr(short g,short m,short d,const char *sh,const char *shk,const char *kor1,const char *kor2,double deb,double kre,const char *kom,int kolpr,int kekv,class SPISOK *sp_prov,class masiv_din_double *sum_prov_dk,FILE *ff_prot);
double sumvpr(const char *sh,const char *shk,short d,short m,short g,const char *nn,int skk,const char *kom,time_t vrem,int metkaprov,const char *kto);

int prospr(short mr,int skl,short d,short m,short g,const char *nn,int tp,int lnds,int skl1,const char *nomdokv,const char *kodop);
int matnast(void);

void uslpoddok(short dd,short md,short gd,short d1,short m1,short g1,const char *nomdok,int podr,int tipz,int metka_soob);
void podudok(short d,short m,short g,const char *nn,int podr,int tipz);
int prosprus(short mr,int podr,short d,short m,short g,const char *nn,int tp,int lnds,const char *kodop,float pnds);
void avtprousl1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot);
int zapuvdok(int tp,short dd,short md,short gd,const char *nomd,int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,int podr,const char *shet_sp,const char *dop_naim);
double readkolu(short d,short m,short g,int podr,const char *nomd,int tipz,int metka,int kodzap,int nomzap);
int zappodu(int tp,short dd,short md,short gd,short dp,short mp,short gp,const char *nomd,int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,int podr,int nomzap);

int iceb_t_poldan_vkl(const char *strpoi,const char *imaf);
int iceb_t_get_kkfname(const char *naim,const char *kodkon,class iceb_tu_str *kkontr,int *nom_kod_kontr);

int iceb_t_zkvsk(const char *strok,int *nom_kod_kontr,FILE *ff_prot);
