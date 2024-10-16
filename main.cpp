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
    }   std::cout << std::endl;
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
    };

    std::unordered_set<std::string> functions = {
        "ln",
        "sin",
        "cos",
        "tan",
    };

    std::unordered_set<std::string> leftAssociated = {
        "(",
    };

    std::cout << "equation:   ";
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

    std::cout << "tokensised: ";
    for (auto i : tokenin) {
        std::cout << i << " ";
    }   std::cout << "\n";

    /* shunting yard algorithm 
       https://en.wikipedia.org/wiki/Shunting_yard_algorithm 
    */

    std::vector<std::string> output;  /* output stack. when only 1 number left here, solve is complete. */
    std::vector<std::string> opstack; /* operator stack */
    opstack.push_back("");

    std::cout << tokenin.size() << "\n";
    for (int i = 0; i < tokenin.size(); i++) {
        std::cout << i << " | "; printStringArray(output);
        std::optional<double> a = getNumber(tokenin[i]);
        std::string currentToken = tokenin[i]; 
        std::string topelement;
        std::string temp;
        if (a != NULL) {
            output.push_back(std::to_string(*a));
        } else {
            if (functions.contains(currentToken)) {
                std::cout << "functions" << std::endl;
                opstack.push_back(currentToken);

            } else if (operators.contains(currentToken)) {
                topelement = opstack.back();
                while (topelement != "(" && (operators[topelement] > operators[currentToken] 
                       || (operators[currentToken] == operators[topelement] 
                       && !leftAssociated.contains(currentToken)))) {
                        /* jesus christ thats some logic and a half */
                    std::cout << "operators" << std::endl;
                    temp = opstack.back();
                    opstack.pop_back();
                    output.push_back(temp);
                    opstack.push_back(currentToken);
                }

            } else if (currentToken == ",") {
                while (opstack.back() != "(") {
                    std::cout << "commas" << std::endl;
                    temp = opstack.back();
                    opstack.pop_back();
                    output.push_back(temp);
                }

            } else if (currentToken == "(") {
                opstack.push_back("(");
                std::cout << "open bracket" << std::endl;

            } else if (currentToken == ")") {
                while (opstack.back() != "(") {
                    std::cout << "close bracket" << std::endl;
                    if (opstack.size() == 0) std::cout << "error: mismatched parenthesis!\n";
                    temp = opstack.back();
                    opstack.pop_back();
                    output.push_back(temp);
                }
                if (opstack.back() != "(") std::cout << "something wrong";
                opstack.pop_back();
                if (functions.contains(opstack.back())) {
                    temp = opstack.back();
                    opstack.pop_back();
                    output.push_back(temp);
                }
            }
        }
    }
    for (int i = 0; i < opstack.size(); i++) {
        std::string temp = opstack.back();
        opstack.pop_back();
        output.push_back(temp);
    }

    for (int i = 0; i < output.size(); i++) {
        std::cout << output[i] << " ";
    }

    std::cout << std::endl;
}
