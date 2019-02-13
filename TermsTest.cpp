#include "Term.hpp"
#include "sub.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
using namespace std;

// In order to complete this assignment you'll need
// a term class
// template<typename T>
// class term {...}
//
// three subclasses
// template<typename T>
// class literal : term<T>

// template<typename T>
// class variable : term<T>

// template<typename T>
// class function : term<T>


// I'm making everything with shared pointers, because
// we have to use pointers or references to do inheritance
//
//template<typename T>
//using term_ptr = shared_ptr<term<T>>;
template<typename T>
using variable_ptr = shared_ptr<variable<T>>;
template<typename T>
using literal_ptr = shared_ptr<literal<T>>;
template<typename T>
using function_ptr = shared_ptr<function<T>>;

// a rule is a pair of terms
//template<typename T>
//using rule = pair<term_ptr<T>, term_ptr<T>>;

/////////////////////////////////
// Boolean algebra
/////////////////////////////////

//variables in terms
variable_ptr<bool> b_v() {return make_shared<variable<bool>>(variable<bool>("v"));}
variable_ptr<bool> b_w() {return make_shared<variable<bool>>(variable<bool>("w"));}
variable_ptr<bool> b_x() {return make_shared<variable<bool>>(variable<bool>("x"));}
variable_ptr<bool> b_y() {return make_shared<variable<bool>>(variable<bool>("y"));}
variable_ptr<bool> b_z() {return make_shared<variable<bool>>(variable<bool>("z"));}

//literal values
literal_ptr<bool> b_true() {return make_shared<literal<bool>>(literal<bool>(true));}
literal_ptr<bool> b_false() {return make_shared<literal<bool>>(literal<bool>(false));}

//functions

function_ptr<bool> b_and(term_ptr<bool> x, term_ptr<bool> y)
{
    return make_shared<function<bool>>(function<bool>("&&", 2, {x, y}));
}
function_ptr<bool> b_or(term_ptr<bool> x, term_ptr<bool> y)
{
    return make_shared<function<bool>>(function<bool>("||", 2, {x, y}));
}
function_ptr<bool> b_not(term_ptr<bool> x)
{
    return make_shared<function<bool>>(function<bool>("!", 1, {x}));
}
function_ptr<bool> b_arrow(term_ptr<bool> x, term_ptr<bool> y)
{
    return make_shared<function<bool>>(function<bool>("->", 2, {x,y}));
}


//variables for rules (to make sure there's no overlap)
//variables for rewrite rules are a and b
//variable_ptr<bool> b_a() {return make_shared<variable<bool>>(variable<bool>("a"));}
//variable_ptr<bool> b_b() {return make_shared<variable<bool>>(variable<bool>("b"));}

// the rule ->(a, false) => !(a)
//rule<bool> contra{ make_pair(b_arrow(b_a(), b_false()), b_not(b_a())) };

/////////////////////////////////
// substitution
/////////////////////////////////


int main()
{

    // the actual terms we'll be using
    term_ptr<bool> b1 = b_or(b_and(b_x(), b_false()), b_y());
    term_ptr<bool> b2 = b_or(b_and(b_true(), b_x()), b_arrow(b_or(b_v(), b_w()), b_false()));


    cout << "print a var b_v" << endl;
    for(auto &t : *b_v()){
        cout << t << endl;
    }
    for(auto &t : *b_true()){
        cout << t << endl;
    }

    cout << "print term b1" << endl;
    for(auto& t : *b1)
    {
        cout << t << endl;
    }
//    cout << "print term b2 backwards" << endl;
    //for(auto t = b2->rbegin(); t != b2->rend(); t++)
//    {
        //cout << *t << endl;
//    }

    // make the substitution [a :-> or(v,w)]
//    Sub<bool> sigma;
//    sigma.extend("a", b_or(b_v(),b_w()));

//    cout << "rewrite " << b1 << " with " << contra.second << endl;

    // now use the rule ->(a, false) => !(a) to rewrite
    // or(and(true,x), arrow(or(v,w), false)) to
    // or(and(true,x), not(or(v,w)))
//    term_ptr<bool> b1r = b1->rewrite(*contra.second, {2}, sigma);

//    cout << b1r << endl;

    // can I unify b1 with b2?
//    Sub<bool> s;
//    cout << "unifying b1 with b2? " << unify(*b1,*b2,s) << endl;
//    s.print();

    return 0;
}
