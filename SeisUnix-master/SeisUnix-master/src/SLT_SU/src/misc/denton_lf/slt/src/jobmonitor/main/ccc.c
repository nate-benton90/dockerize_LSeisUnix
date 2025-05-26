#include <stdio.h>
main() {

	char cmd[2048];	


	sprintf(cmd,"/u76/admin/SunOS5.6_sun4/bin/top | grep kzmig | grep stgpzli > /home/stgpzli/denton.list \n");

	system(cmd);
	/*
	system("/u76/admin/SunOS5.6_sun4/bin/top | grep kzmig | grep stgpzli > /home/stgpzli/denton.list");
	*/
	system("date");

}
