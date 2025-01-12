#include "main.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <exception>
#include <functional>
#include <iomanip>
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
            std::cout << "f_" << std::get<double>(token);
        } else {
            std::cout << "s_" << std::get<std::string>(token);
        }
        std::cout << " ";
    }
}


    /* hashset used to access elements without using integer values. 
       also editable at runtime unlike an enum.
       follows the standard coding operators, eg * instead of X */

std::unordered_map<std::string, int> operators = {
    {"+", 0},
    {"-", 0},
    {"/", 1},
    {"*", 1},
    {"(", 3},
    {")", 3},
    {"%", 1},
    {"!", 2},
    {"^", 2},
    {"_", -1},
    {"log", 2},
    {",", 3}
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
    "sqrt",
    "log",
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

std::unordered_map<std::string, std::function<double(double,double)>> twoOpMap {
    {"+",   [](double a, double b) { return b + a; }},
    {"-",   [](double a, double b) { return b - a; }},
    {"*",   [](double a, double b) { return b * a; }},
    {"/",   [](double a, double b) { return b / a; }},
    {"%",   [](double a, double b) { return std::fmod(b, a); }},
    {"^",   [](double a, double b) { return pow(b, a); }},
    {"log", [](double a, double b) { return log(a) / log(b); }},
};

std::unordered_map<std::string, std::function<double(double)>> singOpMap {
    {"ln",  [](double a) { return log(a); }},
    {"sin", [](double a) { return sin(a); }},
    {"cos", [](double a) { return cos(a); }},
    {"tan", [](double a) { return tan(a); }},
    {"!",   [](double a) { return std::tgamma(a+1); }},
    {"sqrt",[](double a) { return pow(a, 0.5); }},
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
            if (twoOpMap.contains(std::get<std::string>(postfixeq[i]))) {
                if (ansstack.size() > 0) {
                    temp1 = std::get<double>(ansstack.back());
                    ansstack.pop_back();
                } else {
                    temp1 = 0;
                }
                if (ansstack.size() > 0) {
                    temp2 = std::get<double>(ansstack.back());
                    ansstack.pop_back();
                } else {
                    temp2 = 0;
                }

                ansstack.push_back(twoOpMap[std::get<std::string>(postfixeq[i])](temp1, temp2));

            } else if (singOpMap.contains(std::get<std::string>(postfixeq[i]))) {
                temp1 = std::get<double>(ansstack.back());
                ansstack.pop_back();
                ansstack.push_back(singOpMap[std::get<std::string>(postfixeq[i])](temp1));
            }
        }
        // std::cout << i << " "; printTokenArray(ansstack); std::cout << std::endl;
    }
    return std::get<double>(ansstack.front());
}

equation parseEquasion(const std::string& equationin) {
    equation tokenin;
    std::string numberbuffer = "";
    std::string operbuffer = "";
    std::vector<std::string> strbuffer = {};
    
    /* parse and tokenise input to tokenin */

    for (int i = 0; i < equationin.size(); i++) {
        // std::cout << i << " " << equationin[i] << " | "; printStringArray(strbuffer); std::cout << std::endl;
        if (equationin[i] == ' ') continue;                 /* ignore all spaces */

        if (numberbuffer.size() == 0 && equationin[i] == '-' && strbuffer.back() != "(" && strbuffer.back() != ")") {
            numberbuffer += '-';
            continue;
        } 

        if (isdigit(equationin[i]) || equationin[i] == '.') {
            numberbuffer += equationin[i];                  /* input all numbers and decimal points */
            continue;
        }

        operbuffer += equationin[i];
        if (operators.contains(operbuffer) || functions.contains(operbuffer)) {   /* extra check to make sure not to */
            if (numberbuffer != "")                                               /* accidentally add an empty token */
                strbuffer.push_back(numberbuffer);
            numberbuffer = "";
            if (strbuffer.size() > 0 && (operbuffer == "(") && (getNumber(strbuffer.back()) != std::nullopt || strbuffer.back() == ")")) {
                strbuffer.push_back("*");
            }
            if (strbuffer.size() > 0 && getNumber(strbuffer.back()) != std::nullopt && functions.contains(operbuffer)) 
                strbuffer.push_back("*");
            strbuffer.push_back(operbuffer);
            operbuffer = "";
        }

        if (strbuffer.size() == 0) continue;
        if (strbuffer.back() == "-" && strbuffer[strbuffer.size()-2] == "-") { /* all this here is because of idiotic edge cases that */
            strbuffer.pop_back();                                              /* nobody is gonna encounter but i have to add or people */
            strbuffer.pop_back();                                              /* will complain  */
            strbuffer.push_back("+");
        }

        if (strbuffer.back() == "+" && strbuffer[strbuffer.size()-2] == "+") {
            strbuffer.pop_back();
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
    return tokenin;
}

int main(int argc, char* argv[]) {
    for (;;) {
        std::cout << "equation:  ";
        std::string equationin;
        std::getline(std::cin, equationin);
        equation tokenin;

        tokenin = parseEquasion(equationin);

        /* print tokenised input */
        // std::cout << "tokenised: "; printTokenArray(tokenin); std::cout << "\n";

        equation postfixeq = convertToPostfix(tokenin);

        // std::cout << "postfix:   "; printTokenArray(postfixeq); std::cout << std::endl;
        double answer = resolvePostfix(postfixeq);
        std::cout << std::setprecision(10) << "answer:    " << answer << std::endl;
    }
}
