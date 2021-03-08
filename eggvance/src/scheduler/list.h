#pragma once

#include <type_traits>

#include "base/macros.h"

template<typename T>
class Node
{
public:
    T* prev = nullptr;
    T* next = nullptr;
};

template<typename T>
class List
{
public:
    static_assert(std::is_base_of_v<Node<T>, T>);

    void insert(T& value)
    {
        T* node = &value;

        if (head == nullptr)
        {
            head = node;
            head->next = nullptr;
            head->prev = nullptr;
        }
        else if (*node < *head)
        {
            head->prev = node;
            node->next = head;
            node->prev = nullptr;
            head = node;
        }
        else
        {
            T* iter = head;
            while (iter->next && *iter->next < *node)
                iter = iter->next;

            node->next = iter->next;
            node->prev = iter;

            if (node->next)
                node->next->prev = node;

            iter->next = node;
        }
    }

    void erase(T& value)
    {
        T* node = &value;

        if (head == node)
        {
            head = node->next;

            if (head)
                head->prev = nullptr;

            node->next = nullptr;
        }
        else
        {
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;

            node->next = nullptr;
            node->prev = nullptr;
        }
    }

    T& pop()
    {
        SHELL_ASSERT(head);

        T* node = head;
        T* next = head->next;
        
        head->next = nullptr;
        head = next;

        if (next->prev)
            next->prev = nullptr;

        return *node;
    }

    T* head = nullptr;
};
