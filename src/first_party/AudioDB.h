#ifndef AUDIO_DB_H
#define AUDIO_DB_H

#include "AudioHelper.h"

#include <unordered_map>

class AudioDB
{
public:
    static void Init();
    static int PlayChannel(int channel, const std::string& audio_name, bool looping);
    static int HaltChannel(int channel);
    static int SetVolume(int channel, int volume);
private:
    static Mix_Chunk* GetAudio(const std::string& audio_name);
    static void LoadAudio(const std::string& audio_name);
    static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
    static inline const std::vector<std::string> extensions = { ".wav", ".ogg" };
};

#endif
