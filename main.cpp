#include "main.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

std::optional<double> getNumber(std::string& input) {
    try {
        return std::stod(input);
    } catch(std::exception a) {
        return NULL;
    }
}

void printStringArray(std::vector<std::string>& thing) {
    for (int i = 0; i < thing.size(); i++) {
        std::cout << thing[i] << " ";
    }
}

int main(int, char**) {

    /* hashset used to access elements without using integer values. 
       also editable at runtime unlike an enum.
       follows the standard coding operators, eg * instead of X */

    std::unordered_map<std::string, int> operators = {
        {"+", 0},
        {"-", 0},
        {"/", 8},
        {"*", 8},
        {"(", 10},
        {")", 10},
        {"%", 8},
        {"!", 9},
        {"^", 9},
        {"_", -1},
    };

    std::unordered_set<std::string> operatorIgnoreList = {
        "(",
        ")",
    };

    std::unordered_set<std::string> functions = {
        "ln",
        "sin",
        "cos",
        "tan",
    };

    std::unordered_set<std::string> rightAssociated = {
        "^",
    };

    std::cout << "equation:  ";
    std::string equation;
    std::getline(std::cin, equation);

    std::vector<std::string> tokenin;
    std::string numberbuffer = "";
    std::string operbuffer = "";
    
    /* parse and tokenise input to tokenin */

    for (int i = 0; i < equation.size(); i++) {
        if (equation[i] == ' ') continue;                 /* ignore all spaces */

        if (isdigit(equation[i]) || equation[i] == '.') {
            numberbuffer.push_back(equation[i]);          /* input all numbers and decimal points */
            continue;
        }

        operbuffer += equation[i];
        if (operators.contains(operbuffer) || functions.contains(operbuffer)) {   /* extra check to make sure not to */
            if (numberbuffer != "")                                               /* accidentally add an empty token */
                tokenin.push_back(numberbuffer);
            numberbuffer = "";
            tokenin.push_back(operbuffer);
            operbuffer = "";
        }
    }
    tokenin.push_back(numberbuffer);

    /* print tokenised input */

    std::cout << "tokenised: ";
    for (auto i : tokenin) {
        std::cout << i << ".";
    }   std::cout << "\n";

    /* shunting yard algorithm 
       https://en.wikipedia.org/wiki/Shunting_yard_algorithm 
    */

    std::vector<std::string> output;  /* output stack. when only 1 number left here, solve is complete. */
    std::vector<std::string> opstack; /* operator stack */
    std::string temp;
    opstack.push_back("_");

    for (int i = 0; i < tokenin.size(); i++) {

        std::string currentToken = tokenin[i]; 
        std::cout << i << " " << currentToken << " | "; printStringArray(output); std::cout << " | "; printStringArray(opstack); std::cout << std::endl;
        std::optional<double> number = getNumber(tokenin[i]);
        std::string topelement;
        
        if (number != NULL) {
            output.push_back(std::to_string(*number));
        } else {
            if (functions.contains(currentToken)) {
                std::cout << "functions" << std::endl;
                opstack.push_back(currentToken);

            } else if (operators.contains(currentToken) && !operatorIgnoreList.contains(currentToken)) {
                if (opstack.size() == 0) {
                    topelement = "_";
                } else {
                    topelement = opstack.back();
                }
                while (topelement != "(" && topelement != "_" && 
                      (operators[topelement] > operators[currentToken] 
                      || (operators[currentToken] == operators[topelement] 
                      && !rightAssociated.contains(currentToken)))) {
                        /* jesus christ thats some logic and a half */
                    
                    // std::cout << "operators" << std::endl;
                    /*std::cout << "opstack "; printStringArray(opstack);
                    std::cout << std::endl;
                    std::cout << "output "; printStringArray(output);
                    std::cout << std::endl; */
                    output.push_back(opstack.back());
                    opstack.pop_back();
                    topelement = opstack.back();
                }
                opstack.push_back(currentToken);

            } else if (currentToken == ",") {
                std::cout << "commas" << std::endl;
                while (opstack.back() != "(") {
                    temp = opstack.back();
                    opstack.pop_back();
                    output.push_back(temp);
                }

            } else if (currentToken == "(") {
                opstack.push_back("(");
                std::cout << "open bracket" << std::endl;

            } else if (currentToken == ")") {
                std::cout << "close bracket" << std::endl;
                while (opstack.back() != "(") {
                    if (opstack.size() == 0) std::cout << "error: mismatched parenthesis!\n";
                    output.push_back(opstack.back());
                    opstack.pop_back();
                }

                if (opstack.back() != "(")
                    std::cout << "ono\n";
                
                opstack.pop_back();

                if (functions.contains(opstack.back())) {                    
                    output.push_back(opstack.back());
                    opstack.pop_back();
                }
            }
        }
    }
    for (int i = 0; i < opstack.size(); i++) {
        std::string a = opstack.back();
        opstack.pop_back();
        output.push_back(a);
    }

    std::cout << "final postfix: "; printStringArray(output); std::cout << std::endl;
}