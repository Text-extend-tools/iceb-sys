/*$Id: iceb_ps_shrift.h,v 5.4 2013/05/17 14:56:48 sasa Exp $*/
/*03.10.2005	03.10.2005 	Белых А.И.	iceb_ps_shrift.h
Шрифт
*/
const char *zagolovok_shrift={"\n\
%%BeginSetup\n\
% character encoding(s)\n\
/koi8uEncoding [\n\
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n\
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n\
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n\
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n\
/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quotesingle\n\
/parenleft/parenright/asterisk/plus/comma/hyphen/period/slash\n\
/zero/one/two/three/four/five/six/seven\n\
/eight/nine/colon/semicolon/less/equal/greater/question\n\
/at/A/B/C/D/E/F/G\n\
/H/I/J/K/L/M/N/O\n\
/P/Q/R/S/T/U/V/W\n\
/X/Y/Z/bracketleft/backslash/bracketright/asciicircum/underscore\n\
/grave/a/b/c/d/e/f/g\n\
/h/i/j/k/l/m/n/o\n\
/p/q/r/s/t/u/v/w\n\
/x/y/z/braceleft/bar/braceright/asciitilde/.notdef\n\
/space/space/space/space/space/space/space/space\n\
/space/space/space/upblock/dnblock/block/lfblock/rtblock\n\
/ltshade/shade/dkshade/integraltp/filledbox/periodcentered/radical/approxequal\n\
/afii61352/greaterequal/space/integralbt/degree/twosuperior/periodcentered/divide\n\
/SF430000/SF240000/SF510000/afii10071/afii10101/SF390000/afii10103/afii10104\n\
/SF250000/SF500000/SF490000/SF380000/SF280000/afii10098/SF260000/SF360000\n\
/SF370000/SF420000/SF190000/afii10023/afii10053/SF230000/afii10055/afii10056\n\
/SF410000/SF450000/SF460000/SF400000/SF540000/afii10050/SF440000/copyright\n\
/afii10096/afii10065/afii10066/afii10088/afii10069/afii10070/afii10086/afii10068\n\
/afii10087/afii10074/afii10075/afii10076/afii10077/afii10078/afii10079/afii10080\n\
/afii10081/afii10097/afii10082/afii10083/afii10084/afii10085/afii10072/afii10067\n\
/afii10094/afii10093/afii10073/afii10090/afii10095/afii10091/afii10089/afii10092\n\
/afii10048/afii10017/afii10018/afii10040/afii10021/afii10022/afii10038/afii10020\n\
/afii10039/afii10026/afii10027/afii10028/afii10029/afii10030/afii10031/afii10032\n\
/afii10033/afii10049/afii10034/afii10035/afii10036/afii10037/afii10024/afii10019\n\
/afii10046/afii10045/afii10025/afii10042/afii10047/afii10043/afii10041/afii10044\n\
] def\n\
% Reencode fonts\n\
/create-font { % процедура определения нового фонта\n\
	       % параметры: /old-font /new-font encoding-table\n\
   /new-encoding exch def	% таблица перекодировки\n\
   /new-font-name exch def	% имя нового фонта\n\
%   /old-font-name exch def	% имя старого фонта\n\
   findfont	% загрузить старый фонт в стек\n\
   dup length			% длина шрифта в стек\n\
   dict\n\
   begin			% занесение словаря в стек словарей\n\
     {\n\
        exch dup\n\
        /FID eq\n\
   	  { 			% имя != /FID - заносим в словарь\n\
            pop pop \n\
          }\n\
          { 			% имя == /FID - пропускаем\n\
            exch\n\
            def\n\
          } ifelse\n\
     } forall			% цикл по всем элементам фонта\n\
     /FontName new-font-name def % запись нового имени фонта\n\
     /Encoding new-encoding def	% запись таблицы перекодировки\n\
     new-font-name 		% имя фонта -> стек\n\
     currentdict		% словать -> стек\n\
   end\n\
   definefont			% регистрация нового шрифта\n\
   pop			% очистить весь стек\n\
%   count\n\
%   /stack-count exch def\n\
} def\n\
\n\
/Courier /koi8-u koi8uEncoding create-font\n\
/Courier-Bold /koi8-u-bold koi8uEncoding create-font\n\
\n\
%%EndSetup\n\
"};
