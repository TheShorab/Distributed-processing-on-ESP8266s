#include "interpreter.h"

struct Interpreter::LoopState{
    int loopBlockLine;
    std::string dest;
    std::string start;
    std::string op;
    std::string step;
    void increaseStart(Interpreter* interpreter){
        interpreter->runCommand(start + " = " + start + " + " + step);
    }

    std::string generateBoolExpression(){
        return start + " " + op + " " + dest;
    }
};

/** ===================================================
 *
 *                  PCB manager
 *
 *  =================================================== */
#if defined(PCB_MANAGER)
struct Interpreter::PCB{
    ScopeLevel scope = {0,0};

    std::string getLineNumber_s() const {
        return std::to_string(_lineNumber);
    }

    size_t getLineNumber() const {
        return _lineNumber;
    }

    size_t nextLineNumber(){
        return ++_lineNumber;
    }

    void scopeManager(const std::string& line, Interpreter * interpreter){
        _currentLine = line;

        //        if(line.find("function:") != std::string::npos){
        //            setWithinFunctionDeclaration(true);
        //        }

        if(!isWithinFunctionDeclaration())
        {
            if(line.find("ivoke:") != std::string::npos){
                setWithinFunctionCall(true);
            }

            if(line.find("if:") != std::string::npos){
                setWithinIfBlock(true);
            }

            if(line.find("else:") != std::string::npos) {
                setWithinElseBlock(true);
            }

            if(line.find("loop:") != std::string::npos) {
                scope.second++;
                setWithinLoopBlock(true);
            }
        }

        if(line.find('{') != std::string::npos)
        {
            if(isWithinLoopBlock()){
                interpreter->ifElseBlock(_loop->generateBoolExpression());
            }

            scope.first += 1;
            _curlyBraces++;
        }

        if(!isRunAllowed() &&!isWithinIfBlock() && !isWithinElseBlock() && !isWithinFunctionDeclaration()) freeScope({0,0});

        if(line.find('}') != std::string::npos)
        {
            removeVariablesAllocatedInThisScope(interpreter);

            if(isWithinFunctionDeclaration() && (_curlyBraces - 1) == 0){
                setWithinFunctionDeclaration(false);
                goto end;
            }

            if(isWithinFunctionCall()){
                setWithinFunctionCall(false);
                goTo(retAdd() + 1, interpreter);
                goto end;
            }

            if(isWithinIfBlock()) {
                setWithinIfBlock(false);
                if(isRunAllowed()){
                    blockThisScope();
                }
                else freeScope(scope);

                goto end;
            }

            if(isWithinElseBlock()) {
                setWithinElseBlock(false);
                if(!isRunAllowed()){
                    freeScope(scope);
                }

                goto end;
            }

            if(isWithinLoopBlock()){
                _loop->increaseStart(interpreter);
                interpreter->ifElseBlock(_loop->generateBoolExpression());
                if(isRunAllowedOnNextBlock()){
                    goTo(_loop->loopBlockLine, interpreter);
                } else {
                    scope.first--;
                    removeVariablesAllocatedInThisScope(interpreter);
                    scope.first++;
                    scope.second--;
                    //                    setRunAllowed(true);
                    freeLoop();
                    freeScope(scope);
                    setWithinElseBlock(false);
                    setWithinIfBlock(false);
                }
            }

        end:
            scope.first--;
            _curlyBraces--;
        }
    }

    std::vector<std::string> valueNameWithBackwardScopes(const std::string& name, Interpreter * interpreter) const{
        std::vector<std::string> names;
        for(int i = scope.first; i >= 0; i--)
            for(int j = scope.second; j >= 0; j--)
                names.push_back(interpreter->scopizeName(name, {i,j}));

        return names;
    }

    void goTo(size_t line, Interpreter * interpreter)
    {
        interpreter->file->seekg(std::ios::beg);
        for(size_t i = 0; i < line - 1; ++i){
            interpreter->file->ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        }

        this->_lineNumber = line;
    }

    bool isWithinFunctionCall() const {
        if(_blocks.empty()) return false;
        return _blocks.top() == BLOCKS::FN;
    }

    void setWithinFunctionCall(bool newWithinFunctionCall) {
        newWithinFunctionCall ? _blocks.push(BLOCKS::FN) : _blocks.pop();
    }

