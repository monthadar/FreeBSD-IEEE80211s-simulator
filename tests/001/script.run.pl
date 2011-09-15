use strict;
use warnings;
our $nbr_nodes = 4;
print "Running     : 4 nodes in line-topology\n";

my $numArgs = $#ARGV + 1;
my $cmd = "null";
if($numArgs > 0){
	$cmd = shift;
}

print "\t";
if($cmd eq "setup") {
	setup();
} elsif($cmd eq "teardown") {
	teardown();
} elsif($cmd eq "run") {
	run();
} else {	
	setup();
	sleep(40);
	run();
	teardown();
}
print "\n";

sub setup {
	print "Setup test...";
	for (my $i = 0; $i < $nbr_nodes; $i++) {
		system("../ioctl c $i");
		
	}
	for (my $i = 0; $i < $nbr_nodes - 1; $i++){
		my $j = $i + 1;
		system("../vis_map a $i $j");
		system("../vis_map a $j $i");
	}
	system("../vis_map o");
	for (my $i = 0; $i < $nbr_nodes; $i++) {
		system("ifconfig wlan$i create wlandev wtap$i wlanmode mesh");
		system("ifconfig wlan$i meshid mymesh");
		system("wlandebug -i wlan$i hwmp");
		my $vnet = $i + 1;
		system("ifconfig wlan$i vnet $vnet");
	}
	
	for(my $i = 0; $i < $nbr_nodes; $i++) {
		my $vnet = $i + 1;
		system("jexec $vnet ifconfig wlan$i inet 192.168.2.$vnet");
	}
}

sub run {
	print "Running test...";
	`jexec 1 ping -c 1 192.168.2.4 > output`;
	open(my $in,  "<",  "output")  or die "Can't open output: $!";
	my $pass = 0;
	while(<$in>){
		if(/1 packets received/){
			$pass = 1;
		}
	}
	if($pass == 0){
		print "FAIL...";
	}else{
		print "SUCCESS...";
	}
}

sub teardown {
	print "Cleaning test...";
	for(my $i = 0; $i < $nbr_nodes; $i++) {
		my $vnet = $i + 1;
		system("jexec $vnet ifconfig wlan$i destroy");
		system("../ioctl d $i");
	}
}

