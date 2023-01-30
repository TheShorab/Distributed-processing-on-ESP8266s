#ifndef EXPRESSION_SOLVER_H
#define EXPRESSION_SOLVER_H

#include "llcore.h"

#include <algorithm>
#include <math.h>

class ExpressionSolver{
public:

    ExpressionSolver(const std::string& expression, LLCore * core)
        : interpreter(core)
    {
        std::string copy = interpreter->removeSpaces(expression);
        _result = solveExpressionBase(copy);
    }

private:

    inline char priorityToCharacter(int p){
        auto& pcb = interpreter->pcb;
        switch(p){
        case 0: return '+';
        case 1: return '-';
        case 2: return '*';
        case 3: return '/';
        case 4: return '%';
        case 5: return '^';
        default: LLCore::terminateExecution(E("Priority Of Of Range!"));
        }

        NON_VOID(char)
    }

    inline bool isOperator(const char ch){
        std::string operatos = "-+*/^";
        return operatos.find(ch) != std::string::npos;
    }

    inline std::string extractOperand(const std::string& string, bool reverse = false){
        std::string buffer;

        if(reverse){
            for(auto i = string.rbegin(); i != string.rend(); ++i){
                if(isOperator(*i) || *i == '(' || *i == ')' )
                    goto ret;

                buffer.push_back(*i);
            }
        } else {
            for(const char& ch : string){
                if(isOperator(ch) || ch == '(' || ch == ')') goto ret;
                buffer.push_back(ch);
            }
        }

    ret:
        if(reverse) std::reverse(buffer.begin(), buffer.end());
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
        if(reverse) std::reverse(buffer.begin(), buffer.end());
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
        const int priority = 5;
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
        case '%': return (long long int)(fop) % (long long int)(sop);
        case '^': return std::pow(fop, sop);
        default: return fop;
        }
    }

    inline double applyOperator(const std::string& fop, const std::string& sop, const char op)
    {
        return applyOperator(interpreter->accessVariable(fop), interpreter->accessVariable(sop), op);
    }

    inline double applyOperator(const double fop, const std::string& sop, const char op)
    {
        return applyOperator(fop, interpreter->accessVariable(sop), op);
    }

    inline double applyOperator(const std::string& fop, const double sop, const char op)
    {
        return applyOperator(interpreter->accessVariable(fop), sop, op);
    }

    struct RecursivePartResult {
        double retVal;
        size_t findex = 0;
        size_t lindex = 0;
        size_t length() const {return lindex - findex + 1; }
    };

    inline double solveExpressionBase(std::string& string){
        interpreter->trim(string);
        if(interpreter->isNumber(string)) return interpreter->toDouble(string);

        double finalResult = std::numeric_limits<double>::min();
        RecursivePartResult result;
        const auto op = findOperatorWithHigherPriority(string);
        Expression ex;

        ex = extractOperands(string, op);

        {
            std::string s;
            if(ex.left == ex.right && ex.op == '\0')
                s = ex.left;
            else s = ex.left + ex.op + ex.right;
            result.findex = string.find(s);
            result.lindex = result.findex + s.length() - 1;
        }

        removeParantesses(ex.left);
        removeParantesses(ex.right);

        if(!hasOperator(ex.left) && !hasOperator(ex.right))
        {
            result.retVal = applyOperator(ex.left, ex.right, ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return interpreter->toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.left) && hasOperator(ex.right))
        {
            result.retVal = applyOperator(solveExpressionBase(ex.left), solveExpressionBase(ex.right), ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return interpreter->toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.left))
        {
            result.retVal = applyOperator(solveExpressionBase(ex.left), ex.right, ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return interpreter->toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.right))
        {
            result.retVal = applyOperator(ex.left, solveExpressionBase(ex.right), ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return interpreter->toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

    ret:
        return finalResult;
    }

public:

    double result() const
    {
        return _result;
    }

private:
    LLCore * interpreter;
    double _result;
};

#endif // EXPRESSION_SOLVER_H
