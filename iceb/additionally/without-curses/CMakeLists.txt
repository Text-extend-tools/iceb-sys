#cборка iceB
message ("Построение дополнительных программ без библиотеки curses")

project(bezcurses) #имя проекта

#сборка программы для импорта данных в базу iceb_impicebf
# Установка переменной со списком исходников
set(SOURSE_BIN i_impicebf.cpp impmatdok_rf.cpp icebstart_rff.cpp icebstartf.cpp iceb_t_poldanf.cpp readkeyf.cpp iceb_t_getnkontrf.cpp iceb_t_getuidf.cpp 
    iceb_t_pndsf.cpp prshf.cpp prsh1f.cpp sql_zapisf.cpp iceb_t_localef.cpp msql_errorf.cpp readkey_parf.cpp lock_tablesf.cpp zap_prihodf.cpp nomnakf.cpp 
    nomkmf.cpp priv_k_kartf.cpp findkarf.cpp ostkarf.cpp nomkrf.cpp zapvdokf.cpp zapkarf.cpp podtdok1f.cpp prdppf.cpp readkolkf.cpp zapvkrf.cpp podvdokf.cpp 
    iceb_t_pblokf.cpp 
    avtpromu1f.cpp provshf.cpp iceb_t_avpf.cpp zapmpr1f.cpp zapmprf.cpp sumvprf.cpp prosprf.cpp nastimp.cpp 
    imp_kasord_rf.cpp iceb_t_pbpdsf.cpp zaphkorf.cpp zapzkorf.cpp avtprkasf.cpp provpodkof.cpp provprkasf.cpp iceb_t_fopenf.cpp udprgrf.cpp iceb_t_pvglknif.cpp 
    zapprovf.cpp iceb_t_getkuef.cpp error_op_nfilf.cpp iceb_t_pbpf.cpp iceb_t_pbshf.cpp iceb_t_mpodsf.cpp provprkasf.cpp sumkasoruef.cpp 
    impprov_rf.cpp 
    impusldokt_rf.cpp prosprf.cpp avtprousl1f.cpp podudokf.cpp zapuvdokf.cpp readkoluf.cpp uslpoddokf.cpp zappoduf.cpp prosprusf.cpp 
    iceb_t_poldan_vklf.cpp iceb_t_get_kkfnamef.cpp iceb_t_zkvskf.cpp)

add_executable(iceb_impicebf ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_impicebf util sql ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы которая удаляет первый символ во всех строках файла iceb_del1s
# Установка переменной со списком исходников
set(SOURSE_BIN i_del1s.cpp)

add_executable(iceb_del1s ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_del1s util ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы которая проверяет парность тегов в xml файле iceb_provteg
# Установка переменной со списком исходников
set(SOURSE_BIN i_provteg.cpp)

add_executable(iceb_provteg ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_provteg util ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками
