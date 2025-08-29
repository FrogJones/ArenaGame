#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx.h>
#include <sndfile.h>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

class AudioManager {
public:
    AudioManager() : device(nullptr), context(nullptr), efxSupported(false) {}

    ~AudioManager() {
        // Clean up effects and auxiliary effect slots
        for (auto& pair : effects) {
            alDeleteEffects(1, &pair.second);
        }
        for (auto slot : auxSlots) {
            alDeleteAuxiliaryEffectSlots(1, &slot);
        }
        
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

        // Check for EFX support
        efxSupported = alcIsExtensionPresent(device, "ALC_EXT_EFX");
        if (efxSupported) {
            std::cout << "OpenAL initialized with EFX support!\n";
            initializeEFXFunctions();
            createPresetEffects();
        } else {
            std::cout << "OpenAL initialized without EFX support\n";
        }

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

    // NEW: Play sound with effect
    ALuint playSoundWithEffect(ALuint buffer, const std::string& effectName, bool loop = false) {
        if (!efxSupported) {
            return playSound(buffer, loop); // Fallback to normal playback
        }

        ALuint source = playSound(buffer, loop);
        applyEffectToSource(source, effectName);
        return source;
    }

    // NEW: Apply effect to existing source
    void applyEffectToSource(ALuint source, const std::string& effectName) {
        if (!efxSupported) return;

        auto it = effects.find(effectName);
        if (it != effects.end()) {
            ALuint slot = getAuxiliaryEffectSlot();
            alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, it->second);
            
            // Connect source to effect slot
            alSource3i(source, AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL);
        }
    }

    // NEW: Remove effect from source
    void removeEffectFromSource(ALuint source) {
        if (!efxSupported) return;
        alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
    }

    // NEW: Create custom reverb effect for dungeons/caves
    void createDungeonReverb() {
        if (!efxSupported) return;

        ALuint effect;
        alGenEffects(1, &effect);
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

        // Dungeon-like reverb settings
        alEffectf(effect, AL_REVERB_DENSITY, 1.0f);
        alEffectf(effect, AL_REVERB_DIFFUSION, 1.0f);
        alEffectf(effect, AL_REVERB_GAIN, 0.316f);
        alEffectf(effect, AL_REVERB_GAINHF, 0.1f);
        alEffectf(effect, AL_REVERB_DECAY_TIME, 2.91f);
        alEffectf(effect, AL_REVERB_DECAY_HFRATIO, 1.3f);
        alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, 0.5f);
        alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, 0.15f);
        alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, 1.26f);
        alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, 0.011f);
        alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, 0.994f);
        alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, 0.0f);
        alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, AL_TRUE);

        effects["dungeon"] = effect;
    }

    // NEW: Create echo effect for footsteps - improved for dungeon atmosphere
    void createEchoEffect() {
        if (!efxSupported) return;

        ALuint effect;
        alGenEffects(1, &effect);
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_ECHO);

        // Better echo settings for stone dungeon footsteps
        alEffectf(effect, AL_ECHO_DELAY, 0.18f);       // Longer delay for bigger space feel
        alEffectf(effect, AL_ECHO_LRDELAY, 0.22f);     // Asymmetric left-right for realism
        alEffectf(effect, AL_ECHO_DAMPING, 0.7f);      // More damping for stone surfaces
        alEffectf(effect, AL_ECHO_FEEDBACK, 0.15f);    // Less feedback to avoid muddy sound
        alEffectf(effect, AL_ECHO_SPREAD, 0.3f);       // Moderate stereo spread

        effects["echo"] = effect;
    }

    // NEW: Create a subtle short reverb for close surfaces (walls, objects)
    void createShortReverb() {
        if (!efxSupported) return;

        ALuint effect;
        alGenEffects(1, &effect);
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

        // Short reverb for nearby wall reflections
        alEffectf(effect, AL_REVERB_DENSITY, 0.8f);
        alEffectf(effect, AL_REVERB_DIFFUSION, 0.9f);
        alEffectf(effect, AL_REVERB_GAIN, 0.4f);
        alEffectf(effect, AL_REVERB_GAINHF, 0.6f);
        alEffectf(effect, AL_REVERB_DECAY_TIME, 0.8f);     // Much shorter decay
        alEffectf(effect, AL_REVERB_DECAY_HFRATIO, 0.9f);
        alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, 0.8f);
        alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, 0.02f);  // Quick reflections
        alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, 0.4f);
        alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, 0.03f);
        alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, 0.98f);
        alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, 0.2f);
        alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, AL_TRUE);

        effects["short_reverb"] = effect;
    }

    // NEW: Create distortion effect for dramatic sounds
    void createDistortionEffect() {
        if (!efxSupported) return;

        ALuint effect;
        alGenEffects(1, &effect);
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);

        alEffectf(effect, AL_DISTORTION_EDGE, 0.2f);
        alEffectf(effect, AL_DISTORTION_GAIN, 0.05f);
        alEffectf(effect, AL_DISTORTION_LOWPASS_CUTOFF, 8000.0f);
        alEffectf(effect, AL_DISTORTION_EQCENTER, 3600.0f);
        alEffectf(effect, AL_DISTORTION_EQBANDWIDTH, 3600.0f);

        effects["distortion"] = effect;
    }

    // NEW: Get available effect names
    std::vector<std::string> getAvailableEffects() const {
        std::vector<std::string> effectNames;
        for (const auto& pair : effects) {
            effectNames.push_back(pair.first);
        }
        return effectNames;
    }

    bool isEFXSupported() const { return efxSupported; }

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ALuint> buffers;
    std::vector<ALuint> sources;
    std::vector<ALuint> auxSlots;
    std::unordered_map<std::string, ALuint> effects;
    bool efxSupported;

    // EFX function pointers (these need to be loaded dynamically)
    LPALGENEFFECTS alGenEffects;
    LPALDELETEEFFECTS alDeleteEffects;
    LPALISEFFECT alIsEffect;
    LPALEFFECTI alEffecti;
    LPALEFFECTIV alEffectiv;
    LPALEFFECTF alEffectf;
    LPALEFFECTFV alEffectfv;
    LPALGETEFFECTI alGetEffecti;
    LPALGETEFFECTIV alGetEffectiv;
    LPALGETEFFECTF alGetEffectf;
    LPALGETEFFECTFV alGetEffectfv;
    LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
    LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
    LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
    LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
    LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
    LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
    LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
    LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
    LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
    LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
    LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

    void initializeEFXFunctions() {
        // Load EFX functions dynamically
        alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
        alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
        alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
        alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
        alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
        alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
        alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
        alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
        alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
        alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
        alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");

        alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
        alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
        alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
        alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
        alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
        alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
        alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
        alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
        alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
        alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
        alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
    }

    void createPresetEffects() {
        createDungeonReverb();
        createEchoEffect();
        createShortReverb();
        createDistortionEffect();
    }

    ALuint getAuxiliaryEffectSlot() {
        ALuint slot;
        alGenAuxiliaryEffectSlots(1, &slot);
        auxSlots.push_back(slot);
        return slot;
    }
};