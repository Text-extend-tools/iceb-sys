
message ("Построение утилит")

project(util) #имя проекта



set(SOURSE_BIN c_to_cpp.cpp) # Установка переменной со списком исходников
add_executable(c_to_cpp ${SOURSE_BIN}) #сборка программы
target_link_libraries(c_to_cpp util scr ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#**********************************

set(SOURSE_BIN iceb_ps.cpp) # Установка переменной со списком исходников
add_executable(iceb_ps ${SOURSE_BIN}) #сборка программы
target_link_libraries(iceb_ps util ${GLIB2_LIBRARIES}) # Линковка программы с библиотеками

#*************************************