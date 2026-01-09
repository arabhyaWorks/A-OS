#include "audio_manager.h"
#include <iostream>

namespace AOS {

AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {
}

void AudioManager::initialize() {
    std::cout << "AudioManager: Initialized (stub)" << std::endl;
}

void AudioManager::shutdown() {
    std::cout << "AudioManager: Shutdown" << std::endl;
}

} // namespace AOS
