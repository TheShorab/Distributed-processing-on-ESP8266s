#ifndef LLCORE_H
#define LLCORE_H

#include "error.h"

/**
 * ----- llcore
 * ----- modules
 * ----- core
 * ----- PCB
 * ----- front
 */

#include "scope.h"

class FrontEnd;
class ExpressionSolver;
class Core;
class Function;
class IfElseBlock;
class Loop;
class Variable;
struct PCBManager;
struct PCB;
struct FunctionType;

class LLCore {
    friend class FrontEnd;
    friend class Loop;
    friend class ExpressionSolver;
    friend class Core;
    friend class Function;
    friend class Variable;
    friend class IfElseBlock;
    friend struct PCBManager;

public:
    typedef std::pair<std::string, std::any> TypedValue;

    enum class BLOCKS{FN, IF, ELSE, LOOP};
    enum class BOPS{EQ, GTEQ, LTEQ, GT, LT, NEQ};
    enum class DTS{InValid, Int, LInt, LLInt, UInt, F32, F64};
    enum class KWS{Declare, Syscall, If, Else, Loop, Return, BranchTo, Function, Invoke, Delete, Print};

public:

    LLCore();

    LLCore(const std::string& filename);

    inline static void
    terminateExecution(const Error& e)
    {
        std::cout << e.cause() << EXF;
        exit(0);
    }

    INDEPENDENT void print(const std::string& command)
    {
        std::string name = checkAccessToVariableAndScopizeName(trim_copy(command));
        Print(accessVariable(name));
    }

    INDEPENDENT std::string scopizeName(const std::string &name, const Scope &scope)
    {
        return name + ":" + std::to_string(scope.first) + "." + std::to_string(scope.second);
    }

    INDEPENDENT std::pair<std::string, size_t> getWordAndItsEnd(const std::string &line, int start = 0)
    {
        std::string word = "";
        bool withinWord = false;
        char sep = ' ';
        size_t index;

        for(size_t i = start; i < line.length(); i++){
            if(line[i] == sep && !withinWord) continue;

            if(line[i] != sep && !withinWord) {
                withinWord = true;
            }

            if(line[i] != sep && withinWord){
                word += line[i];
                if(i != line.size() -1) continue;
            }

            if((line[i] == sep &&  withinWord) || i == line.size() -1) {
                //            withinWord = false;
                index = i;
                break;
            }
        }

        return {word, index};
    }

    //    void checkDataTypeOnOperation(int n, ...);
    //    std::any stringToTypedAny(const std::string &type, const std::string &value);
    //    void variableAndTypeNameValidator(const std::string &name);
    //    std::string enumTypeToStringType(DTS dt);

    INDEPENDENT bool checkValue(const std::string &value)
    {
        return isNumber(value);
    }

    INDEPENDENT BOPS operatorStringToOperatorEnum(const std::string &type)
    {
        std::map<std::string, BOPS> ops =
            {{"==", BOPS::EQ}, {">=", BOPS::GTEQ}, {"<=", BOPS::LTEQ},
             {"<", BOPS::LT}, {">", BOPS::GT}, {"!=", BOPS::NEQ}};

        return ops[type];
    }

    INDEPENDENT KWS keywordStringTokeywordEnum(const std::string &type)
    {
        std::map<std::string, KWS> keywords =
            {{"declare", KWS::Declare}, {"branch_to", KWS::BranchTo},
             {"function", KWS::Function}, {"return", KWS::Return},
             {"if", KWS::If}, {"else", KWS::Else}, {"print", KWS::Print},
             {"syscall", KWS::Syscall}, {"invoke", KWS::Invoke},
             {"delete", KWS::Delete}, {"loop", KWS::Loop}};

        return keywords[type];
    }

    INDEPENDENT DTS stringTypeToEnumType(const std::string &type)
    {
        std::map<std::string, DTS> dataTypes =
            {{"int", DTS::Int}, {"lint", DTS::LInt}, {"llint", DTS::LLInt},
             {"uint", DTS::UInt}, {"F32", DTS::F32}, {"F64", DTS::F64}};

        return dataTypes[type];
    }

    INDEPENDENT void trimLeft(std::string &string)
    {
        string.erase(string.begin(), std::find_if(string.begin(), string.end(),
                                                  [](unsigned char ch) {
                                                      return !std::isspace(ch);
                                                  }));
    }

