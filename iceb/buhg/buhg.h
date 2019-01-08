/*$Id: buhg.h,v 5.413 2014/08/31 06:18:17 sasa Exp $*/
/*10.06.2018	26.01.1999	Белых А.И.	buhg.h
Объявление подпрограмм
*/
#include <buhl.h>
#include "bsiz.h"
#include "ostuos.h"
#include "iceb_t_razuz.h"
#include "poiprov.h"
#include "ostatokt.h"
#include "zar_read_tn1.h"
#include "zarrsov.h"
#include "zar_sumtn.h"

class ZARP
 {
  public:
  short dz; //День записи
  short godn; //Год в счет котрого запись
  short mesn; //Месяц в счет которого запись
  short nomz; //Номер записи
  int   podr; //Подразделение
  char  shet[64]; //Счет

//  дополнительные реквизиты которые присутствуют в графической версии
  int tabnom; //Табельный номер
  short prn; //1-начисление 2-удержание
  short knu; //Код начисления/удержание
  short mz,gz; //Дата записи

  ZARP()
   {
    clear();
   }

  void clear()
   {
    dz=godn=mesn=nomz=podr=0;
    memset(shet,'\0',sizeof(shet));
    tabnom=prn=knu=mz=gz=0;
    
   }
 };

class guror_rr
 {
  public:
   class iceb_tu_str shet;
   class iceb_tu_str dat1; /*Дата начала*/
   class iceb_tu_str dat2; /*Дата конца*/
   class iceb_tu_str mso; /*коды групп контрагентов*/
   class iceb_tu_str kod_val; /*код валюты*/
   guror_rr()
    {
     clear_data();
    }
  void clear_data()
   {
    shet.new_plus("");
    dat1.new_plus("");
    dat2.new_plus("");
    mso.new_plus("");
    kod_val.new_plus("");
   }
 };


class GLKTMP
 {
public:
  class SPISOK           masf; /*Общий массив счетов*/
  class SPISOK           msr;  /*Массив счетов с развернутым сальдо*/
  class masiv_din_double sl; /*Массив до периода (шахматка)*/
  class masiv_din_double pe; /*Массив за период (шахматка)*/
  class masiv_din_double ssd; /*Массив стартовых сальдо по счетам*/
  class masiv_din_double ssk; /*Массив стартовых сальдо по счетам*/

  class masiv_din_double deb_end; //Массив конечного сальдо по счетам
  class masiv_din_double kre_end; //Массив конечного сальдо по счетам

 };

const short ICEB_DNDESV=1,ICEB_MNDESV=1,ICEB_GNDESV=2011;
const float ICEB_PROCENT_PODOH_NAL_2016=18.;

void	hrvnks(FILE*,int*);
void     hdisk(int,double,char*);
void     koncinv(const char*,short,FILE*);
void     sapvin(const char*,const char*,short,short,int*,int*,const char *sklad_podrm,FILE*);
void     krasact(const char*,const char*,double,double,double,double,FILE*);
void	hrasact(int*,int*,FILE*);
void	sapkkiv(int*,int,FILE*);
void	sapkk1iv(int*,int*,FILE*);
void	sprozn(int,FILE*);
void zagf1df(int kolrh,int kolrs,short kvrt,const char *kod,short TYP,int *klst,int vidd,short dn,short mn,short gn,short dk,short mk,short gk,int metka_oth,FILE *ff1,FILE *ff2);
void koncf8dr(short kvrt,short gn,const char *koded,short TYP,double it1,double it1n,double it2,double it2n,double it11,double it11n,double it21,double it21n,int kolsh,short klls,int *npp1,int metkar,double vs_nd,double vs_vd,double vs_nn,double vs_vn,FILE *ff1,FILE *ff2,FILE *ff4);
void     ssf8dr(int*,short*,FILE*);
void sozkom_prov(class iceb_tu_str *koment,char *komentprov,char *kto,char *kodop);
double	sosnsum(char*,short,short,short,short,short,short);
void cennik(short d,short m,short g,const char *kodtv,const char *naim,double cena,const char *shrihkod,const char *organ,FILE *ff);
void cennikm(char*);
void	cenniks(char*);
/*---------------------Кассовый сервер-------------------------------------*/
typedef	int icebcon;
struct	KASSA {
	icebcon	handle;
	int	sklad;
	int	nomer;
	char 	*host;
	char	*parol;
	char	*port;
	char	*prodavec;
	};

typedef struct KASSA KASSA;
	
void rozprod(short,short,short,short,int,int,const char*,int,KASSA);
void smdok1(short, short, short, KASSA);
int  rasshek(SQLCURSOR*,short,short,short,int,int,const char*,int,KASSA,float);
void printdayshek(KASSA);
void marsiv(KASSA,int);
void mariqvv(KASSA,int);
int  mariqtxt(KASSA,int,const char*);
int  settimmar(KASSA);
int  nastrmariq(int, KASSA*);
void printpershek(KASSA);
int dirdok1(short dd,short md,short gd,int tipz,int skl,class iceb_tu_str *nomdok, KASSA kasr);

int	errorkan(int errn);
int	errorkas(KASSA, char *str);
int	iceb_cmd(KASSA, char*);

