#pragma once

#include <type_traits>
#include <shell/macros.h>

#include "node.h"

template<typename T>
class CircularList
{
public:
    static_assert(std::is_base_of_v<Node<T>, T>);

    void setHead(T& item)
    {
        head = &item;
        head->prev = head;
        head->next = head;
    }

    void insert(T& item)
    {
        T*  node = &item;
        T** iter = &head;

        while (**iter < *node)
            iter = &(*iter)->next;

        node->prev = (*iter)->prev;
        node->next = (*iter);
        node->prev->next = node;
        node->next->prev = node;

        (*iter) = node;
    }

    void erase(T& item)
    {
        SHELL_ASSERT(item.prev);
        SHELL_ASSERT(item.next);

        T* node = &item;

        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (head == node)
            head = node->next;
    }

    T& pop()
    {
        SHELL_ASSERT(head);

        T* node = head;

        head->prev->next = head->next;
        head->next->prev = head->prev;
        head = head->next;

        return *node;
    }

    T* head = nullptr;
};
