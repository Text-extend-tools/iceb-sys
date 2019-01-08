/*$Id: iceb_ps.h,v 5.12 2013/05/17 14:56:48 sasa Exp $*/
/*Заголовок PostScript файла*/

const char *zagolovok={"%!PS-Adobe-3.0\n\
%$Id: iceb_ps.h,v 5.12 2013/05/17 14:56:48 sasa Exp $\n\
%%BoundingBox: 0 0 595.275 841.897 % ширина и высота в пунктах 72*210/25.4  72*297/25.4\n\
%%Creator: epson-ps\n\
%%Title: iceB\n\
%%Pages: (attend)\n\
  %DocumentFonts: Courier Courier-Bold\n\
  %Orientation: Portrait\n\
  %DocumentPaperSizes: A4\n\
%%EndComments\n\
\n\
%%BeginProlog\n\
  /EXC {exch definefont pop} bind def\n\
  /milim {25.4 div 72 mul} def % Процедура перевода миллиметров в пункты\n\
%  /OTSTY 15.5 milim def % Отступ от верхнего края листа до первой строки\n\
  /OTSTY 12.8 milim def % Отступ от верхнего края листа до первой строки\n\
  /OTSTXMIN 6.44 milim def  % Граница начала зоны печати с левой стороны листа\n\
\n\
  /mstrr 25.4 6 div milim def    % Межстрочное растояние 1/6 дюйма\n\
\n\
  /perstx OTSTXMIN def              % позиция первой строки по Х\n\
  /persty 841.897 OTSTY sub def      % позиция первой строки по Y высота листа в пунктах 72*297/25.4\n\
\n\
  /tekpx perstx def              % начало текущей строки по Х\n\
  /tekpy persty def              % Начало текущей строки по Y\n\
\n\
  /hs10z 25.4 10 div milim def   % Ширина символа 10 знаков на дюйм\n\
%  /hs10zu 25.4 17 div milim def  % Ширина символа 10 знаков на дюйм в ужатом режиме (17 ЗНАКОВ)\n\
%  /hs12z 25.4 12 div milim def   % Ширина символа 12 знаков на дюйм\n\
%  /hs12zu 25.4 20 div milim def  % Ширина символа 12 знаков на дюйм в ужатом режиме (20 ЗНАКОВ)\n\
  /tekhs hs10z def              % Текущая ширина символа\n\
  /tln 1 def                    % Толщина линии нормальная\n\
  /tln2 2 def                   % Толщина линии двойная\n\
  /tektl tln def                % Толщина линии текущая\n\
\n\
  /metkaunderline 0 def % метка подчеркивания символа 0 - без подчеркивания 1- с подчеркиванием\n\
\n\
  /SV  % вычисление коэффициента масштабирования ширины символа\n\
   {\n\
    dup                % Копируем в стек строку\n\
    show               % выводим строку на печать\n\
%    stringwidth        % Вычисляем координаты после вывода символа\n\
%    pop                % Убираем из стека координату по Y\n\
%    /tekstr exch def    % Запоминаем координату по Х после вывода строки\n\
    length              % Вычисляем размер строки  \n\
    /dlinastr exch      % меняем местами вычисленную длинну стрки и переменную\n\
    tekhs mul def\n\
    /tekstr tekpx dlinastr add def    % Запоминаем координату по Х после вывода строки\n\
\n\
    metkaunderline 1 eq\n\
     {\n\
      newpath            % начало графического пути\n\
      tekpx tekpy moveto % становимся в точу начала вывода строки\n\
      tekstr tekpy lineto    % проводим линию на ширину текущей строки\n\
      closepath          % замыкание графического пути\n\
      0.5 setlinewidth   % установка толщины линии\n\
      stroke             % построение линии вдоль пути\n\
     } if\n\
    /tekpx tekstr def  % Переместить текущюю позицию символа по X\n\
    tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /MV  % передвижение в начало следующей строки\n\
  {\n\
   tekpy mstrr sub\n\
   /tekpy exch def\n\
   /tekpx perstx def\n\
   tekpx tekpy moveto\n\
  } def\n\
\n\
 /otstup\n\
  {\n\
   tekhs mul % Умножить заданный отступ на текущюю ширину символа\n\
   /ots exch def % Присвоить вычисленное значение\n\
   /ots ots OTSTXMIN add def % прибавляем размер левой мёртвой зоны\n\
% -----------старый принцип вычисления отступа--------\n\
%   ots OTSTXMIN lt   Если ots меньше минимально учитываемого\n\
%    {\n\
%     /ots OTSTXMIN def      не отрабатывать\n\
%    }\n\
%   if\n\
% -----------------------------------------------------\n\
   /perstx ots def % записать в константу начального отступа\n\
   /tekpx ots def  % установить текущий x\n\
   tekpx tekpy moveto % Переместить точку вывода\n\
  } def\n\
\n\
 /liniq % замена псевдографики - горизонтальная линия\n\
  {\n\
   newpath            % начало графического пути\n\
   /liny tekpy 4 add def\n\
   tekpx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 0 rlineto    % проводим линию на ширину текущего символа\n\
   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def \n\
\n\
 /vliniq %Вертикальная линия\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy 2 sub def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 14  rlineto    % проводим линию вверх\n\
   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /krest   % +\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy 2 sub def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 14  rlineto    % проводим линию вверх\n\
   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   /liny tekpy 4 add def\n\
   tekpx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 0 rlineto    % проводим линию на ширину текущего символа\n\
   closepath          % замыкание графического пути\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
  } def\n\
