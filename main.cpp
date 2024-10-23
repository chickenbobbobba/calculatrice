#include "main.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

using equation = std::vector<std::variant<double, std::string>>;

std::optional<double> getNumber(const std::string& input) {
    try {
        return std::stod(input);
    } catch(std::exception a) {
        return std::nullopt;
    }
}

std::string removeTrailingZeros(const std::string& str) {
    size_t decimal = str.find('.');
    if (decimal == std::string::npos)
        return str;

    size_t last = str.find_last_not_of('0');
    if (last == decimal) {
        return str.substr(0, decimal);
    }
    return str.substr(0, last + 1);
}

void printStringArray(const std::vector<std::string>& thing) {
    for (int i = 0; i < thing.size(); i++) {
        std::cout << thing[i] << " ";
    }
}

void printTokenArray(const equation& tokens) {
    for (const auto& token : tokens) {
        if (std::holds_alternative<double>(token)) {
            std::cout << "f" << std::get<double>(token);
        } else {
            std::cout << "s" << std::get<std::string>(token);
        }
        std::cout << ".";
    }
}


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

equation convertToPostfix(const equation& tokenin) {
    equation poststack;  /* output stack. when only 1 number left here, solve is complete. */
    equation opstack;  /* operator stack */

    /* shunting yard algorithm 
       https://en.wikipedia.org/wiki/Shunting_yard_algorithm 
    */

    opstack.push_back("_");

    for (int i = 0; i < tokenin.size(); i++) {
        
        std::string currentToken = "";
        if (std::holds_alternative<double>(tokenin[i])) {
            double numToken = std::get<double>(tokenin[i]);
            poststack.push_back(numToken);
            continue;
        }

        currentToken = std::get<std::string>(tokenin[i]);

        std::string topElement;
        // std::cout << i << " " << currentToken << " |#| "; printTokenArray(poststack); std::cout << " |#| "; printTokenArray(opstack); std::cout << std::endl;
        
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

    return poststack;
}

std::unordered_map<std::string, std::function<double(double,double)>> opmap {
    {"+", [](double a, double b) { return b + a; }},
    {"-", [](double a, double b) { return b - a; }},
    {"*", [](double a, double b) { return b * a; }},
    {"/", [](double a, double b) { return b / a; }},
    {"%", [](double a, double b) { return std::fmod(b,a); }},
};

double resolvePostfix(const equation& postfixeq) {
    equation ansstack = {};
    double temp1;
    double temp2;
    for (int i = 0; i < postfixeq.size(); i++) {
        if (std::holds_alternative<double>(postfixeq[i])) {
            ansstack.push_back(postfixeq[i]);
        } else {
            if (std::get<std::string>(postfixeq[i]) == "_") continue;
            temp1 = std::get<double>(ansstack.back());
            ansstack.pop_back();
            temp2 = std::get<double>(ansstack.back());
            ansstack.pop_back();
            ansstack.push_back(opmap[std::get<std::string>(postfixeq[i])](temp1, temp2));
        }
        // std::cout << i << " "; printTokenArray(ansstack); std::cout << std::endl;
    }
    return std::get<double>(ansstack.front());
}

int main(int, char**) {
    std::cout << "equation:  ";
    std::string equationin;
    std::getline(std::cin, equationin);

    equation tokenin;
    std::string numberbuffer = "";
    std::string operbuffer = "";
    std::vector<std::string> strbuffer = {};
    
    /* parse and tokenise input to tokenin */

    for (int i = 0; i < equationin.size(); i++) {
        if (equationin[i] == ' ') continue;                 /* ignore all spaces */

        if (isdigit(equationin[i]) || equationin[i] == '.') {
            numberbuffer += equationin[i];                  /* input all numbers and decimal points */
            continue;
        }

        operbuffer += equationin[i];
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
        std::optional<double> temp = getNumber(strbuffer[i]);
        if (temp != std::nullopt) {                         /* is it a number? */ 
            tokenin[i] = *temp;
        } else {
            tokenin[i] = strbuffer[i];
        }
    }

    /* print tokenised input */
    
    std::cout << "tokenised: ";

    printTokenArray(tokenin); std::cout << "\n";
    equation postfixeq = convertToPostfix(tokenin);

    std::cout << "postfix:   "; printTokenArray(postfixeq); std::cout << std::endl;
    float answer = resolvePostfix(postfixeq);
    std::cout << "answer:    " << answer << std::endl;
}
