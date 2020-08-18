#pragma once

#include "core/audiocontext.h"
#include "core/inputcontext.h"
#include "core/videocontext.h"

class Context
{
public:
    friend class Core;

    ~Context();

    AudioContext audio;
    InputContext input;
    VideoContext video;

private:
    void init();
    void deinit();
};
