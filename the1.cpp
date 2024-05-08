#pragma once

#include "Iterable.hxx"

// Your code goes here.
template<typename T>
class repeatIterator : public IIterator<T> {
public:
    repeatIterator(T value) : value(value) {}

    ~repeatIterator() {}

    T next() override {
        return value;
    }

private:
    T value;
};

template<typename T>
class repeatIterable : public IIterable<T> {
public:
    repeatIterable(T value) : value(value) {}

    ~repeatIterable() {}

    IIterator<T>* iter() const override {
        return new repeatIterator<T>(value);
    }

private:
    T value;
};

template<typename T>
IIterable<T>* repeat(T v) {
    return new repeatIterable<T>(v);
}



template<typename T>
class CountIterator : public IIterator<T> {
public:
    CountIterator(T start, T delta) : current(start), delta(delta) {}

    ~CountIterator() {}

    T next() override {
        T value = current;
        current += delta;
        return value;
    }

private:
    T current;
    T delta;
};

template<typename T>
class CountIterable : public IIterable<T> {
public:
    CountIterable(T start, T delta) : start(start), delta(delta) {}

    ~CountIterable() {}

    IIterator<T>* iter() const override {
        return new CountIterator<T>(start, delta);
    }

private:
    T start;
    T delta;
};

template<typename T>
IIterable<T>* count(T start, T delta) {
    return new CountIterable<T>(start, delta);
}

template<typename T>
class SkipIterator : public IIterator<T> {
public:
    SkipIterator(unsigned k, IIterator<T> *iterator) : k(k), iterator(iterator), n(0){}
    
    ~SkipIterator() {
        delete iterator;
    }
    
    T next() override{
        while(n<k){
            try{
                iterator->next();
                n++;
            }
            catch(StopIteration &){
                throw StopIteration();
            }
        }
        return iterator->next();
    }
private:
    unsigned k;
    unsigned int n;
    IIterator<T> *iterator;
};

template<typename T>
class SkipIterable : public IIterable<T> {
public:
    SkipIterable(unsigned k, IIterable<T> *iterable) : k(k), iterable(iterable){}
    
    ~SkipIterable(){
        delete iterable;
    }
    
    IIterator<T>* iter() const override{
        return new SkipIterator<T>(k, iterable->iter());
    }
private:
    unsigned k;
    IIterable<T> *iterable;
};



template<typename T>
IIterable<T>* skip(unsigned int k, IIterable<T>* i) {
    return new SkipIterable<T>(k, i);
}

template<typename T>
class TakeIterator : public IIterator<T> {
public:
    TakeIterator(unsigned k, IIterator<T> *iterator) : k(k), iterator(iterator), n(0){}
    
    ~TakeIterator() {
        delete iterator;
    }
    
    T next() override{
        while(n<k){
            try{
                n++;
                return iterator->next();
            }
            catch(StopIteration &){
                throw StopIteration();
            }
        }
        throw StopIteration();
        return iterator->next();
    }
private:
    unsigned k;
    unsigned int n;
    IIterator<T> *iterator;
};


template<typename T>
class TakeIterable : public IIterable<T> {
public:
    TakeIterable(unsigned k, IIterable<T> *iterable) : k(k), iterable(iterable){}
    
    ~TakeIterable(){
        delete iterable;
    }
    
    IIterator<T>* iter() const override{
        return new TakeIterator<T>(k, iterable->iter());
    }
private:
    unsigned k;
    IIterable<T> *iterable;
};

template<typename T>
IIterable<T>* take (unsigned k ,IIterable<T>* i) {
    return new TakeIterable<T>(k,i);
}

template<typename T>
class AlternateIterator : public IIterator<T> {
public:
    AlternateIterator(IIterator<T>* i, IIterator<T>* j) : i(i), j(j), used(true), secEnd(false){}
    
    ~AlternateIterator(){
        delete i;
        delete j;
    }
    T next() override{
        if(secEnd == true){
            return i->next();
        }
        if(used){
            try{
                used = !used;
                return i->next();
            }
            catch(StopIteration &){
                used = false;
            }
        }
        try{
            used = !used;
            return j->next();
        }catch(StopIteration &){
            secEnd = true;
            return i->next();
        }
        
    }
private:
    IIterator<T> *i, *j;
    bool used, secEnd;
    T value;
};

template<typename T>
class AlternateIterable : public IIterable<T> {
public:
    AlternateIterable(IIterable<T>* i, IIterable<T> *j) : i(i), j(j){}
    
    ~AlternateIterable(){
        delete i;
        delete j;
    }
    
    IIterator<T>* iter() const override{
        return new AlternateIterator<T>(i->iter(), j->iter());
    }
private:
    IIterable<T> *i, *j;
};

