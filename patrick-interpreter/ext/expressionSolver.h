#ifndef EXPRESSIONSOLVER_H
#define EXPRESSIONSOLVER_H

#include <algorithm>
#include <string>
#include <stdexcept>
#include <math.h>

namespace Solver{
inline char priorityToCharacter(int p){
    switch(p){
    case 0: return '+';
    case 1: return '-';
    case 2: return '*';
    case 3: return '/';
    case 4: return '^';
    default: throw std::runtime_error("fuck");
    }
}

inline double toDouble(const std::string& number, const char op = '-'){
    if(number.empty())
    {
        switch(op){
        case '+': return 0;
        case '-': return 0;
        case '*': return 1;
        case '/': return 1;
        case '^': return 1;
        default: throw std::runtime_error("ridi");
        }
    }

    return std::stod(number);
}

inline bool isOperator(const char ch){
    std::string operatos = "-+*/^";
    return operatos.find(ch) != std::string::npos;
}

inline std::string extractOperand(const std::string& string, bool reverse = false){
    std::string buffer;

    if(reverse){
        for(auto i = string.rbegin(); i != string.rend(); ++i){
            if(isOperator(*i)) goto ret;
            buffer.push_back(*i);
        }
        std::reverse(buffer.begin(), buffer.end());
    } else {
        for(const char& ch : string){
            if(isOperator(ch)) goto ret;
            buffer.push_back(ch);
        }
    }

ret:
    return buffer;
}

inline std::string extractBetweenParanteses(const std::string& string, bool reverse = false){
    std::string buffer;
    int paranteses = 0;


    if(reverse){
        for (auto i = string.rbegin(); i != string.rend(); ++i ) {
            if(*i == ')') {
                paranteses++;
            }

            if(*i == '(') {
                paranteses--;
            }

            buffer.push_back(*i);

            if(paranteses == 0){
                std::reverse(buffer.begin(), buffer.end());
                goto ret;
            }

        }
    } else {
        for(const char& ch : string){
            if(ch == '(') {
                paranteses++;
            }

            if(ch == ')') {
                paranteses--;
            }

            buffer.push_back(ch);

            if(paranteses == 0)
                goto ret;

        }
    }

ret:
    return buffer;
}

inline std::string removeParantesses_copy(const std::string& string){
    std::string s = string;
    if(string[0] == '('){
        s.pop_back();
        s.erase(0,1);
    }
    return s;
}

inline std::string removeParantesses(std::string& string){
    if(string[0] == '('){
        string.pop_back();
        string.erase(0,1);
    }
    return string;
}

inline std::pair<size_t, char> findOperatorWithHigherPriority(const std::string& string){
    const int priority = 4;
    size_t index = std::string::npos;
    char op = '\0';
    for(int i = priority; i >= 0; i--)
    {
        index = string.find(priorityToCharacter(i));
        if(index != std::string::npos){
            op = priorityToCharacter(i);
            goto ret;
        }
    }

ret:
    return {index, op};
}

struct Expression{
    std::string left;
    std::string right;
    char op;
};

//there is no space in the string

inline std::string extractLeft(const std::string& string, size_t index){
    if(string[index - 1] == ')'){
        return extractBetweenParanteses(string.substr(0, index), true);
    } else {
        return extractOperand(string.substr(0, index), true);
    }
}

inline std::string extractRight(const std::string& string, size_t index){
    if(string[index + 1] == '('){
        return extractBetweenParanteses(string.substr(index + 1));
    } else {
        return extractOperand(string.substr(index + 1));
    }
}

inline Expression extractOperands(const std::string& string, std::pair<size_t, char> op)
{
    return {
        extractLeft(string, op.first),
        extractRight(string, op.first),
        op.second
    };
}

inline bool hasOperator(std::string& string)
{
    const auto op = findOperatorWithHigherPriority(string);
    return op.first != std::string::npos;
}

inline double applyOperator(const double fop, const double sop, const char op){
    switch(op){
    case '+': return fop + sop;
    case '-': return fop - sop;
    case '*': return fop * sop;
    case '/': return fop / sop;
    case '^': return std::pow(fop, sop);
    default: throw std::runtime_error("ridi-2");
    }
}

inline double applyOperator(const std::string& fop, const std::string& sop, const char op){
    return applyOperator(toDouble(fop), toDouble(sop), op);
}

inline double applyOperator(const double fop, const std::string& sop, const char op){
    return applyOperator(fop, toDouble(sop), op);
}

inline double applyOperator(const std::string& fop, const double sop, const char op){
    return applyOperator(toDouble(fop), sop, op);
}

struct RecursivePartResult {
    double retVal;
    size_t findex = 0;
    size_t lindex = 0;
    size_t length() const {return findex + lindex; }
};

inline double solveExpression_h(const std::string& string){
    const auto op = findOperatorWithHigherPriority(string);

    if(op.first == std::string::npos)
    {
        removeParantesses_copy(string);
        return std::stod(string);
    }

    Expression ex = extractOperands(string, op);

    removeParantesses(ex.left);
    removeParantesses(ex.right);

    if(!hasOperator(ex.left) && !hasOperator(ex.right))
    {
        return applyOperator(ex.left, ex.right, ex.op);
    }

    if(hasOperator(ex.left) && hasOperator(ex.right))
    {
        return applyOperator(solveExpression_h(ex.left), solveExpression_h(ex.right), ex.op);
    }

    if(hasOperator(ex.left))
    {
        return applyOperator(solveExpression_h(ex.left), ex.right, ex.op);
    }

    if(hasOperator(ex.right))
    {
        return applyOperator(ex.left, solveExpression_h(ex.right), ex.op);
    }

    return 0;
}

inline RecursivePartResult solveExpression_h(const std::string& string, size_t * iteration){
    RecursivePartResult result;
    const auto op = findOperatorWithHigherPriority(string);
    Expression ex;

    ex = extractOperands(string, op);

    if(!iteration)
    {
        result.findex = string.find(ex.left);
        result.lindex = result.findex + ex.left.length() + 1 + ex.right.length();
    }

    removeParantesses(ex.left);
    removeParantesses(ex.right);

    if(!hasOperator(ex.left) && !hasOperator(ex.right))
    {
        result.retVal = applyOperator(ex.left, ex.right, ex.op);
        goto ret;
    }

    if(hasOperator(ex.left) && hasOperator(ex.right))
    {
        result.retVal = applyOperator(solveExpression_h(ex.left), solveExpression_h(ex.right), ex.op);
        goto ret;
    }

    if(hasOperator(ex.left))
    {
        result.retVal = applyOperator(solveExpression_h(ex.left), ex.right, ex.op);
        goto ret;
    }

    if(hasOperator(ex.right))
    {
        result.retVal = applyOperator(ex.left, solveExpression_h(ex.right), ex.op);
        goto ret;
    }

ret:
    return result;
}

inline double solveExpression(const std::string& string){
    std::string copy = string;
    RecursivePartResult result;

    while(hasOperator(copy)){
        result = solveExpression_h(copy, nullptr);
        copy.replace(result.findex, result.length(), std::to_string(result.retVal));
    }

    removeParantesses_copy(copy);
    result.retVal = std::stod(copy);

    return result.retVal;
}
}

#endif // EXPRESSIONSOLVER_H