    bool isWithinIfBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() == BLOCKS::IF;
    }

    void setWithinIfBlock(bool newWithinIfBlock)
    {
        newWithinIfBlock ? _blocks.push(BLOCKS::IF) : _blocks.pop();
    }

    bool isWithinElseBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() == BLOCKS::ELSE;
    }

    void setWithinElseBlock(bool newWithinElseBlock)
    {
        newWithinElseBlock ? _blocks.push(BLOCKS::ELSE) : _blocks.pop();
    }

    bool isWithinLoopBlock() const
    {
        if(_blocks.empty()) return false;
        return _blocks.top() == BLOCKS::LOOP;
    }

    void setWithinLoopBlock(bool newWithinLoopBlock)
    {
        newWithinLoopBlock ? _blocks.push(BLOCKS::LOOP) : _blocks.pop();
    }

    bool isRunAllowed() const {
        if(isWithinFunctionDeclaration()) return false;
        if(_blockedScopes.empty()) return true;
        auto& blockedScope = _blockedScopes.top();
        if(blockedScope.first <= scope.first) return false;
        return true;
    }

    bool isRunAllowedOnNextBlock() const {
        if(_blockedScopes.empty()) return true;
        auto& blockedScope = _blockedScopes.top();
        if(blockedScope.first <= scope.first + 1 /*&& blockedScope.second >= scope.second + 1*/) return false;
        return true;
    }

    void blockNextScope(){
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first + 1) return;

    end:
        _blockedScopes.push( {
            scope.first + 1,
            scope.second
        });
    }

    void blockPrevScope(){
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first - 1) return;

    end:
        _blockedScopes.push({
            scope.first - 1,
            scope.second
        });
    }

    void blockThisScope(){
        if(_blockedScopes.empty()) goto end;
        if(_blockedScopes.top().first <= scope.first) return;

    end:
        _blockedScopes.push(scope);
    }

    void freeScope(ScopeLevel s){
        if(_blockedScopes.empty()) return;

        if(s == ScopeLevel({0, 0})) {
            _blockedScopes.pop();
            return;
        }

        if(s == _blockedScopes.top())
            _blockedScopes.pop();
    }

    void setIfElseMetaData(bool condition){
        _condition = condition;
        if(!condition) blockNextScope();
    }

    size_t condition() const { return _condition; }

    void setLoop(LoopState * loop){
        _loop = loop;
    }

    LoopState * loop(){
        return _loop;
    }

    void freeLoop(){
        free(_loop);
        _loop = nullptr;
    }

    std::string getCurrentLine() const {
        return _currentLine;
    }

    size_t retAdd() {
        auto a = _retAdds.top();
        _retAdds.pop();
        return a;
    }

    void addReturnAddress(size_t add){
        _retAdds.push(add);
    }

    double getReturnValue(){
        return _retVal;
    }

    void setReturnValue(double v){
        _retVal = v;
    }

    void addToCallStack(const std::string& s){
        _callStack.push(s);
    }

    std::string getLastInvokedFn() const{
        return _callStack.top();
    }

    void popCallStack(){
        _callStack.pop();
    }

    void setWithinFunctionDeclaration(bool c){
        _withinFnDeclaration = c;
    }

    bool isWithinFunctionDeclaration() const {
        return _withinFnDeclaration;
    }

    void removeVariablesAllocatedInThisScope(Interpreter * interpreter){
        std::vector<std::string> forErase;
        for (const auto& [key, value] : interpreter->_values)
        {
            auto const scp = interpreter->scopeFromName(key);
            if(scp.first == scope.first && scp.second == scope.second) forErase.push_back(key);
        }

        for(const auto& key : forErase)
            interpreter->_values.erase(key);
    }

private:
    bool _condition = false;
    bool _withinFnDeclaration = false;
    size_t _invokeLineNumber;
    size_t _lineNumber = 0;
    size_t _curlyBraces = 0;
    double _retVal;
    std::stack<BLOCKS> _blocks;
    std::stack<ScopeLevel> _blockedScopes;
    std::stack<size_t> _retAdds;
    std::stack<std::string> _callStack;
    std::stack<std::string> branchingInstructions;
    LoopState * _loop = nullptr;
    std::string _currentLine;
};

void Interpreter::pcbManager(const std::string& line, bool file = false)
{
    if(!file) {
        pcb->nextLineNumber();
        pcb->scopeManager(line, this);
    }

    if(file) {
        pcb->goTo(pcb->nextLineNumber(), this);
        pcb->scopeManager(line, this);
        //        if(pcb->isWithinFunctionCall())
        //            pcb->functionManager(line);
    }
}

#endif

/** ===================================================
 *
 *                  Constructors
 *
 *  =================================================== */
Interpreter::Interpreter() : pcb(new PCB)
{

}

Interpreter::Interpreter(const std::string &fileName) : file(new std::ifstream(fileName)), pcb(new PCB)
{
    file->close();
    file->open(fileName, std::fstream::in);

    if(!file->is_open()){
        std::cout << "file closed!\n";
        return;
    }

    std::string line;
    while(file->peek() != -1){
        //DEBUG
        //        std::cout << line << std::endl;
        //        std::cout << "peek: " << file->peek() << std::endl;
        std::getline(*this->file, line);
        pcbManager(line, true);
        if(pcb->isRunAllowed()) runCommand(line);
    }
}

