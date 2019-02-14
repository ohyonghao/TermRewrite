#ifndef TERM_HPP
#define TERM_HPP

#include <string>
#include <vector>
#include <memory>
#include <stack>

template<typename T>
class term_iterator;

/*!
 * \brief Class Term, base class for terms
 */
template<typename T>
class term
{

public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef T&          reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef term_iterator<T>        iterator;
    typedef term_iterator<const T>  const_iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::reverse_iterator<iterator> reverse;
    typedef std::reverse_iterator<const_iterator> const_r;

    virtual iterator begin()=0;
    virtual iterator end()=0;
    virtual iterator cbegin() const=0;
    virtual iterator cend() const=0;

    virtual bool operator!=(const term<T>& rhs)=0;
    virtual bool operator==(const term<T>& rhs) = 0;

    virtual std::vector< std::shared_ptr< term< T > > > children( )=0;

    virtual std::ostream& pp(std::ostream&) const=0;

};

template<typename T>
class term_iterator{
private:
    std::shared_ptr< term< T > > _root;

    bool done{false};
    std::stack< term_iterator* > _its;
    term_iterator* current_iterator;

    bool isSelf()const{ return current_iterator == this; }
    term_iterator<T>* getptr(){return this;}

    unsigned int _child{0};

public:
    term_iterator(std::shared_ptr<term<T> >);
    term<T>& operator*() const;
    term<T>* operator->() const;
    term_iterator& operator++();
    term_iterator& operator--();
    term_iterator operator++(int);
    term_iterator operator--(int);
    term_iterator& operator+=(unsigned int);
    term_iterator& operator-=(unsigned int);
    bool operator!=(const term_iterator &/*rhs*/){return !done;}
    bool operator==(const term_iterator &/*rhs*/){return done;}
};

/*! ***************************************************************
 * \brief Class variable, derived class that holds variables
 * ***************************************************************
 */
template<typename T>
class variable: public term<T>{
public:
    variable(std::string __var):_var{__var}{}

    std::string var(){ return _var; }

    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > > children( ){return std::vector< std::shared_ptr< term< T > > >();}
    std::ostream& pp(std::ostream&) const;

    typedef T           value_type;
    typedef T*          pointer;
    typedef T&          reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef term_iterator<T>        iterator;
    typedef term_iterator<const T>  const_iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::reverse_iterator<iterator> reverse;
    typedef std::reverse_iterator<const_iterator> const_r;

    iterator begin();
    iterator end();
    iterator cbegin() const;
    iterator cend() const;
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
    literal( T __value ):_value{__value}{}
    T& value(){return _value;}
    const T& value()const{return _value;}
    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > > children( ){return std::vector< std::shared_ptr< term< T > > >();}
    std::ostream& pp(std::ostream&) const;

    typedef T           value_type;
    typedef T*          pointer;
    typedef T&          reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef term_iterator<T>        iterator;
    typedef term_iterator<const T>  const_iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::reverse_iterator<iterator> reverse;
    typedef std::reverse_iterator<const_iterator> const_r;

    iterator begin();
    iterator end();
    iterator cbegin() const;
    iterator cend() const;
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
    std::string& name(){return _name;}
    const std::string& name()const{ return _name; }
    // No children, so return empty vector
    std::vector< std::shared_ptr< term< T > > > children( ){return _subterms;}
    std::ostream& pp(std::ostream&) const;

    typedef T           value_type;
    typedef T*          pointer;
    typedef T&          reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef term_iterator<T>        iterator;
    typedef term_iterator<const T>  const_iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::reverse_iterator<iterator> reverse;
    typedef std::reverse_iterator<const_iterator> const_r;

    iterator begin();
    iterator end();
    iterator cbegin() const;
    iterator cend() const;
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Variable
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& variable<T>::pp(std::ostream& out) const{
    out << _var;
    return out;
}
template<typename T>
term_iterator<T> variable<T>::begin(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<variable<T>>(*this));
}

template<typename T>
term_iterator<T> variable<T>::cbegin() const {
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<variable<T>>(*this));
}


template<typename T>
term_iterator<T> variable<T>::end(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<variable<T>>(*this));
}

template<typename T>
term_iterator<T> variable<T>::cend() const{
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<variable<T>>(*this));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Literal
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& literal<T>::pp(std::ostream& out) const{
    out << _value;
    return out;
}
template<typename T>
term_iterator<T> literal<T>::begin(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<literal<T>>(*this));
}

template<typename T>
term_iterator<T> literal<T>::cbegin() const {
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<literal<T>>(*this));
}


template<typename T>
term_iterator<T> literal<T>::end(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<literal<T>>(*this));
}

template<typename T>
term_iterator<T> literal<T>::cend() const{
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<literal<T>>(*this));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: Function
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
function<T>::function(std::string __name, uint32_t __arity, std::vector<std::shared_ptr< term<T>>> __subterms ):
    _name{__name},
    _arity{__arity},
    _subterms{__subterms}
{}

template<typename T>
std::ostream& function<T>::pp(std::ostream& out) const{
    out << _name ;
    return out;
}
template<typename T>
term_iterator<T> function<T>::begin(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<function<T>>(*this));
}

template<typename T>
term_iterator<T> function<T>::cbegin() const {
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<function<T>>(*this));
}


template<typename T>
term_iterator<T> function<T>::end(){
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<function<T>>(*this));
}

template<typename T>
term_iterator<T> function<T>::cend() const{
    // Check for children, then move to the beginning
    return term_iterator<T>(std::make_shared<function<T>>(*this));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation: term_iterator
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
term_iterator<T>::term_iterator( std::shared_ptr<term<T> > __root ):
    _root{__root},
    current_iterator{this}
{
}

// I'm thinking that for this I want to use a stack with pairs that are the term and
// the child that I am on, most might be 1. Alternatively we might store iterators on the stack
// instead.
template<typename T>
term_iterator<T>& term_iterator<T>::operator++(){
    // If we've set the done flag, don't do anything
    if( done ){
        return *this;
    }
    if(isSelf()){
        if( _root->children().empty() ){
            done = true;
            return *this;
        }else if(_child < _root->children().size()){
            _its.push(this);
            current_iterator = (_root->children()[_child])->begin().getptr();
            ++_child;
            return *current_iterator;
        }else{
            done = true;
            return *this;
        }
    }

    // If not self, increment the iterator
    if(++*current_iterator != *current_iterator ){
        // != ignores the rhs, but we do need something there, I know, it's silly
        // After incrementing we want to check if we hit the end. If we did, then
        // pop and increment the last iterator
        current_iterator = _its.top();
        _its.pop();
        return ++*this;
    }
    return *this;
}

template<typename T>
term<T>& term_iterator<T>::operator*() const
{
    if( isSelf() ){
        return *_root;
    }else{
        return **current_iterator;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// operator<< Streaming
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, const term<T>& rhs ){
    rhs.pp( out );
    return out;
}
#endif // TERM_HPP
