#include <stdio.h>
#include <sys/times.h>  /* times() */
#include <unistd.h>     /* sysconf() */

int prime3(int lb, int ub,long double *time){
	int  i, flag;
  unsigned int k;

	long double t1,t2,ticspersec;
	struct tms tb1, tb2;
	ticspersec = (long double) sysconf(_SC_CLK_TCK);
	t1 = (long double) times(&tb1);

	if(lb==2 || lb==3){		//check if the first number is 2 or 3 so as not to go in the loop
		t2 = (long double) times(&tb2);
		*time= (t2 - t1) / ticspersec;
		return lb;
	}
	k=lb|1;		//if the number is even go to the next one
  while (k <= ub){	//for every odd number in the range
    flag = 1;
		if (k%3==0 && k!=3){		//check if it is diveded by 3
			flag=0;
		}else{
			for(i=5; i*i<=k;i=i+6){		//for the certain number search only before his root (starting from 5 and adding 6 every time)
				if(k%i==0 || k%(i+2)==0){
					flag=0;
					break;
				}
			}
		}
    if(flag==1){
			t2 = (long double) times(&tb2);
			*time= (t2 - t1) / ticspersec;
      return k;
		}

    k=k+2;	//skip even numbers
  }
	t2 = (long double) times(&tb2);
	*time= (t2 - t1) / ticspersec;
	return -1;
}
