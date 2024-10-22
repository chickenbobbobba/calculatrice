#include "main.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

std::optional<double> getNumber(std::string& input) {
    try {
        return std::stod(input);
    } catch(std::exception a) {
        return std::nullopt;
    }
}

std::string removeTrailingZeros(std::string str) {
    size_t decimal = str.find('.');
    if (decimal == std::string::npos)
        return str;

    size_t last = str.find_last_not_of('0');
    if (last == decimal) {
        return str.substr(0, decimal);
    }
    return str.substr(0, last + 1);
}

void printStringArray(std::vector<std::string> thing) {
    for (int i = 0; i < thing.size(); i++) {
        std::cout << thing[i] << " ";
    }
}

void printTokenArray(const std::vector<std::variant<float, std::string>>& tokens) {
    for (const auto& token : tokens) {
        if (std::holds_alternative<float>(token)) {
            std::cout << "f" << std::get<float>(token);
        } else {
            std::cout << "s" << std::get<std::string>(token);
        }
        std::cout << ".";
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

    std::vector<std::variant<float, std::string>> tokenin;
    std::string numberbuffer = "";
    std::string operbuffer = "";
    std::vector<std::string> strbuffer = {};
    
    /* parse and tokenise input to tokenin */

    for (int i = 0; i < equation.size(); i++) {
        if (equation[i] == ' ') continue;                 /* ignore all spaces */

        if (isdigit(equation[i]) || equation[i] == '.') {
            numberbuffer += equation[i];                  /* input all numbers and decimal points */
            continue;
        }

        operbuffer += equation[i];
        if (operators.contains(operbuffer) || functions.contains(operbuffer)) {   /* extra check to make sure not to */
            if (numberbuffer != "")                                               /* accidentally add an empty token */
                strbuffer.push_back(numberbuffer);
            numberbuffer = "";
            strbuffer.push_back(operbuffer);
            operbuffer = "";
        }
    }

    strbuffer.push_back(numberbuffer);
    tokenin.resize(strbuffer.size());
    for (int i = 0; i < strbuffer.size(); i++) {
        std::optional<float> temp = getNumber(strbuffer[i]);
        if (temp != std::nullopt) {                         /* is it a number? */ 
            tokenin[i] = *temp;
        } else {
            tokenin[i] = strbuffer[i];
        }
    }

    /* print tokenised input */
    
    std::cout << "tokenised: ";

    printTokenArray(tokenin); std::cout << "\n";

    /* shunting yard algorithm 
       https://en.wikipedia.org/wiki/Shunting_yard_algorithm 
    */

    std::vector<std::variant<float, std::string>> poststack;  /* output stack. when only 1 number left here, solve is complete. */
    std::vector<std::variant<float, std::string>> opstack;  /* operator stack */

    opstack.push_back("_");

    for (int i = 0; i < tokenin.size(); i++) {
        
        std::string currentToken = "";
        if (std::holds_alternative<float>(tokenin[i])) {
            float numToken = std::get<float>(tokenin[i]);
            poststack.push_back(numToken);
            continue;
        }

        currentToken = std::get<std::string>(tokenin[i]);

        std::string topElement;
        std::cout << i << " " << currentToken << " |#| "; printTokenArray(poststack); std::cout << " |#| "; printTokenArray(opstack); std::cout << std::endl;
        
        if (functions.contains(currentToken)) {
            opstack.push_back(currentToken);

        } else if (operators.contains(currentToken) && !operatorIgnoreList.contains(currentToken)) {
            if (opstack.size() == 0) {
                topElement = "_";
            } else {
                topElement = std::get<std::string>(opstack.back());
            }
            while (topElement != "(" && topElement != "_" && 
                  (operators[topElement] > operators[currentToken] 
                  || (operators[currentToken] == operators[topElement] 
                  && !rightAssociated.contains(currentToken)))) {
                
                poststack.push_back(opstack.back());
                opstack.pop_back();
                topElement = std::get<std::string>(opstack.back());
            }
            opstack.push_back(currentToken);

        } else if (currentToken == ",") {
            while (std::get<std::string>(opstack.back()) != "(") {
                poststack.push_back(opstack.back());
                opstack.pop_back();
            }

        } else if (currentToken == "(") {
            opstack.push_back("(");

        } else if (currentToken == ")") {
            while (std::get<std::string>(opstack.back()) != "(") {
                if (opstack.size() == 0) std::cout << "error: mismatched parenthesis!\n";
                poststack.push_back(opstack.back());
                opstack.pop_back();
            }

            if (std::get<std::string>(opstack.back()) != "(")
                std::cout << "ono\n";
              
            opstack.pop_back();
            if (functions.contains(std::get<std::string>(opstack.back()))) {                    
                poststack.push_back(opstack.back());
                opstack.pop_back();
            }
        }
    }

    while (!opstack.empty()) {
        poststack.push_back(opstack.back());
        opstack.pop_back();
    }

    std::cout << "final postfix: "; printTokenArray(poststack); std::cout << std::endl;
}
