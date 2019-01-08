/*$Id: rnnrd5.c,v 5.40 2014/07/31 07:09:17 sasa Exp $*/
/*09.02.2016	19.01.2015	Белых А.И.	rnnrd5.c
Распечатка додатка 5
*/
#include "buhg.h"
#include "rnn_d5.h"

int rnnrd5_5(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5);
int rnnrd5_6(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5);
int rnnrd5_8(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5);

int rnnrd5(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot, /*Дополнительные отчёты*/
double proc_dot, /*Процент дотации*/
class rnn_d5 *dod5)
{
if(sravmydat(dn,mn,gn,1,12,2014) < 0)
 return(rnnrd5_5(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_svod_naim,imaf_xml_dot,imaf_svod_dot,proc_dot,dod5));
if(sravmydat(dn,mn,gn,1,12,2014) >= 0  && sravmydat(dn,mn,gn,1,1,2016) < 0)
 return(rnnrd5_6(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_svod_naim,imaf_xml_dot,imaf_svod_dot,proc_dot,dod5));
if(sravmydat(dn,mn,gn,1,1,2016) >= 0)
 return(rnnrd5_8(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_svod_naim,imaf_xml_dot,imaf_svod_dot,proc_dot,dod5));
return(1);
}