/*-------------------------------------------------------------------------*/
/******************Главная гнига******************************************/
int glkni_zvt(int per,const char *shet,const char *kod_kontr,double deb,double kre,const char *imatabl);
int impprov_r(const char *imafim);
int dirsval(int metkar,class iceb_tu_str *kodm,class iceb_tu_str *naim);
void buhdenshet(const char *zagol,class SPISOK *dni,class SPISOK *sheta,class masiv_din_double *den_shet_suma,int metka_f,FILE *ff);
void rpshet_ss(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,double *deb_kre);
void rpshet_rs(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,double *deb_kre);
int pvbanks_kprov(class SPISOK *KONT,int snom,const char *pris,int metkadk,double *sum,class iceb_tu_str *tabnom,double *deb,double *kre,int dlpris,class iceb_tu_str *fio,class iceb_tu_str *bankshet,class iceb_tu_str *inn,const char *grupk,int kod_banka);
double buh_rh(int metka_dkv,class SPISOK *sheta_deb,class SPISOK *sheta_kre,class masiv_din_double *sum_pr,FILE *ff);
void priv_k_kart(short dk,short mk,short gk,const char *nomd,int sklad,int kodm,double kolih,double cena,const char *shetu,const char *ei,double ndss,int mnds,int voztar,const char *nomzak,int metka_priv,const char*);
void raspitog(double,double,int,FILE*);
int zarstfn53(const char*,short mr,short gr,const char*,const char*,int*,FILE*);
int krdvp(short,short,short,short,short,short);	
int nalndoh(void);
void spdfo(short,short,short,short,short,short,SPISOK*,SPISOK*,double*,char*);
void prosprkr(short,short,short,const char*,int);
int provnds(const char*,SQL_str);
void sapitsh(FILE*);
void sumprved(masiv_din_double*,masiv_din_int*,char*);
int provprov(SQL_str row,class poiprov_rek *poi);
void	sapreestp(short,short,int,FILE*);
int	buhnast(void);
int	matnast(void);
void     poiplatdok(const char*);
int	provkod(short*,short);
int	provkod(short*,char*);
void	provtime(void);
void     provzzag(short,short,long,int,int);
int      zaptabel(int,short,short,int,int,float,float,float,short,short,short,short,short,short,float,float,const char*,int,int);
void readkom(const char *tabl,short dd,short md,short gd,const char *nomdk,class iceb_tu_str *koment);
int buhpnk(class iceb_tu_str *naimkontr,char *kodkontr,int metka_pod,int tipz,char *datdok,char *nomdok,int podr);

int      zarudnu(short,short,short,long,short,short,short,short,int,const char*,short);
void     zapmass(char *sh,short dn,short dk,short m,short g,double *sd,double *sk,short vds,short sub,masiv_din_double *debt,masiv_din_double *kredt,class SPISOK *msd,class SPISOK *msk,masiv_din_double *idebt,masiv_din_double *ikredt);
short    opks(char *sh,short dn,short mn,short gn,short dk,short mk,short gk,short sub,class SPISOK *msd,class SPISOK *msk,class SPISOK *mso);
short	readpdok(const char*,short,const char*);
void     rudnfv(double *suma_zar,double suma_ndfnz,double proc_podoh);
void     sumzapuos(int,int,int,const char*,double*,double*,int,int);
void     sumzap1u(int,int,int,const char*,int,double*,double*,int,int);
void	sumzap1(int,int,int,const char*,int,double*,double*,int,short,double*,int,double*);
int      provblokzarp(long,short,short);
void zarsoc(short,short,int);
void    	itog(FILE*,double);
void     sspp1(FILE *,int,int *kolstrok);
double	sumprz(short,short,const char*,int,double*,double*);
void     zapzarn(short,short,int,long,short,short,short,const char*,const char*,short,short,short,short,short,short,short,const char*);
void	zapzarn1(long,short,short,short,short,short,const char*);
void	zaravpr(int,short,short,FILE*);
void	creatheaddbf(char*,long);
void	creatheaddbfsz(char*,long);
void  rozden(const char *shh,short d,short m,short g,double *debet,double *kredit,short vds);

int	poidirnu(SQL_str,struct dirnupoi*,class iceb_tu_str *fio);
void	spkvd(int*,FILE*);
void	sozvnu(VVOD*);
int vnu(int tabn,short knah,short prn,short dz,short mz,short gz,short mp,short gp,int podri,const char *shet,int nomz);

int buh_prov_shet(char *shet,char *shet_kor,class SPISOK *nastr);

void     vnsi(int);
int korsdok(int tipz,short *dd,short *md,short *gd,int *skl,class iceb_tu_str *nomdok,const char *nomnn,const char *nomdokp,const char *kpos,int lnds,class iceb_tu_str *kprr,short dpnn,short mpnn,short gpnn);
void	dirprzar(int,short,short,const char*);
void	dirf8dr(class iceb_tu_str *nomd);
void	kolrab(const char*,int*);
double	pens(int,short,short,double,FILE*);
int	prvkp(const char *kto);

void     itogiz(double,double,double,double,double,double,double,double,FILE*,FILE*,FILE*,FILE*);
void     katsp(short,short,short,short,short);
void     pnr(short,short,short,short,short,short);
int	zarotpusk(long,short,short,double*,double*,double*);
double	zarsald(short,short,short,long,double*);
int	zarudkar(int tn);
void	rasshetz(int tabn,short,short,int,int,const char *nah_only,const char *uder_only);
void ras_pens(long tn,short mp,short gp,int podr,double *sumapo,double *sumapob,int metka_zap,int,double,double*,double*,const char *uder_only,FILE *ff);
void ras_soc(long tn,short mp,short gp,int podr,double *sumapo,double *sumapob,int metka_zap,int,double,double*,double*,const char *uder_only,FILE *ff);
double ras_podoh1(int tn,short mp,short gp,int podr,int metka_zap,double suma_boln,const char *uder_only);

