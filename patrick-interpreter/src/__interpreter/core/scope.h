#ifndef SCOPE_H
#define SCOPE_H

#include <tuple>
#include <algorithm>
#include <string>

class Scope {
public:

    typedef unsigned int ScopeSize;

    Scope() : first(0), second(0)
    {}

    Scope(ScopeSize first, ScopeSize second)
        : first(first), second(second)
    {}

    Scope(Scope const &scope)
        : Scope(std::move(scope))
    {}

    Scope(Scope const &&scope)
        : first(scope.first), second(scope.second)
    {}

    Scope& operator=(const Scope& o)
    {
        this->first = o.first;
        this->second = o.second;
        return *this;
    }

    Scope& operator++()
    {
        first++;
        return *this;
    }

    Scope& operator--()
    {
        if(!first) return *this;
        first--;
        return *this;
    }

    Scope& operator++(int)
    {
        second++;
        return *this;
    }

    Scope& operator--(int)
    {
        if(!second) return *this;
        second--;
        return *this;
    }

    bool operator==(const Scope& o){
        return o.first == this->first && o.second == this->second;
    }

public:
    ScopeSize first;
    ScopeSize second;
};

void fn(){
    Scope scope;
    ++scope;
}

#endif // SCOPE_H
