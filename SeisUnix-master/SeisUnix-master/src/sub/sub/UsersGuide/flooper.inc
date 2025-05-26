//
//  200 passes by 1000 elements takes 0.74 sec of cpu time
//	about 3.7 milliseconds/pass.
//
func computeL2(lv) {
    return sum(lv*lv);
}

func Begin() {
    auto i;
    lv = random(1000);
    for(i = 0; i < 200; i++)
    	computeL2(lv);
    exit(0);
}
