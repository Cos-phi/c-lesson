#include <stdio.h>

void print_address(int address) {
    printf("address: %x\n", address);
}

void print_msg(char *str) {
    printf("We get (%s)\n", str);
}

void func3 () {
    // TODO: get main_msg from here as target someway.
    char *target = "mue-";
    print_address( (int)&target ); // target
    print_address( *((int*)((int)&target+4)) );  // func2 r11 
    print_address( *(int*)*((int*)((int)&target+4)) );  // func1 r11 
    print_address( *(int*)*(int*)*(int*)((int)&target+4) );  // main r11 
    print_address( (int)*(int*)*(int*)*(int*)((int)&target+4) -8 ); // main message
    print_msg( (char*)(*(int*) ((int)*(int*)*(int*)*(int*)((int)&target+4)-8) ) ); // main message
    
    printf("We are in func3\n");
    print_msg(target);
}

void func2() {
    char *msg = "func2 message.";
    printf("We are in func2, %s\n", msg);
    func3();
}

void func1() {
    char *msg = "func1 msg";
    printf("We are in func1, %s\n", msg);
    func2();
}


int main() {
    char *main_msg = "We are in main.";
    printf("We are in main, %s\n", main_msg);
    func1();
    return 0;
}

