#pragma once


// ***********************
// * ITERABLE INTERFACES *
// ***********************

#include <stdexcept>

class StopIteration : public std::exception {};

template<typename T>
class IIterator
{
    public:
        virtual T next() = 0;
        virtual ~IIterator() {};
};

template<typename T>
class IIterable
{
    public:
        virtual IIterator<T> * iter() const = 0;
        virtual ~IIterable() {};
};


// ********************************
// * FUNCTIONS YOU MUST IMPLEMENT *
// ********************************

template<typename T>
IIterable<T> * repeat(T v);

template<typename T>
IIterable<T> * count(T start, T delta);

template<typename T>
IIterable<T> * skip(unsigned k, IIterable<T> *i);

template<typename T>
IIterable<T> * take(unsigned k, IIterable<T> *i);

template<typename T>
IIterable<T> * alternate(IIterable<T> *i, IIterable<T> *j);

template<typename T>
IIterable<T> * cycle(IIterable<T> *i);

template<typename T>
IIterable<IIterable<T> *> * multab(IIterable<T> *i, IIterable<T> *j);

template<typename T>
IIterable<T> * flatten(IIterable<IIterable<T> *> *i);


// *******************************************
// * ACCUMUATE IMPLEMENTATION PROVIDED BY US *
// *******************************************

// Represents the iterators of the iterables returned by accumulate().
template<typename T>
class AccumulateIterator : public IIterator<T>
{
    private:
        IIterator<T> * const mSourceIterator; // Stores the *source* iterator
                                              // which will be used to iterate
                                              // over the elements of the source
                                              // iterable to which accumulate()
                                              // was applied.

        T mAccumulator; // Stores the accumulator. This is the last value
                        // returned from next() for this iterator.

        bool mHasIterationStarted; // A flag indicating whether next() was
                                   // called on this iterator. mAccumulation is
                                   // valid only if this flag is true.
        
    public:
        // A newly created *source* iterator is passed to this iterator's
        // constructor.
        AccumulateIterator(IIterator<T> *sourceIterator)
            : mSourceIterator(sourceIterator),
              mAccumulator(), // Default construction.
              mHasIterationStarted(false)
        {
        }
        
        T next() override
        {
            if (!mHasIterationStarted)
            {
                // At the first next(), we get the first element from the source
                // iterator and assign it as the accumulator.
                mAccumulator = mSourceIterator->next();
                mHasIterationStarted = true;
            }
            else
            {
                // For further nexts, we pick the next element from the source
                // iterator and add it to the accumulator.
                mAccumulator += mSourceIterator->next();
            }

            // The accumulator is returned as the next element of this iterator.
            return mAccumulator;

            // Recall, we are also expected to throw a StopIteration exception
            // when the elements are exhausted. In this particular case, we do
            // not need to throw the exception ourselves. The source iterator
            // throws the same exception when its elements are exhausted; we
            // simply let it through. From outside, it looks like we threw the
            // exception.
        }
        
        ~AccumulateIterator() override
        {
            // We own the source iterator passed to us. We destroy it when we
            // ourselves are destroyed. Thanks to the virtual destructor, we do
            // not need to know the exact type of the iterator.
            delete mSourceIterator;
        }
};

// Represents the iterables returned by accumulate().
template<typename T>
class AccumulateIterable : public IIterable<T>
{
    private:
        // Stores the source *iterable* to which accumulate() was applied to get
        // this iterable.
        IIterable<T> * const mSourceIterable;
        
    public:
        // The source iterable, to which accumulate() is applied, is passed to
        // this iterable's constructor.
        AccumulateIterable(IIterable<T> *sourceIterable)
            : mSourceIterable(sourceIterable)
        {
        }
        
        IIterator<T> * iter() const override
        {
            // Construct a new AccumulateIterator and return it. We create a
            // brand new iterator on the source iterable and pass it to the
            // AccumulateIterator. The AccumulateIterator can use this source
            // iterator to iterate over the elements of the source iterable.
            return new AccumulateIterator<T>(mSourceIterable->iter());
        }
        
        ~AccumulateIterable() override
        {
            // We own the source iterable passed to us. We destroy it when we
            // ourselves are destroyed.
            delete mSourceIterable;
        }
};

template<typename T>
IIterable<T> * accumulate(IIterable<T> *i)
{
    // Construct a new AccumulateIterable, to which 'i' is passed as the source
    // iterable.
    return new AccumulateIterable<T>(i);
}
