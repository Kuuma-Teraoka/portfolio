
#include <iostream>

void method() {
    throw std::runtime_error("throw") ;
    //throw "no" ;
}

int main()
{
    
//try {
    method() ;
/*}
catch(const std::runtime_error& e) {
    std::cout << "Error: " << e.what() << "\n" ;
}*/
}