    INDEPENDENT void trimRight(std::string &string)
    {
        string.erase(std::find_if(string.rbegin(), string.rend(),
                                  [](unsigned char ch) {
                                      return !std::isspace(ch);
                                  }).base(), string.end());
    }

    INDEPENDENT void trim(std::string &string)
    {
        trimRight(string);
        trimLeft(string);
    }

    INDEPENDENT std::string trimLeft_copy(std::string string)
    {
        string.erase(string.begin(), std::find_if(string.begin(), string.end(),
                                                  [](unsigned char ch) {
                                                      return !std::isspace(ch);
                                                  }));
        return string;
    }

    INDEPENDENT std::string trimRight_copy(std::string string)
    {
        string.erase(std::find_if(string.rbegin(), string.rend(),
                                  [](unsigned char ch) {
                                      return !std::isspace(ch);
                                  }).base(), string.end());
        return string;
    }

    INDEPENDENT std::string trim_copy(std::string string)
    {
        trimRight(string);
        trimLeft(string);
        return string;
    }

    INDEPENDENT std::string removeSpaces(const std::string &string)
    {
        std::string result;
        std::copy_if(string.begin(), string.end(),
                     std::back_inserter(result),
                     [](char ch) { return !std::isspace(ch); });

        return result;
    }

    INDEPENDENT bool isNumber(const std::string &string)
    {
        bool dotSeen = false;
        auto begin = string.begin();

        if(string[0] == '-') begin++;

        return !string.empty() && std::find_if(begin, string.end(),
                                               [&dotSeen](unsigned char c)
                                               {
                                                   if(c == '.'){
                                                       if(!dotSeen){
                                                           dotSeen = true;
                                                           return false;
                                                       } else {
                                                           return true;
                                                       }
                                                   }

                                                   return !std::isdigit(c);
                                               }) == string.end();
    }

    INDEPENDENT bool isVariable(const std::string &string)
    {
        return !isNumber(string);
    }

    INDEPENDENT bool isLvalueFloat(const std::string &string)
    {
        return (string.find(".") != std::string::npos);
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
                //            default: throw std::runtime_error("ridi");
            }
        }

        return std::stod(number);
    }

    void variableAndTypeNameValidator(const std::string &name);
    std::string enumTypeToStringType(DTS dt);
    void checkDataTypeOnOperation(int n, ...);
    std::string checkAccessToVariableAndScopizeName(const std::string &name);
    double accessVariable(const std::string &name);

private:
    PCB * pcb;

    /// first: name of value, second: Value Pair
    std::map<std::string, TypedValue> _values;
    std::map<std::string, std::string> _refs;
    std::map<std::string, FunctionType*> _functions;
    std::ifstream * file;
};

struct PCBManager;
struct PCB {
    friend struct PCBManager;
public:
    Scope scope = {0,0};

private:
    double _retVal = 0;
    size_t _lineNumber = 0;
    bool _condition = false;
    bool _withinFnDeclaration = false;

    MAYBE_UNUSED void * _loop = nullptr;
    MAYBE_UNUSED size_t _curlyBraces = 0;
    MAYBE_UNUSED size_t _invokeLineNumber = 0;

    std::string _currentLine;
    std::stack<size_t> _retAdds;
    std::stack<Scope> _blockedScopes;
    std::stack<LLCore::BLOCKS> _blocks;
    std::stack<std::string> _callStack;
    std::stack<std::string> branchingInstructions;

public:

    INDEPENDENT void checkScopeCompatiblity(const std::string& name)
    {
        auto e = EPCB("Invalid Variable Access! different scops.", checkScopeCompatiblity);
        const Scope& scope = scopeFromName(name);
        if(this->scope.first < scope.first)
            LLCore::terminateExecution(e);

        if(this->scope.first > scope.first && !this->isWithinFunctionCall())
            return;

        if(this->scope.first == scope.first)
        {
            if(this->scope.second == scope.second)
                return;
            else
                LLCore::terminateExecution(e);
        }

        LLCore::terminateExecution(EPCB("Undefined Behavior! function -> accessVariable, args -> "
                                            + name, checkScopeCompatiblity));
    }

