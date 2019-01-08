/* $Id: util.h,v 5.122 2013/11/24 08:23:08 sasa Exp $ */
/*13.02.2017	04.03.1999	Белых А.И.	util.h
Описание подпрограмм утилит
*/
//#include <scr.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libintl.h>
#include <unistd.h>
#include "masiv.h"
#include "spisok.h"
#include "iceb_tu_str.h"
#include "iceb_tu_int.h"
#include "iceb_tu_file.h"

int 	vstavfil(const char*,FILE*);
void	kodkontr(class iceb_tu_str*,class iceb_tu_str*,const char*,const char*);
double	ATOFM(const char*);
int	dirmak1(char*,char*,int,int);
int	dirmak(char *name,char *proekt);
void	dpm(short*,short*,short*,short);
void	mesc(short,short,char*);
void	mesc(short,short,class iceb_tu_str*);
double	okrug(double,double);
short	period(short,short,short,short,short,short,short);
int	POLE(const char*,char*,int,char);
int	polen(char const*,char*,int,int,char);
int	polen(char const*,class SPISOK*,int,char);
int	polen(char const*,class masiv_din_int*,int,char);
int	polen(char const*,int*,int,char);
int	polen(char const*,short*,int,char);
int	polen(char const*,long*,int,char);
int	polen(char const*,double*,int,char);
int	polen(char const*,float*,int,char);
int	polen(char const*,class iceb_tu_str*,int,char);
short	pole1(const char*,const char*,char,short,int*);
int	pole2(const char*,char);
int	pole3(const char*,char*,int,char);
int	pole3(const char*,class iceb_tu_str*,int,char);
short	pole4(char*,const char*,char);
void	preobr(double,char*,short);
void	preobr(double,class iceb_tu_str*,short);
void	prnb(double,char*);
void	prnb(double,class iceb_tu_str*);
void	prnb1(double,char*);
char	*prnbr(double);
int	rsdat(short*,short*,short*,const char*,int);
short	rsdat1(short*,short*,const char*);
int	SRAV(const char*,const char*,int);
int	SRAV1(short,short,short,short,short,short);
int	strstrm(const char*,const char*);
int	FGETSS(char*,int,FILE*);
char	*prcn(double);
int	runsystem1(const char*,int);
int	readkey(int,char**,char**,char**,char**,int*,FILE*);
void	denrog(short*,short*,short*,int);
void	sozmas(short**,char*);
void	sozmas(short**,char*,FILE*);
int	sravmydat(short,short,short,short,short,short);
int	sravmydat(const char *data1,const char *data2);
int	sravmydat(const char *data1,const char *data2,int metka);
int 	proverka(const char*,const char*,int,int);
int 	proverka(const char*,int,int,int);
void	zagolov(const char*,const char *datan,const char *datak,const char*,FILE*);
void	zagolov(const char*,struct tm*,short,short,short,short,short,short,const char*,FILE*);
void	zagolov(const char*,short,short,short,short,short,short,const char*,FILE*);
void	startfil(FILE*);
char	*workid();
int      rstime(short*,short*,short*,const char*);
void poltekdat(short *dt,short *mt,short *gt);
const char *poltekdat();
int kol_str(int max_kol_std_na_liste,int ras_kol_str);
int kol_str_l(int max_kol_std_na_liste,int ras_kol_str);
void    ud_lp(char *stroka);
void    ud_simv(char *stroka,const char*);
void iceb_tu_ekrs(char *stroka,int razmer,char ekran,const char *simv);
void iceb_tu_z_plus(char *stroka_in,int dlinna,char *stroka_plus);
void iceb_tu_tolower_str(char*);
void iceb_tu_toupper_str(char*);
int  iceb_tu_srav_r(const char*,const char*,int);
void iceb_tu_fplus(const char *stroka,class iceb_tu_str *stroka1,FILE *ff);
void iceb_tu_fplus(int metka_sp,const char *stroka,class SPISOK *stroka1,FILE *ff);
const char *iceb_tu_datzap(const char *data);
const char *iceb_tu_datzap_mg(const char *data);
void iceb_tu_fio(const char *fio,class iceb_tu_str *fio_in);

int iceb_tu_zvstr(class iceb_tu_str *stroka,const char *obr,const char *zamena);
void iceb_tu_zvstrp(char *stroka,int poz_zam,const char *vstr,int dlinna_str);

void iceb_tu_cpbstr(char *strin,int dlstrin,const char *strout,const char *udstr);
void iceb_tu_cpbstr(class iceb_tu_str *strin,const char *strout,const char *udstr);
void iceb_tu_cpbstr(class SPISOK *strin,const char *strout,const char *udstr);
int iceb_tu_ustpeh(int dlinna_stroki,int orient,FILE *ff);
int iceb_tu_cp(const char *imaf_out,const char *imaf_in);
int iceb_tu_cat(const char *imaf1,const char *imaf2);
const char *iceb_tu_adrsimv(int nomer_simv,const char *str);

int iceb_tu_kolbait(int kolih_simv,const char *str);
int iceb_tu_kolsimv(int kolbait_str,const char *stroka);

int iceb_tu_strlen(const char *stroka);
int poldan(const char *obr,char *str,const char *imaf);
int poldan(const char *obr,class iceb_tu_str *str,const char *imaf);
void iceb_tu_udsimv(char *stroka,int nomer_us);
void iceb_tu_vsimv(char *stroka,int dlinastr,int poz,const char *kodsimv);
void iceb_tu_zsimv(char *stroka,int dlinastr,int poz,const char *simvol);
//const char *iceb_tu_dattosql(const char *data);
const char *iceb_tu_vremzap(char *vrem);
const char *iceb_tu_vremzap(time_t vrem);
const char *iceb_tu_kszap(char *kto);
const char *iceb_tu_kszap(int kto);
const char *iceb_tu_getlogin();
const char *iceb_tu_sqlfiltr(const char *stroka);
const char *iceb_tu_filtr_xml(const char *stroka);
const char *iceb_tu_filtr_from_xml(const char *stroka);
const char *iceb_tu_nofwords(double number);
const char *iceb_tu_propis(double hs,int metka_1s);

void iceb_tu_vstav(class iceb_tu_str *stroka,int znah,int poz1,int poz2,int hag);
void iceb_tu_vstav(class iceb_tu_str *stroka,const char *strin,int poz1,int poz2,int hag);
void iceb_tu_vstav(char *stroka,const char *strin,int poz1,int poz2,int hag,int kolbait_stroka);
void iceb_tu_vstav(char *stroka,int strin,int poz1,int poz2,int hag,int kolbait_stroka);

const char *iceb_tu_tosqldat(const char *data);
void iceb_tu_strspisok(const char *stroka,int razdelitel,class SPISOK *spisok);
const char *iceb_tu_getcurrentdir();
int iceb_tu_fgets(class iceb_tu_str *str,FILE *ff);
const char *iceb_tu_double_to_char_zp(double suma,int drob);
const char *iceb_tu_tipz(const char *pm);
const char *iceb_tu_getfiosin(const char *fio,int metka);

