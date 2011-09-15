use strict;
use warnings;
my $nbrTests = 3;
print "Running all tests (4 JAILS)...\n";
`jail -c jid=1 vnet persist`;
`jail -c jid=2 vnet persist`;
`jail -c jid=3 vnet persist`;
`jail -c jid=4 vnet persist`;
my $numArgs = $#ARGV + 1;
if($numArgs == 0) { #run all tests
	for(my $i = 1; $i <= $nbrTests; $i++){
		chdir "00$i";
		system("/usr/bin/perl ./script.run.pl");
		chdir "..";
	}
}else{
	my $test = shift;
	chdir "00$test";
	system("/usr/bin/perl ./script.run.pl @ARGV");
	chdir "..";
}
