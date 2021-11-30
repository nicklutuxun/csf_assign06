#include "calc.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>

struct Calc {
private:
    // fields
    std::map<std::string, int> var_dict;
public:
    // public member functions
    Calc();
    ~Calc();

    int evalExpr(const std::string &expr, int &result);

    int var_exist(std::string var);
private:
    // tokenize expression
    std::vector<std::string> tokenize(const std::string &expr);

    // check whether operand is a variable
    int is_variable(std::string operand);

    // check whether operand is an integer
    int is_integer(std::string operand);

    // check whether op is an operator
    int is_operator(std::string op);
};

// constructor
Calc::Calc() {}

// destructor
Calc::~Calc() {}

extern "C" struct Calc *calc_create(void) {
    return new Calc();
}

extern "C" void calc_destroy(struct Calc *calc) {
    delete calc;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    return calc->evalExpr(expr, *result);
}

/**
 * Evaluate a given expression and store the answer into result
 * @return 1 if successfully evaluated, 0 otherwise
 */
extern "C" int Calc::evalExpr(const std::string &expr, int &result) {
    std::vector<std::string> tokens = tokenize(expr);       // tokenize the expression
    int num_tokens = tokens.size();     
    
    //  switch to correct number of tokens
    switch (num_tokens)
    {
        case 1:
        {
            std::string operand = tokens.at(0);     // get the operand
            if (is_integer(operand) == 1)       // if operand is integer
            {
                result = std::stoi(operand);
                return 1;       // evaluation succeeds
            }
            else if (is_variable(operand) == 1)     // if operand is variable
            {
                if (var_exist(operand) == 1)        // check whether variable is in the dictionary
                {
                    result = this->var_dict.at(operand);
                    return 1;       // evaluation succeeds
                }
            }
            
            break;
        }

        case 3:
        {
            std::string operand1 = tokens.at(0);
            std::string op = tokens.at(1);
            std::string operand2 = tokens.at(2);
            
            // INT op INT
            if (is_integer(operand1) == 1 && is_integer(operand2) == 1 && is_operator(op) == 1)     
            {
                switch (op[0])      // use this syntax so that switch works
                {
                case '+':
                    result = std::stoi(operand1) + std::stoi(operand2);
                    return 1;
                    break;
                case '-':
                    result = std::stoi(operand1) - std::stoi(operand2);
                    return 1;
                    break;
                case '*':
                    result = std::stoi(operand1) * std::stoi(operand2);
                    return 1;
                    break;
                case '/':
                    if (std::stoi(operand2) == 0) {     // divide by zero error
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    result = std::stoi(operand1) / std::stoi(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            }
            // VAR op INT
            else if (is_variable(operand1) == 1 && is_integer(operand2) == 1 && is_operator(op) == 1)
            {   
                if (var_exist(operand1) == 0)       // return 0 if operand1 is not in dictionary
                {
                    return 0;
                }
                
                switch (op[0])
                {
                case '+':
                    result = var_dict.at(operand1) + std::stoi(operand2);
                    return 1;
                    break;
                case '-':
                    result = var_dict.at(operand1) - std::stoi(operand2);
                    return 1;
                    break;
                case '*':
                    result = var_dict.at(operand1) * std::stoi(operand2);
                    return 1;
                    break;
                case '/':
                    if (std::stoi(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    result = var_dict.at(operand1) / std::stoi(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            } 
            // INT op VAR
            else if (is_integer(operand1) == 1 && is_variable(operand2) == 1 && is_operator(op) == 1)
            {   
                if (var_exist(operand2) == 0)       // check whether operand2 exist in dictionary
                {
                    return 0;
                }

                switch (op[0])
                {
                case '+':
                    result = std::stoi(operand1) + var_dict.at(operand2);
                    return 1;
                    break;
                case '-':
                    result = std::stoi(operand1) - var_dict.at(operand2);
                    return 1;
                    break;
                case '*':
                    result = std::stoi(operand1) * var_dict.at(operand2);
                    return 1;
                    break;
                case '/':
                    if (var_dict.at(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    result = std::stoi(operand1) / var_dict.at(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            }
            // VAR op VAR
            else if (is_variable(operand1) == 1 && is_variable(operand2) == 1 && is_operator(op) == 1)
            {   
                if (var_exist(operand1) == 0 || var_exist(operand2) == 0)       // return 0 is either operand1 or operand2 is not in dictionary
                {
                    return 0;
                }

                switch (op[0])
                {
                case '+':
                    result = var_dict.at(operand1) + var_dict.at(operand2);
                    return 1;
                    break;
                case '-':
                    result = var_dict.at(operand1) - var_dict.at(operand2);
                    return 1;
                    break;
                case '*':
                    result = var_dict.at(operand1) * var_dict.at(operand2);
                    return 1;
                    break;
                case '/':
                    if (var_dict.at(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    result = var_dict.at(operand1) / var_dict.at(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            }
            // VAR = INT
            else if (is_variable(operand1) == 1 && is_integer(operand2) == 1 && op == "=")
            {
                if (var_exist(operand1) == 0)       // insert into dictionary if operand1 does not exist
                {
                    var_dict.insert(std::pair<std::string, int>(operand1, std::stoi(operand2)));
                } else {        // otherwise change the value in dictionary
                    var_dict.at(operand1) = std::stoi(operand2);
                }
                result = var_dict.at(operand1);
                return 1;
            }
            // VAR = VAR
            else if (is_variable(operand1) == 1 && is_variable(operand2) == 1 && op == "=")
            {   
                if (var_exist(operand2) == 0)       // return 0 if operand2 is not in dictionary 
                {
                    return 0;
                }
                 else if (var_exist(operand1) == 0)     // insert into dictionary if operand1 does not exist
                {
                    var_dict.insert(std::pair<std::string, int>(operand1, var_dict.at(operand2)));
                } else {        // otherwise change the value in dictionary
                    var_dict.at(operand1) = var_dict.at(operand2);
                }
                result = var_dict.at(operand1);
                return 1;
            }
            
            break;
        }

        case 5:
        {
            std::string var = tokens.at(0);
            std::string op1 = tokens.at(1);
            std::string operand1 = tokens.at(2);
            std::string op2 = tokens.at(3);
            std::string operand2 = tokens.at(4);
            int temp_res = 0;       // store the value of expression after '='

            if (op1 != "=" || is_variable(var) == 0)        // return 0 if format invalid
            {
                return 0;
            }
            
            // VAR = INT op INT
            if (is_integer(operand1) == 1 && is_integer(operand2) == 1 && is_operator(op2) == 1)
            {
                switch (op2[0])
                {
                case '+':
                    temp_res = std::stoi(operand1) + std::stoi(operand2);
                    break;
                case '-':
                    temp_res = std::stoi(operand1) - std::stoi(operand2);
                    break;
                case '*':
                    temp_res = std::stoi(operand1) * std::stoi(operand2);
                    break;
                case '/':
                    if (std::stoi(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    temp_res = std::stoi(operand1) / std::stoi(operand2);
                    break;
                default:
                    break;
                }
            }
            // VAR = VAR op INT
            else if (is_variable(operand1) == 1 && is_integer(operand2) == 1 && is_operator(op2) == 1)
            {   
                if (var_exist(operand1) == 0)
                {
                    return 0;       // var1 does not exist in dictionary
                }

                switch (op2[0])
                {
                case '+':
                    temp_res = var_dict.at(operand1) + std::stoi(operand2);
                    break;
                case '-':
                    temp_res = var_dict.at(operand1) - std::stoi(operand2);
                    break;
                case '*':
                    temp_res = var_dict.at(operand1) * std::stoi(operand2);
                    break;
                case '/':
                    if (std::stoi(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    temp_res = var_dict.at(operand1) / std::stoi(operand2);
                    break;
                default:
                    break;
                }
            }
            // VAR = INT op VAR
            else if (is_integer(operand1) == 1 && is_variable(operand2) == 1 && is_operator(op2) == 1)
            {   
                if (var_exist(operand2) == 0)
                {
                    return 0;
                }

                switch (op2[0])
                {
                case '+':
                    result = std::stoi(operand1) + var_dict.at(operand2);
                    return 1;
                    break;
                case '-':
                    result = std::stoi(operand1) - var_dict.at(operand2);
                    return 1;
                    break;
                case '*':
                    result = std::stoi(operand1) * var_dict.at(operand2);
                    return 1;
                    break;
                case '/':
                    if (var_dict.at(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    result = std::stoi(operand1) / var_dict.at(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            } 
            // VAR = VAR op VAR
            else if (is_variable(operand1) == 1 && is_variable(operand2) == 1 && is_operator(op2) == 1)
            {   
                if (var_exist(operand1) == 0 || var_exist(operand2) == 0)
                {
                    return 0;       // var1 or var2 does not exist in dictionary
                }

                switch (op2[0])
                {
                case '+':
                    temp_res = var_dict.at(operand1) + var_dict.at(operand2);
                    return 1;
                    break;
                case '-':
                    temp_res = var_dict.at(operand1) - var_dict.at(operand2);
                    return 1;
                    break;
                case '*':
                    temp_res = var_dict.at(operand1) * var_dict.at(operand2);
                    return 1;
                    break;
                case '/':
                    if (var_dict.at(operand2) == 0) {
                        // std::cout << "Expression is invalid (attempt to divide by 0)." << std::endl;
                        return 0;
                    }
                    temp_res = var_dict.at(operand1) / var_dict.at(operand2);
                    return 1;
                    break;
                default:
                    break;
                }
            }
            

            if (var_exist(var) == 0) {      // insert into dictionary if not exist
                var_dict.insert(std::pair<std::string, int>(var, temp_res));
            }
            else {      // otherwise change value in dictionary
                var_dict.at(var) = temp_res;
            }
            result = temp_res;
            return 1;
            break;
        }
        default:
        {
            return 0;
        }
    }
    return 0;
}

/**
 * Tokenize given expression into seperate strings and store in a vector
 * @return a vector of strings
 */
extern "C" std::vector<std::string> Calc::tokenize(const std::string &expr) {
    std::vector<std::string> vec;
    std::stringstream s(expr);

    std::string tok;
    while (s >> tok) {
        vec.push_back(tok);
    }

    return vec;
}

/**
 * Check whether given operand is a valid variable
 * @return 1 is valid variable, 0 otherwise
 */
extern "C" int Calc::is_variable(std::string operand) {

    for (std::string::iterator it = operand.begin(); it != operand.end(); it++)
    {
        if (isalpha(*it) == 0)
        {
            return 0;       // operand is not valid variable
        }
        
    }
    
    return 1;       // operand is variable
}

/**
 * Check whether given operand is a valid integer
 * @return 1 is valid integer, 0 otherwise
 */
extern "C" int Calc::is_integer(std::string operand) {
    for (std::string::iterator it = operand.begin(); it != operand.end(); it++)
    {   
        if ((it == operand.begin()) && (*it == 45))
        {
            continue;
        }
        
        
        if (isdigit(*it) == 0)
        {
            return 0;       // operand is not valid integer
        }
        
    }

    return 1;       // operand is integer
}

/**
 * Check whether a given variable exists in var_dictionary
 * @return 1 if exist, 0 otherwise
 */
extern "C" int Calc::var_exist(std::string var) {
    if (var_dict.find(var) != var_dict.end())
    {
        return 1;       // variable found
    }
    
    return 0;       // variable not found
}

/**
 * Check whether a give string op is a valid operator
 * @return 1 if is any of "+", "-", "*", "/", 0 otherwise
 */
extern "C" int Calc::is_operator(std::string op) {
    std::vector<std::string> operators {"+", "-", "*", "/"};
    if (std::find(operators.begin(), operators.end(), op) != operators.end())
    {
        return 1;       // is valid operator
    }
    
    return 0;       // is not valid operator
}