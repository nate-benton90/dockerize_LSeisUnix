#include <signal.h>
#include <time.h>
#include <osfcn.h>
#include <XSYS/dval.h>
#define iv_boolean_h
#include <XSYS/service.h>
#include <XSYS/writer.h>
#include <SeisView/liststring.h>

int kirmigs ( char * datain, char * dataout, int lseekin, int lseekout, char * timefile, char * ampfile, int iamp, int nt, int nx, int nz, int lt, float tmin, float x0, float z0, float dt, float dx, float dz, float smint, float xmint, float zmint, float dst, float dxt, float dzt, int nst, int nxt, int nzt, float ofimin, float ofimax, float ofomin, float dofo, int nofo, int* mtrace, int i2p5, int intps, int intpx, int intpz, int intype, float dcdp, int amptype, int mlimit, int lagc, float tpow, float aper, float apanl, float apanr, float v0, int cdpx0, int dcdpx, char * dipfile, float dxdip, float dxtol );


extern "C" {
    void initargs(int argc,char *argv[]) ;
}
    

static RService * TheService ;
static StringList * LFiles ;

static void beKilled() {
      TheService-> writer().sendJOBEND(-9);
      delete TheService ;

      // must kill all potential children
       

      // remove LFiles
      if( LFiles ) {
	 for( LFiles-> First(); !LFiles-> AtEnd(); LFiles-> Next() ) {
	     const char * lfile = LFiles-> GetCur()-> GetString() ;
	     unlink(lfile) ;
	 }
      }
      exit(1) ;
}

static void addLFile(const char *lfile) {
      if( LFiles == nil ) LFiles = new StringList() ;
      LFiles-> Append(new StringNode(lfile)) ;
}

static void cleanLFiles() {
      delete LFiles ;
      LFiles = nil ;
}

main(int argc,char *argv[]) {
	if( argc < 2 ) return 1 ;
        signal(SIGTERM,(SIG_PFV)beKilled) ;
        signal(SIGINT,(SIG_PFV)beKilled) ;
        signal(SIGHUP,(SIG_PFV)beKilled) ; // on bug?

	
	initargs(argc, argv);
    

	// Open the connection with the given path
	RService service(argv[1],51) ;
	TheService = &service ;

	// Get arguments
	DVal ** rargs ;
	void * _v ;
	while( (rargs=service.GetRArgs()) != nil ) {

		char * datain;
		rargs[0]-> Get(datain) ;
		char * dataout;
		rargs[1]-> Get(dataout) ;
		int lseekin;
		rargs[2]-> Get(lseekin) ;
		int lseekout;
		rargs[3]-> Get(lseekout) ;
		char * timefile;
		rargs[4]-> Get(timefile) ;
		char * ampfile;
		rargs[5]-> Get(ampfile) ;
		int iamp;
		rargs[6]-> Get(iamp) ;
		int nt;
		rargs[7]-> Get(nt) ;
		int nx;
		rargs[8]-> Get(nx) ;
		int nz;
		rargs[9]-> Get(nz) ;
		int lt;
		rargs[10]-> Get(lt) ;
		float tmin;
		rargs[11]-> Get(tmin) ;
		float x0;
		rargs[12]-> Get(x0) ;
		float z0;
		rargs[13]-> Get(z0) ;
		float dt;
		rargs[14]-> Get(dt) ;
		float dx;
		rargs[15]-> Get(dx) ;
		float dz;
		rargs[16]-> Get(dz) ;
		float smint;
		rargs[17]-> Get(smint) ;
		float xmint;
		rargs[18]-> Get(xmint) ;
		float zmint;
		rargs[19]-> Get(zmint) ;
		float dst;
		rargs[20]-> Get(dst) ;
		float dxt;
		rargs[21]-> Get(dxt) ;
		float dzt;
		rargs[22]-> Get(dzt) ;
		int nst;
		rargs[23]-> Get(nst) ;
		int nxt;
		rargs[24]-> Get(nxt) ;
		int nzt;
		rargs[25]-> Get(nzt) ;
		float ofimin;
		rargs[26]-> Get(ofimin) ;
		float ofimax;
		rargs[27]-> Get(ofimax) ;
		float ofomin;
		rargs[28]-> Get(ofomin) ;
		float dofo;
		rargs[29]-> Get(dofo) ;
		int nofo;
		rargs[30]-> Get(nofo) ;
		rargs[31]-> Get(_v) ;
		int* mtrace = (int*)_v ;
		int i2p5;
		rargs[32]-> Get(i2p5) ;
		int intps;
		rargs[33]-> Get(intps) ;
		int intpx;
		rargs[34]-> Get(intpx) ;
		int intpz;
		rargs[35]-> Get(intpz) ;
		int intype;
		rargs[36]-> Get(intype) ;
		float dcdp;
		rargs[37]-> Get(dcdp) ;
		int amptype;
		rargs[38]-> Get(amptype) ;
		int mlimit;
		rargs[39]-> Get(mlimit) ;
		int lagc;
		rargs[40]-> Get(lagc) ;
		float tpow;
		rargs[41]-> Get(tpow) ;
		float aper;
		rargs[42]-> Get(aper) ;
		float apanl;
		rargs[43]-> Get(apanl) ;
		float apanr;
		rargs[44]-> Get(apanr) ;
		float v0;
		rargs[45]-> Get(v0) ;
		int cdpx0;
		rargs[46]-> Get(cdpx0) ;
		int dcdpx;
		rargs[47]-> Get(dcdpx) ;
		char * dipfile;
		rargs[48]-> Get(dipfile) ;
		float dxdip;
		rargs[49]-> Get(dxdip) ;
		float dxtol;
		rargs[50]-> Get(dxtol) ;

		// Save time for computing elapsed Time
		long etime = time(0) ;
		// Function return code
		int _status ;

		// Function call
		if( (_status=kirmigs(
			datain, dataout, lseekin, lseekout, timefile, ampfile, iamp, nt, nx, nz, lt, tmin, x0, z0, dt, dx, dz, smint, xmint, zmint, dst, dxt, dzt, nst, nxt, nzt, ofimin, ofimax, ofomin, dofo, nofo, mtrace, i2p5, intps, intpx, intpz, intype, dcdp, amptype, mlimit, lagc, tpow, aper, apanl, apanr, v0, cdpx0, dcdpx, dipfile, dxdip, dxtol 
		) ) != 0 ) {
			service.writer().sendJOBEND(_status);
			cleanLFiles();
			return(0);
		}

		// Send output arguments back to the server
		rargs[1]-> Send(service.writer()) ;
		rargs[31]-> Send(service.writer()) ;

		service.writer().sendJOBEND(_status,etime);

		// Clean arrays and input only LFILE
		delete datain ;
		delete dataout ;
		delete timefile ;
		delete ampfile ;
		delete dipfile ;
		delete mtrace ;
		cleanLFiles();
	}

	return 0 ;
}
