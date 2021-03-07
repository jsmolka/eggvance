#pragma once

#include "base/int.h"
#include "base/macros.h"

template<typename T>
struct Node
{
    T* prev = nullptr;
    T* next = nullptr;
};

struct Event : Node<Event>
{
    using Callback = void(*)(void*, u64);

    u64 when = 0;
    void* data = nullptr;
    Callback callback = nullptr;
};

class List
{
public:
    void insert(Event* node)
    {
        if (head == nullptr)
        {
            head = node;
            head->next = nullptr;
            head->prev = nullptr;
        }
        else if (node->when < head->when)
        {
            head->prev = node;
            node->next = head;
            node->prev = nullptr;
            head = node;
        }
        else
        {
            Event* iter = head;
            while (iter->next && iter->next->when < node->when)
                iter = iter->next;

            node->next = iter->next;
            node->prev = iter;

            if (node->next)
                node->next->prev = node;

            iter->next = node;
        }
    }

    void remove(Event* node)
    {
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

    Event* head = nullptr;
};
