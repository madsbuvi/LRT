#define forrange( i, imin, imax ) for( int i = imin; i < imax; i++ )
#define forrange2( i, imin, imax, j, jmin, jmax) forrange(i, imin, imax)forrange(j, jmin, jmax)
#define forrange3( i, imin, imax, j, jmin, jmax, k, kmin, kmax) forrange(i, imin, imax)forrange(j, jmin, jmax)forrange(k, kmin, kmax)