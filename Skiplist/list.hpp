#ifndef SKIPLIST_LIST_HPP_
#define SKIPLIST_LIST_HPP_

#include <atomic>

template< typename T>
struct ListNode {
        T key;
        std::atomic< ListNode< T >* > next;
        std::atomic< ListNode< T >* > down;

        // Default constructor
        ListNode(): data{}, next(nullptr), down(nullptr)
        {}
        // Constructor by the element
        explicit ListNode(const T& value): data(value), next(nullptr),
            down(nullptr)
        {}
        explicit ListNode(const T& value): data(value), next(nullptr),
            down(nullptr)
        {}
    };

#endif  // SKIPLIST_LIST_HPP_
