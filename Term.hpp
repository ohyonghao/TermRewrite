#ifndef TERM_HPP
#define TERM_HPP

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <iostream>

template<typename T>
class term_iterator;

/*!
 * \brief Class Term, base class for terms
 */
template<typename T>
class term
{

public:
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


    virtual iterator begin();
    virtual iterator end();
    virtual iterator cbegin();
    virtual iterator cend();
    virtual iterator rbegin();
    virtual iterator rend();

    virtual bool operator!=(const term<T>& rhs)=0;
    virtual bool operator==(const term<T>& rhs) = 0;

    virtual std::vector< std::shared_ptr< term< T > > >& children( )=0;

    virtual std::ostream& pp(std::ostream&) const=0;

    term();
    term(const term<T>& );
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
    term_iterator(term<T> *);
    term_iterator(const term_iterator&);
    term_iterator& operator=(const term_iterator&);
    term<T>& operator*() const;
    term<T>* operator->() const;
    term_iterator& operator++();
    term_iterator& operator--();
    term_iterator operator++(int);
    term_iterator operator--(int);
    term_iterator& operator+=(unsigned int);
    term_iterator& operator-=(unsigned int);
    bool operator!=(const term_iterator &rhs){return (!(*this == rhs) && (current_iterator != _path.end()));}
    bool operator==(const term_iterator &rhs){return _path == rhs._path;}
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

    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > > _children{};
    std::vector< std::shared_ptr< term< T > > >& children( ){return _children;}
    std::ostream& pp(std::ostream&) const;

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
    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > > _children{};
    std::vector< std::shared_ptr< term< T > > >& children( ){return _children;}
    std::ostream& pp(std::ostream&) const;

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
    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > >& children( ){return _subterms;}
    std::ostream& pp(std::ostream&) const;

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


template<typename T>
term_iterator<T> term<T>::begin(){
    return term_iterator<T>(this);
}

template<typename T>
term_iterator<T> term<T>::cbegin() {
    return term_iterator<T>(this);
}

template<typename T>
term_iterator<T> term<T>::rbegin() {
    return term_iterator<T>(this);
}

template<typename T>
term_iterator<T> term<T>::end(){
    return term_iterator<T>();
}

template<typename T>
term_iterator<T> term<T>::cend(){
    return term_iterator<T>();
}

template<typename T>
term_iterator<T> term<T>::rend() {
    return term_iterator<T>();
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
term_iterator<T>::term_iterator( term<T>* __root ):
    _root{__root},
    _path{},
    current_iterator{}
{
    _loadpath(_root);
    current_iterator = _path.begin();
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

// I'm thinking that for this I want to use a stack with pairs that are the term and
// the child that I am on, most might be 1. Alternatively we might store iterators on the stack
// instead.
template<typename T>
term_iterator<T>& term_iterator<T>::operator++(){
    ++current_iterator;
    return *this;
}

template<typename T>
term_iterator<T> term_iterator<T>::operator++(int){
    auto temp = *this;
    ++*this;
    return temp;
}
template<typename T>
term<T>& term_iterator<T>::operator*() const
{
    return **current_iterator;
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
#endif // TERM_HPP
