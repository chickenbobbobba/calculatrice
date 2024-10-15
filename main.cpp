#include "main.h"

int main(int, char**) {
    std::unordered_set<char> operators = {
        '+',
        '-',
        '/',
        '*',
        '(',
        ')',
        '%',
        '!',
        '^',
    };

    std::string equation;
    std::getline(std::cin, equation);

    std::vector<std::string> tokenin;
    std::string temp = "";
    
    // parse input words into the std::vector tokenin
    for (int i = 0; i < equation.size(); i++) {
        if (!operators.contains(equation[i])) {
            temp += equation[i];
        } else {
            tokenin.push_back(temp);
            temp = equation[i];
            tokenin.push_back(temp);
            temp = "";
        }
    }
    tokenin.push_back(temp);

    for (int i = 0; i < tokenin.size(); i++) {
        std::cout << tokenin[i] << "_";
    }

    std::vector<std::string> output;
    std::vector<std::string> ostack;

    
}
