#pragma once

#include <memory>

#include "device.h"

class AudioDevice : public Device
{

};

extern std::shared_ptr<AudioDevice> audio_device;
