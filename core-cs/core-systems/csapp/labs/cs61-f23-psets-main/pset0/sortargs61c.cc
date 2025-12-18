#include <cstdio> 
#include <cstring>

//c style
int main(int argc, char* argv[]) {
    for(int i = 2; i < argc; i++)
    {   
        int j = i - 1;
        char* key = argv[i];
        while(j >= 1 && strcmp(argv[j], key) > 0)
        {
            argv[j+1] = argv[j];
            j--;
        }
        argv[j+1] = key;
    }
    for(int k = 1; k < argc; k++)
        printf("%s\n", argv[k]);
    return 0;
}