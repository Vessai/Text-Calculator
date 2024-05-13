#include <iostream>
#include "tree.hpp"

int main() {
    while (true) {
        std::string instr;
        std::cout << "Please input a calculation:\n";
        std::cin >> instr;
        // myCalc::getTokens(instr);


        myCalc::CalcTree tree = myCalc::CalcTree(instr);
        std::cout << tree.evaluate();
        return 0;
    }
}