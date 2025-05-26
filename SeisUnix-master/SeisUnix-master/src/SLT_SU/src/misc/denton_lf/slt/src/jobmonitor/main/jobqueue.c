#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

main(int argc, char *argv[])
{
	char *dir, *script, *errfile, *host;
	unsigned seconds=60;
	char *userid, *uid;
	char cmd[4096];
	char filename[2048];
	char *c81;
	FILE *infp;
	time_t ftime, tt;
	int itt, iitime, ii, jj;

	int i, pid, prgid, ppid, iprgid;
	int jobrun=0;

	/* get parameters */
	if(argc==1) {
		fprintf(stderr," jobqueue   job queue program \n");
		fprintf(stderr,"  \n");
		fprintf(stderr," jobqueue   dir script stderr \n"); 
		fprintf(stderr,"    dir     directory where the job script is \n"); 
		fprintf(stderr,"    script  name of the job script \n"); 
		fprintf(stderr,"    stderr  name of standard error file of the job \n"); 
		fprintf(stderr,"  \n");
		fprintf(stderr," Example: \n"); 
		fprintf(stderr,"    jobqueue /data/userid/jobdir job.sh err.job \n"); 
		fprintf(stderr," Author: Z. Li        11/10/2000 \n"); 
		exit(0);
	} else if (argc<3 ) {
		fprintf(stderr," three arguments needed \n"); 
		exit(1);
	}

	dir = argv[1];	
	script = argv[2];	
	errfile = argv[3];	

	fprintf(stderr," JOB QUEUEINZG start .... \n");
	fprintf(stderr," dir=%s script=%s stderr=%s \n",dir,script,errfile);

	/* get your id */
	userid = getenv("USER");
	/* get this program's process pid */
	pid = getpid(); 
	/* get host name */
	host = getenv("HOST");

	/* create a queue file */
	ftime = time(&tt);
	itt = (int)tt;
	sprintf(filename,"/tmp/%d.%s.%s.%d.jobqueue",itt,userid,host,pid);
	sprintf(cmd,"ls /tmp | grep jobqueue >%s \n",filename);
	system(cmd);

	c81 = (char*) malloc(81*sizeof(char));

	

	/* loop to job monitoring */
	i = 0;
	do {

		/* check to see all jobqueues in the system  */
		sprintf(cmd,"ls /tmp | grep jobqueue | cut -d. -f1,10 >%s \n",
			filename);
		system(cmd);
		/* read the times of each queued job */
		if( (infp=fopen(filename,"r")) == NULL) { 
			fprintf(stderr," open %s failed \n",filename);
			fprintf(stderr," JOB QUEUEING end \n");
			sprintf(cmd,"/bin/rm %s \n",filename);
			system(cmd);
			exit(1);
		}
		for(ii=0;ii<100000;ii++) {
			if(feof(infp)!=0) break;
			iitime  = -1;
			bzero(c81,81);
			fgets(c81,81,infp);
			sscanf(c81,"%d",&iitime);

			if(iitime>0 && iitime<itt) {
				jobrun = 0;
				sprintf(cmd,"touch %s \n", filename);
				system(cmd);
				break;
			} else if(iitime>0 && iitime>=itt && jobrun==0) {
				sprintf(cmd,"cd %s; %s >& %s & \n",dir,script,errfile);
				system(cmd);
				/* get prgid */
				fclose(infp);
				sprintf(cmd,"ps -ef | grep %s | grep %d  >%s \n",userid,pid,filename);
				system(cmd);
				if( (infp=fopen(filename,"r")) == NULL) { 
					fprintf(stderr," open %s failed \n",filename);
					fprintf(stderr," JOB QUEUEING end \n");
					sprintf(cmd,"/bin/rm %s \n",filename);
					system(cmd);
					exit(1);
				}
				prgid = -1;
				for(jj=0;jj<1000;jj++) {
					bzero(c81,81);
					fgets(c81,81,infp);
					sscanf(c81,"%s %d %d",uid,&prgid,&ppid);
					if(ppid==pid) {
						jobrun = 1;
						break;
					}
					if(feof(infp)!=0) break;
				}
				break;
			} else if(jobrun==1) {
				/* get prgid */
				fclose(infp);
				sprintf(cmd,"ps -ef | grep %s | grep %d  | grep %d >%s \n",userid,pid,prgid,filename);
				system(cmd);
				if( (infp=fopen(filename,"r")) == NULL) { 
					fprintf(stderr," open %s failed \n",filename);
					fprintf(stderr," JOB QUEUEING end \n");
					sprintf(cmd,"/bin/rm %s \n",filename);
					system(cmd);
					exit(1);
				}
				iprgid = -1;
				for(jj=0;jj<1000;jj++) {
					bzero(c81,81);
					fgets(c81,81,infp);
					sscanf(c81,"%s %d %d",uid,&iprgid,&ppid);
					if(ppid==pid && iprgid==prgid) {
						jobrun = 1;
						break;
					} else {
						fprintf(stderr," job pid=%d done \n",prgid);
						fprintf(stderr," JOB QUEUEING end \n");
						sprintf(cmd,"/bin/rm %s \n",filename);
						system(cmd);
						exit(0);
					}
					if(feof(infp)!=0) break;
				}
				break;
			}
		}

		if(iitime<0) {
			fprintf(stderr," no queues found in /tmp \n");
			fprintf(stderr," JOB QUEUEING end \n");
			exit(0);
		}

		sleep(seconds);

	} while(i==0);

	fprintf(stderr," JOB QUEUEING end \n");
	exit(0);
}