    INDEPENDENT Scope scopeFromName(const std::string &name) const noexcept
    {
        const std::string scope = name.substr(name.find(':') + 1);
        Scope::ScopeSize lvl = std::stoul(scope.substr(0, scope.find('.')));
        Scope::ScopeSize slvl = std::stoul(scope.substr(scope.find('.') + 1));
        return {lvl, slvl};
    }

    INDEPENDENT std::string getLineNumber_s() const
    {
        return std::to_string(_lineNumber);
    }

    INDEPENDENT size_t getLineNumber() const
    {
        return _lineNumber;
    }

    INDEPENDENT size_t nextLineNumber()
    {
        return ++_lineNumber;
    }

    INDEPENDENT std::vector<std::string>
    valueNameWithBackwardScopes(const std::string& name, LLCore * interpreter) const
    {
        std::vector<std::string> names;
        for(int i = scope.first; i >= 0; i--)
            for(int j = scope.second; j >= 0; j--)
                names.push_back(interpreter->scopizeName(name, {Scope::ScopeSize(i),Scope::ScopeSize(j)}));

        return names;
    }

    INDEPENDENT bool isWithinFunctionCall() const {
        if(_blocks.empty()) return false;
        return _blocks.top() ==  LLCore::BLOCKS::FN;
    }

    INDEPENDENT void setWithinFunctionCall(bool newWithinFunctionCall)
    {
        newWithinFunctionCall ? _blocks.push( LLCore::BLOCKS::FN) : _blocks.pop();
    }

    INDEPENDENT bool isWithinIfBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() ==  LLCore::BLOCKS::IF;
    }

    INDEPENDENT void setWithinIfBlock(bool newWithinIfBlock)
    {
        newWithinIfBlock ? _blocks.push(LLCore::BLOCKS::IF) : _blocks.pop();
    }

    INDEPENDENT bool isWithinElseBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() ==  LLCore::BLOCKS::ELSE;
    }

    INDEPENDENT void setWithinElseBlock(bool newWithinElseBlock)
    {
        newWithinElseBlock ? _blocks.push(LLCore::BLOCKS::ELSE) : _blocks.pop();
    }

    INDEPENDENT bool isWithinLoopBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() == LLCore::BLOCKS::LOOP;
    }

    INDEPENDENT void setWithinLoopBlock(bool newWithinLoopBlock)
    {
        newWithinLoopBlock ? _blocks.push(LLCore::BLOCKS::LOOP) : _blocks.pop();
    }

    INDEPENDENT bool isRunAllowed() const
    {
        if(isWithinFunctionDeclaration()) return false;
        if(_blockedScopes.empty()) return true;
        auto& blockedScope = _blockedScopes.top();
        if(blockedScope.first <= scope.first) return false;
        return true;
    }

    INDEPENDENT bool isRunAllowedOnNextBlock() const
    {
        if(_blockedScopes.empty()) return true;
        auto& blockedScope = _blockedScopes.top();
        if(blockedScope.first <= scope.first + 1 /*&& blockedScope.second >= scope.second + 1*/) return false;
        return true;
    }

    INDEPENDENT void blockNextScope()
    {
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first + 1) return;

    end:
        _blockedScopes.push( {
            scope.first + 1,
            scope.second
        });
    }

    INDEPENDENT void blockPrevScope()
    {
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first - 1) return;

    end:
        _blockedScopes.push({
            scope.first - 1,
            scope.second
        });
    }

    INDEPENDENT void blockThisScope()
    {
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first) return;

    end:
        _blockedScopes.push(scope);
    }

    INDEPENDENT void freeScope(Scope s)
    {
        if(_blockedScopes.empty()) return;

        if(s == Scope({0, 0})) {
            _blockedScopes.pop();
            return;
        }

        if(s == _blockedScopes.top())
            _blockedScopes.pop();
    }

    INDEPENDENT void setIfElseMetaData(bool condition)
    {
        _condition = condition;
        if(!condition) blockNextScope();
    }

    INDEPENDENT size_t condition() const
    {
        return _condition;
    }

    INDEPENDENT std::string getCurrentLine() const
    {
        return _currentLine;
    }

    INDEPENDENT void setCurrentLine(const std::string& line)
    {
        _currentLine = line;
    }

    INDEPENDENT size_t& curlyBraces()
    {
        return _curlyBraces;
    }

    INDEPENDENT size_t retAdd()
    {
        auto a = _retAdds.top();
        _retAdds.pop();
        return a;
    }

    INDEPENDENT void addReturnAddress(size_t add)
    {
        _retAdds.push(add);
    }

    INDEPENDENT double getReturnValue()
    {
        return _retVal;
    }

    INDEPENDENT void setReturnValue(double v)
    {
        _retVal = v;
    }

    INDEPENDENT void addToCallStack(const std::string& s)
    {
        _callStack.push(s);
    }

    INDEPENDENT std::string getLastInvokedFn() const
    {
        return _callStack.top();
    }

    INDEPENDENT void popCallStack(){
        _callStack.pop();
    }

    INDEPENDENT void setWithinFunctionDeclaration(bool c)
    {
        _withinFnDeclaration = c;
    }

    INDEPENDENT bool isWithinFunctionDeclaration() const
    {
        return _withinFnDeclaration;
    }

    INDEPENDENT void setLineNumber(size_t n)
    {
        _lineNumber = n;
    }
};

