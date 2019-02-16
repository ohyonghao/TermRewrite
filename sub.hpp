#ifndef SUB_HPP
#define SUB_HPP

#include<unordered_map>
#include<iostream>
#include<utility>
#include<memory>

template<typename T>
class term;

template<typename T>
using term_ptr = std::shared_ptr<term<T>>;

/**
 * a simple implementation of a substitution class.
 * contains an extend method and an operator()
 *
 * if I have:
 * Sub s;
 * s.extend("x", t);
 * cout << s("x") << endl;
 * I'll print out t.
 */
template<typename T>
class Sub
{
    std::unordered_map<std::string, term_ptr<T>> _map;

public:
    term<T>& operator()(std::string s)
    {
        return *_map.at(s);
    }
    void extend(std::string s, term_ptr<T> t)
    {
        _map[s] = t;
    }

    // print the substitution so I can verify that unify works
    void print()
    {
        std::cout << "[" << std::endl;
        for(std::pair<std::string, term_ptr<T>> p : _map)
        {
            //std::cout << p.first << " :-> " << *p.second << std::endl;
        }
        std::cout << "]" << std::endl;
    }
};
#endif // SUB_HPP