Interpreter::~Interpreter()
{
    for(auto [key, value]: _functions){
        delete value;
    }

    delete pcb;
    delete file;
}

/** ===================================================
 *
 *              Public Function Members
 *
 *  =================================================== */
void Interpreter::runOneCommand(const std::string &code)
{
    return this->runCommand(code);
}

void Interpreter::runMultipleCommands(const std::string &code)
{
    std::vector<std::string> lines;
    std::string line;

    for(auto& c : code){
        if(c != '\n')
            line += c;
        else {
            lines.push_back(line);
            line.clear();
        }
    }

    for(auto& line : lines){
        pcbManager(line);
        if(pcb->isRunAllowed()) runCommand(line);
    }
}

/** ===================================================
 *
 *                  declare variable
 *
 *  =================================================== */
#if defined(DECLARE_VARIABLE)
struct Interpreter::DeclarableValue {
private:
    std::string type;
    std::string name;
    std::any value;
    ScopeLevel scope;

public:
    DeclarableValue(const std::string& line, Interpreter * interpreter) {
        size_t indexOfAssign = line.find('=');

        if(indexOfAssign != std::string::npos) {
            std::tie(this->type, this->name) = nameAndType(line.substr(0, indexOfAssign), interpreter);
            this->value = interpreter->stringToTypedAny(this->type, interpreter->trim_copy(line.substr(indexOfAssign + 1)));
        } else {
            std::tie(this->type, this->name) = nameAndType(line, interpreter);
            this->value = interpreter->stringToTypedAny(this->type, "0");
        }

        this->scope = interpreter->pcb->scope;
    }

    static std::pair<std::string, std::string>
    nameAndType(const std::string& line, Interpreter * interpreter, bool scopize = true)
    {
        std::vector<std::string> typeName;
        std::string word; size_t index;

        for(size_t i = 0; i < line.length(); i++){
            std::tie(word, index) = interpreter->getWordAndItsEnd(line, i);
            interpreter->variableAndTypeNameValidator(word);
            if(!word.empty()) typeName.push_back(word);
            i = index;
        }

        if(typeName.size() != 2)
            interpreter->terminateExecution(E(
                "invalid instruction, in a line of patrick code, you can create only one variable!",
                nameAndType));

        if(scopize) return {typeName[0], interpreter->scopizeName(typeName[1], interpreter->pcb->scope)};

        return {typeName[0], typeName[1]};
    }

    TypedValue toValue(){
        return {this->type, this->value};
    }

    std::string getName() const {
        return this->name;
    }
};

void Interpreter::declareVariable(const std::string &line)
{
    DeclarableValue dv(line, this);
    TypedValue newValue = dv.toValue();
    this->_values.insert({dv.getName(), newValue});
}

#endif


/** ===================================================
 *
 *                  access variable
 *
 *  =================================================== */
