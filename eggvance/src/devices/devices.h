#pragma once

#include <memory>

#include "audiodevice.h"
#include "inputdevice.h"
#include "videodevice.h"

extern std::shared_ptr<AudioDevice> audio_device;
extern std::shared_ptr<InputDevice> input_device;
extern std::shared_ptr<VideoDevice> video_device;
