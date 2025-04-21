#include "AudioDB.h"


// Open audio channels and make allocate
void AudioDB::Init()
{
    AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    AudioHelper::Mix_AllocateChannels(50);
}

// Play a sound on channel
int AudioDB::PlayChannel(int channel, const std::string& audio_name, bool looping)
{
    if (looping)
        return AudioHelper::Mix_PlayChannel(channel, GetAudio(audio_name), -1);
    return AudioHelper::Mix_PlayChannel(channel, GetAudio(audio_name), 0);
}

// Halt sound playing on channel
int AudioDB::HaltChannel(int channel)
{
    return AudioHelper::Mix_HaltChannel(channel);
}

// Halt sound playing on channel
int AudioDB::SetVolume(int channel, int volume)
{
    return AudioHelper::Mix_Volume(channel, volume);
}

// Alter the volume of a channel
Mix_Chunk* AudioDB::GetAudio(const std::string& audio_name)
{
    auto it = loaded_audio.find(audio_name);
    if (it == loaded_audio.end())
    {
        LoadAudio(audio_name);
        it = loaded_audio.find(audio_name);
    }

    return it->second;
}

// Load audio from file by name
void AudioDB::LoadAudio(const std::string& audio_name)
{
    std::string file_path = "resources/audio/" + audio_name;
    for (const auto& ext : extensions)
    {
        if (std::filesystem::exists(file_path + ext))
        {
            loaded_audio.insert({ audio_name, AudioHelper::Mix_LoadWAV((file_path + ext).c_str()) });
            return;
        }
    }

    std::cout << "error: failed to play audio clip " << audio_name;
    exit(0);
}
