#ifndef TERM_HPP
#define TERM_HPP

#include <string>
#include <vector>
#include <memory>
#include <iterator>
#include <stack>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include <deque>
#include <exception>
#include <utility>
#include "sub.hpp"

template<typename T>
class function;
template<typename T>
class literal;
template<typename T>
class variable;

template<typename T>
using term_ptr = std::shared_ptr<term<T>>;
template<typename T>
using variable_ptr = std::shared_ptr<variable<T>>;
template<typename T>
using literal_ptr = std::shared_ptr<literal<T>>;
template<typename T>
using function_ptr = std::shared_ptr<function<T>>;

template<typename T>
using rule = std::pair<term_ptr<T>, term_ptr<T>>;

typedef std::deque<uint32_t> path;

template<typename T>
class term_iterator;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////

class InvalidPathException: public std::exception
{public: const char * what() const noexcept{ return "Invalid Path, you do not pass go, do not collect $200";}};

class InvalidRuleException: public std::exception
{public: const char * what() const noexcept{ return "Invalid Rule";}};

/*!
 * \brief Class Term, base class for terms
 */
template<typename T>
class term
{

public:
    typedef term<T>                                 type;
    typedef term<T>                                 value_type;
    typedef term<T>*                                pointer;
    typedef term<T>&                                reference;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef std::bidirectional_iterator_tag         iterator_category;
    typedef term_iterator<T>                        iterator;
    typedef term_iterator<const T>                  const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    // Our base class constructor

    term(){}

    // Our iterators
    iterator begin(){return iterator(this);}
    iterator end(){return iterator(this,true);}
    iterator cbegin(){return iterator(this);}
    iterator cend(){return iterator(this,true);}
    iterator rbegin(){return iterator(this,false,true);}
    iterator rend(){return iterator(this,true,true);}

    // Operators
    virtual bool operator!=(const term<T>& /*rhs*/)const{return true;}
    virtual bool operator==(const term<T>& /*rhs*/)const{return false;}

    // Eh, pains me to write, base class shouldn't know anything about
    // derived classes
    virtual bool isVariable( )const{return false;}
    virtual bool isLiteral ( )const{return false;}
    virtual bool isFunction( )const{return false;}

    // Find a specific term and build its path
    virtual bool find_path(path& p, term<T>& t)=0;

    // Utility functions
    virtual std::ostream& pp(std::ostream&) const=0;
    virtual term_ptr<T> clone() const = 0;

    // Does anyone thing of the children?
    virtual std::vector< term_ptr<T> >& children( )=0;

    // Rewrite routines
            term_ptr<T> rewrite(term_ptr<T>, path, Sub<T>);
    virtual term_ptr<T> rewrite(Sub<T>&) = 0;

private:

    term_ptr<T> rewrite(term_ptr<T> t, term_ptr<T> r, path p);

};

template<typename T>
struct std::iterator_traits<term_iterator<T>>
{
    typedef term<T>                                 value_type;
    typedef term<T>*                                pointer;
    typedef term<T>&                                reference;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef std::bidirectional_iterator_tag         iterator_category;
    typedef term_iterator<T>                        iterator;
    typedef term_iterator<const T>                  const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
};

template<typename T>
class term_iterator{
public:
    // Our constructors
    term_iterator();
    term_iterator(term<T> *, bool end=false, bool reverse=false);
    term_iterator(const term_iterator&);

    // Our operators
    term_iterator& operator=(const term_iterator&);
    term<T>& operator*(){ return **current_iterator;}
    term<T>* operator->(){ return &*current_iterator;}
    term_iterator& operator++(){++current_iterator; return *this;}
    term_iterator& operator--(){--current_iterator; return *this;}
    term_iterator operator++(int){ auto temp = *this; ++*this; return temp;}
    term_iterator operator--(int){ auto temp = *this; --*this; return temp;}
    term_iterator& operator+=(unsigned int i){current_iterator+=i; return *this;}
    term_iterator& operator-=(unsigned int);
    bool operator!=(const term_iterator &rhs)const{return !(!(*this == rhs) || (current_iterator == _path.end()));}
    bool operator==(const term_iterator &rhs)const{return _path == rhs._path;}

private:
    // Used in construction to flatten the tree
    void _loadpath(term<T>*);

