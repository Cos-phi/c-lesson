#include<stdio.h>
int div(int r2, int r3){
    int ansp = 0;
    int ansn = 0;
    while(1){
        ansp++;
        if( (r2-r3*ansp)*(r2-r3*ansp) < r3*r3 ) {
            return ansp;
        }
        
        ansn--;
        if( (r2-r3*ansn)*(r2-r3*ansn) < r3*r3 ){
            return ansn;
        }
    }
}

void main(){
    printf("%d\n",div(42,7));
    printf("%d\n",div(42,-7));
    printf("%d\n",div(-42,7));
    printf("%d\n",div(-42,-7));
}