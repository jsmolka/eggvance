#pragma once

#include "common/config.h"
#include "common/input.h"
#include "common/constants.h"
#include "synchronizer.h"
#include "videodevice.h"

class Platform
{
public:
    void init(int argc, char* argv[]);
    void main();

protected:
    void reset();

    virtual void initHookBeg() = 0;
    virtual void initHookEnd() = 0;
    virtual void mainHookBeg() = 0;
    virtual void mainHookEnd() = 0;

    virtual void processEvents() = 0;

    template<typename T>
    void processInput(const ShortcutConfig<T>& shortcuts, T input);
    void updateTitle();

    bool running = true;

private:
    void emulateFrame();

    std::string title;
    Synchronizer synchronizer;
};

template<typename T>
void Platform::processInput(const ShortcutConfig<T>& shortcuts, T input)
{
    if (input == shortcuts.reset)
        reset();

    if (input == shortcuts.fullscreen)
        video_device->fullscreen();

    if (input == shortcuts.fps_default)
        synchronizer.setFps(REFRESH_RATE);

    if (input == shortcuts.fps_custom_1)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[0]);

    if (input == shortcuts.fps_custom_2)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[1]);

    if (input == shortcuts.fps_custom_3)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[2]);

    if (input == shortcuts.fps_custom_4)
        synchronizer.setFps(REFRESH_RATE * config.fps_multipliers[3]);

    if (input == shortcuts.fps_unlimited)
        synchronizer.setFps(REFRESH_RATE * 1000);
}