#ifdef CHECK_ACCESS_VARIABLE
std::string Interpreter::checkAccessToVariableAndScopizeName(const std::string &name)
{
    std::string nameWithScope, nameOnThisScope = scopizeName(name, pcb->scope);

    if(_refs.find(nameOnThisScope) == _refs.end()){
        const auto names = pcb->valueNameWithBackwardScopes(name, this);

        for(const auto &n : names){
            if(_values.find(n) == _values.end()){
                if(n == names.back()){
                    terminateExecution(E(name + " is undefined!", checkAccessToVariableAndScopizeName));
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

double Interpreter::accessVariable(const std::string &name)
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
    return double();
}

#endif

#if defined(CHANGE_VARIABLE)
void Interpreter::changeVariable(const std::string &name, const std::string& value)
{
    ///exception
    checkValue(value);
    auto& v = _values[checkAccessToVariableAndScopizeName(name)];
    auto e = E("Invalid Data Type :: variable name : " + name, changeVariable);

    switch(stringTypeToEnumType(v.first)){
    case DTS::Int:
        v.second = std::stoi(value);
        break;

    case DTS::LInt:
        v.second = std::stol(value);
        break;

    case DTS::LLInt:
        v.second = std::stoll(value);
        break;

    case DTS::UInt:
        v.second = std::stoul(value);
        break;

    case DTS::F32:
        v.second = std::stof(value);
        break;

    case DTS::F64:
        v.second = std::stod(value);
        break;

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }
}

void Interpreter::changeVariable(const std::string &name, const double &value)
{
    auto& v = _values[checkAccessToVariableAndScopizeName(name)];
    auto e = E("Invalid Data Type :: variable name : " + name, changeVariable);

    switch(stringTypeToEnumType(v.first)){
    case DTS::Int:
        v.second = static_cast<int>(value);
        break;

    case DTS::LInt:
        v.second = static_cast<long int>(value);
        break;

    case DTS::LLInt:
        v.second = static_cast<long long>(value);
        break;

    case DTS::UInt:
        v.second = static_cast<unsigned int>(value);
        break;

    case DTS::F32:
        v.second = static_cast<float>(value);
        break;

    case DTS::F64:
        v.second = static_cast<double>(value);
        break;

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }
}
#endif


/** ===================================================
 *
 *                  declare function
 *
 *  =================================================== */
#if defined(DECLARE_FUNCTION)
struct Interpreter::FunctionType {
    DTS type;
    size_t start = std::numeric_limits<size_t>::max();
    std::vector<std::pair<DTS, std::string>> params;
    std::string invokeLine;
    /// shoud ref to exact variable name with this scope
};

void Interpreter::declareFunction(const std::string &line)
{
    FunctionType *fn = new FunctionType;
    std::string name, param;
    bool nameEnded = false, paramsEnded = false, commaSeen = false;

    auto manageParam = [&]() {
        auto x = DeclarableValue::nameAndType(param, this, false);
        fn->params.push_back({stringTypeToEnumType(x.first), x.second});
        param.clear();
        commaSeen = false;
    };

    for(auto ch = line.begin(); ch < line.end(); ch++){
        if((!nameEnded || (nameEnded && paramsEnded)) && *ch == ' ') continue;

        if(*ch == '(') {
            nameEnded = true;
            continue;
        }

        if(*ch == ')') {
            paramsEnded = true;
            manageParam();
            commaSeen = false;
            continue;
        }

        if(!nameEnded) name.push_back(*ch);

        if(nameEnded && !paramsEnded && *ch == ','){
            manageParam();
            commaSeen = true;
        }

        if(nameEnded && !paramsEnded && !commaSeen) param.push_back(*ch);

        if(paramsEnded) param.push_back(*ch);

        commaSeen = false;
    }

    fn->type = stringTypeToEnumType(param);
    fn->start = pcb->getLineNumber();

    _functions.insert({name, fn});
    pcb->setWithinFunctionDeclaration(true);
}

void Interpreter::invokeFunction(const std::string &line)
{
    std::vector<std::pair<DTS, std::string>> params;
    std::string name, param, copy;
    bool nameEnded = false, paramsEnded = false, commaSeen = false;

    size_t index = line.find("=");
    if(index != std::string::npos)
        copy = line.substr(index + 1);
    else
        copy = line;

    auto manageParams = [&]() {
        /// call by valueeeeeeeee!
        trim(param);
        auto nop = checkAccessToVariableAndScopizeName(param);
        params.push_back({stringTypeToEnumType(_values[nop].first), nop});

        param.clear();
    };

    for(auto ch = copy.begin(); ch < copy.end(); ch++){
        if((!nameEnded || (nameEnded && paramsEnded)) && *ch == ' ') continue;

        if(*ch == '(') {
            nameEnded = true;
            continue;
        }

        if(*ch == ')') {
            paramsEnded = true;
            manageParams();
            commaSeen = false;
            continue;
        }

        if(!nameEnded) name.push_back(*ch);

        if(nameEnded && !paramsEnded && *ch == ',')
        {
            manageParams();
            commaSeen = true;
        }

        if(nameEnded && !paramsEnded && !commaSeen) param.push_back(*ch);

        if(paramsEnded) {
            param.clear();
            break;
        }

        commaSeen = false;
    }

    if(_functions.find(name) == _functions.end())
        terminateExecution(E(name + " is undefined!", loadValueToItsStack));


    auto& fn = _functions[name];

    /// check data type
    if(fn->params.size() != params.size())
        terminateExecution(E("You Passed " +
                                 std::to_string(params.size()) + " arguments but need " +
                                 std::to_string(fn->params.size()), loadValueToItsStack));

    size_t sz = params.size();
    for(size_t i = 0; i < sz; i++){
        if(params[i].first != fn->params[i].first){
            terminateExecution(E("You Passed Parameter " + params[i].second + " with " +
                                     enumTypeToStringType(params[i].first)+ " type that is incompatible with  " +
                                     enumTypeToStringType(fn->params[i].first), loadValueToItsStack));
        }

        _refs.insert({scopizeName(fn->params[i].second, {pcb->scope.first + 1, pcb->scope.second}),
                      params[i].second});
    }

    pcb->addReturnAddress(pcb->getLineNumber());
    pcb->goTo(fn->start + 1, this);
    pcb->addToCallStack(name);
    fn->invokeLine = line;
}

void Interpreter::returnState(const std::string &line)
{
    auto& fn = _functions[pcb->getLastInvokedFn()];
    pcb->setReturnValue(solveExpression(line));

    // scope mager
    pcb->removeVariablesAllocatedInThisScope(this);
    pcb->scope.first--;

    // generate expression to solve
    std::string &ln = fn->invokeLine;
    size_t index = ln.find(pcb->getLastInvokedFn());
    auto e = E("Invalid Data Type", changeVariable);

    switch (fn->type) {
    case DTS::Int:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<int>(pcb->getReturnValue())));
        break;

    case DTS::LInt:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<long>(pcb->getReturnValue())));
        break;

    case DTS::LLInt:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<long long>(pcb->getReturnValue())));
        break;

    case DTS::UInt:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<unsigned int>(pcb->getReturnValue())));
        break;

    case DTS::F32:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<float>(pcb->getReturnValue())));
        break;

    case DTS::F64:
        ln.replace(index, ln.size() - index, std::to_string(static_cast<double>(pcb->getReturnValue())));
        break;

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }


    runCommand(ln);

    // free values
    fn->invokeLine.clear();
    pcb->setWithinFunctionCall(false);
    pcb->popCallStack();

    // goto
    pcb->goTo(pcb->retAdd() + 1, this);
}
#endif

