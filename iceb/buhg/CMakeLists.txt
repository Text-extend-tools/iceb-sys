#cборка iceB
message ("Построение бухгалтерских подсистем")

project(iceB) #имя проекта

#cmake_minimum_required(VERSION 2.8) #минимальная версия cmake

#сборка программы для системного администрирования баз данных i_admint
# Установка переменной со списком исходников
set(SOURSE_BIN i_admint.cpp 
    admin_alter.cpp admin_alter_r.cpp admin_kodkl.cpp admin_klaviat.cpp 
    admin_cp_alx.cpp admin_del_alx.cpp admin_newuser.cpp alxin.cpp 
    crtabl.cpp check_table.cpp 
    diralx.cpp dirbaz.cpp dirbuser.cpp dirbuserd.cpp dirduser.cpp docin.cpp 
    prov_uduser.cpp poiin.cpp 
    repair_table.cpp)


add_executable(iceb_admint ${SOURSE_BIN}) #сборка программы
#target_link_libraries(iceb_admint buh util sql scr ncursesw mysqlclient ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками
target_link_libraries(iceb_admint buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************


#сборка головной программы
set(SOURSE_BIN startxbu.cpp optimbaz.cpp 
    repair_table.cpp check_table.cpp) # Установка переменной со списком исходников


add_executable(iceb_start ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_start buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#сборка подсистемы "Главная книга
# Установка переменной со списком исходников
set(SOURSE_BIN xbu.cpp 
    balans_f1.cpp balans_f2.cpp 
    buhdenshet.cpp buhgsp.cpp buhhah.cpp buhkaogo.cpp buhkom.cpp buhmesshet.cpp buhnast.cpp buhpsd.cpp buhrpz.cpp buhpnk.cpp 
    buhrznp.cpp buhsdkr.cpp buhvdz.cpp buhspk.cpp buh_hps.cpp buh_kaukz.cpp buh_kaufz.cpp buh_kekv.cpp buh_prov_shet.cpp 
    buh_rh.cpp buh_rnr_sh.cpp 
    clprov.cpp
    dirprov.cpp dirsaldo.cpp dirsval.cpp dirkv.cpp 
    findprov.cpp 
    glkni.cpp glkninbs.cpp gosrsn.cpp gosss.cpp gurornbs.cpp gurornrs.cpp gurorr.cpp
    gurorrp.cpp guritog.cpp 
    hdisk.cpp hrvnks.cpp 
    impprov.cpp impprov_r.cpp 
    korprov.cpp korprovnbs.cpp 
    mord1.cpp mord4.cpp mord5.cpp mord6.cpp mord9.cpp mord12.cpp 
    ostkar.cpp ostuos.cpp
    pdokmu.cpp pdokko.cpp pdokusl.cpp pdokpd.cpp pdokuos.cpp pdokzp.cpp pdokukr.cpp pib_itl.cpp pib_end_dok.cpp pib_str.cpp prvkp.cpp provkont.cpp provtime.cpp prpp.cpp poiin.cpp poiinpd.cpp 
    poiprov.cpp pvbanks.cpp pvbanks_kprov.cpp 
    raspitog.cpp rasprov.cpp rozden.cpp rpshet_ss.cpp rpshet_rs.cpp rpsrs.cpp 
    saldogk.cpp saldonbs.cpp sapgors.cpp salallk.cpp 
    ukreksim_hap.cpp ukreksim_end.cpp ukreksim_zstr.cpp ukreksim_h.cpp 
    ukrsib_start.cpp ukrsib_end.cpp ukrsib_str.cpp 
    vipiska.cpp vprov.cpp vprovgr.cpp vprovnbs.cpp 
    xbudkz.cpp 
    zarbanks.cpp 
    zvb_privatbank_start.cpp zvb_privatbank_str.cpp zvb_privatbank_end.cpp 
    zvb_hreshatik_start.cpp zvb_hreshatik_str.cpp zvb_hreshatik_end.cpp 
    zvb_ibank2ua_start.cpp zvb_ibank2ua_str.cpp zvb_ibank2ua_end.cpp 
    zvb_ukrgaz_end.cpp  zvb_ukrgaz_start.cpp zvb_ukrgaz_str.cpp 
    zvb_kievrus_start.cpp zvb_kievrus_str.cpp zvb_kievrus_end.cpp 
    zvb_corp2_start.cpp zvb_corp2_str.cpp zvb_corp2_end.cpp) 


add_executable(iceb_xbu ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xbu buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**********************************************************************************

#сборка подсистемы "Материальный учёт"
# Установка переменной со списком исходников
set(SOURSE_BIN xmu.cpp 
    avtpromu.cpp avtpromu1.cpp 
    cennik.cpp cennikd.cpp cennikm.cpp copdok.cpp
    dok_dover.cpp dok_end_mu.cpp dooc.cpp doocr.cpp doocsum.cpp dirdok.cpp dirmatr.cpp dirmusl.cpp 
    dirnalog.cpp diropld.cpp dirprmu.cpp dirsklad.cpp dirspec.cpp dirtab1.cpp dirusl.cpp dirzkart.cpp dirrashod.cpp dvmat.cpp dvtmcf1.cpp dvtmcf2.cpp dvusl_mu.cpp
    dirkas.cpp dirkasop.cpp dirsval.cpp dirprihod.cpp dirdokummat4.cpp dirsnrm.cpp
    eksmu.cpp eks_dok_mu.cpp 
    findkar.cpp 
    hap_act_vr.cpp hrasact.cpp 
    iceb_t_razuz_kod.cpp impmatdok.cpp impmatdok_r.cpp impmatdok_xml_rt.cpp impmatdok_xml_mt.cpp impmatost.cpp in_d12_it.cpp in_d12_start.cpp
    koncinv.cpp kormat.cpp korsdok.cpp korznkr.cpp krasact.cpp kriost.cpp 
    makkor.cpp makkorr.cpp matnast.cpp 
    matudd.cpp matuddok.cpp matud.cpp matulk.cpp mat_act_vr.cpp menuvnp.cpp mu_pvku.cpp
    mudmc.cpp muinv.cpp musspi.cpp mumo.cpp muodk.cpp muraskod.cpp murfn.cpp mu_shet_fak.cpp muso.cpp mugetdpp.cpp muprih.cpp 
    nalndoh.cpp nomkr.cpp nomkm.cpp
    oplmu.cpp opss.cpp ostdok1.cpp ostdok.cpp ostkar.cpp ostkar1.cpp ost_skl_shet.cpp 
    perecen.cpp perecenp.cpp podtdok.cpp podtdok1.cpp podvdok.cpp poimatdok.cpp praisl.cpp 
    prcn.cpp prdpp.cpp prihod.cpp priv_k_kart.cpp prognoz.cpp prospr.cpp provkol.cpp provnds.cpp provudvdok.cpp 
    rasactmu.cpp rasalk.cpp rasdm20.cpp rasdok2.cpp rasttn.cpp rasttn1.cpp rasttn2.cpp rasdok1.cpp rasdok.cpp rasdoksp.cpp rasgrup.cpp rashap.cpp rashod.cpp rasizdel.cpp 
    rasmat.cpp rasmedpr.cpp rasnaln.cpp rasnds.cpp raspril.cpp rasnak.cpp rasmushet.cpp rasdokkr.cpp
    rasosdk.cpp rasoskr.cpp rasprop.cpp rasspdok.cpp rasspec.cpp raszkar.cpp razuz.cpp razuz1.cpp readdok.cpp 
    readkolk.cpp rozkor.cpp rozsvod.cpp rozvse.cpp rppvo1.cpp rppvo.cpp 
    rscena.cpp 
    saldomu.cpp sapkkiv.cpp sapkk1iv.cpp sapvin.cpp shet_fak_end.cpp shet_fak_sap.cpp skoplmu.cpp smdok.cpp sortdokmu.cpp specras.cpp spis_p_kontr.cpp 
    spiz.cpp sprozn.cpp srvcen.cpp srvcenr.cpp sumdok.cpp sumzap1.cpp sumzap1u.cpp sumzapuos.cpp
    udkar.cpp 
    vdnd.cpp vivost.cpp vmat.cpp vvoddok.cpp 
    xmurspd.cpp xml_nn_mat.cpp 
    zapdok.cpp zaphkor.cpp zapkar.cpp zapvdok.cpp zapvkr.cpp zapzkor.cpp zap_prihod.cpp
    zap_s_mudok.cpp)


add_executable(iceb_xmu ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xmu buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************
#сборка подсистемы "Платёжные документы"
# Установка переменной со списком исходников
set(SOURSE_BIN xdk.cpp 
    avtprpld.cpp
    baval.cpp bnadra.cpp bukrinbank.cpp b_brokbiznes.cpp b_b2_bank_on_line.cpp b_soft_review.cpp b_iBank_2_UA.cpp b_pervinv.cpp b_privat.cpp 
    b_akb_imexbank.cpp b_privatinvest.cpp b_skarb.cpp b_tiny.cpp b_ukrprom.cpp b_ukrbiznes.cpp b_ukrexim.cpp b_ukrsoc.cpp b_corp2.cpp 
    dirbank.cpp dirpzpd.cpp dirprpd.cpp dirobq.cpp dirtipd.cpp dirtab1.cpp dok_dover.cpp 
    nalish.cpp nastdk.cpp 
    paspddos.cpp pgpld.cpp platp.cpp platdok.cpp platdok_v.cpp pld.cpp provpd.cpp pvbanks_kprov.cpp 
    peshetn.cpp poiplatdok.cpp 
    rasobqv.cpp readkom.cpp readpdok.cpp
    smpld.cpp sumprpd.cpp sumpzpd.cpp
    vibshbn.cpp vzrkbz.cpp vzrkf.cpp 
    udpld.cpp udpdok.cpp 
    xdkbanks.cpp xdkplc.cpp xdkpg.cpp 
    zappdok.cpp)


add_executable(iceb_xdk ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xdk buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Заработная плата"
# Установка переменной со списком исходников
set(SOURSE_BIN xzp.cpp 
    alim.cpp avtprozar.cpp 
    boln1.cpp 
    clearzar.cpp creatheaddbf.cpp 
    denrog.cpp dirprzar.cpp diresnf.cpp dirf8dr.cpp dirnu.cpp dirsdf8dr.cpp dirf1dfvs.cpp dirsoc.cpp dirtabel.cpp dirzarmzpm.cpp dirzvan.cpp dirzarrud.cpp dirzarlgot.cpp dirzargr.cpp 
    dirzarsdo.cpp dirzarpodr.cpp
    dirkkr.cpp dirsb.cpp dirtabelo.cpp dirtab.cpp dirzardk.cpp dirzardkz.cpp dirzarkh.cpp dirzarp.cpp dirzarsl.cpp dirzarsoc.cpp dirnach.cpp diruder.cpp dirzarskrdh.cpp dirzarsvd.cpp 
    dirkas.cpp dirkasop.cpp dirsval.cpp 
    dopdomin.cpp dolhnn.cpp dolizs.cpp 
    f1df_xml_kon.cpp f1df_xml_str.cpp f1df_xml_zag.cpp findnahm.cpp findnah.cpp
    hdisk.cpp hrvnks.cpp 
    imp_ks.cpp impotvr.cpp impzarnu.cpp impzkart.cpp impzkartf.cpp imp_zardok.cpp imp_zar_kr.cpp itog.cpp itogiz.cpp 
    kategor.cpp kateg.cpp kategsh.cpp knnfnds.cpp koncf8dr.cpp konrz.cpp koresh.cpp krdvp.cpp kredit.cpp 
    nahpodr.cpp nahud.cpp  
    oklad.cpp okladtr.cpp otrshd.cpp otrsh.cpp
    peh1.cpp pib_itl.cpp pib_end_dok.cpp pib_str.cpp podoh1.cpp podohrs2016.cpp poifl.cpp poizardok.cpp prbper1.cpp 
    prbper.cpp procent.cpp profs.cpp promes.cpp provblokzarp.cpp
    provkod.cpp provzzag.cpp
    rasdolgi1.cpp rasdolgi2.cpp rasdolgi.cpp 
    rasf8dr.cpp rasnash.cpp rasnu.cpp rasnuptn1.cpp rasnuptn.cpp raspol.cpp
    rasshetz.cpp raszall.cpp raszallr.cpp raszpodr.cpp raszpodrr.cpp 
    rastab.cpp rassocp.cpp rasved.cpp raszkart.cpp ras_podoh1.cpp read_nast_ind.cpp redkal.cpp reestal.cpp 
    rspis.cpp
    rudnfv.cpp 
    saldozr.cpp socstr.cpp socstrm.cpp sosnsum.cpp snu.cpp spdfo.cpp spiin.cpp 
    spininu.cpp spkvd.cpp sprmatpom.cpp sprtab.cpp sprtabotp_m.cpp sspp1.cpp 
    strfil.cpp sumprz.cpp sumprved.cpp svodnu.cpp
    tabel.cpp tabnom.cpp
    ukreksim_end.cpp ukreksim_hap.cpp ukreksim_h.cpp ukreksim_zstr.cpp 
    ukrsib_start.cpp ukrsib_end.cpp ukrsib_str.cpp 
    vedprz1.cpp viknu.cpp vkart1.cpp vkart.cpp vnu.cpp 
    vvodf8dr.cpp vztb.cpp vvodztab.cpp
    zardzn.cpp zarkvrt_zapvtabl.cpp zagf1df.cpp zagf8dr1.cpp zapf8dr.cpp zagf1df_csv.cpp zaphkor.cpp zapkrtb.cpp zaptabel.cpp zarpd.cpp zarpv.cpp zarpdoh.cpp zapzarn1.cpp zapzarn.cpp 
    zapzagot.cpp zapzardk.cpp zapzarp.cpp zapzarpv.cpp zapzkor.cpp zaravpr.cpp zaravpr_old.cpp zaravpt_os.cpp zarbanks.cpp zarboln.cpp zarindex.cpp zarindexv.cpp 
    zaresvo.cpp zarkart.cpp zarkdpen.cpp zarkmd.cpp zarkvrt1.cpp zarkvrt1_kr.cpp zarkvrt_gk.cpp zarlgot.cpp zarnast.cpp zarnrn.cpp zarnvp.cpp zarpensm1.cpp zarpodpr.cpp zarppn.cpp zarppn1.cpp zarprtn.cpp zarrasp.cpp zarrasp2.cpp zarraspr.cpp zarreadn.cpp 
    zarsdf1df.cpp zarsf1df.cpp zarsbka.cpp zarkvrt_gk.cpp zarsoc.cpp zarsdrv.cpp zarbol.cpp zarzapkk.cpp 
    zarsald.cpp zarsbsl.cpp zarsend.cpp zarspek.cpp zarspek1.cpp zarsprpr.cpp zarsrsl.cpp zarvds.cpp
    zarsprvmn.cpp zarsrbol.cpp zarsrbold.cpp zarstfn53.cpp zarstomn.cpp zarsnpm.cpp zarsnpsmr.cpp 
    zarstom.cpp zarstved.cpp zartarroz.cpp zartarroz1.cpp zartarrozrs.cpp zarudkar.cpp zarudnu.cpp zaruoz.cpp zarvpn.cpp zar_mpsl.cpp zar_rp.cpp zar_sdpf.cpp zarszk.cpp zvanie.cpp 
    zar_read_tn1.cpp zarrnesv.cpp zar_pr_start.cpp zar_pr_ins.cpp zar_pr_show.cpp zar_pr_end.cpp zargetfio.cpp 
    zar_sumtn.cpp zarsn.cpp zarvn.cpp zarrsov.cpp 
    zarvbank.cpp 
    zvb_privatbank.cpp zvb_privatbank_start.cpp zvb_privatbank_str.cpp zvb_privatbank_end.cpp 
    zvb_alfabank.cpp zvb_alfabank_start.cpp zvb_alfabank_end.cpp zvb_alfabank_str.cpp 
    zvb_hreshatik.cpp zvb_hreshatik_end.cpp zvb_hreshatik_start.cpp zvb_hreshatik_str.cpp 
    zvb_ibank2ua.cpp zvb_ibank2ua_start.cpp zvb_ibank2ua_end.cpp zvb_ibank2ua_str.cpp 
    zvb_kreditprom.cpp zvb_nadra.cpp 
    zvb_pib.cpp zvb_ukreksim.cpp zvb_ukrprom.cpp zvb_ukrprom_m.cpp zvb_ukrsoc.cpp zvb_ukrsib.cpp 
    zvb_promekonom.cpp zvb_promekonom_end.cpp zvb_promekonom_start.cpp zvb_promekonom_str.cpp 
    zvb_ukrgaz.cpp zvb_ukrgaz_end.cpp zvb_ukrgaz_start.cpp zvb_ukrgaz_str.cpp 
    zvb_kievrus.cpp zvb_kievrus_start.cpp zvb_kievrus_str.cpp zvb_kievrus_end.cpp 
    zvb_corp2.cpp zvb_corp2_start.cpp zvb_corp2_str.cpp
    zvb_corp2_end.cpp) 


add_executable(iceb_xzp ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xzp buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт основных средств"
# Установка переменной со списком исходников
set(SOURSE_BIN xuos.cpp 
    amort23.cpp amort.cpp amortby.cpp avtprouos.cpp 
    bsiz23.cpp bsiz23p.cpp bsiz.cpp bsizp.cpp
    diramort.cpp diramortby.cpp dirdm.cpp dirindm.cpp dirmatot.cpp diropld.cpp diros.cpp dirsklad.cpp diruosdok.cpp 
    diruosgrbu.cpp diruosgrnu.cpp diruospod.cpp diruospr.cpp diruosls.cpp dirrashod.cpp
    diruospri.cpp diruosras.cpp dvos.cpp 
    eksuos.cpp 
    hrasact.cpp 
    impuosdok.cpp impuosost.cpp in_d12_it.cpp in_d12_start.cpp
    krasact.cpp koncinv.cpp 
    menuvnp.cpp 
    opluos.cpp ostuos.cpp ostkar.cpp
    poduosd.cpp poiin.cpp poiinpd.cpp poiuosdok.cpp provuin.cpp
    prpuos.cpp pudok.cpp puzap.cpp 
    rasactuos.cpp rasamort.cpp rasamortnu.cpp rasamortby.cpp rasdopuos.cpp rasnalnuos.cpp rasnas.cpp rasdokuos.cpp
    sapkkiv.cpp sapkk1iv.cpp skopluos.cpp smenakof.cpp sumzapuos.cpp 
    udamort.cpp udinn.cpp uduosdok.cpp uduoszd.cpp 
    uosact.cpp uosavve.cpp uosbd.cpp uosdok.cpp uosdoo.cpp uosdvos.cpp uosgetls.cpp uosinved.cpp uosinu.cpp uosnast.cpp 
    uosndok.cpp uosplc.cpp uospin.cpp uosprovar.cpp uosprovgr.cpp uosrbs.cpp uosrp.cpp uosrp1.cpp uosrem.cpp uosrsd1.cpp uosrsd.cpp uosrs.cpp uossin.cpp uossas.cpp 
    uossos.cpp uosvamot.cpp uosvamot_h.cpp uosvamotby.cpp uosvdok.cpp uosvos.cpp uosvosii.cpp uosvibo.cpp uos_fomtr.cpp uos_otvm.cpp uos_sp.cpp uospos.cpp uosoik.cpp uosisos.cpp uosdi.cpp uosoz6.cpp 
    uosvibu.cpp uosvvd.cpp uospzp.cpp uosrapp.cpp uosavp.cpp uosik818.cpp uosasoz818.cpp uosastz818.cpp uosavve818.cpp
    vos.cpp 
    xml_nn_uos.cpp 
    zap_s_mudok.cpp zapvdok.cpp)


add_executable(iceb_xuos ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xuos buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт услуг"
# Установка переменной со списком исходников
set(SOURSE_BIN xusl.cpp 
    avtprousl.cpp avtprousl1.cpp 
    cennik.cpp cennikm.cpp copdoku.cpp
    dirmatr1.cpp diropld.cpp dirpodrusl.cpp dirpodu.cpp dirprus.cpp dirsklad.cpp dirusl.cpp dirusdok.cpp dirudok.cpp dirrashod.cpp diruslop1.cpp diruslop2.cpp dok_end_usl.cpp dvmat.cpp dvuslf2.cpp
    dirkas.cpp dirkasop.cpp dirsval.cpp dirprihod.cpp 
    findkar.cpp 
    hap_act_vr.cpp 
    impusldok.cpp impusldokt_r.cpp impusldok_xml_r.cpp impusldok_xml_m.cpp 
    makkor.cpp makkorr.cpp menuvnp.cpp 
    nalndoh.cpp nomkr.cpp nomkm.cpp
    opss_u.cpp ostdok1.cpp ostkar.cpp 
    praisl.cpp prdpp.cpp prosprus.cpp prilog.cpp priv_k_kart.cpp perecenu.cpp podtdok1.cpp podudok.cpp podvdok.cpp poiusldok.cpp
    rasdoku.cpp rasnalnu.cpp readkolk.cpp readkolu.cpp rasspdoku.cpp 
    rppvou.cpp rppvou1.cpp 
    shet_fak_end.cpp shet_fak_sap.cpp skoplusl.cpp sumzap1.cpp sumzap1u.cpp sumzapuos.cpp 
    usldupk.cpp 
    vdnu.cpp vusl.cpp
    uddusl.cpp uduvdok.cpp usldopnal.cpp uslnast.cpp uslpoddok.cpp usl_act_vr.cpp usl_sm.cpp usl_shet_fak.cpp uslspo.cpp uslgetsd.cpp 
    usl_pvku.cpp
    xuslplc.cpp xml_nn_usl.cpp 
    zapkar.cpp zappodu.cpp zapuvdok.cpp zapvdok.cpp zapvkr.cpp zaphkor.cpp zapzkor.cpp zap_prihod.cpp
    zap_s_mudok.cpp)


add_executable(iceb_xusl ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xusl buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт кассовых ордеров"
# Установка переменной со списком исходников
set(SOURSE_BIN xkas.cpp 
    avtprkas.cpp
    dirkasord.cpp dirkasords.cpp dirkcn.cpp dirkcnsl.cpp dirnomved.cpp dirprkas.cpp dirkas.cpp dirsval.cpp dirkasop.cpp 
    imp_kasord.cpp imp_kasord_r.cpp itog.cpp 
    kasnast.cpp kasoth.cpp kasothcn.cpp kasothr.cpp kasothr1.cpp kasgods.cpp 
    pbkas.cpp provpodko.cpp provprkas.cpp poikasdok.cpp
    saldokaskcn.cpp sspp1.cpp sumkasor.cpp sumkasorue.cpp sumprved.cpp 
    vkasord.cpp 
    udkasdok.cpp udkasordg.cpp 
    xkasplc.cpp 
    zaphkor.cpp zapzkor.cpp zarstfn53.cpp zarstved.cpp)


add_executable(iceb_xkas ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xkas buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт командировочных расходов"
# Установка переменной со списком исходников
set(SOURSE_BIN xukr.cpp 
    avtprukr.cpp 
    dirdokkr.cpp dirdokskr.cpp dirkkr.cpp dirprkr.cpp 
    kprik.cpp 
    poikrdok.cpp prosprkr.cpp 
    rppvokr.cpp rppvokr1.cpp 
    ukrku.cpp ukrnast.cpp ukrras.cpp ukrskk.cpp ukrspdok.cpp ukrspdok1.cpp ukrplc.cpp ukrudgod.cpp ukrgods.cpp 
    vdkr.cpp vvodkr.cpp vvodkr1.cpp 
    xukropp.cpp)


add_executable(iceb_xukr ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xukr buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт Учёт путевых листов"
# Установка переменной со списком исходников
set(SOURSE_BIN xupl.cpp 
    diravt0.cpp dirout.cpp dirscmt.cpp dirsklad.cpp dirupld1.cpp diruplds.cpp dirupls.cpp diruplnst.cpp dirupldok3.cpp diruplanst.cpp dirupldok2a.cpp 
    ostva.cpp 
    poiupldok.cpp 
    saldo_upl.cpp 
    upl_obvm.cpp upl_obv.cpp upl_plc.cpp upl_pt.cpp upl_sp_top.cpp upl_zapvdok.cpp uplovdt.cpp uplvstpsh.cpp 
    uplnast.cpp uplnomdok.cpp uplrpl.cpp uplrpl_r.cpp uplrpla_r.cpp uplvst_kg.cpp uplztf.cpp 
    upludgod.cpp uplgods.cpp 
    vdupl1.cpp vvodtop.cpp)


add_executable(iceb_xupl ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xupl buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Реестр налоговых накладных"
# Установка переменной со списком исходников
set(SOURSE_BIN xrnn.cpp 
    dirrnn.cpp dirrnn1.cpp dirrashod.cpp diruosras.cpp diruslop1.cpp diruslop2.cpp dirprihod.cpp 
    ree_imp.cpp ree_nast.cpp 
    rasrnn.cpp rasrnn_p.cpp rasrnn_p7.cpp rasrnn_p8.cpp rasrnn_v.cpp rasrnn_v7.cpp rasrnn_v8.cpp 
    rnngods.cpp rnnovd.cpp rnnrd5.cpp rnnrd5_5.cpp rnnrd5_6.cpp rnnrd5_8.cpp rnn_vs.cpp rnn_rkontr.cpp rpvnn_ps_p.cpp rpvnn_pi.cpp rpvnn_vi.cpp 
    sumzap1.cpp sumzap1u.cpp sumzapuos.cpp 
    xml_nn_mat.cpp xml_nn_ukr.cpp xml_nn_uos.cpp xml_nn_usl.cpp 
    zapvree.cpp zapvree1.cpp)


add_executable(iceb_xrnn ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xrnn buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт доверенностей"
# Установка переменной со списком исходников
set(SOURSE_BIN xud.cpp 
    dirdov.cpp dirdovs.cpp dirtab1.cpp 
    poidov.cpp 
    udvhd.cpp 
    xudrasdok.cpp xududgod.cpp xudgods.cpp)


add_executable(iceb_xud ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xud buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка программы ввода спецификаций
# Установка переменной со списком исходников
set(SOURSE_BIN spec.cpp 
    cennik.cpp cennikm.cpp 
    dirmatr.cpp dirsklad.cpp dirspec.cpp dirusl.cpp dirzkart.cpp dvmat.cpp dirrashod.cpp dirprihod.cpp 
    findkar.cpp iceb_t_razuz_kod.cpp kormat.cpp 
    muraskod.cpp mugetdpp.cpp mu_pvku.cpp
    ostdok.cpp ostkar.cpp ostkar1.cpp ostdok1.cpp
    podtdok1.cpp prihod.cpp praisl.cpp podvdok.cpp prospr.cpp
    prdpp.cpp
    rashod.cpp rscena.cpp readkolk.cpp rasizdel.cpp rasspec.cpp raszkar.cpp 
    razuz.cpp razuz1.cpp
    nomkr.cpp nomkm.cpp
    specras.cpp specnast.cpp spiz.cpp 
    vmat.cpp vivost.cpp
    zap_s_mudok.cpp zapvdok.cpp zapkar.cpp zapvkr.cpp)


add_executable(iceb_spec ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_spec buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка программы списания розницы
# Установка переменной со списком исходников
set(SOURSE_BIN fsprozn.cpp 
    findkar.cpp 
    iceb_t_razuz_kod.cpp 
    ostkar.cpp 
    matnast.cpp 
    prdpp.cpp priv_k_kart.cpp
    razuz.cpp razuz1.cpp readkolk.cpp
    podtdok1.cpp podvdok.cpp 
    sprozn.cpp 
    zapvdok.cpp zapvkr.cpp zap_s_mudok.cpp)


add_executable(iceb_fsprozn ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_fsprozn buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка программы выписки накладных не бухгалтером
# Установка переменной со списком исходников
set(SOURSE_BIN xpro.cpp 
    cennik.cpp cennikm.cpp 
    dirdok1.cpp dirmatr1.cpp dirobq.cpp dirsklad.cpp dok_end_mu.cpp dirrashod.cpp dirprihod.cpp 
    dvden.cpp dvmat.cpp
    errorkas.cpp
    korsdok.cpp korznkr.cpp
    mariqtxt.cpp mariqvv.cpp marsiv.cpp matnast.cpp matudd.cpp makkor.cpp 
    makkorr.cpp matud.cpp mu_pvku.cpp
    nalish.cpp nalndoh.cpp nastrmariq.cpp
    ostkar.cpp ostdok1.cpp
    perecen.cpp praisl.cpp poimatdok1.cpp podvdok.cpp prospr.cpp
    printdayshek.cpp printpershek.cpp
    rasdok.cpp rasobqv.cpp rasnaln.cpp rasdok2.cpp rasdm20.cpp 
    rasshek.cpp rasosmat.cpp rasspdok.cpp rasdokkr.cpp readkolk.cpp readdok.cpp
    settimmar.cpp smdok1.cpp sortdokmu.cpp sumdok.cpp
    udkar.cpp
    vdnd.cpp vmat2.cpp
    zapdok.cpp zapvdok.cpp zap_s_mudok.cpp)


add_executable(iceb_xpro ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xpro buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************

#сборка программы выписки накладных не бухгалтером
# Установка переменной со списком исходников
set(SOURSE_BIN rozn.cpp 
    cennik.cpp cennikm.cpp 
    dirmatr1.cpp dvmat.cpp dirsklad.cpp 
    errorkas.cpp 
    iceb_t_razuz_kod.cpp 
    nastrmariq.cpp
    razuz.cpp razuz1.cpp rozprod.cpp rozvse.cpp rozsvod.cpp 
    matnast.cpp 
    ostdok1.cpp praisl.cpp ostkar.cpp 
    ostdok.cpp nomkr.cpp 
    zapkar.cpp zapvdok.cpp zapvkr.cpp vivost.cpp podvdok.cpp 
    prdpp.cpp readkolk.cpp prospr.cpp
    tovshek.cpp)


add_executable(iceb_rozn ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_rozn buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#****************************************************************
