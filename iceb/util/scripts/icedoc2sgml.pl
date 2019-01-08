#!/usr/bin/perl

#
# Принимает единственный параметр: имя каталога в котором лежат txt файлы с описанием 
# IceBugh 
#
# Если в директории присутствует файл CONTENTS - файлы будут включаться в книгу в
# соответствии с перечислением в нем ; иначе - в алфавитном порядке.
#
# Результат работы - собранный из этих файлов файл iceB.sgml который будет помещен в 
# этот же каталог
# 

$name = "_";
$verb = 0;

$dir = shift;

if( $dir eq "" ) { die "Usage: icedoc2sgml.pl <dirname_with_txt_docs>\n"; }

@ls = `if test -f \"$dir/CONTENTS\" ; then cat \"$dir/CONTENTS\" ;\
	else find \"$dir\" -name \"*.txt\" -exec basename {} \\; ; fi`;

open( INDEX, "> $dir/iceB.sgml");

print INDEX "<!doctype linuxdoc system>\n";
print INDEX "<article>\n";
print INDEX "<title>Документация на бухгалтерский комплекс IceBuhg.\n";
print INDEX "<author>Генератор sgml файла написал Титков Дмитрий <tt>dima_gold\@bigfoot.com</tt>\n";
$time = localtime(time());
print INDEX "<date>$time\n";
print INDEX "<abstract>\n";
print INDEX "Документация на бухгалтерский комплекс IceBuhg.";
print INDEX "</abstract>\n\n";
print INDEX "<toc>\n\n";

foreach $ii ( @ls )
  {
  
  $line1 = $line2 = "";
  
  $i = "$dir/$ii";	# полный путь к файлу

  print "Processing <$i>\t";

  chomp($i);
  open( INFILE, "< $i");
 
  do { $_ = <INFILE>; chomp($_); $line1 = $_; $_ =~ s/ //g;  } while ( $_ eq "" );
       $_ = <INFILE>; chomp($_); $line2 = $_;
  
  $line1 =~ /(^\s*)(\S.*\S)(\s*$)/; $line1 = $2;

  if( $line2 =~ /\s*\-+/ ) { $line2 = ""; }

  if( $line2 =~ /(^\s+)(\S.*\S)(\s*$)/ ) 
    { 
    $line1 = $line1." ".$2;
    $line2 = "";
    }

  if( $verb ) { print INDEX "</verb>\n"; } # закрыть если было открыто

  if( substr($ii,0,1) ne $name ) 
    { 
    $name = substr($ii,0,1); 
    print INDEX "<sect>$line1<p>\n"; 
    print "<sect>$line1\n"; 
    }
  else 
    { 
    print INDEX "<sect1>$line1<p>\n"; 
    print "\t<sect1>$line1\n"; 
    }

  if( $line2 ne "" ) { goto HitryjVhod; }

  while(<INFILE>) 
    {
    chomp($_);

HitryjVhod:
    
    while(1) # своеобразный case
      {
      $_ =~ s/\t/        /g;

#    прямопечатные <pre></pre> 
#функциональные клафиши
#квадратные блоки и то что начинаеться с +
#строки начинающиеся с цифр
#или начинаеться с " ----------------- убрал
      if( /^\s*F\d/ || /^\s*Shift/ || /^\s*Enter/ || /^\s*[\|\-\+]/ || /^\s*\d/ )
        {
	if( !$verb ) { $_ = "<verb>".$_; $verb = 1; } 
	last; 
        }

#пустая строка
      if( /^$/ )
        {
	if( !$verb ) { $_ = "<newline>"; }
        last; 
        }

      if( /^\s{5,}?\s*(\S.*)/ )
        {
	if( $verb ) { $_ = "</verb>\n<p>".$1; $verb = 0; }
	else        { $_ = "<p>".$1; }
        last; 
        }

#строки начинающиеся с 2-3х пробелов
      if( /^\s+/ )
        {
	if( !$verb ) { $_ = "<verb>".$_; $verb = 1; } 
	last; 
        }

    if( !$verb && length($_) < 30 ) 
      {
      $_ = $_."<newline>";
      last;
      }

    last; 
    }

    print INDEX "$_\n" ; 
  }

  if( $verb ) { print INDEX "</verb>"; $verb = 0; }
  
  close( INFILE );
  }
  
print INDEX "<p>\n";
print INDEX "</article>\n";
  
  
close( INDEX );
  