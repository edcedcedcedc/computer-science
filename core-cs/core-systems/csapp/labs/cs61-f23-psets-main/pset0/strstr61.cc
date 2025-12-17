#include <cstring>
#include <cassert>
#include <cstdio>

char* mystrstr(const char* s1, const char* s2) {
    if (*s2 == '\0') return (char*) s1;
    while(*s1 != '\0')
    {
        const char* p1 = s1;
        const char* p2 = s2;
        while(*p2 != '\0' &&  *p1 == *p2)
        {
            p1++;
            p2++;
        }
        if (*p2 == '\0') return (char*) s1;
        s1++;
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    assert(argc == 3);
    printf("strstr(\"%s\", \"%s\") = %p\n",
           argv[1], argv[2], strstr(argv[1], argv[2]));
    printf("mystrstr(\"%s\", \"%s\") = %p\n",
           argv[1], argv[2], mystrstr(argv[1], argv[2]));
    assert(strstr(argv[1], argv[2]) == mystrstr(argv[1], argv[2]));
}