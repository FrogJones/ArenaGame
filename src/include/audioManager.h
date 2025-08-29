#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <vector>
#include <iostream>
#include <string>

class AudioManager {
public:
    AudioManager() : device(nullptr), context(nullptr) {}

    ~AudioManager() {
        // Clean up sources and buffers
        for (auto src : sources) alDeleteSources(1, &src);
        for (auto buf : buffers) alDeleteBuffers(1, &buf);
        if (context) alcMakeContextCurrent(nullptr);
        if (context) alcDestroyContext(context);
        if (device) alcCloseDevice(device);
    }

    bool init() {
        device = alcOpenDevice(nullptr);
        if (!device) {
            std::cerr << "Failed to open OpenAL device\n";
            return false;
        }

        context = alcCreateContext(device, nullptr);
        if (!context || !alcMakeContextCurrent(context)) {
            std::cerr << "Failed to create OpenAL context\n";
            if (context) alcDestroyContext(context);
            alcCloseDevice(device);
            return false;
        }

        std::cout << "OpenAL initialized!\n";
        return true;
    }

    ALuint loadAudio(const std::string& filename) {
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
        if (!sndfile) {
            std::cerr << "Failed to open audio file: " << filename << "\n";
            return 0;
        }

        std::vector<short> samples(sfinfo.frames * sfinfo.channels);
        sf_read_short(sndfile, samples.data(), samples.size());
        sf_close(sndfile);

        ALenum format = (sfinfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, samples.data(), samples.size() * sizeof(short), sfinfo.samplerate);

        buffers.push_back(buffer);
        return buffer;
    }

    ALuint playSound(ALuint buffer, bool loop = false) {
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSourcePlay(source);

        sources.push_back(source);
        return source;
    }

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ALuint> buffers;
    std::vector<ALuint> sources;
};
