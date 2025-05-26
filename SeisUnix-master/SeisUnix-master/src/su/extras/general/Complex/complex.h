typedef struct {   /* complex type with float components */
	float r;   /* real part		*/
	float i;   /* imaginary part	*/
} fcomplex;


extern fcomplex Cadd();
extern fcomplex Csub();
extern fcomplex Cmul();
extern fcomplex Complex();
extern fcomplex Conjg();
extern fcomplex Cdiv();
extern fcomplex Csqrt();
extern fcomplex RCmul();
extern float Cabs();
extern float Re();
extern float Im();
