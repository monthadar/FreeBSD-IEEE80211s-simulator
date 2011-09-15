#!/bin/sh

myprint1 () {
var=`expr $1 + 0`
while [ "$var" -lt "$2" ]
do
  echo "$3$var$4" 

  var=`expr $var + 1`
done
}

myprint2 () {
var=`expr $1 + 0`
var2=`expr $1 + 1`
while [ "$var" -lt "$2" ]
do
  echo "$3$var$4$var2$5" 

  var=`expr $var + 1`
  var2=`expr $var2 + 1`
done
}

myprint3 () {
var=`expr $1 + 0`
var2=`expr $1 + 1`
var3=`expr $1 + 1`
while [ "$var" -lt "$2" ]
do
  echo "$3$var2$4$var$5$var3$6" 

  var=`expr $var + 1`
  var2=`expr $var2 + 1`
  var3=`expr $var3 + 1`
done
}

times=450
jails=`expr $times + 1`
#myprint1 0 $times "./ioctl c " ""
#myprint1 0 $times "ifconfig wlan create wlandev wtap" " wlanmode mesh" 
#echo "ifconfig wlan0 hwmprootmode normal"
#myprint1 0 $times "ifconfig wlan" " meshid mymesh"
myprint1 1 $jails "jail -i -c jid=" " vnet persist"
#myprint2 0 $times "ifconfig wlan" " vnet " ""
#myprint3 0 $times "jexec " " ifconfig wlan" " inet 192.168.2." ""

exit 0