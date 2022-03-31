#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */

#define YES 1
#define NO  0

int isPrime2(int n){
        int i=0, limitup=0;
        limitup = (int)(sqrt((float)n));

        if (n==1) return(NO);
        for (i=2 ; i <= limitup ; i++)
                if ( n % i == 0) return(NO);
        return(YES);
}

int prime2(int lb, int ub,long double *time){
        int i=0;
        long double t1,t2,ticspersec;
        struct tms tb1, tb2;
        ticspersec = (long double) sysconf(_SC_CLK_TCK);
        t1 = (long double) times(&tb1);

        // if ( (argc != 3) ){
        //         printf("usage: prime1 lb ub\n");
        //         exit(1); }

        if ( ( lb<1 )  || ( lb > ub ) ) {
            return -1;    ///cscsdcds
                printf("usage: prime1 lb ub\n");
                exit(1); }

        for (i=lb ; i <= ub ; i++){
                if ( isPrime2(i)==YES ){
                  t2 = (long double) times(&tb2);
                  *time= (t2 - t1) / ticspersec;
                  return i;
                }
        }
                      //  printf("%d ",i);
      //add time for the last diasthma
      t2 = (long double) times(&tb2);
      *time= (t2 - t1) / ticspersec;
        return -1;
}
