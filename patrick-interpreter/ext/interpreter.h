#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <any>
#include <stack>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdarg.h>
#include <algorithm>
#include <math.h>
#include <queue>
#include "error.h"

#define OK
#define TEST 1

#if TEST == 1
#include <iostream>
#endif

class Interpreter
{
public:
    Interpreter();
    Interpreter(const std::string& fileName);
    ~Interpreter();
    void runOneCommand(const std::string& code);
    void runMultipleCommands(const std::string& code);

#if TEST == 1
    std::any getValue(const std::string& name){
        return _values[name].second;
    }

    std::vector<std::string> getValueNames(){
        std::vector<std::string> keys;
        for (const auto& [key, value] : _values)
        {
            keys.push_back(key);
        }
        return keys;
    }

    void printValues(){
        for (const auto& [key, value] : _values)
        {
            std::cout << key << " -> type: " << value.first << " and has value: " << value.second.has_value() << std::endl;
        }
    }

#endif

private: // types
    /// @brief first: Type, second: value
    typedef std::pair<std::string, std::any> TypedValue;

    /// @brief first: level (prent-child), second: sub-level (brotherhood)
    typedef std::pair<unsigned int, unsigned int> ScopeLevel;

    //    /** @brief first: ScopeLevel object, second: TypedValue
    //     *  @details Before accessing the variable, the scope must be checked
    //     */
    //    typedef std::pair<ScopeLevel, TypedValue> Value;

    /// @brief data types
    enum class DTS{InValid, Int, LInt, LLInt, UInt, F32, F64};

    /// @brief keywords
    enum class KWS{Declare, Syscall, If, Else, Loop, Return, BranchTo, Function, Invoke, Delete};

    /// @brief keywords
    enum class BOPS{EQ, GTEQ, LTEQ, GT, LT, NEQ};

    enum class BLOCKS{FN, IF, ELSE, LOOP};

#define DECLARE_VARIABLE
    /**
     * @brief The DeclarableValue class; The fields of this structure are filled by the fromString function
     *  from the lines of code related to the variable declaration,
     *  and an instance of this structure is used as the input of the declareValue function.
     */
    struct DeclarableValue;

#define DECLARE_FUNCTION
    /**
     * @brief The FunctionType class; Each function, when implemented, gets an instance of this structure,
     *  which holds the beginning and end of the function definition in the code.
     *  Then, during execution, this object fills its other fields using the fromRawString function and is ready to execute.
     *  Finally, by using the toExecutableString function,
     *  the final code of the function that can be executed by the interpreter will be created.
     */
    struct FunctionType;

#define IF_ELSE_BLOCK
    struct IfElseBlock;

#define PCB_MANAGER
    /**
     * @brief process control block
     */
    struct PCB;

    /**
     * @brief The calculationMetaData class:
     * This structure determines the index at which each of our variables is placed in the stack specific to its type
     */
    struct CalculationMetaData {
        std::string type;
        std::size_t index;
    };

#define LOOP_BLOCK
    /**
     * @brief The LoopState structure:
     */
    struct LoopState;

private: // members
#ifdef DECLARE_VARIABLE
    void declareVariable(const std::string& line);
#endif

#define CHECK_ACCESS_VARIABLE
    double accessVariable(const std::string& name);
    std::string checkAccessToVariableAndScopizeName(const std::string& name);

#define CHANGE_VARIABLE
    void changeVariable(const std::string& name, const std::string &value);
    void changeVariable(const std::string& name, const double &value);

#ifdef IF_ELSE_BLOCK
    void ifElseBlock(const std::string &line);
#endif

#ifdef LOOP_BLOCK
    bool loop(const std::string& line, bool runFaster = false);
    bool loopFaster(const std::string& type, const std::string initValue);
#endif

#ifdef DECLARE_FUNCTION
    void declareFunction(const std::string& line);
    void invokeFunction(const std::string& line);
    void returnState(const std::string& line);
#endif

//#define BRANCH_TO
//branchTo()

#define SOLVE_EXPRESSION
    double solveExpression(const std::string& string);
    void deleteVariable(const std::string& line);

    /*cloudLinker() --> Due to the lack of resources in ESPs, we do not have the ability to precompile,
     *                  so we have to create a cloud link between calls and implementations of functions. */

#define RUN_COMMAND
    void runCommand(const std::string& line);