/** ===================================================
 *
 *                  if-else block
 *
 *  =================================================== */
#ifdef IF_ELSE_BLOCK
void Interpreter::ifElseBlock(const std::string& line)
{
    //    pcb->setWithinIfBlock(true);

    // check condition
    bool badOperator = true;
    const std::vector<std::string> operators = {"==", "!=", ">=", "<=", ">", "<"};
    for(auto& op : operators){
        const size_t index = line.find(op);

        if(index != std::string::npos){
            badOperator = false;
            std::string fop = getWordAndItsEnd(line.substr(0, index)).first;
            std::string sop = getWordAndItsEnd(line.substr(index + op.length())).first;

            if(isVariable(fop)){
                variableAndTypeNameValidator(fop);
                CalculationMetaData fop_m = loadValueToItsStack(fop);
                fop.clear();

                if(isVariable(sop)){ //both are variable
                    variableAndTypeNameValidator(sop);
                    CalculationMetaData sop_m = loadValueToItsStack(sop);
                    sop.clear();
                    checkDataTypeOnOperation(2, fop_m.type.c_str(), sop_m.type.c_str());
                } else { // first is variable and second is lvalue
                    CalculationMetaData sop_m = loadValueToItsStack(fop_m.type, sop);
                    sop.clear();
                }
                pcb->setIfElseMetaData(booleanOperation(fop_m.type, op));
            } else {
                if(isVariable(sop)){ // first is lvalue and second is variable
                    variableAndTypeNameValidator(sop);
                    CalculationMetaData sop_m = loadValueToItsStack(sop);
                    sop.clear();
                    CalculationMetaData fop_m = loadValueToItsStack(sop_m.type, fop);
                    sop.clear();
                    pcb->setIfElseMetaData(booleanOperation(fop_m.type, op));
                } else { // both are lvalue
                    std::string type = isLvalueFloat(fop) ? "F64" : "int";
                    CalculationMetaData fop_m = loadValueToItsStack(type, fop);
                    fop.clear();
                    CalculationMetaData sop_m = loadValueToItsStack(type, sop);
                    sop.clear();
                    pcb->setIfElseMetaData(booleanOperation(type, op));
                }
            }

            break;
        }
    }

    if(badOperator) terminateExecution(E("Bad Operator", ifElseBlock));
}

Interpreter::CalculationMetaData Interpreter::loadValueToItsStack(const std::string &name)
{
    const auto& value = _values[checkAccessToVariableAndScopizeName(name)];
    return loadValueToItsStack(value.first, value.second);
}

Interpreter::CalculationMetaData Interpreter::loadValueToItsStack(const std::string &type, const std::string &value)
{
    return loadValueToItsStack(type, stringToTypedAny(type, value));
}

Interpreter::CalculationMetaData Interpreter::loadValueToItsStack(const std::string &type, const std::any &value)
{
    auto e = E("Invalid Data Type :: variable type : " + type, loadValueToItsStack);

    switch (stringTypeToEnumType(type)) {
    case DTS::Int:
        intStack.push(std::any_cast<int>(value));
        return {type, intStack.size() - 1};

    case DTS::LInt:
        lintStack.push(std::any_cast<long>(value));
        return {type, lintStack.size() - 1};

    case DTS::LLInt:
        llintStack.push(std::any_cast<long long>(value));
        return {type, llintStack.size() - 1};

    case DTS::UInt:
        uintStack.push(std::any_cast<unsigned int>(value));
        return {type, uintStack.size() - 1};

    case DTS::F32:
        f32Stack.push(std::any_cast<float>(value));
        return {type, f32Stack.size() - 1};

    case DTS::F64:
        f64Stack.push(std::any_cast<double>(value));
        return {type, f64Stack.size() - 1};

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }

    terminateExecution(e);
    return CalculationMetaData();
}