int      dolhn1(int,double*,float*,float*,int*,int*,int*,int*,class iceb_tu_str *shet,int*,short mr,short gr);
double	zarpodpr(int,double*);
void rasnuptn1(short mn,short gn,short dk,short mk,short gk,int tabn,const char *fio,FILE *ff);
void     vztb(long,short,short,short*,double*,short*,short,FILE*);
void     strfil(const char*,const char*,short,FILE*);
void     sspp1(FILE *,short);
void     redkal(short,short,float*,float*);
short    zapmas(long,short,short);

int matspo(int *k1,int *k2,int *k3,int *k4,class motpk_rs *mas);
void	vkart(int tabn,int);
double   findnahm(short,short,long,short,short*,FILE*,double*,double*,double*,double*,double*,double*,double *suma_boln_rm);
double	sumpzpd(const char*,short*,short*,short*,double*);
double	sumprpd(short,short,short,const char*,int,const char*,const char*);
void	provpd(const char*);
short    rozrax(short,short,short,short,short,short,short,GLKTMP*);

int vmat(int tipz,const char *kpos,short dd,short md,short gd,int skl,const char *nomdok,int lnds,int kodmm,double cnn,short cp,const char *sheta,const char *nomdokp,double kolih,
const char *kodop,class iceb_tu_str *nomz,float pnds);

void sapgors(short dn,short mn,short gn,short dk,short mk,short gk,int *nom_lista,int*,FILE *ff);

short provsh(char*,char*,short*);
int zapzarp(short,short,short,long,short,short,double,const char*,short,short,short,short,const char*,int,const char*,class ZARP);
int zapzarp(short,short,short,long,short,short,double,const char*,short,short,short,short,const char*,int,const char*,class ZARP,FILE *ff_prot);
int zapzarp(class ZARP *zp,double suma,short denz,short mv,short gv,short kdn,const char *shet,const char *koment,short nz,int podr,const char *nomdok);
int zapzarpv(short,short,short,long,short,short,double,const char*,short,short,short,short,const char*,int,short,short,short,const char*,class ZARP);

int      zapzagot(short,short,long,int,int);
void     peh1(const char*,const char*,long,const char*,short,short,const char*,int,FILE*,FILE*,FILE*,FILE*,FILE*);

void     pld(short kp,const char *tabl);

int	findprov(SQLCURSOR*,short,short,short,char*,char*,time_t,char*,double,double,short);


/* Прочитать штрих-код */
//int readscan(char *cod);

/* Прочитать вес */
//int	readves(double *rves);

void copdok(short,short,short,const char*,int,short);

short poidog(char*,char*,char*);

int prdpp(int,int,short*,short*,short*);


int provkont(char*,char*);
int rasnaln(short,short,short,int,const char*,const char*,short);

int rasoskr(class spis_oth*);

int readdok(short *dd,short *md,short *gd,const char *nomdok,int skk,int *tipz,class iceb_tu_str *kpos,class iceb_tu_str *naimo,class iceb_tu_str *kprr,
class iceb_tu_str *naimpr,int *metkaprov,class iceb_tu_str *naiskl,int *skl1,int *lnds,short *mvnp,short *mdd,int *pro,int *ktoi,class iceb_tu_str *nomnn,
class iceb_tu_str *nomon,class iceb_tu_str *sheta,int *blokpid,int *loginrash,short *dpnn,short *mpnn,short *gpnn,double *sumandspr,float *pnds,time_t *vrem);

void rozsvod(short,short,short,int,const char*);

void rscena();

short udkar(int,int);
void  sozvdnd(int,VVOD*);
short vdnd(VVOD*,class iceb_tu_str*,int,class iceb_tu_str*,int,int,double,int,int);

void vvoddok();




int zapvdok(short dd,short md,short gd,int skll,int nkk,int km,const char nnn[],double kol,double cen,const char ei[],double nds,int mnds,int voztar,int tipz,int nomkarp,const char *shet,const char *nomz,int metka_imp_tov,const char *kdstv);
void zapvdokf(short,short,short,int,int,int,const char*,double,double,const char*,double,int,int,int,int,const char*,const char*);

int sravrek(SQL_str,char*,char*,char*,char*,char*,char*,char*,char*);

int	dirtab(int*,class iceb_tu_str*,int,int);
int dirtab1(int *tbn,class iceb_tu_str *fio,int podr);
void	dirtabel(short,short,long,const char*);
void	dirnu(short,short,short,short);

int dirprov(class poiprov_rek *poi);

int dirsaldo(const char*);

int nomkm();
int nomkm(int nom_start);


void rasmaskm(char *mas);


int dirmatr(int*,int*,class iceb_tu_str*,int,int,const char*,int,int);

int dirmatr1(int *kgr,int *kod,class iceb_tu_str *nai,int skl,const char *grup,int regim,int,int);

void korcen(int,int,double,short);




void rozvse(short,short,short,int,const char*);
void sumdok(short,short,short,int,const char*,SQLCURSOR*,double*,double*,double*,int*,int,double*,double*,float,int *max_dlina_nazv);
void vivost(short,short,short,class ostatokt *ost);



void rabdokm();

void rasoth();

void nsimatu();

short zapkar(int,int,int,double,const char*,const char*,double,double,int,double,int,double,short,short,short,const char*,const char*,const char*,short,short,short,int);

int zapvkr(short,short,short,const char*,int,int,short,short,short,int,double,double);