inline LLCore::LLCore()
    : pcb(new PCB), file()
{}

inline LLCore::LLCore(const std::string& filename)
    : pcb(new PCB), file(new std::ifstream(filename))
{}

std::string LLCore::checkAccessToVariableAndScopizeName(const std::string &name)
{
    std::string nameWithScope, nameOnThisScope = scopizeName(name, pcb->scope);

    if(_refs.find(nameOnThisScope) == _refs.end()){
        const auto names = pcb->valueNameWithBackwardScopes(name, this);

        for(const auto &n : names){
            if(_values.find(n) == _values.end()){
                if(n == names.back()){
                    LLCore::terminateExecution(E(name + " is undefined!", checkAccessToVariableAndScopizeName));
                }
            } else {
                _values.at(n);
                nameWithScope = n;
                break;
            }
        }
    } else nameWithScope = _refs[nameOnThisScope];

    return nameWithScope;
}

double LLCore::accessVariable(const std::string &name)
{
    if(isNumber(name)) return toDouble(name);

    auto e = E("Invalid Data Type :: variable name : " + name, accessVariable);
    auto v = _values[checkAccessToVariableAndScopizeName(name)];

    switch(stringTypeToEnumType(v.first)){
    case DTS::Int:
        return std::any_cast<int>(v.second);

    case DTS::LInt:
        return std::any_cast<long>(v.second);

    case DTS::LLInt:
        return std::any_cast<long long>(v.second);

    case DTS::UInt:
        return std::any_cast<unsigned int>(v.second);

    case DTS::F32:
        return std::any_cast<float>(v.second);

    case DTS::F64:
        return std::any_cast<double>(v.second);

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }

    terminateExecution(e);
    NON_VOID(double)
}

void LLCore::variableAndTypeNameValidator(const std::string &name)
{
    std::string invalidChars = ".-*+/=+)(^%!~>< ";
    for(auto& c : name)
        if(invalidChars.find(c) != std::string::npos)
            terminateExecution(E(
                "you use invalid chars to define your variable name or use an invalid type name!" ,
                variableAndTypeNameValidator));
}

std::string LLCore::enumTypeToStringType(DTS dt)
{
    auto e = E("Invalid Data Type", booleanOperation);
    switch (dt) {
    case DTS::Int: return "int";
    case DTS::LInt: return "lint";
    case DTS::LLInt: return "llint";
    case DTS::UInt: return "uint";
    case DTS::F32: return "F32";
    case DTS::F64: return "F64";
    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }

    terminateExecution(e);
    return std::string();
}

void LLCore::checkDataTypeOnOperation(int n, ...)
{
    va_list pointer;
    va_start(pointer, n);

    std::string defType = va_arg(pointer, char*);
    auto e = E("Operatiorn of Incompatible Data types, all operands must have " +
                   defType + " data type in this expression.",
               checkDataTypeOnOperation);

    for(int i = 1; i < n; i++){
        std::string tmp = va_arg(pointer, char*);
        if(tmp != defType) terminateExecution(e);
    }

    va_end(pointer);
}

#endif // LLCORE_H
