#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

//c++ style
int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
    std::sort(args.begin(), args.end());
    for (const auto& s : args) {
        std::cout << s << "\n";
    }
    return 0;
}