void korznkr(int tipz,short dd,short md,short gd,int skl,const char *nomdok,int kodm,int nk,double kolih,double cena,int sklad1,const char *nomdokp,int nomkarp,const char *shet,const char *nomz,time_t vremz,int ktozap,float pnds,int metka_blok);
int dirdok(short,short,short,int,int,class iceb_tu_str*);
void   perecen(short,short,short,const char*,int,float);

int 	makkor(VVOD*,short,const char*,double**,const char*);
double 	makkorr(double,double*);


void read_ini();

void prosprprint(int);
double fbezr_sumnal(double suma);
double zar_rsoc(double soc_suma,float prog_min_tek,FILE *ff);
void	rapsap(FILE*);
int zap_s_mudok(int,short,short,short,int,const char*,const char*,const char*,const char*,int,int,const char*,short,short,short,int metka_opl,const char *k00,float pnds);
void pib_itl(int *kolstrkartr,double *sumalist,int *kol_na_liste,const char *rukov,const char *glavbuh,FILE *ffkartr);
void pib_end_dok(int kol_na_liste,double sumalist,double itogoo,int nomer,const char *rukov,const char *glavbuh,FILE *ffkartr);
void pib_str(int nomer,const char *bankshet,double sum,const char *fio,const char *indkod,FILE *ffkartr);
void vztr(short,short,short,int,const char*,double*,FILE*,short);
double dok_end_mu(int tipz,short dd,short md,short gd,const char *nomdok,int skl,int dlinna,double itogo,float pnds,int lnds,int mnt,int nnds,FILE *ff);
int ukreksim_hap(char *imafr,char *imaftmp,FILE **fftmp,FILE **ffr);
void ukreksim_end(int nomer,const char *imaftmp,char *imafdbf,double sumai,const char *rukov,const char *glavbuh,FILE *fftmp,FILE *ffr);
void ukreksim_h(char *imaf,long kolz);
void ukreksim_zstr(FILE *fftmp,FILE *ffr,int *nomer,double suma,const char *fio,const char *kartshet,const char *inn,short dt,short mt,short gt);
int ukrsib_start(char *imafr,char *imaftmp,class iceb_tu_str *oper,FILE **fftmp,FILE **ffr);
void ukrsib_end(char *imafdbf,char *imaftmp,double sumai,int nomer,const char *rukov,const char *glavbuh,FILE *ffr,FILE *fftmp);
void ukrsib_str(short dt,short mt,short gt,double suma,const char *kartshet,const char *fio,int *nomer,const char *tabnom,const char*,FILE *ffr,FILE *fftmp);
void vzrkf(void);
void vzrkbz();
int dirnuprovb(short,short,int);
void fbezr(long,short,short,int,double*,double*,int,int,double,double*,double*,const char *uder_only,FILE*);
void rashap(int tipz,int *kolstrok,const char *kodkontr00,const char *kontr,int ots,int kor,FILE *ffdok);
void diropld(int metka_ps,short dd,short md,short gd,const char *nomdok,int podr,double suma_opl);
void rasobqv(const char *dt0,const char *dt1,const char *dt2,const char *dt3,const char *fam);
const char *zargetfio(const char *tabnom);
const char *zargetfio(int tabnom);

