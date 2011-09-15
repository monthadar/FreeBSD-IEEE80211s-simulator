use strict;
use warnings;
our $nbr_nodes = 3;
print "Running     : 3 nodes in mesh-topology\n";
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
	system("../vis_map a 0 1");
	system("../vis_map a 1 0");
	system("../vis_map a 0 2");
	system("../vis_map a 2 0");
	system("../vis_map a 1 2");
	system("../vis_map a 2 1");
	system("../vis_map o");
	for (my $i = 0; $i < $nbr_nodes; $i++) {
		system("ifconfig wlan$i create wlandev wtap$i wlanmode mesh");
		system("ifconfig wlan$i meshid mymesh");
		system("wlandebug -i wlan$i hwmp+mesh");
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
	`jexec 1 ping -c 1 192.168.2.3 > output1`;
	open(my $in,  "<",  "output1")  or die "Can't open output1: $!";
	my $pass1 = 0;
	while(<$in>){
		if(/1 packets received/){
			$pass1 = 1;
		}
	}
	system("../vis_map d 0 2");
	system("../vis_map d 2 0");
	`jexec 1 ping -c 1 192.168.2.3 > output2`;
	open($in,  "<",  "output2")  or die "Can't open output2: $!";
	my $pass2 = 0;
	while(<$in>){
		if(/1 packets received/){
			$pass2 = 1;
		}
	}
	if($pass1 == 0 || $pass2 == 0){
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

