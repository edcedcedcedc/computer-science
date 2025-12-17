#include <cstdio> 
#include <cctype>

int main() {
    unsigned long bytes = 0;
    unsigned long words = 0;
    unsigned long lines = 0;
    bool is_word = false;
    int c = fgetc(stdin);
    while(c != EOF){
        if(isspace(c) && is_word)
        {
            words++;
            is_word = false;
        }
        if(c == '\n'){
            lines++;
            is_word = false;
        }
        is_word = true;
        bytes++;
      c = fgetc(stdin);
    }
    fprintf(stdout, "%7lu %7lu %7lu\n", lines, words, bytes); 
    return 0;
}