    void terminateExecution(const std::string& what);

#ifdef PCB_MANAGER
    void pcbManager(const std::string &line, bool file);
#endif

private: // helper memebers
    OK void checkScopeCompatiblity(const std::string& name); //ok
    OK DTS stringTypeToEnumType(const std::string& type);
    OK KWS keywordStringTokeywordEnum(const std::string& type);
    OK BOPS operatorStringToOperatorEnum(const std::string& type);
    OK bool checkValue(const std::string &value);
    OK void checkDataTypeOnOperation(int n, ...);
    OK std::any stringToTypedAny(const std::string &type, const std::string &value);
    OK ScopeLevel scopeFromName(const std::string& name) const noexcept; //ok
    void goTo(size_t line);
    OK std::pair<std::string, size_t> getWordAndItsEnd(const std::string& line, int start = 0);
    OK void variableAndTypeNameValidator(const std::string& name);
    OK std::string scopizeName(const std::string &name, const ScopeLevel &scope);
    ///for variables
    OK CalculationMetaData loadValueToItsStack(const std::string& name);
    ///for lvalue
    OK CalculationMetaData loadValueToItsStack(const std::string& type, const std::string &value);
    //load value base
    OK CalculationMetaData loadValueToItsStack(const std::string& type, const std::any &value);
    OK bool booleanOperation(const std::string &type, const std::string &op);
    OK bool isVariable(const std::string& string);
    OK bool isLvalueFloat(const std::string& string);
    OK void trimLeft(std::string &string);
    OK void trimRight(std::string &string);
    OK void trim(std::string &string);
    OK std::string trimLeft_copy(std::string string);
    OK std::string trimRight_copy(std::string string);
    OK std::string trim_copy(std::string string);
    OK std::string removeSpaces(const std::string& string);
    OK bool isNumber(const std::string& string);
    OK std::pair<size_t, size_t> findLoopOperator(const std::string& line); //ok
    OK std::string loopOperatorToLogicalOperator(const std::string& op); //ok
    OK std::string enumTypeToStringType(DTS dt);

private: // exceptions
    void invalidKeywordException() noexcept;

private: // inline members
    inline char priorityToCharacter(int p){
        switch(p){
        case 0: return '+';
        case 1: return '-';
        case 2: return '*';
        case 3: return '/';
        case 4: return '%';
        case 5: return '^';
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

    inline double applyOperator(const std::string& fop, const std::string& sop, const char op){
        return applyOperator(accessVariable(fop), accessVariable(sop), op);
    }

    inline double applyOperator(const double fop, const std::string& sop, const char op){
        return applyOperator(fop, accessVariable(sop), op);
    }

    inline double applyOperator(const std::string& fop, const double sop, const char op){
        return applyOperator(accessVariable(fop), sop, op);
    }

    struct RecursivePartResult {
        double retVal;
        size_t findex = 0;
        size_t lindex = 0;
        size_t length() const {return lindex - findex + 1; }
    };

public:
    inline double solveExpressionBase(std::string& string){
        trim(string);
        if(isNumber(string)) return toDouble(string);

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
            if(!hasOperator(string)) return toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.left) && hasOperator(ex.right))
        {
            result.retVal = applyOperator(solveExpressionBase(ex.left), solveExpressionBase(ex.right), ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.left))
        {
            result.retVal = applyOperator(solveExpressionBase(ex.left), ex.right, ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

        if(hasOperator(ex.right))
        {
            result.retVal = applyOperator(ex.left, solveExpressionBase(ex.right), ex.op);
            string.replace(result.findex, result.length(), std::to_string(result.retVal));
            if(!hasOperator(string)) return toDouble(string);
            finalResult = solveExpressionBase(string);
            goto ret;
        }

    ret:
        return finalResult;
    }

public:

private: // variables
    /// first: name of value, second: Value Pair
    std::map<std::string, TypedValue> _values;
    std::map<std::string, std::string> _refs;
    std::map<std::string, FunctionType*> _functions;

    /// calclulation stacks
    std::stack<int> intStack;
    std::stack<long> lintStack;
    std::stack<long long> llintStack;
    std::stack<unsigned int> uintStack;
    std::stack<float> f32Stack;
    std::stack<double> f64Stack;
    std::map<std::string, CalculationMetaData> metadata;
    std::stack<std::string> calculationStack;

    bool exceptionOrTerminate;

    std::ifstream * file;
    PCB* pcb;

private: // constants

};

#endif // INTERPRETER_H
