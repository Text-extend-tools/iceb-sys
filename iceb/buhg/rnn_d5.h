/*$Id: rnn_d5.h,v 5.7 2014/02/28 05:13:48 sasa Exp $*/
/*09.02.2016	03.04.2008	Белых А.И.	rnn_d5.h
Реквизиты для расчёта "Розшифровки податкових зобов'язань та податкового кредиту в розрізі контрагентів"
*/

class rnn_d5
 {
  public:
    /*полученные налоговые накладные - податковий кредит********************/
    class SPISOK innn1;
    class SPISOK innn_per1; /*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds1;
    class masiv_din_double suma_nds1;
    class masiv_din_double suma_nds1_7;
    class SPISOK naim_kontr1;
    double os_sr1[2]; /*Сумма по основным сдерствам*/

/***************************это уже не используется в последнем отчёте************************************************/
 
    /*Операції з придбання з податком на додану вартість, які не надають права формування
    податкового кредиту - попадают строки из реестра в которых заполнена колонка 13 и 14*/
    class SPISOK innn1d;
    class SPISOK innn_per1d; /*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds1d;
    class masiv_din_double suma_nds1d;
    class masiv_din_double suma_nds1d_7;
    class SPISOK naim_kontr1d;

    /*Операції з придбання без податку на додану вартість - Если не заполнены колонки 13 и 14 и 10 колонка равна 0. и код документа не ПНП и ПНЕ*/
    class SPISOK innn1d1;
    class SPISOK innn_per1d1; /*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds1d1;
    class masiv_din_double suma_nds1d1;
    class SPISOK naim_kontr1d1;
/********************************************************************************************************************/


    /*выданные налоговые накладные - податкові зобов'язання***************/
       
    class SPISOK innn2;
    class SPISOK innn_per2;/*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds2;
    class masiv_din_double suma_nds2;
    class masiv_din_double suma_nds2_7;
    class SPISOK naim_kontr2;
    double os_sr2[2]; /*Сумма по основным сдерствам*/




/*****************это уже не используется в последнем отчёте**************************************/
    /*операції що оподатковуються за нульовою  ставкою (рядок 2.2 декларації)*/    
    class SPISOK innn2d;
    class SPISOK innn_per2d;/*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds2d;
    class masiv_din_double suma_nds2d;
    class SPISOK naim_kontr2d;

    /*Операції які звільнені від оподаткування (рядок 5 декларації)- записи из реестра у которых заполнена колонка 11*/
    class SPISOK innn2d1;
    class SPISOK innn_per2d1;/*индивидуальный налоговый номер|месяц.год*/
    class masiv_din_double suma_bez_nds2d1;
    class masiv_din_double suma_nds2d1;
    class SPISOK naim_kontr2d1;
/*************************************************************************************/

  rnn_d5()
   {
    os_sr1[0]=os_sr1[1]=0.;
    os_sr2[0]=os_sr2[1]=0.;
   }

 };