bool Interpreter::booleanOperation(const std::string& type, const std::string& op)
{
    auto helper = [this, op] (auto oper1, auto oper2){
        auto e = E("Invalid Operator, used operator: " + op, booleanOperation_helper);
        switch (operatorStringToOperatorEnum(op)) {
        case BOPS::EQ: return oper1 == oper2;
        case BOPS::NEQ: return oper1 != oper2;
        case BOPS::GTEQ: return oper1 >= oper2;
        case BOPS::LTEQ: return oper1 <= oper2;
        case BOPS::GT: return oper1 > oper2;
        case BOPS::LT: return oper1 < oper2;
        default: terminateExecution(e);
        }

        terminateExecution(e);
        return bool();
    };

    // oper2, oper1, az chap be rast operator ha miyan!
    auto e = E("Invalid Data Type :: variable type : " + type, booleanOperation);
    switch (stringTypeToEnumType(type)) {
    case DTS::Int:{
        int oper1 = intStack.top(); intStack.pop();
        int oper2 = intStack.top(); intStack.pop();
        return helper(oper2, oper1);
    }

    case DTS::LInt:{
        long oper1 = lintStack.top(); lintStack.pop();
        long oper2 = lintStack.top(); lintStack.pop();
        return helper(oper2, oper1);
    }

    case DTS::LLInt:{
        long long oper1 = llintStack.top(); llintStack.pop();
        long long oper2 = llintStack.top(); llintStack.pop();
        return helper(oper2, oper1);
    }

    case DTS::UInt:{
        unsigned int oper1 = uintStack.top(); uintStack.pop();
        unsigned int oper2 = uintStack.top(); uintStack.pop();
        return helper(oper2, oper1);
    }

    case DTS::F32:{
        float oper1 = f32Stack.top(); f32Stack.pop();
        float oper2 = f32Stack.top(); f32Stack.pop();
        return helper(oper2, oper1);
    }

    case DTS::F64:{
        double oper1 = f64Stack.top(); f64Stack.pop();
        double oper2 = f64Stack.top(); f64Stack.pop();
        return helper(oper2, oper1);
    }

    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }

    terminateExecution(e);
    return bool();
}

bool Interpreter::isVariable(const std::string &string)
{
    return !isNumber(string);
}

bool Interpreter::isLvalueFloat(const std::string &string)
{
    return (string.find(".") != std::string::npos);
}
#endif

/** ===================================================
 *
 *                     loop block
 *
 *  =================================================== */
#if defined(LOOP_BLOCK)

bool Interpreter::loop(const std::string &line, bool runFaster)
{
    // check this line has how may ,s
    const std::string::difference_type count = std::count(line.begin(), line.end(), ',');

    //exception
    if(count  != 1) return false;

    //exception
    size_t opFIndx, opLength;
    std::tie(opFIndx, opLength) = findLoopOperator(line);
    if(opFIndx == std::string::npos) return false;

    size_t stepIndex = line.find("step");
    if(stepIndex != std::string::npos && stepIndex < opFIndx + opLength) return false;

    std::string secondPart = line.substr(opFIndx + opLength + 1);

    std::string initPart = line.substr(0, opFIndx);
    trim(initPart);

    secondPart = removeSpaces(secondPart);
    size_t assignIndex = secondPart.find("=");
    if(assignIndex == std::string::npos) return false;

    declareVariable(initPart);
    auto tn = DeclarableValue::nameAndType(initPart.substr(0, initPart.find('=')), this);
    std::string start = tn.second;
    LoopState * ls = new LoopState();
    ls->start = start.substr(0, start.find(':'));
    ls->dest = secondPart.substr(0, secondPart.find(','));
    ls->op = loopOperatorToLogicalOperator(line.substr(opFIndx, opLength));
    ls->step = secondPart.substr(assignIndex + 1);
    ls->loopBlockLine = pcb->getLineNumber();
    pcb->setLoop(ls);

    if(runFaster){
        loopFaster(tn.first, initPart.substr(initPart.find('=') + 1));
    }

    return true;
}

#define LOOP(TP, TP_CONVERTER)\
auto op = pcb->loop()->op;\
    TP dest = TP_CONVERTER(pcb->loop()->dest);\
    TP step = TP_CONVERTER(pcb->loop()->step);\
    for(TP i = TP_CONVERTER(initValue); loopCondition(i,op,dest); i += step){\
        for(const auto& instruction : instructions){\
            runCommand(instruction);\
    }\
}

