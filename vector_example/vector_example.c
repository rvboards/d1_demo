#include <stdio.h>

int main(){
  float a[]={1.0,2.0,3.0,4.0};
  float b[]={1.0,2.0,3.0,4.0};
  float c[]={0.0,0.0,0.0,0.0};
  int len=4;
  int i=0;

  //inline assembly for RVV 0.7.1
  //for(i=0; i<len; i++){c[i]=a[i]+b[i];}

  asm volatile(
	       "mv         t4,   %[LEN]       \n\t"
	       "mv         t1,   %[PA]        \n\t"
	       "mv         t2,   %[PB]        \n\t"
	       "mv         t3,   %[PC]        \n\t"
	       "LOOP1:                        \n\t"
	       "vsetvli    t0,   t4,   e32,m1 \n\t" 
	       "sub        t4,   t4,   t0     \n\t" 
	       "slli       t0,   t0,   2      \n\t" //Multiply number done by 4 bytes
	       "vle.v      v0,   (t1)         \n\t" 
	       "add        t1,   t1,   t0     \n\t"
	       "vle.v      v1,   (t2)         \n\t" 
	       "add        t2,   t2,   t0     \n\t"
	       "vfadd.vv   v2,   v0,   v1     \n\t" 
	       "vse.v      v2,   (t3)         \n\t" 
	       "add        t3,   t3,   t0     \n\t"
	       "bnez       t4,   LOOP1        \n\t"
	       :
	       :[LEN]"r"(len), [PA]"r"(a),[PB]"r"(b),[PC]"r"(c)
	       :"cc","memory", "t0", "t1", "t2", "t3", "t4",
		"v0", "v1", "v2"
	       );
  
  for(i=0; i<len; i++){
    printf("%f\n",c[i]);
  }
  
  return 0;
}
