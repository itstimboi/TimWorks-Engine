// #pragma once
// 
// #include <string>
// #include <cstdint>
// 
// using SoundHandle = uint32_t;
// 
// class Audio
// {
// public:
//     // Initialize / shut down the audio system
//     static bool Init();
//     static void Shutdown();
// 
//     // Load and decode a sound into memory
//     static SoundHandle Pre_CacheSound(const std::string& path);
// 
//     // Playback
//     static void PlaySound(SoundHandle sound, bool loop = false);
//     static void StopSound(SoundHandle sound);
// 
//     // Volume
//     static void SetVolume(SoundHandle sound, float volume);
// 
// private:
//     Audio() = delete;
// };

#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace TWE
{
	namespace Audio
	{
		using SoundHandle = uint32_t;
		
		bool Init();
		void Shutdown();

		SoundHandle Pre_CacheSound(const std::string& path);

		void PlaySound(SoundHandle sound, bool loop = false);
		void StopSound(SoundHandle sound);

		void SetVolume(SoundHandle sound, float volume);
		void SetPanning(SoundHandle sound, float panning);

		void PlaySound3D(SoundHandle sound, glm::vec3 position, bool loop = false);
		void StopSound3D(SoundHandle sound);

		void SetVolume3D(SoundHandle sound, float volume);
		void SetPanning3D(SoundHandle sound, float panning);
	}
}