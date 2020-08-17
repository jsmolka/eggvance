#pragma once

#include "core/audiocontext.h"
#include "core/inputcontext.h"
#include "core/videocontext.h"

// Todo: friend core for init

class Context
{
public:
    virtual ~Context();

    void init();

    AudioContext audio;
    InputContext input;
    VideoContext video;

private:
    void deinit();
};
