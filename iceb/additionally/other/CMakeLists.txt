#cборка iceB
message ("Построение дополнительных программ")

project(other) #имя проекта

#сборка программы ввода нарядов для завода "Форт"
# Установка переменной со списком исходников
set(SOURSE_BIN fort.cpp fort_vhn.cpp fort_an.cpp fort_anz.cpp fort_rz.cpp dirceh.cpp 
    dirfortti.cpp dirmatr2.cpp dirfortnp.cpp dirfortnpz.cpp fort_rnp.cpp)

add_executable(iceb_fort ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_fort buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы разметки нужных таблиц в базе для программы fort для завода "Форт"
# Установка переменной со списком исходников
set(SOURSE_BIN fort_cb.cpp)

add_executable(iceb_fort_cb ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_fort_cb buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы расчёта премии для "Госстандарта"
# Установка переменной со списком исходников
set(SOURSE_BIN vcss.cpp zar_read_tn.cpp oklad.cpp zarnast.cpp zarrnesv.cpp okladtr.cpp zar_read_tn1.cpp zar_pr_ins.cpp zartarroz.cpp zar_pr_start.cpp zar_pr_end.cpp 
    ../../buhg/dirzarpodr.cpp)

add_executable(iceb_vcss ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_vcss buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы расчёта платежей за общежитие для коледжа "НУХТ"
# Установка переменной со списком исходников
set(SOURSE_BIN xkus.cpp 
    kusnast.cpp raskus.cpp dirsk.cpp dirpmd.cpp zapuvdok.cpp uslpoddok.cpp podudok.cpp prosprus.cpp avtprousl1.cpp karta.cpp energy.cpp readkolu.cpp zappodu.cpp
    dirlg.cpp ../../buhg/uduvdok.cpp)

add_executable(iceb_xkus ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_xkus buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**************************************************

#сборка программы разметки таблиц в базе нужных для программы iceb_xkus
# Установка переменной со списком исходников
set(SOURSE_BIN sozku.cpp)

add_executable(iceb_sozku ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_sozku buh util sql scr ${NCURSESW_LIB} ${MYSQLCLIENT_LIB} ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками
