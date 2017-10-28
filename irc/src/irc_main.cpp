#include <iostream>


using namespace std;

int main(int argc, char* argv[]) {
        if(argc < 3){
                std:cout << "missing start parameter\n";
                return 0;
        }

        if(std::string(argv[1]) == "-s"){
                cout << std::string(argv[2]);
        }
   return 0;
}
