#include "Iterable.hxx"
#include "the1.hxx"

#include <iostream>

template<typename T>
void printAndDestroy(IIterable<T> *i, unsigned printLimit = 7u)
{
    IIterator<T> *iterator = i->iter();

    try
    {
        while (true)
        {
            T t = iterator->next();

            if (printLimit == 0u)
            {
                std::cout << "... ";
                break;
            }
            else
            {
                std::cout << t << " ";
                printLimit--;
            }
        }
    }
    catch (StopIteration &)
    {
    }

    std::cout << std::endl;

    delete iterator;
    delete i;
}

int main()
{
    // printAndDestroy(repeat(1));                      // 1 1 1 1 1 1 1 ...
    // printAndDestroy(accumulate(repeat(1)));          // 1 2 3 4 5 6 7 ...
    // printAndDestroy(count(1, 2));                    // 1 3 5 7 9 11 13 ...
    // printAndDestroy(skip(3u, count(1, 2)));          // 7 9 11 13 15 17 19 ...
    // printAndDestroy(take(3u, count(1, 2)));          // 1 3 5
    // printAndDestroy(cycle(take(3u, count(1, 2))));   // 1 3 5 1 3 5 1 ...
    // printAndDestroy(alternate(repeat('p'),
    //                           repeat('q')));         // p q p q p q p ...
    // printAndDestroy(
    //     flatten(multab(count(1.0, 1.0),
    //                    take(3u, count(0.5, 0.5))))); // 0.5 1 1.5 1 2 3 1.5 ...
    return 0;
}