/******************материальный учёт******************/
void rasnak_sap(class iceb_rnl_c *rh,int *nom_str,FILE *ff);
void rasmushet_sap(class iceb_rnl_c *rh,int *nom_str,FILE *ff);
int mu_pvku(int kodop_v_prov);
int prospr(short,int,short,short,short,const char*,int,int,int,const char*,const char*);
int prospr(short mr,int skl,const char *datdok,const char *nn,int tp,int lnds,int skl1,const char *nomdokv,const char *kodop);
void zap_prihod(short dd,short md,short gd,const char *nomdok,int sklad,int kodmat,double kolih,double cena,const char *eiz,const char *shetu,double nds,int vtara,int mnds,const char *nomzak,const char*);
void zap_prihod(const char *datdok,const char *nomdok,int sklad,int kodmat,double kolih,double cena,const char *eiz,const char *shetu,double nds,int vtara,int mnds,const char *nomzak,const char*);
void avtpromu1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot);
void avtpromu1(int tipz,const char *datdok,const char *nomdok,FILE *ff_prot);
void   ostkar(short,short,short,short,short,short,int,int,class ostatokt *ost);
void   ostkarf(short,short,short,short,short,short,int,int,class ostatokt *ost);
double ostkar1(int kodmat,int sklad,short dost,short most,short gost);
int	findkar(short,short,short,int,int,double,masiv_din_int*,masiv_din_double*,int,double,const char*);
int	findkarf(short,short,short,int,int,double,masiv_din_int*,masiv_din_double*,int,double,const char*);
int impmatdok_r(const char *imafim,int metkaproh);
int sortdokmuw(short dd,short mm,short gg,int skl,const char *nomdok,const char *imatmptab,class iceb_t_tmptab *tabtmp,int *kol_znak_nomn);
int dirsklad(int metkar,class iceb_tu_str *kod_skl,class iceb_tu_str *naim_skl);
int in_d12_start(FILE *ff);
void in_d12_it(class SPISOK *sp_shet,class masiv_din_double *kol_shet,class masiv_din_double *sum_shet,FILE *ffipsh);
void menuvnp(int metka,int tipz);
double skoplmu(short dd,short md,short gd,const char *nomdok,int sklad);
void     rasdokkr(double,double,double,FILE*);
int dirprihod(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int dirrashod(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int poizkart(const char *nomdp,const char *nomdp1,const char *kontrp,const char *kontrp1,const char *kodop,const char *kodop1,int tipzz,int tipzz1);
int mugetdpp(int kodmat,class mugetdpp_str *rk,const char *datan,const char *datak);
void motpk_rrp(class dvtmcf3_rr *rp,FILE *ff);
void matud(const char*,short,short,short,int,int);
int nomkr(int);

double ostdok1(short,short,short,int,int);

void podtdok(short,short,short,int,const char*,int,int,const char*);
void podtdok1(short,short,short,int,const char*,int,short,short,short,int);
void podtdok1(const char *datdok,int skl,const char *nomdok,int tipz,const char *datpod,int metkaeho);
void            dvmat(short,short,short,short,short,short,int,int);

void dirzkart(int,int);

int matudd(short,short,short,int,short,short,short,const char*,int,int,int);
void ostdok(short,short,short,short,short,short,int,int,class ostatokt *ost);

void podvdok(short,short,short,const char*,int);
void podvdok(const char *datdok,const char*,int);

void praisl();

int prihod(short dd,short md,short gd,int skl,const char *nomd,int km,int nk,double cena,const char *ei,int mnds,double ndsg,const char *nomz,int kmm);

int rasdok(short dg,short mg,short gg,int skl,const char *nomdok,const char *imaf,short lnds,short vr,FILE *f1,double ves);
void rasdok2(short,short,short,int,const char*,short);

void rasdm20(short dd,short mm,short gg,int skl,const char *nomdok,int lnds);

double readkolk(int,int,short,short,short,const char*);

/******************Платёжные документы******************/
int platp(const char *imaf1,short kp,short nomk,FILE *ff,int viddok);
void sozkom_pd(char *imaf,double nds,double sumad);



/******************Расчёт зарплаты********************/
int dirzarpodr(int metkar,class iceb_tu_str *kod_podr,class iceb_tu_str *naim_podr);
int zarzapkk(const char *kod_kontr);
void dirzarsdo(int tbn);
void zar_sumtn(short mes,short god,int tabnom,int metka_bn,class zar_sumtn *tnsum);
int vvodf8dr(class iceb_tu_str *nomd,int vidd);
int tabnom(void);
int dirzvan(int metkar,class iceb_tu_str *kod_zv,class iceb_tu_str *naim_zv);
void zarsf1df(int kolih_str,int kolih_fiz_os,int kolih_list,int TYP,FILE *ff1);
void zarnvp();
int  zaravpt_os(int tbn,short mp,short gp,SPISOK *data,masiv_din_double *suma);
double prbper(short,short,short,short,long,double*,int);
double prbper1(short mz,short gz,short dk,short mk,short gk,int tabb,FILE *ff_prot);
double  zaresv(int tabn,short mp,short gp,int podr,double *suma_only_zar,double *suma_only_zarb,int metka_zap,double suma_boln_for_per,const char *uder_only);
double zaresv_rs(class zaresv_rk *resv);
double  podohrs2(int tabn,short mr,short gr,double suma_zar,double suma_soc_ot,double suma_bsl,double suma_ndfv,int metka_zap,int metka_zap_lgot,int metka_p_boln);
double podohrs3(int tabn,short mr,short gr,class zar_sumtn *tnsum,int metka_zap,int metka_zap_lgot);
double podohrs2016(int tabn,short mr,short gr,int metka_zap_lgot,int metka_p_boln,class zar_sumtn *tnsum);
void zarrnesv();
void zarrnesvp(short mr,short gr,class zarrnesvp_rek *all_proc);
int diruder(int metka_rr,class iceb_tu_str *kod_ud,class iceb_tu_str *naim_kod);
int dirnach(int metka_rr,class iceb_tu_str *kod_ud,class iceb_tu_str *naim_kod);
int dirsb(int metkar,class iceb_tu_str *kod_ban,class iceb_tu_str *naim_ban);
void     zapf8dr(long,short,short,const char*,short,const char*,double,double,double,double,const char*,const char*,const char*,short,short,FILE*);
void zar_read_tn(short dn,short mn,short gn,FILE *ffprot);
void zar_read_tn1(short dn,short mn,short gn,class zar_read_tn1h *nastr);
void  zarkvrt_zapvtabl(const char *imatmptab,const char *inn,int kod_doh,const char *kod_lgoti,double dohod_nah,double dohod_vip,double podoh_nah,double podoh_vip,const char *datanr,const char *datakr,const char *fio,const char *tab_kontr,FILE *ff_prot);

float zarlgot(int,short,short,int *kollgtot,class iceb_tu_str *kod_lgot);
void f1df_xml_zag(int kvrt,int god,const char *kod,int TYP,int metka_oth,int kolih_v_htate,int kolih_po_sovm,const char *naim_kontr,char *imaf_xml,FILE **ff_xml);
void f1df_xml_str(int *nomer_xml_str,const char *inn,double sumadn,double sumad,double sumann,double suman,const char *priz,const char *data11,const char *data21,const char *lgotach,const char *metka_zap,FILE *ff_xml);
void f1df_xml_kon(double isumadn,double isumad,double isumann,double isuman,int nomer_xml_str,int kolr,int klls,const char *naim_kontr,const char *telef_kontr,int TYP,double doh_nah_vs,double doh_vip_vs,double vs_nah,double vs_vip,FILE *ff_xml);

int zvb_hreshatik_start(char *imafr,char *imaf,double suma,short *d_pp,short *m_pp,short *g_pp,FILE **ff,FILE **ffr);
void zvb_hreshatik_str(short d_pp,short m_pp,short g_pp,int *nomer_zap,const char *fio,const char *shet_b,const char *inn,double suma,FILE *ff,FILE *ffr);
void zvb_hreshatik_end(double sumai,int kolzap,FILE *ffr,FILE *ff);
int zvb_ibank2ua_start(char *imafr,char *imaf,class iceb_tu_str *vidnahis,const char *kod00,FILE **ff,FILE **ffr);
void zvb_ibank2ua_end(double sumai,const char *vid_nah,FILE *ffr,FILE *ff);
void zvb_ibank2ua_str(int *nomer_zap,const char *fio,const char *shet_b,const char *inn,double suma,FILE *ff,FILE *ffr);

void zvb_ukrgaz_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
int zvb_ukrgaz_start(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr);
void zvb_ukrgaz_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);

void zvb_promekonom_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
int zvb_promekonom_start(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr);
void zvb_promekonom_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);

