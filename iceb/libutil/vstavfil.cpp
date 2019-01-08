/*$Id: vstavfil.c,v 1.14 2013/08/13 05:50:33 sasa Exp $*/
/*22.07.2013	14.01.2002	Белых А.И.	vstavfil.c
Подпрограмма вставки содержимого одного файла в другой
Возвращает количество вставленных строк
*/
#include        <stdio.h>
#include        <string.h>
#include        <errno.h>


int vstavfil(const char *imafil,FILE *ff)
{
FILE	*ffvs;
char	strok[2048];
int kol_v_str=0;
//printw("\nvstavfil-imaf=%s\n",imafil);
if((ffvs = fopen(imafil,"r")) == NULL)
 {
  return(0);
 }
memset(strok,'\0',sizeof(strok));
while(fgets(strok,sizeof(strok),ffvs) != NULL)
 {
  if(strok[0] == '#')
   continue;
  fprintf(ff,"%s",strok);
  kol_v_str++;
 }
fclose(ffvs);
return(kol_v_str);
}
