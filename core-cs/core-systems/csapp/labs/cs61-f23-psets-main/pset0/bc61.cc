#include <cstdio> 

int main() {
    unsigned long count = 0;
    while(fgetc(stdin) != EOF){
        count++;
    }
    fprintf(stdout, "%lu", count); 
    return 0;
}