void zvb_kievrus_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
int zvb_kievrus_start(char *imafr,char *imaf_dbf, char *imaf_dbf_tmp,FILE **ff,FILE **ffr);
void zvb_kievrus_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);

int zvb_privatbank_start(char *imafr,char *imaf_dbf, char *imaf_dbf_tmp,class iceb_tu_str *branch,class iceb_tu_str *zpkod,class iceb_tu_str *rlkod,FILE **ff,FILE **ffr);
void zvb_privatbank_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
void zvb_privatbank_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,const char *branch,const char *zpkod,const char *rlkod,FILE *ff_dbf_tmp,FILE *ffras);

int zarprtn(short mr,short gr,int tabn,int metka_s);
int dirzardk_vvod(short prn,short *dd,short *md,short *gd,class iceb_tu_str *nomdk);
void dirzardkz(short prn,short god_dok,short dd,short md,short gd,class iceb_tu_str *nomdok);
int zartarroz();
float zartarroz(int roz);
void zartarroz(double min_zar,FILE *ff);
int zartarroz1();
double zartarroz1(int roz,int kof,int tabnom,short mr,short gr);
double okladtr(short mr,short gr,int roz);
double oklad(int,short mr,short gr,int*);
double oklad(int,int*);
void zarreadn(short dr,short mr,short gr,int metka_r);
void    	nahud(short,short,long,double*,double*,double*,double*,double*,double*,double*,short,double,short,const char*);
void zarstved(int *nomer_vedom,int tabb,const char *fio,double suma,FILE *ff);
void zarstved(int *nomer_vedom,const char *tabb,const char *fio,double suma,FILE *ff);
double knnfnds(int metka,int knu,const char *shet,double suma,double *sumab,int metka_repl);
int zvb_alfabank_start(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr);
void zvb_alfabank_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
void zvb_alfabank_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *kedrpou,FILE *ff_dbf_tmp,FILE *ffras);
int zarbanks(class iceb_tu_str *kod00,int *kod_ban);
int zarvbank();
void zar_pr_start(int tipz,int tabn,short mr,short gr);
int  zar_pr_start(const char *imaf_prot);
void zar_pr_ins(const char *sodzap);
void zar_pr_show(int tipz,int tabn,short mr,short gr);
void zar_pr_end();
void zvb_corp2_str(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);
int zvb_corp2_start(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr);
void zvb_corp2_end(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp);
void zarrsov(short mn,short gn,short mk,short gk,int tabnom,class iceb_zarrsov *svod_otvr,FILE *ff);

