/*$Id: ukreksim_end.c,v 5.9 2013/05/17 14:56:13 sasa Exp $*/
/*16.07.2015	25.01.2006	Белых А.И.	ukreksim_end.c
Концовка формирования распечатки и дискеты для Укрэксимбанка
*/
#include "buhg.h"

void ukreksim_end(int nomer,
const char *imaftmp,char *imafdbf,double sumai,const char *rukov,const char *glavbuh,FILE *fftmp,FILE *ffr)
{
char strsql[512];

fputc(26, fftmp);
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(29,"Разом"),"Разом",sumai);
memset(strsql,'\0',sizeof(strsql));

preobr(sumai,strsql,0);

fprintf(ffr,"\nСумма прописом:\n%s\n",strsql);

fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);
podpis(ffr); 
fclose(fftmp);
fclose(ffr);


sprintf(imafdbf,"OPER.DBF");

ukreksim_h(imafdbf,nomer);

/*Сливаем два файла*/
iceb_t_cat(imafdbf,imaftmp);
unlink(imaftmp);
}