template<typename T>
IIterable<T>* alternate(IIterable<T>* i ,IIterable<T>* j){
    return new AlternateIterable<T>(i, j);
}

template<typename T>
class cycleIterator : public IIterator<T> {
public:
    cycleIterator(IIterable<T> *iterable) : iterator(iterable->iter()), iterable(iterable) {}
    
    ~cycleIterator() {
        delete iterator;
    }
    
    T next() override {
        try {
            return iterator->next();
        }
        catch(StopIteration &){
            delete iterator;
            iterator = iterable->iter();
            return iterator->next();
        }
    }
private:
    IIterable<T> *iterable;
    IIterator<T> *iterator;
};

template<typename T>
class cycleIterable : public IIterable<T> {
public:
    cycleIterable(IIterable<T>* iterable) : iterable(iterable) {}
    
    ~cycleIterable() {
        delete iterable;
    }
    
    IIterator<T>* iter() const override {
        return new cycleIterator<T>(iterable);
    }
private:
    IIterable<T>* iterable;
};

template<typename T>
IIterable<T>* cycle(IIterable<T>* i) {
    return new cycleIterable<T>(i);
}


template <typename T>
class multabRowIterator : public IIterator<T> {
public:
    multabRowIterator(IIterable<T>* j, T val) : jterator(j ? j->iter() : nullptr), val(val) {}

    ~multabRowIterator() override {
        delete jterator;
    }

    T next() override {
        if (jterator == nullptr) {
            throw StopIteration();
        }

        try {
            return val * jterator->next();
        }
        catch (StopIteration&) {
            delete jterator;
            jterator = nullptr;
            throw StopIteration();
        }
    }
private:
    IIterator<T>* jterator;
    T val;
};

template <typename T>
class multabRowIterable : public IIterable<T> {
public:
    multabRowIterable(IIterable<T>* j, T val) : jterable(j), val(val) {}

    IIterator<T>* iter() const override {
        return new multabRowIterator<T>(jterable, val);
    }
private:
    IIterable<T>* jterable;
    T val;
};

template <typename T>
class multabIterator : public IIterator<IIterable<T>*> {
public:
    multabIterator(IIterable<T>* i, IIterable<T>* j) : iterable(i), iterator(i ? i->iter() : nullptr), jterable(j) {}

    ~multabIterator() override {
        delete iterator;
    }

    IIterable<T>* next() override {
        if (iterator == nullptr) {
            throw StopIteration();
        }

        try {
            return new multabRowIterable<T>(jterable, iterator->next());
        } catch (StopIteration&) {
            delete iterator;
            iterator = nullptr;
            throw StopIteration();
        }
    }
private:
    IIterable<T>* iterable;
    IIterator<T>* iterator;
    IIterable<T>* jterable;
};

template <typename T>
class multabIterable : public IIterable<IIterable<T>*> {
public:
    multabIterable(IIterable<T>* i, IIterable<T>* j) : iterable(i), jterable(j) {}
    
    ~multabIterable() {
        delete iterable;
        delete jterable;
    }

    IIterator<IIterable<T>*>* iter() const override {
        return new multabIterator<T>(iterable, jterable);
    }
private:
    IIterable<T>* iterable;
    IIterable<T>* jterable;
};

template <typename T>
IIterable<IIterable<T>*>* multab(IIterable<T>* i, IIterable<T>* j) {
    return new multabIterable<T>(i, j);
}

template <typename T>
class flattenIterator : public IIterator<T> {
public:
    flattenIterator(IIterator<IIterable<T>*>* outer)
        : outer_iterator(outer), inner_iterator(nullptr) {}

    ~flattenIterator() override {
        if (inner_iterator) {
            delete inner_iterator;
            inner_iterator = nullptr;
        }
        delete outer_iterator;
    }

    T next() override {
        while (true) {
            if (inner_iterator) {
                try {
                    return inner_iterator->next();
                } catch (StopIteration&) {
                    delete inner_iterator;
                    inner_iterator = nullptr;
                }
            }

            try {
                IIterable<T>* next_iterable = outer_iterator->next();
                inner_iterator = next_iterable->iter();
                delete next_iterable;
            } catch (StopIteration&) {
                throw StopIteration();
            }
        }
    }

private:
    IIterator<IIterable<T>*>* outer_iterator;
    IIterator<T>* inner_iterator;
};

template <typename T>
class flattenIterable : public IIterable<T> {
public:
    flattenIterable(IIterable<IIterable<T>*>* i) : iterable(i) {}

    ~flattenIterable() override {
        delete iterable;
    }

    IIterator<T>* iter() const override {
        return new flattenIterator<T>(iterable->iter());
    }

private:
    IIterable<IIterable<T>*>* iterable;
};

template <typename T>
IIterable<T>* flatten(IIterable<IIterable<T>*>* i) {
    return new flattenIterable<T>(i);
}