/***************Учет основных средств***************/
void uosvamot_h(int nom_lst,int *kolstr,FILE *ff);
void rasmes(short,short,double[],double,const char*,FILE*);
int diruospri(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int diruosras(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int uosvibu(int *vidu);

int dirmatot(int metkar,class iceb_tu_str *kod,class iceb_tu_str *naim,int metka_is);
void    	nsiuos(void);
void uosnd(short,short,class iceb_tu_str*);
int      diruospr(short,short,short,const char*);
void     diramort(int innom,int,int kodmo,const char*,const char*);
void     diramortby(int,int,int kodmo,const char*,const char*);
int	diros(long*,class iceb_tu_str*,int);
int	poiin(int,short,short,short,int*,int*);
int poiinpd(int in,short m,short g,class poiinpdw_data *data);
int bsiz(int in,int ceh,short dp,short mp,short gp,class bsizw_data *bal_st,FILE *ff);

int      bsizp(int in,int ceh,int kodmo,short dn,short mn,short gn,short dk,short mk,short gk,class bsiz_data *data,FILE *ff_prot);
void     bsiz23(const char*,short,short,short,double*,double*,double*,FILE*);
void     bsiz23p(const char *kodgr,short dn,short mn,short gn,short dk,short mk,short gk,class bsiz_data *data);
short    amort(int,short,short,short,short,const char*,FILE*);
short    amortby(int,short,short,short,const char*,FILE*);
void     amort23(int,int,int kodmo,short,short,const char*,FILE*);
void     prpuos(short d,short m,short g,const char *ndk,int podd,class SPISOK *spsh,class masiv_din_double *sn,class masiv_din_double *sp,short mr);

void	uosdok(short,short,short,class iceb_tu_str *nomdok);
int	puzap(int,int,short,short,short,const char*);
void	diruosdok(void);
int      vos(VVOD*,long,short,short,short,short,int,int);
void     uosndok(void);
void	sozuosvdok(VVOD*);
int      uosvdok(VVOD*,int);
void  ostuos(int in,short dn,short mn,short gn,short dk,short mk,short gk,int,int,class ostuos_data *uos_ost,FILE *ffprot);
void	uosrsd1(long);
void uosshap(short mn,short gn,short mk,short gk,struct tm *bf,int *sl,short mr,FILE *ffi,int *kst);

int diruospod(int metkar,class iceb_tu_str *kod,class iceb_tu_str *naim,int metka_isp_mo);
int dirdm(int metkar,class iceb_tu_str *kodm,class iceb_tu_str *naim);
int diruosgrnu(int metkar,class iceb_tu_str *kod_gr,class iceb_tu_str *naim_gr);
int diruosgrbu(int metkar,class iceb_tu_str *kod_gr,class iceb_tu_str *naim_gr);
int uosprovgr(const char *kodgr);
int uosprovar(const char *kodgr,int);
double skopluos(short dd,short md,short gd,const char *nomdok);
double uosgetls(int innom,short d,short m,short g,int metka_u);

/******************Услуги*********************/
int usl_pvku(int kodop_v_prov);
int prosprus(short mr,int podr,const char *datdok,const char *nn,int tp,int lnds,const char *kodop,float pnds);
int prosprus(short,int,short,short,short,const char*,int,int,const char*,float);
void podtudok(short,short,short,int,const char*,int);
int diruslop1(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int diruslop2(int metka_rr,class iceb_tu_str *kod_op,class iceb_tu_str *naim_kod);
int      dirusl(int*,int*,class iceb_tu_str*,int,const char*);
int vdnu(short *dend,short *mesd,short *godd,int *podrd,class iceb_tu_str *nomdok,int tipz);
void avtprousl(int tipz,short dd,short md,short gd,const char *kop,const char *nn,int podr,const char *kor,double sumkor,const char *nn1,float pnds,FILE*);
void avtprousl1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot);
void avtprousl1(int tipz,const char *datdok,const char *nomdok,FILE *ff_prot);

void	dirusdok(short,short,short);
void	dirudok(short,short,short,int,int,const char*);
int	uduvdok(int,short,short,short,const char*,int,int,int,int);
double	readkolu(short,short,short,int,const char*,int,int,int,int);
int	zappodu(int,short,short,short,short,short,short,const char*,int,int,double,double,const char*,const char*,int,int);
void     dirpodu(int,int);
void     podudok(short,short,short,const char*,int,int);
void podudok(const char *datdok,const char *nn,int podr,int tipz);
int	zapuvdok(int,short,short,short,const char*,int,int,double,double,const char*,const char*,int,const char*,const char*);
int zapuvdok(int tp,const char *datdok,const char *nomd,int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,int podr,const char *shet_sp,const char *dop_naim);
void	vusl(short,short,short,int,int,const char*,int,float);
void     copdoku(short,short,short,const char*,int,short);
void     rasdoku(short,short,short,int,const char*,short,short,FILE*,int,int);
int rasnalnu(short,short,short,int,const char*,const char*,short);
int	prilog(char,int,short,short,const char*,const char*,const char*);
int	usldopnal(int,const char*,float*,class iceb_tu_str *naim);
double  dok_end_usl(int tipz,short dg,short mg,short gg,const char *nomdok,int podr,int dlina,double sumkor,double itogo,float pnds,int lnds,float procent,const char *naimnal,FILE *ff);
int shet_fak_sap(short dd,short md,short gd,const char*,const char *ras_shet,const char *mfo,const char *kod,const char *naim_kontr,const char *dover,const char *herez,FILE *ff);
void shet_fak_end(FILE *ff);
int hap_act_vr(short dd,short md,short gd,const char *nomdok,const char *naim_kontr,const char *dover,const char *ispolnitel,FILE *ff);
void uslpoddok(short dd,short md,short gd,short d1,short m1,short g1,const char *nomdok,int podr,int tipz,int metka_soob);
void uslpoddok(const char *datdok,const char *datpod,const char *nomdok,int podr,int tipz,int metka_soob);
double skoplusl(short dd,short md,short gd,const char *nomdok,int podr);
int dirpodrusl(int metkar,class iceb_tu_str *kod_podr,class iceb_tu_str *naim_skl);


/**************Кассовые ордера***********************/
int dirkasop(int tipop,int metkar,class iceb_tu_str *kodm,class iceb_tu_str *naim);
int	udkasdok(const char*,const char*,short,short,short,short);
int	vkasord(int,int,char*,char*,short*,short*,short*);
int	vkasord(int tipz,int mkor,class iceb_tu_str *kassa,class iceb_tu_str *nomd,short *dd,short *md,short *gd);
void	sozvkasord(VVOD*,int);
void     dirkasords(short,short,short);
void	dirkasord(const char*,const char*,short,short,short,short);
void	provpodko(const char*,const char*,short,short,short,short);
int	pbkas(short,short);
void	poikasdok(void);
int      dirprkas(short,short,short,const char*,const char*,const char*,int,const char*);
int sumkasor(const char*,short,const char*,short,double*,double*);
int sumkasorue(const char *kassa,short tipz,const char *nomd,short god,double *sumd,double *sumpod);
void	provprkas(const char*,short,const char*,short,short,short,const char*);
int      zaphkor(int,const char*,int,short,short,short,const char*,const char*,const char*,const char*,const char*,const char*,const char*,\
const char*,const char*,short,short,short,const char*,const char*);
int	zapzkor(int,const char*,int,short,short,short,const char*,const char*,double,const char*,const char*);
int dirkcn(int metkareg,class iceb_tu_str *kod,class iceb_tu_str *naim);
void	avtprkas(const char*,short,const char*,short,short,short,const char*,const char *shetd,const char*);
int nomved_pol_nom(void );
int dirkas(int metkar,class iceb_tu_str *kodkas,class iceb_tu_str *naim);

/************ Спецификации на изделия****************/
int      dirspec(int,int*,const char*,const char*,int*,int);
int	razuz(class iceb_t_razuz_data*);
int	razuz1(class iceb_t_razuz_data*);
int	iceb_t_razuz_kod(class iceb_t_razuz_data*);
void	rasspec(int);
void	specras(int);


/********Учет командировочных расходов*******************/
int 	dirkkr(class iceb_tu_str*,class iceb_tu_str*,int);
int	vdkr(short*,short*,short*,class iceb_tu_str*);
void	dirdokkr(short,short,short,class iceb_tu_str*);
void	vvodkr1(short,short,short,const char*,const char*,const char*,int,int);
int	dirdokkrpiz(short,short,short);

/*************Учет путевых листов *************************/
void dirupld1(short,short,short,class iceb_tu_str*,class iceb_tu_str*);
int	dirout(class iceb_tu_str*,class iceb_tu_str*,int,int);
int diravt0(class iceb_tu_str*,class iceb_tu_str*,int);
int vdupl1(short *dd,short *md,short *gd,class iceb_tu_str *nomd,class iceb_tu_str *podr);
double   ostva(short,short,short,const char*,const char*,int,SPISOK*,masiv_din_double*,FILE *fil);
void     provpst(short,short,short,const char*,const char*);
void dirupldok2a_provpst(short dd,short md,short gd,const char *nomd,int podr);

double upl_pt(short dn,short mn,short gn,short dk,short mk,short gk,const char *kod_avt,const char *kod_vod);
void uplrpl_sapi(FILE *ff);
int upl_zapvdok(short dd,short md,short gd,int kp,const char *nomd,int ka,int kv,int kodsp,int tz,const char *kodtp,double kolih,const char *kom,int kap,int nz);
int diruplnst(int metkar,class iceb_tu_str *kod_nst,class iceb_tu_str *naim_nst);
void dirupldok3(short dd,short md,short gd,const char *nom_dok,int podr);
int diruplanst(int metkar,int kod_avt,class iceb_tu_str *knst);
double uplztf(short dd,short md,short gd,int podr,const char *nom_dok);
double uplztf(short dd,short md,short gd,int podr,const char *nom_dok,double *ztpn);

/****************реестр налоговых накладных******************/
int zapvree(short dd,short md,short gd,const char *nnn,double sumd,double snds,int mt,const char *inn,const char *nkontr,int mi,const char *datdi,const char *nomdi,const char *koment,const char *kodgr,const char *kodop,const char *viddok,const char *datv);
int zapvree1(short dd,short md,short gd,const char *nnn,double sumd1,double snds1,double sumd2,double snds2,double sumd3,double snds3,double sumd4,double snds4,const char *inn,const char *nkontr,int mi,const char *datdi,const char *nomdi,const char *koment,const char *kodgr,const char *kodop,short,short,short,const char *viddok);
void rpvnn_ps_p(FILE *ff_prot);
void ree_imp(int metka_tp);
int xml_nn_mat(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int,const char *vid_dok,class iceb_tu_str *if_xml);
int xml_nn_uos(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int,const char *vid_dok,class iceb_tu_str *if_xml);
int xml_nn_usl(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int,const char *vid_dok,class iceb_tu_str *if_xml);
int xml_nn_ukr(int *pnd,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int,const char *vid_dok,class iceb_tu_str *imaf_xml);
char *filtr_xml(const char *stroka);
void end_nn_xml(int metka_nds,double isuma_bnds,double sumkor,double suma_voz_tar,double suma_nds_v,double procent_nds,const char *kodop,const char *imaf_nastr,FILE *ff_xml);
void str_nn_xml(int nomer,int metka_nds,const char *datop,double kolih,double cena,double suma,const char *ei,const char *naim_mat,const char *kodtov,FILE *ff_xml);
void str_nn_xml(int nomer,int metka_nds,const char *datop,char *kolih,double cena,double suma,const char *ei,const char *naim_mat,const char *kodtov,FILE *ff_xml);
void zag_nn_xml(int tipz,const char *nomdok,short dnp,short mnp,short gnp,short dd,short md,short gd,const char *kontr,const char *uslov_prod,const char *forma_rash,FILE *ff_xml);
int open_fil_nn_xml(char *imaf_xml,int pnd,int tipz,short mn,short gn,FILE **ff_xml);
int rasrnn_r(class xrnn_poi *data,class spis_oth*);
int rnn_vs();

/******************учёт доверенностей****************************/
void dirdov(class iceb_tu_str *datadov,class iceb_tu_str *nomdov);
int udvhd(class iceb_tu_str *datdov,class iceb_tu_str *ndov);

/******************администрирование баз данных******************/
int prov_uduser(const char *user,const char *imabazp,int metka_soob);
int alxin(const char *put_alx,int metka,WINDOW *win);
void docin(const char *put_na_doc,WINDOW *win);
int check_table(class spis_oth *oth,const char *namebase);
int repair_table(class spis_oth *oth,const char *namebase);

/*----  глобальные параметры внешних устройств ----*/
extern char *cassa_port;
extern char *cassa_ps;
extern int	kassa;	/* номер раб. места */

//extern char *scanner_port;
//extern double scanner_tim;
//extern char *scale_port;
//extern double *scale_tim;
//extern int *scale_att;
/*-------------------------------------------------*/