    // The term we are iterating over
    term< T >* _root;

    // Our flattened tree
    std::vector<term<T>*> _path;

    // This will hold the iterator from _path, the rest of the
    // iterator is a wrapper around this
    decltype(std::vector<term<T>*>().begin()) current_iterator;
};

/*! ***************************************************************
 * \brief Class variable, derived class that holds variables
 * ***************************************************************
 */
template<typename T>
class variable: public term<T>{
public:
    // Our Constructors
    variable(std::string __var);
    variable( const variable<T>& );
    variable<T>& operator=(const variable<T>&);

    // Move symantics
    variable( const variable<T>&& );
    variable<T>& operator=(const variable<T>&&);

    // Var
    std::string var(){ return _var; }

    // Utility Functions
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<variable>(*this);}

    // Our operators
    bool operator!=(const term<T>& rhs)const {return !(*this == rhs);}
    bool operator==(const term<T>& rhs)const
    {
        if(rhs.isVariable())
        {
            // This is never called which is why I didn't realize the
            // problem it will have
            // Stupid double dispatch
            //return *this == static_cast<const variable<T>&>(rhs);
            return _var == static_cast<const variable<T>&>(rhs)._var;
        }
        return false;
    }
    // These two are overshadowed by their more general counterparts
    bool operator!=(const variable<T>& rhs)const{return !(*this == rhs);}
    bool operator==(const variable<T>& rhs)const{return _var == rhs._var;}

    // Rewrite
    term_ptr<T> rewrite(Sub<T> &sigma);

    // Utility
    bool find_path(path& p, term<T>& t);
    bool isVariable()const{return true;}

    std::vector< term_ptr<T> >& children( ){return _children;}

private:
    // We have no children
    std::vector< term_ptr<T> > _children{};
    std::string _var;
};

/*!
 * \brief Class literal, derived class that holds literals
 */

template<typename T>
class literal: public term<T>{
public:
    literal( T __value );
    literal( const literal<T>& );
    literal<T>& operator=(const literal<T>&);

    // move symantics
    literal( const literal<T>&& );
    literal<T>& operator=(const literal<T>&&);

    // Our values
    T& value(){return _value;}
    const T& value()const{return _value;}

    std::vector< term_ptr<T> > _children{};
    std::vector< term_ptr<T> >& children( ){return _children;}
    term_ptr<T> rewrite(term_ptr<T>, Sub<T>);
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<literal>(*this);}

    // Our operators
    bool operator!=(const term<T>& rhs)const{return !(*this == rhs);}
    bool operator==(const term<T>& rhs)const
    {
        if(rhs.isLiteral()){
            // Silly thing calls this function again
            // Stupid double dispatch
            //return *this == static_cast<const literal<T>&>(rhs);
            return _value == static_cast<const literal<T>&>(rhs)._value;
        }
        return false;
    }
    // These two are overshadowed by their more general counterparts
    bool operator!=(const literal<T>& rhs)const{return !(*this == rhs);}
    bool operator==(const literal<T>& rhs)const{return _value == rhs._value;}

    // Rewrite
    term_ptr<T> rewrite(Sub<T>&);

    // Utilities
    bool find_path(path& p, term<T> &t);
    bool isLiteral( )const{return true;}

private:
    T _value;
};

/*!
 * \brief Class function, derived class that holds functions
 */


template<typename T>
class function: public term<T>{
public:
    // Our constructors construct
    function(std::string __name, uint32_t __arity, std::vector<std::shared_ptr< term<T>>> __subterms );
    function( const function<T>& );
    function<T>& operator=(const function<T>&);

    // Move Semantics
    function( const function<T>&&);
    function<T>& operator=(const function<T>&&);

    // Why yes we have children, would you like to see?
    std::vector< term_ptr<T> >& children( ){return _subterms;}

