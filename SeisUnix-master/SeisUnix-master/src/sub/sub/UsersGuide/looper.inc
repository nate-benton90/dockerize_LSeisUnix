//
//  20 passes by 1000 elements takes 19.2 sec of cpu time
//	about 1 second/pass
//
func computeL2(lv) {
    auto i, s;
    s = 0;
    for(i = 0; i < size(lv); i++)
    	s = s + lv[i];
    return s;
}

func Begin() {
    auto i;
    lv = random(1000);
    for(i = 0; i < 20; i++)
    	computeL2(lv);
    exit(0);
}
