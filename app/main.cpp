
#include "bencoding/bdec.h"
#include "bencoding/benctypes.h"
#include <iostream>
int main(){

    auto val = trrt::benc::parse_bencoding(std::cin);
    trrt::benc::print_human_readable(val, std::cout);
    
}