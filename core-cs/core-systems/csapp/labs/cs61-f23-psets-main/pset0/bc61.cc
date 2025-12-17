#include <cstdio> 

int main() {
    unsigned long bytes = 0;
    while(fgetc(stdin) != EOF){
        bytes++;
    }
    fprintf(stdout, "%lu\n", bytes); 
    return 0;
}