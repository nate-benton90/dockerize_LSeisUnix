#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

main(int argc, char *argv[])
{
	char *ownerid, *program, *userid, *host;
	int checktime;
	float owneruse, owneract;
	unsigned seconds;
	char cmd[4096];
	char filename[2048];
	char *c80;
	FILE *infp;


	int i, pid, prgid;
	char *username, *size, *res, *state, *time, *cpu, *com;
	int thr, pri, nice;
	float prguse;

	/* get parameters */
	if(argc==1) {
		fprintf(stderr," jobmonitor   automatic job suspension and continue program \n");
		fprintf(stderr,"  \n");
		fprintf(stderr," jobmonitor program ownerid owneruse checktime \n"); 
		fprintf(stderr,"    program    program name of the job  \n"); 
		fprintf(stderr,"    ownerid    unix id of owner of the machine  \n"); 
		fprintf(stderr,"    owneruse   thredhold of owner cpu percentage use \n"); 
		fprintf(stderr,"    checktime  time interval (in sec) to check the job \n"); 
		fprintf(stderr,"  \n");
		fprintf(stderr," when ownerid is anybody, it will check with anybody, \n");
		fprintf(stderr," except you and root \n");
		fprintf(stderr,"  \n");
		fprintf(stderr," Example: \n"); 
		fprintf(stderr,"    jobmonitor kzmig oxtexyz 1 60 \n"); 
		fprintf(stderr," Author: Z. Li        10/28/2000 \n"); 
		exit(0);
	} else if (argc<5 ) {
		fprintf(stderr," 4 arguments needed \n"); 
		exit(1);
	}

	program = argv[1];	
	ownerid = argv[2];	
	owneruse = atof(argv[3]);
	seconds = atoi(argv[4]);

	fprintf(stderr," JOB MONITORING start .... \n");
	fprintf(stderr," program=%s ownerid=%s owneruse=%g checktime=%d \n",
		program,ownerid,owneruse,seconds);

	/* get your id */
	userid = getenv("USER");
	/* get this program's process pid */
	pid = getpid(); 
	/* get host name */
	host = getenv("HOST");

	/* get your process id */
	sprintf(filename,"/tmp/%s.%s.%d.jobmonitor",userid,host,pid);
	sprintf(cmd,"/u76/admin/SunOS5.6_sun4/bin/top 50 | grep %s | grep %s >%s \n",
		program,userid,filename);
	system(cmd);

	/*
	fprintf(stderr,"%s",cmd);
	*/

	/* read the program id from the filename */
	if( (infp=fopen(filename,"r")) == NULL) { 
		fprintf(stderr," open %s failed \n",filename);
		fprintf(stderr," JOB MONITORING end \n");
		exit(1);
	}
	prgid = 0;
	if(fscanf(infp,"%d ",&prgid)==0) {
		fprintf(stderr," no %s of %s is currently on %s \n",
			program, userid, host);
		sprintf(cmd,"/bin/rm %s \n",filename);
		system(cmd);
		fprintf(stderr," JOB MONITORING end \n");
		exit(1);
	}
	if(prgid==0) {
		fprintf(stderr," no %s of %s is currently on %s \n",
			program, userid, host);
		sprintf(cmd,"/bin/rm %s \n",filename);
		system(cmd);
		fprintf(stderr," JOB MONITORING end \n");
		exit(1);
	}
	fclose(infp);

	fprintf(stderr," userid=%s jobmonitor_pid=%d host=%s program_id=%d\n",
		userid,pid,host,prgid);


	/* loop to job monitoring */
	i = 0;
	do {

		/* get the program cpu usage  */
		sprintf(cmd,"/u76/admin/SunOS5.6_sun4/bin/top 50 | grep %s | grep %s | grep %d | cut -c54-58 >%s \n",
			program,userid,prgid,filename);
		system(cmd);
		/* read the program cpu percentage from the filename */
		if( (infp=fopen(filename,"r")) == NULL) { 
			fprintf(stderr," open %s failed \n",filename);
			fprintf(stderr," JOB MONITORING end \n");
			exit(1);
		}
		prguse = -99.;
		if(fscanf(infp,"%f",&prguse)==0) {
			fprintf(stderr," no %s of %s is currently on %s \n",
				program, userid, host);
			sprintf(cmd,"/bin/rm %s \n",filename);
			system(cmd);
			fprintf(stderr," JOB MONITORING end \n");
			exit(1);
		}
		fclose(infp);
		if(prguse == -99.) {
		fprintf(stderr," no %s of %s with program_id=%d is currently on %s \n",
			program, userid, prgid,host);
			sprintf(cmd,"/bin/rm %s \n",filename);
			system(cmd);
			fprintf(stderr," JOB MONITORING end \n");
			exit(1);
		}

		/* get owner current actual cpu percentage */
		if(strncmp(ownerid,"anybody",7)==0) {
sprintf(cmd,"/u76/admin/SunOS5.6_sun4/bin/top 50 | grep -v %s | grep -v root | awk '{if(NR>7)print $0;}' | head -1 | cut -c54-58 >%s \n", userid,filename);
		} else {
	sprintf(cmd,"/u76/admin/SunOS5.6_sun4/bin/top 50 | grep %s | head -1 | cut -c54-58 >%s \n",
			ownerid,filename);
		}
		system(cmd);
		if( (infp=fopen(filename,"r")) == NULL) { 
			fprintf(stderr," open %s failed \n",filename);
			fprintf(stderr," JOB MONITORING end \n");
			exit(1);
		}
		owneract = 0.;
		fscanf(infp,"%f ",&owneract);
		fclose(infp);

		/* if owner current use exceeds the owner thredhold, stop the job */
		if(owneract>=owneruse && prguse>10. ) {
			sprintf(cmd,"kill -STOP %d \n",prgid);
			system(cmd);
			sleep(3600);
		} else if(owneract<owneruse && prguse<1.) {
			sprintf(cmd,"kill -CONT %d \n",prgid);
			system(cmd);
			sleep(seconds);
		} else {
			sleep(seconds);
		}

	} while(i==0);

	fprintf(stderr," JOB MONITORING end \n");
	exit(0);
}