\n\
 /vlugol % Верхний левый уголок\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy 2 sub def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 6  rlineto    % проводим линию вверх\n\
   tekhs 2 div 0  rlineto    % проводим линию вправо\n\
%   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /vpugol % Верхний правый уголок\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx def\n\
   /liny tekpy 4 add def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 2 div 0  rlineto    % проводим линию вправо\n\
   0 -6  rlineto    % проводим линию вниз\n\
%   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /nlugol % Нижний левый уголок\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy 11 add def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 -7 rlineto    % проводим линию ввниз\n\
   tekhs 2 div 0 rlineto    % проводим линию вправо\n\
%   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /npugol % Нижний правы уголок\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx def\n\
   /liny tekpy 4 add def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 2 div 0 rlineto    % проводим линию вправо\n\
   0 7 rlineto    % проводим линию вверх\n\
%   closepath          % замыкание графического пути\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
\n\
  } def\n\
\n\
 /tvverh   % Т вверх\n\
  {\n\
   newpath            % начало графического пути\n\
   /liny tekpy 4 add def\n\
   tekpx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 0 rlineto    % проводим линию на ширину текущего символа\n\
\n\
   tekpx tekhs 2 div add liny moveto % возвращаемся на половину ширины символа\n\
   0 7  rlineto    % проводим линию вверх\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
  } def\n\
\n\
 /tvniz   % Т вниз\n\
  {\n\
   newpath            % начало графического пути\n\
   /liny tekpy 4 add def\n\
   tekpx liny moveto % становимся в точку начала вывода символа\n\
   tekhs 0 rlineto    % проводим линию на ширину текущего символа\n\
\n\
   tekpx tekhs 2 div add liny moveto % возвращаемся на половину ширины символа\n\
   0 -7  rlineto    % проводим линию вниз\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
  } def\n\
\n\
\n\
 /tvpravo   % Т вправо\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy -3 add def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 14 rlineto    % проводим линию вверх\n\
   linx tekpy 4 add  moveto % становимся в точку горизонтального отрезка\n\
   tekhs 2 div 0 rlineto    % проводим линию на ширину текущего символа\n\
\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
  } def\n\
\n\
 /tvlevo   % Т влево\n\
  {\n\
   newpath            % начало графического пути\n\
   /linx tekpx tekhs 2 div add def\n\
   /liny tekpy -3 add def\n\
   linx liny moveto % становимся в точку начала вывода символа\n\
   0 14 rlineto    % проводим линию вверх\n\
   linx tekpy 4 add  moveto % становимся в точку горизонтального отрезка\n\
   tekhs 2 div -1 mul 0 rlineto    % проводим линию на ширину текущего символа\n\
\n\
   tektl setlinewidth   % установка толщины линии\n\
   stroke             % построение линии вдоль пути\n\
\n\
   /tekpx tekpx tekhs add def  % Переместить текущюю позицию символа по X\n\
   tekpx tekpy moveto % стать в позицию вывода нового символа\n\
  } def\n\
\n\
\n\
%%EndProlog\n\
"};