    // Our Operators
    bool operator!=(const term<T>& rhs)const{return !(*this == rhs);}
    bool operator==(const term<T>& __rhs)const
    {
        const function<T>& rhs = static_cast<const function<T>&>(__rhs);
        if(rhs.isFunction())
        {
            // Stupid double dispatch
            //return *this == static_cast<const function<T>&>(rhs);
            return _name == rhs._name && _arity == rhs._arity && _subterms == rhs._subterms;
        }
        return false;
    }
    // These two are overshadowed by their more general counterparts
    bool operator!=(const function<T>& rhs)const{return !(*this == rhs);}
    bool operator==(const function<T>& rhs)const{return _name == rhs._name && _arity == rhs._arity && _subterms == rhs._subterms;}

    // Rewrite
    term_ptr<T> rewrite(Sub<T> &);

    // Utilities
    bool find_path(path& p, term<T>& t);
    bool isFunction( )const{return true;}
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<function>(*this);}

    // Our getters
    std::string& name(){return _name;}
    const std::string& name()const{ return _name; }

private:
    std::string _name;
    // I literally never use this
    uint32_t _arity;
    std::vector< term_ptr<T> > _subterms;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Term
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// rewrite & unify
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
term_ptr<T> term<T>::rewrite(term_ptr<T> rhs, path p, Sub<T> sigma)
{
    term_ptr<T> t = this->clone();
    term_ptr<T> r = rhs->clone();
    // Rewrite the r now, and then screw sigma
    r = r->rewrite(sigma);

    // Private recursive call
    return rewrite(t, r, p);
}

/*!
 * \brief Rewrites a term at the given path with the given rewrite
 * \param term_ptr<T> t, the term to be rewritten
 * \param term_ptr<T> r, the term to write
 * \param term_ptr<T> p, the path to the term in t to be rewritten
 *
 * \return term_ptr<T> the term t after modifications, if any
 */
