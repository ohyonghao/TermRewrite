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

typedef std::deque<uint32_t> path;

template<typename T>
class term_iterator;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////

class InvalidPathException: public std::exception
{public: inline const char * what() const noexcept{ return "InvalidPath";}};

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

    iterator begin(){return iterator(this);}
    iterator end(){return iterator(this,true);}
    iterator cbegin(){return iterator(this);}
    iterator cend(){return iterator(this,true);}
    iterator rbegin(){return iterator(this,false,true);}
    iterator rend(){return iterator(this,true,true);}

    virtual bool operator!=(const term<T>& rhs)=0;
    virtual bool operator==(const term<T>& rhs) = 0;

    virtual std::vector< std::shared_ptr< term< T > > >& children( )=0;

    term_ptr<T> rewrite(term_ptr<T>, path, Sub<T>);
    virtual term_ptr<T> rewrite(term_ptr<T>, Sub<T>)=0;
    virtual std::ostream& pp(std::ostream&) const=0;

    virtual term_ptr<T> clone() const = 0;

    term();
    term(const term<T>& );

private:

    term_ptr<T> rewrite(term_ptr<T> t, term_ptr<T>, path, Sub<T>);
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
private:
    term< T >* _root;

    std::vector<term<T>*> _path;
    decltype(std::vector<term<T>*>().begin()) current_iterator;
    void _loadpath(term<T>*);

    unsigned int _child{0};

public:
    term_iterator();
    term_iterator(term<T> *, bool end=false, bool reverse=false);
    term_iterator(const term_iterator&);
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
};

/*! ***************************************************************
 * \brief Class variable, derived class that holds variables
 * ***************************************************************
 */
template<typename T>
class variable: public term<T>{
public:
    variable(std::string __var);
    variable( const variable<T>& );
    variable<T>& operator=(const variable<T>&);

    std::string var(){ return _var; }

    std::vector< std::shared_ptr< term< T > > > _children{};
    std::vector< std::shared_ptr< term< T > > >& children( ){return _children;}
    std::shared_ptr<term<T>> rewrite(std::shared_ptr<term<T>>, Sub<T>);
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<variable>(*this);}

    bool operator!=(const term<T>& rhs){return !(*this == rhs);}
    bool operator==(const term<T>& /*rhs*/){return false;}
    bool operator!=(const variable<T>& rhs){return !(*this == rhs);}
    bool operator==(const variable<T>& rhs){return _var == rhs._var;}

private:
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

    T& value(){return _value;}
    const T& value()const{return _value;}

    std::vector< std::shared_ptr< term< T > > > _children{};
    std::vector< std::shared_ptr< term< T > > >& children( ){return _children;}
    std::shared_ptr<term<T>> rewrite(std::shared_ptr<term<T>>, Sub<T>);
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<literal>(*this);}

    bool operator!=(const term<T>& rhs){return !(*this == rhs);}
    bool operator==(const term<T>& /*rhs*/){return false;}
    bool operator!=(const variable<T>& rhs){return !(*this == rhs);}
    bool operator==(const variable<T>& rhs){return _value == rhs._value;}
private:

    T _value;
};

/*!
 * \brief Class function, derived class that holds functions
 */


template<typename T>
class function: public term<T>{
public:
    function(std::string __name, uint32_t __arity, std::vector<std::shared_ptr< term<T>>> __subterms );
    function( const function<T>& );
    function<T>& operator=(const function<T>&);

    std::string& name(){return _name;}
    const std::string& name()const{ return _name; }

    std::vector< std::shared_ptr< term< T > > >& children( ){return _subterms;}
    std::shared_ptr<term<T>> rewrite(std::shared_ptr<term<T>>, Sub<T>);
    std::ostream& pp(std::ostream&) const;
    term_ptr<T> clone() const{return std::make_shared<function>(*this);}

    bool operator!=(const term<T>& rhs){return !(*this == rhs);}
    bool operator==(const term<T>& /*rhs*/){return false;}
    bool operator!=(const function<T>& rhs){return !(*this == rhs);}
    bool operator==(const function<T>& rhs){return _name == rhs._name && _arity == rhs._arity && _subterms == rhs._subterms;}
private:
    std::string _name;
    uint32_t _arity;
    std::vector< std::shared_ptr< term<T> > > _subterms;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Term
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
term<T>::term(){}

template<typename T>
term<T>::term(const term<T>&){}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// rewrite & unify
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
term_ptr<T> term<T>::rewrite(term_ptr<T> r, path p, Sub<T> sigma){
    term_ptr<T> t = this->clone();
    return rewrite(t, r, p, sigma);
}
template<typename T>
term_ptr<T> term<T>::rewrite(term_ptr<T> t, term_ptr<T> r, path p, Sub<T> sigma){
    if( p.empty() )
    {
        throw InvalidPathException();
    }
    auto pos = p.front();
    if( pos >= t->children().size()){
        throw InvalidPathException();
    }
    p.pop_front();
    auto child = t->children()[pos];
    if( p.empty() ){
        t->children()[pos] = child->rewrite( r, sigma );
    }else{
        t->children()[pos] = child->rewrite( r, p, sigma );
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
variable<T>& variable<T>::operator=(const variable<T>& rhs)
{
    _var = rhs._var;
}

template<typename T>
term_ptr<T> variable<T>::rewrite(term_ptr<T> t, Sub<T> sigma){
    sigma.extend(this->_var, t);
    return t;
}

template<typename T>
std::ostream& variable<T>::pp(std::ostream& out) const{
    out << _var;
    return out;
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
term_ptr<T> literal<T>::rewrite(term_ptr<T> t, Sub<T> /*sigma*/){
    return t;
}

template<typename T>
std::ostream& literal<T>::pp(std::ostream& out) const{
    out << _value;
    return out;
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
    _subterms.reserve(c._subterms.size());
    for(auto& t :c._subterms ){
        _subterms.push_back(std::shared_ptr<term<T>>(t));
    }
}

template<typename T>
function<T>& function<T>::operator=(const function<T>& rhs)
{
    _name = rhs._name;
    _arity = rhs._arity;

    for(auto& t :rhs._subterms ){
        _subterms.push_back(std::shared_ptr<term<T>>(t));
    }
}

template<typename T>
term_ptr<T> function<T>::rewrite(term_ptr<T> t, Sub<T> /*sigma*/){
    return t;
}

template<typename T>
std::ostream& function<T>::pp(std::ostream& out) const{
    out << _name << std::string(" ( ");
    for( auto t = _subterms.begin(); t != _subterms.end(); ++t ){
        out << **t;

        // Get rid of that last damn ,
        if( std::next(t) != _subterms.end())
            out << std::string(", ");
    }
    out << std::string(" ) ");
    return out;
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
    if( reverse ){
        std::reverse(_path.begin(), _path.end());
    }
    current_iterator = end? _path.end() : _path.begin();
}

template<typename T>
void term_iterator<T>::_loadpath(term<T>* tptr )
{
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


//template<typename T, typename Sub>
//bool unify(term<T> t1, term<T> t2, Sub& sigma);


#endif // TERM_HPP
