#!/usr/bin/perl

#
# Конвертер довідки IceB з формату txt в формат html
# 09.10.2009 Володимир Піх (volodya_25@mail.ru) icedoc2html.pl 
# Створено на основі icedoc2sgml.pl (Титков Дмитрий dima_gold@bigfoot.com) 
#
# Конвертує файли *.txt в *.txt.htm, які розміщує в тому ж каталозі 
# Запускати: icedoc2html.pl <ШляхДоКаталогуЗДовідкою>


$name = "_";
$verb = 0;

$dir = shift;

if( $dir eq "" ) { die "Usage: icedoc2html.pl <dirname_with_txt_docs>\n"; }

@ls = `if test -f \"$dir/CONTENTS\" ; then cat \"$dir/CONTENTS\" ;\
	else find \"$dir\" -name \"*.txt\" -exec basename {} \\; ; fi`;


$time = localtime(time());
open( IN, "> $dir/index.htm");
print IN "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
print IN "<HTML><HEAD>\n";
print IN "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">\n";
print IN "<META NAME=\"Author\" content=\"Icedoc2html.pl by Volodymyr Pikh (volodya_25\@mail.ru)\">\n";
print IN "</HEAD>\n";
print IN "<body>\n <h1>Документация на систему iceB</h1>"; 


foreach $ii ( @ls )
  {
 
  $line1 = $line2 = "";
  
  $i = "$dir/$ii";	# полный путь к файлу

#  print "Processing <$i>\t";
  print IN "<a href=\"$i.htm\">";
  chomp($i);
open( INDEX, "> $dir/$i.htm");
print INDEX "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
print INDEX "<HTML><HEAD>\n";
print INDEX "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">\n";
print INDEX "<META NAME=\"Author\" content=\"Icedoc2html.pl by Volodymyr Pikh (volodya_25\@mail.ru)\">\n";
print INDEX "</HEAD>\n";
print INDEX "<body>\n";

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

  if( $verb ) { print INDEX "</tt><br>\n"; } # закрыть если было открыто

  if( substr($ii,0,1) ne $name ) 
    { 
    $name = substr($ii,0,1); 
    print INDEX "<h1><a href=\"index.htm\">$line1</a></h1><p>\n"; 
    print IN "<h3>$line1</h3></a>\n"; 
    }
  else 
    { 
    print INDEX "<h2><a href=\"index.htm\"> $line1</a></h2><p>\n"; 
    print IN  "$line1</a><br>\n"; 
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
#сxтроки начинающиеся с цифр
#или начинаеться с " ----------------- убрал
      if( /^\s*F\d/ || /^\s*Shift/ || /^\s*Enter/ || /^\s*[\|\-\+]/ || /^\s*\d/ )
        {
	if( !$verb ) { $_ = "<tt>".$_; $verb = 1; } 
	last; 
        }

#пустая строка
      if( /^$/ )
        {
	if( !$verb ) { $_ = "<br>"; }
        last; 
        }

      if( /^\s{5,}?\s*(\S.*)/ )
        {
	if( $verb ) { $_ = "</tt>\n<p>".$1; $verb = 0; }
	else        { $_ = "<p>".$1; }
        last; 
        }

#строки начинающиеся с 2-3х пробелов
      if( /^\s+/ )
        {
	if( !$verb ) { $_ = "<tt>".$_; $verb = 1; } 
	last; 
        }

    if( !$verb && length($_) < 30 ) 
      {
      $_ = $_."<br>";
      last;
      }

    last; 
    }

    print INDEX "$_<br>\n" ; 
  }

  if( $verb ) { print INDEX "</tt>"; $verb = 0; }
  
  close( INFILE );
print INDEX "<p>\n";
print INDEX "</body></html>\n";
close( INDEX );
  }
close( IN );  

  