bool Interpreter::loopFaster(const std::string &type, const std::string initValue)
{
    std::vector<std::string> instructions;
    std::string loopLine;
    size_t curlyBraces = 0;

    const auto loopCondition = [](auto i, const std::string& op, auto dest){
        if(op == ">")
            return i > dest;

        if(op == ">=")
            return i >= dest;

        if(op == "<")
            return i < dest;

        if(op == "<=")
            return i <= dest;

        return false;
    };

    while(true){
        std::getline(*this->file, loopLine);
        pcb->nextLineNumber();
        instructions.push_back(loopLine);

        if(loopLine == "{")
        {
            curlyBraces++;
        }

        if(loopLine == "}")
        {
            curlyBraces--;
        }

        if(loopLine == "}" && curlyBraces == 0)
        {
            pcb->nextLineNumber();
            break;
        }
    }

    switch (stringTypeToEnumType(type)) {
    case DTS::Int:
    {
        LOOP(int, std::stoi)
        break;
    }
    case DTS::LInt:
    {
        LOOP(long, std::stol)
        break;
    }
    case DTS::LLInt: {
        LOOP(long long, std::stoll)
        break;
    }
    case DTS::UInt: {
        LOOP(unsigned int, std::stoul)
        break;
    }
    case DTS::F32: {
        LOOP(float, std::stof)
        break;
    }
    case DTS::F64: {
        LOOP(double, std::stod)
        break;
    }
    case DTS::InValid: return false;
    default: return false;
    }

    pcb->freeLoop();
    return true;
    //case DTS::InValid: throw std::runtime_error("invalid type on loop variable.");
    //default: throw std::runtime_error("invalid type on loop variable.");
}

double Interpreter::solveExpression(const std::string &string)
{
    std::string copy = removeSpaces(string);
    return solveExpressionBase(copy);
}

void Interpreter::deleteVariable(const std::string &line)
{
    auto name =  checkAccessToVariableAndScopizeName(line);
    _values.erase(name);
}

std::pair<size_t, size_t> Interpreter::findLoopOperator(const std::string &line)
{
    std::vector<std::string> operators = {"up_to_eq", "up_to", "down_to_eq", "down_to"};
    size_t index, size;

    for(auto& op : operators){
        index = line.find(op);
        if(index != std::string::npos){
            size = op.length();
            goto ret;
        }
    }

ret:
    return {index, size};
}

std::string Interpreter::loopOperatorToLogicalOperator(const std::string &op)
{
    std::map<std::string, std::string> ops =
        {{"up_to", "<"}, {"up_to_eq", "<="}, {"down_to", ">"}, {"down_to_eq", ">="}};

    return ops[op];
}

std::string Interpreter::enumTypeToStringType(DTS dt)
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

#endif

void Interpreter::trimLeft(std::string &string)
{
    string.erase(string.begin(), std::find_if(string.begin(), string.end(),
                                              [](unsigned char ch) {
                                                  return !std::isspace(ch);
                                              }));
}

void Interpreter::trimRight(std::string &string)
{
    string.erase(std::find_if(string.rbegin(), string.rend(),
                              [](unsigned char ch) {
                                  return !std::isspace(ch);
                              }).base(), string.end());
}

void Interpreter::trim(std::string &string)
{
    trimRight(string);
    trimLeft(string);
}

std::string Interpreter::trimLeft_copy(std::string string)
{
    string.erase(string.begin(), std::find_if(string.begin(), string.end(),
                                              [](unsigned char ch) {
                                                  return !std::isspace(ch);
                                              }));
    return string;
}

std::string Interpreter::trimRight_copy(std::string string)
{
    string.erase(std::find_if(string.rbegin(), string.rend(),
                              [](unsigned char ch) {
                                  return !std::isspace(ch);
                              }).base(), string.end());
    return string;
}

std::string Interpreter::trim_copy(std::string string)
{
    trimRight(string);
    trimLeft(string);
    return string;
}

std::string Interpreter::removeSpaces(const std::string &string)
{
    std::string result;
    std::copy_if(string.begin(), string.end(),
                 std::back_inserter(result),
                 [](char ch) { return !std::isspace(ch); });

    return result;
}

bool Interpreter::isNumber(const std::string &string)
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


/** ===================================================
 *
 *                  run command
 *
 *  =================================================== */
#if defined(RUN_COMMAND)
void Interpreter::runCommand(const std::string &line)
{
    //DEBUG
//    std::cout << line << std::endl;
    std::string copy = trim_copy(line);
    const auto index = copy.find(':');

    if(index == std::string::npos){
        const auto index = copy.find('=');
        if(index != std::string::npos)
        {
            double executionResult = solveExpression(copy.substr(index + 1));
            std::string name = copy.substr(0, index);
            trim(name);
            changeVariable(name, executionResult);
        }

    } else { //keyword
        const std::string keyword = trim_copy(copy.substr(0, index));
        const std::string command = trim_copy(copy.substr(index + 1));

        if(keyword == "///") return;

        switch (keywordStringTokeywordEnum(keyword)) {
        case KWS::BranchTo: break;
        case KWS::Declare: declareVariable(command); break;
        case KWS::Else: break;
        case KWS::If: ifElseBlock(command); break;
        case KWS::Function: declareFunction(command); break;
        case KWS::Loop: loop(command, false); break;
        case KWS::Return: returnState(command); break;
        case KWS::Syscall: break;
        case KWS::Invoke: invokeFunction(command); break;
        case KWS::Delete: deleteVariable(command); break;

        default:
            invalidKeywordException();
            break;
        }
    }

    return;
}
#endif