template<typename T>
term_ptr<T> term<T>::rewrite(term_ptr<T> t, term_ptr<T> r, path p)
{
    // If we get here our path is invalid
    if( p.empty() )
    {
        throw InvalidPathException();
    }
    auto pos = p.front();

    // No need to iterate over this if we know now.
    if( pos == 0 || pos > t->children().size() ){
        throw InvalidPathException();
    }
    --pos;
    p.pop_front();

    // Grab the child and replace 'em if this is it,
    // Our call the next one
    auto child = t->children()[pos];
    if( p.empty() ){
        t->children()[pos] = r;
    }else{
        t->children()[pos] = child->rewrite(child, r, p );
    }

    return t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Variable
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
variable<T>::variable(std::string __var):
    _var{__var}
{
}

template<typename T>
variable<T>::variable(const variable<T>& rhs ):
    term<T>{rhs},
    _var{rhs._var}
{
}

template<typename T>
variable<T>::variable(const variable<T>&& rhs ):
    term<T>{rhs},
    _var{std::move(rhs._var)}
{
}

template<typename T>
variable<T>& variable<T>::operator=(const variable<T>&& rhs )
{
    _var = rhs._var;
}

template<typename T>
variable<T>& variable<T>::operator=(const variable<T>& rhs)
{
    _var = rhs._var;
}

template<typename T>
std::ostream& variable<T>::pp(std::ostream& out) const
{
    out << _var;
    return out;
}

template<typename T>
term_ptr<T> variable<T>::rewrite(Sub<T>& sigma)
{
    return sigma(_var).clone();
}

template<typename T>
bool variable<T>::find_path(path& /*p*/, term<T> &t)

{
    bool found = false;
    if(t.isVariable())
    {
        found = (*this == static_cast<variable<T>&>(t));
    }
    return found;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Literal
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
literal<T>::literal( T __value ):
    _value{__value}
{
}

template<typename T>
literal<T>::literal(const literal<T>& c ):
    term<T>{c},
    _value{c._value}
{
}

template<typename T>
literal<T>& literal<T>::operator=(const literal<T>& rhs)
{
    _value = rhs._value;
}

template<typename T>
literal<T>::literal(const literal<T>&& rhs ):
    term<T>{rhs},
    _value{std::move(rhs._value)}
{
}

template<typename T>
literal<T>& literal<T>::operator=(const literal<T>&& rhs )
{
    this->_value = std::move(rhs._value);
    return *this;
}

template<typename T>
std::ostream& literal<T>::pp(std::ostream& out) const
{
    out << _value;
    return out;
}

template<typename T>
term_ptr<T> literal<T>::rewrite(Sub<T> &)
{
    return this->clone();
}

template<typename T>
bool literal<T>::find_path(path& /*p*/, term<T>& t)
{
    bool found = false;
    if(t.isLiteral())
    {
        found = (*this == static_cast<literal<T>&>(t));
    }
    return found;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Function
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
function<T>::function(std::string __name, uint32_t __arity, std::vector<std::shared_ptr< term<T>>> __subterms ):
    term<T>{},
    _name{__name},
    _arity{__arity},
    _subterms{__subterms}
{}

template<typename T>
function<T>::function(const function<T>& c ):
    term<T>{c},
    _name{c._name},
    _arity{c._arity},
    _subterms{}
{
    // Remake our subterms as we don't want to point to
    // things others are pointing to
    _subterms.reserve(c._subterms.size());
    for(auto& t :c._subterms ){
        _subterms.push_back(term_ptr<T>(t));
    }
}

template<typename T>
function<T>& function<T>::operator=(const function<T>& rhs)
{
    _name = rhs._name;
    _arity = rhs._arity;

    // Remake our subterms as we don't want to point to
    // things others are pointing to
    for(auto& t :rhs._subterms ){
        _subterms.push_back(term_ptr<T>(t));
    }

    return *this;
}

template<typename T>
function<T>::function(const function<T>&& rhs ):
    _name{std::move(rhs._name)},
    _arity{rhs._arity},
    _subterms{std::move(rhs._subterms)}
{
}

template<typename T>
function<T>& function<T>::operator=(const function<T>&& rhs )
{
    _name = rhs._name;
    _arity = rhs._arity;
    _subterms = rhs._subterms;

    return *this;
}


template<typename T>
std::ostream& function<T>::pp(std::ostream& out) const
{
    out << _name << std::string(" ( ");
    for( auto t = _subterms.begin(); t != _subterms.end(); ++t )
    {
        out << **t;

        // Get rid of that last damn ,
        if( std::next(t) != _subterms.end())
            out << std::string(", ");
    }
    out << std::string(" ) ");
    return out;
}

template<typename T>
term_ptr<T> function<T>::rewrite(Sub<T>& sigma)
{
    for( auto&s : _subterms)
    {
        s = s->rewrite(sigma);
    }
    return this->clone();
}

template<typename T>
bool function<T>::find_path(path& p, term<T> &t)
{
    bool found = false;
    // Check if we are searching for ourself
    if(t.isFunction())
    {
        found = (*this == static_cast<function<T>&>(t));
    }

    // If it isn't us, maybe it's them
    if( !found )
    {
        uint32_t steps = 0;
        for(auto& i: _subterms)
        {
            ++steps;
            found = i->find_path(p, t);
            if(found)
            {
                // Load from front as we'll be doing this on the way back
                p.push_front(steps);
                break;
            }
        }
    }
    return found;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: term_iterator
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
term_iterator<T>::term_iterator():
    _root{nullptr},
    _path{},
    current_iterator{_path.begin()}
{

}

template<typename T>
term_iterator<T>::term_iterator(term<T>* __root, bool end , bool reverse):
    _root{__root},
    _path{},
    current_iterator{}
{
    _loadpath(_root);
    // Since iterator types are different for normal vector iterators and
    // their reverse counterparts, we'll use a std algorithm to simply
    // reverse our vector once its loaded, this way we keep the same
    // iterators.
    if( reverse ){
        std::reverse(_path.begin(), _path.end());
    }

    // If we're making an end iterator, return the _paths end
    current_iterator = end ? _path.end() : _path.begin();
}

template<typename T>
void term_iterator<T>::_loadpath(term<T>* tptr )
{
    // recursively go through our tree in a depth first manner
    _path.push_back(tptr);
    for(auto& t: tptr->children()){
        _loadpath(t.get());
    }
}

template<typename T>
term_iterator<T>::term_iterator( const term_iterator& c ):
    _root{c._root},
    _path{c._path},
    current_iterator{c.current_iterator}
{
}

template<typename T>
term_iterator<T>& term_iterator<T>::operator=( const term_iterator& rhs )
{
    _root = rhs._root;
    _path = rhs._path;
    current_iterator = rhs.current_iterator;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// operator<< Streaming
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, const term<T>& rhs ){
    rhs.pp( out );
    out.flush();
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Unify
////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Unify t1 with t2 and put the rules into sigma, and return if unification was a success
 * \param term<T>& t1 is a reference to the first term
 * \param term<T>& t2 is a reference to the second term
 * \param Sub& sigma is the substitution class for sigma
 *
 * \return bool if unification was successful
 */
template<typename T, typename Sub>
bool unify(term<T>& t1, term<T>& t2, Sub& sigma)
{

    // Because we can't reflect on which types these terms are,
    // and inherited overloading with virtual functions only applies
    // to the object being called and not to its arguments
    // we hacked it with virtual functions to do reflection for us
    if( t1.isVariable() ){
        return unify(static_cast<variable<T>&>(t1), t2, sigma);
    }
    if( t2.isVariable() ){
        return unify(static_cast<variable<T>&>(t2), t1, sigma);
    }
    if( t1.isLiteral() && t2.isLiteral() ){
        return ( t1 == t2 );
    }
    if( t1.isFunction() && t2.isFunction() ){
        return unify( static_cast<function<T>&>(t1), static_cast<function<T>&>(t2), sigma);
    }
    return false;
}

template<typename T, typename Sub>
bool unify(function<T>& t1, function<T>& t2, Sub& sigma)
{
    // iterate through both, probably recursively, then stuff the unification into the sigma.
    // It is a set of rules, so we can put it into the sigma by using extend.

    // We want to iterate over both at the same time.

    // If they aren't the same, and their children are different sizes, then fail
    if( t1.name() != t2.name() || t1.children().size() != t2.children().size() ){
        return false;
    }
    auto it1 = t1.children().begin();
    auto it2 = t2.children().begin();

    bool unifies = true;
    for(; it1 != t1.children().end() && unifies; ++it1, ++it2){
        unifies = unify( **it1, **it2, sigma);
    }

    return unifies;
}

template<typename T, typename Sub>
bool unify(literal<T>& t1, literal<T>& t2, Sub& )
{
    return ( t1 == t2);
}

template<typename T, typename Sub>
bool unify( variable<T>& t1, variable<T>& t2, Sub& sigma )
{
    sigma.extend( t1.var(), t2);
    return true;
}
template<typename T, typename Sub>
bool unify( variable<T>& t1, term<T>& t2, Sub& sigma )
{
    sigma.extend( t1.var(), t2.clone() );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reduce
////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief reduces the term by the given rules
 *
 * \param term_ptr<T> t is the term to be reduced
 * \param std::vector<rule<T>& is a set of rules to do the reduction
 *
 * \return term_ptr<T> a copy of the term now rewritten
 */
template<typename T>
term_ptr<T> reduce( const term_ptr<T> t, const std::vector<rule<T>>& rules)
{
    term_ptr<T> ret = t->clone();
    // First unify the term with each rule

    // We want to load the sigma with changes, but only once per rule
    for(auto& r: rules)
    {
        Sub<T> sigma;

        for(auto& subterm: *ret)
        {
            // Skip variables, we don't like 'em.
            if(subterm.isVariable())
            {
                continue;
            }
            if( unify( subterm, *(r.first), sigma) )
            {
                // Find path to here

                path p;
                ret->find_path(p, subterm);
                ret = ret->rewrite(r.second, p, sigma);
                break;
            }
        }
    }

    return ret;
}

#endif // TERM_HPP
