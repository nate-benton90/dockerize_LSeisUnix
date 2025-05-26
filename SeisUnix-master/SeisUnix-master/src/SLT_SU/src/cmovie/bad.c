/*--------------------------------------------------------------------*\
   percentile subroutine based on Canales, SEP-10

    p - percentile <0.,99.999999999999>
    x - data
    n - vector length

    this routine changes data order, so sort a copy
\*--------------------------------------------------------------------*/

float    DataCent(float *x, int n, float p)
{
    int      q;
    register float *i, *j, ak;
    float   *low, *hi, buf, *k;

    p = p < 99.999 ? p : 99.999;
    p = p > 0.0 ? p : 0.0;
    q = (p * n) / 100.;
    if( q == n ){
        q--;
    }
    for( low = x, hi = x + n - 1, k = x + q; low < hi; ){
        ak = *k;
        i = low;
        j = hi;
        do {
            while (*i < ak)
                i++;
            while (*j > ak)
                j--;
            if( i <= j ){
                buf = *i;
                *i++ = *j;
                *j-- = buf;
            }
        } while (i <= j);
        if( j < k ){
            low = i;
        }
        if( k < i ){
            hi = j;
        }
    }
    return (*k);
}

