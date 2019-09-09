#pragma once

#include <vector>

#include "common/integer.h"
#include "constants.h"
#include "objectdata.h"

// Todo: move into layer?
enum LayerFlag
{
    LF_BG0 = 1 << 0,
    LF_BG1 = 1 << 1,
    LF_BG2 = 1 << 2,
    LF_BG3 = 1 << 3,
    LF_OBJ = 1 << 4,
    LF_BDP = 1 << 5
};

struct Layer
{
    Layer(int id, u16* data, int prio, int flag)
        : id(id), data(data), prio(prio), flag(flag) {};

    inline bool opaque(int x) const
    {
        return data[x] != COLOR_T;
    }

    int  id;
    u16* data;
    int  prio;
    int  flag;
};

struct LayerOld
{
    virtual inline void next() = 0;

    inline bool opaque() const
    {
        return color != COLOR_T;
    }

    inline bool matches(int flags) const
    {
        return flags & flag;
    }

    int color;
    int prio;
    int flag;
};

struct BackdropLayerOld : public LayerOld
{
    BackdropLayerOld(int backdrop)
    {
        color = backdrop;
        prio  = 4;
        flag  = LF_BDP;
    }

    virtual inline void next() final
    {

    }
};

struct BackgroundLayerOld : public LayerOld
{
    BackgroundLayerOld(u16* data, int prio, int flag)
        : data(data)
    {
        this->color = *data;
        this->prio  = prio;
        this->flag  = flag;
    }

    virtual inline void next() final
    {
        data++;
        color = *data;
    }

    u16* data;
};

struct ObjectLayerOld : public LayerOld
{
    ObjectLayerOld(ObjectData* data)
        : data(data)
    {
        color = data->color;
        prio  = data->prio;
        flag  = LF_OBJ;
    }

    virtual inline void next() final
    {
        data++;
        color = data->color;
        prio  = data->prio;
    }

    ObjectData* data;
};

// https://stackoverflow.com/a/57399634/7057528
template<typename T> 
void move(std::vector<T>& v, size_t old_index, size_t new_index)
{
    if (old_index == new_index)
        return;

    if (old_index > new_index)
        std::rotate(v.rend() - old_index - 1, v.rend() - old_index, v.rend() - new_index);
    else        
        std::rotate(v.begin() + old_index, v.begin() + old_index + 1, v.begin() + new_index + 1);
}

struct LayersOld : public std::vector<std::shared_ptr<LayerOld>>
{
    void sort()
    {
        std::sort(begin(), end(), [](const std::shared_ptr<LayerOld>& lhs, const std::shared_ptr<LayerOld>& rhs) {
            if (lhs->prio == rhs->prio)
            {
                if (lhs->flag == LF_OBJ) return true;
                if (rhs->flag == LF_OBJ) return false;
                return lhs->flag < rhs->flag;
            }
            return lhs->prio < rhs->prio;
        });
    }

    void pushBackdrop(const std::shared_ptr<LayerOld>& layer)
    {
        push_back(layer);
    }

    void pushObjects(const std::shared_ptr<LayerOld>& layer)
    {
        for (auto iter = begin(); iter != end(); ++iter)
        {
            obj++;
            if (layer->prio < (*iter)->prio)
            {
                insert(iter, layer);
                return;
            }
        }
        obj++;
        push_back(layer);
    }

    // Todo: this function can be cleaner / faster
    void resort()
    {
        // Use iterswap?
        if (obj < 0)
            return;
        sort();
        //int curp = at(obj)->prio;

        //int x = 0;
        //while (at(x)->prio < curp) x++;

        //move(*this, obj, x);
        //obj = x;
    }

    int obj = -1;
};
