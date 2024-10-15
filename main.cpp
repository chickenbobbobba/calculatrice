#include "main.h"
#include <cmath>
#include <cstddef>
#include <exception>

std::optional<double> getNumber(std::string& input) {
    try {
        return std::stod(input);
    } catch(std::exception a) {
        return NULL;
    }
}

int main(int, char**) {

    /* hashset used to access elements without using integer values. 
       also editable at runtime unlike an enum.
       follows the standard coding operators, eg * instead of X */

    std::unordered_set<std::string> operators = {
        "+",
        "-",
        "/",
        "*",
        "(",
        ")",
        "%",
        "!",
        "^",
        "banan",
    };
    std::cout << "equation: ";
    std::string equation;
    std::getline(std::cin, equation);

    std::vector<std::string> tokenin;
    std::string numberbuffer = "";
    std::string operbuffer = "";
    
    /* parse and tokenise input to tokenin */

    for (int i = 0; i < equation.size(); i++) {
        if (equation[i] == ' ') continue;

        if (isdigit(equation[i])) {
            numberbuffer.push_back(equation[i]);
            continue;
        }

        operbuffer += equation[i];
        if (operators.contains(operbuffer)) {
            if (numberbuffer != "")
                tokenin.push_back(numberbuffer);
            numberbuffer = "";
            tokenin.push_back(operbuffer);
            operbuffer = "";
        }
    }
    tokenin.push_back(numberbuffer);

    /* print tokenised input */

    std::cout << "tokensised: ";
    for (auto i : tokenin) {
        std::cout << i << " ";
    }   std::cout << "\n";

    /* shunting yard algorithm */

    std::vector<std::string> output; /* output stack. when only 1 number left here, solve is complete. */
    std::vector<std::string> ostack; /* operator stack */

    for (int i = 0; i < tokenin.size(); i++) {
        std::optional<double> a = getNumber(tokenin[i]);
        if (a != NULL) {
            std::cout << *a << " ";
        } else {
            std::cout << "optional ";
        }
    }
 }