/** ===================================================
 *
 *               exception handler
 *
 *  =================================================== */
void Interpreter::terminateExecution(const std::string &what)
{
    std::cout << EE(what, pcb->getLineNumber_s(), pcb->getCurrentLine(), "CPU1")
              << EXF;
    exit(0);
}

/** ===================================================
 *
 *              check scope compatibilty
 *
 *  =================================================== */
void Interpreter::checkScopeCompatiblity(const std::string& name)
{
    auto e = E("Invalid Variable Access! different scops.", checkScopeCompatiblity);
    const ScopeLevel& scope = scopeFromName(name);
    if(pcb->scope.first < scope.first)
        terminateExecution(e);

    if(pcb->scope.first > scope.first && !pcb->isWithinFunctionCall())
        return;

    if(pcb->scope.first == scope.first)
    {
        if(pcb->scope.second == scope.second)
            return;
        else
            terminateExecution(e);
    }

    terminateExecution(E("Undefined Behavior! function -> accessVariable, args -> " + name, checkScopeCompatiblity));
}

/** ===================================================
 *
 *                  helper functions
 *
 *  =================================================== */
Interpreter::DTS Interpreter::stringTypeToEnumType(const std::string &type)
{
    std::map<std::string, DTS> dataTypes =
        {{"int", DTS::Int}, {"lint", DTS::LInt}, {"llint", DTS::LLInt},
         {"uint", DTS::UInt}, {"F32", DTS::F32}, {"F64", DTS::F64}};

    return dataTypes[type];
}

Interpreter::KWS Interpreter::keywordStringTokeywordEnum(const std::string &type)
{
    std::map<std::string, KWS> keywords =
        {{"declare", KWS::Declare}, {"branch_to", KWS::BranchTo}, {"function", KWS::Function}, {"return", KWS::Return},
         {"if", KWS::If}, {"else", KWS::Else}, {"loop", KWS::Loop}, {"syscall", KWS::Syscall}, {"invoke", KWS::Invoke},
         {"delete", KWS::Delete}};

    return keywords[type];
}

Interpreter::BOPS Interpreter::operatorStringToOperatorEnum(const std::string &type)
{
    std::map<std::string, BOPS> ops =
        {{"==", BOPS::EQ}, {">=", BOPS::GTEQ}, {"<=", BOPS::LTEQ}, {"<", BOPS::LT}, {">", BOPS::GT}, {"!=", BOPS::NEQ}};

    return ops[type];
}

bool Interpreter::checkValue(const std::string &value)
{
    return isNumber(value);
}

void Interpreter::checkDataTypeOnOperation(int n, ...)
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

std::any Interpreter::stringToTypedAny(const std::string &type, const std::string &value)
{
    double v = solveExpression(value);
    auto e = E("Invalid Data Type :: variable type : " + type, stringToTypedAny);

    switch(stringTypeToEnumType(type)){
    case DTS::Int: return static_cast<int>(v);
    case DTS::LInt: return static_cast<long>(v);
    case DTS::LLInt: return static_cast<long long>(v);
    case DTS::UInt: return static_cast<unsigned int>(v);
    case DTS::F32: return static_cast<float>(v);
    case DTS::F64: return static_cast<double>(v);
    case DTS::InValid: terminateExecution(e); break;
    default: terminateExecution(e);
    }

    terminateExecution(e);
    return std::any();
}

Interpreter::ScopeLevel Interpreter::scopeFromName(const std::string &name) const noexcept
{
    const std::string scope = name.substr(name.find(':') + 1);
    size_t lvl = std::stoul(scope.substr(0, scope.find('.')));
    size_t slvl = std::stoul(scope.substr(scope.find('.') + 1));
    return {lvl, slvl};
}

std::pair<std::string, size_t> Interpreter::getWordAndItsEnd(const std::string &line, int start)
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

void Interpreter::variableAndTypeNameValidator(const std::string &name)
{
    std::string invalidChars = ".-*+/=+)(^%!~>< ";
    for(auto& c : name)
        if(invalidChars.find(c) != std::string::npos)
            terminateExecution(E(
                "you use invalid chars to define your variable name or use an invalid type name!" ,
                variableAndTypeNameValidator));
}

std::string Interpreter::scopizeName(const std::string &name, const ScopeLevel &scope)
{
    return name + ":" + std::to_string(scope.first) + "." + std::to_string(scope.second);
}

void Interpreter::invalidKeywordException() noexcept
{

}
