#include "Audio.h"

#include <iostream>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <filesystem>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#ifdef PlaySound
#undef PlaySound
#endif

namespace
{
    struct CachedSound
    {
        std::string path;

        std::unique_ptr<ma_sound> sound;

        float volume = 1.0f;
		float panning = 0.0f;
    };


    ma_engine g_Engine;

    bool g_Initialized = false;

    TWE::Audio::SoundHandle g_NextSoundHandle = 1;

    std::unordered_map<TWE::Audio::SoundHandle, CachedSound> g_Sounds;

    std::mutex g_AudioMutex;
}

namespace TWE
{

	namespace Audio
	{

		// =========================================================
		// Init
		// =========================================================

		bool Init()
		{
			if (g_Initialized)
				return true;


			ma_engine_config config = ma_engine_config_init();

			ma_result result = ma_engine_init(
				&config,
				&g_Engine
			);


			if (result != MA_SUCCESS)
			{
				std::cerr
					<< "Audio: Failed to initialize audio engine.\n";

				return false;
			}


			g_Initialized = true;


			std::cout
				<< "Audio initialized.\n";


			return true;
		}


		// =========================================================
		// Shutdown
		// =========================================================

		void Shutdown()
		{
			std::lock_guard<std::mutex> lock(g_AudioMutex);


			if (!g_Initialized)
				return;


			for (auto& pair : g_Sounds)
			{
				CachedSound& sound = pair.second;


				if (sound.sound)
				{
					ma_sound_uninit(
						sound.sound.get()
					);
				}
			}


			g_Sounds.clear();


			ma_engine_uninit(&g_Engine);


			g_Initialized = false;


			std::cout
				<< "Audio shut down.\n";
		}
		
		SoundHandle Pre_CacheSound(const std::string& path)
		{
			if (!g_Initialized)
			{
				std::cerr
					<< "Audio: Audio system is not initialized.\n";

				return 0;
			}

			// Check the path first
			if (!std::filesystem::exists(path))
			{
				std::cerr
					<< "Audio: File does not exist:\n"
					<< "  " << path << "\n";

				std::cerr
					<< "Audio: Current working directory:\n"
					<< "  " << std::filesystem::current_path() << "\n";

				return 0;
			}

			std::cout
				<< "Audio: Loading:\n"
				<< "  " << std::filesystem::absolute(path)
				<< "\n";


			std::lock_guard<std::mutex> lock(g_AudioMutex);


			// Check if already cached
			for (const auto& pair : g_Sounds)
			{
				if (pair.second.path == path)
				{
					return pair.first;
				}
			}


			auto sound = std::make_unique<ma_sound>();


			ma_result result = ma_sound_init_from_file(
				&g_Engine,
				path.c_str(),
				MA_SOUND_FLAG_DECODE,
				nullptr,
				nullptr,
				sound.get()
			);


			if (result != MA_SUCCESS)
			{
				std::cerr
					<< "Audio: miniaudio failed to load:\n"
					<< "  " << path << "\n"
					<< "  Error code: " << result << "\n"
					<< "  Error: " << ma_result_description(result)
					<< "\n";

				return 0;
			}


			SoundHandle handle = g_NextSoundHandle++;


			CachedSound cachedSound;

			cachedSound.path = path;
			cachedSound.sound = std::move(sound);
			cachedSound.volume = 1.0f;


			g_Sounds.emplace(
				handle,
				std::move(cachedSound)
			);


			std::cout
				<< "Audio: Precached \""
				<< path
				<< "\" as sound "
				<< handle
				<< "\n";


			return handle;
		}

		// =========================================================
		// PlaySound
		// =========================================================

		void PlaySound(
			SoundHandle sound,
			bool loop
		)
		{
			if (!g_Initialized)
				return;


			std::lock_guard<std::mutex> lock(g_AudioMutex);


			auto it = g_Sounds.find(sound);


			if (it == g_Sounds.end())
			{
				std::cerr
					<< "Audio: Invalid sound handle: "
					<< sound
					<< "\n";

				return;
			}


			CachedSound& cachedSound = it->second;


			ma_sound* audio = cachedSound.sound.get();


			// -----------------------------------------------------
			// Set looping
			// -----------------------------------------------------

			ma_sound_set_looping(
				audio,
				loop
			);


			// -----------------------------------------------------
			// Reset to beginning
			// -----------------------------------------------------

			ma_sound_seek_to_pcm_frame(
				audio,
				0
			);


			// -----------------------------------------------------
			// Apply volume
			// -----------------------------------------------------

			ma_sound_set_volume(
				audio,
				cachedSound.volume
			);

			// -----------------------------------------------------
			// Apply panning
			// -----------------------------------------------------

			ma_sound_set_pan(
                audio,
                cachedSound.panning
            );


			// -----------------------------------------------------
			// Play
			// -----------------------------------------------------

			ma_result result = ma_sound_start(audio);


			if (result != MA_SUCCESS)
			{
				std::cerr
					<< "Audio: Failed to play sound "
					<< sound
					<< "\n";
			}
		}


		// =========================================================
		// StopSound
		// =========================================================

		void StopSound(
			SoundHandle sound
		)
		{
			if (!g_Initialized)
				return;


			std::lock_guard<std::mutex> lock(g_AudioMutex);


			auto it = g_Sounds.find(sound);


			if (it == g_Sounds.end())
				return;


			CachedSound& cachedSound = it->second;


			ma_sound_stop(
				cachedSound.sound.get()
			);


			// Reset position so the next PlaySound starts
			// from the beginning.

			ma_sound_seek_to_pcm_frame(
				cachedSound.sound.get(),
				0
			);
		}


		// =========================================================
		// SetVolume
		// =========================================================

		void SetVolume(
			SoundHandle sound,
			float volume
		)
		{
			if (!g_Initialized)
				return;


			std::lock_guard<std::mutex> lock(g_AudioMutex);


			auto it = g_Sounds.find(sound);


			if (it == g_Sounds.end())
				return;


			CachedSound& cachedSound = it->second;


			// Clamp volume

			if (volume < 0.0f)
				volume = 0.0f;

			if (volume > 1.0f)
				volume = 1.0f;


			cachedSound.volume = volume;


			ma_sound_set_volume(
				cachedSound.sound.get(),
				volume
			);
		}
		
		void SetPanning(SoundHandle sound, float panning)
		{
			if (!g_Initialized)
				return;
			
			std::lock_guard<std::mutex> lock(g_AudioMutex);


			auto it = g_Sounds.find(sound);


			if (it == g_Sounds.end())
				return;


			CachedSound& cachedSound = it->second;


			// Clamp panning

			if (panning < -1.0f)
				panning = -1.0f;

			if (panning > 1.0f)
				panning = 1.0f;


			cachedSound.panning = panning;


			ma_sound_set_pan(
				cachedSound.sound.get(),
				panning
			);
		}

		// =========================================================
        // PlaySound3D
        // =========================================================

        void PlaySound3D(
            SoundHandle sound, 
			glm::vec3 position,
            bool loop
        )
        {
            if (!g_Initialized)
                return;


            std::lock_guard<std::mutex> lock(g_AudioMutex);


            auto it = g_Sounds.find(sound);


            if (it == g_Sounds.end())
            {
                std::cerr
                    << "Audio: Invalid sound handle: "
                    << sound
                    << "\n";

                return;
            }


            CachedSound& cachedSound =
                it->second;


            ma_sound* audio =
                cachedSound.sound.get();


            // Enable 3D spatialization.

            ma_sound_set_spatialization_enabled(
                audio,
                MA_TRUE
            );

			ma_sound_set_position(
				audio,
				position.x,
				position.y,
				position.z
			);


            ma_sound_set_looping(
                audio,
                loop
            );


            ma_sound_seek_to_pcm_frame(
                audio,
                0
            );


            ma_sound_set_volume(
                audio,
                cachedSound.volume
            );


            ma_result result =
                ma_sound_start(audio);


            if (result != MA_SUCCESS)
            {
                std::cerr
                    << "Audio: Failed to play 3D sound "
                    << sound
                    << "\n";
            }
        }


        // =========================================================
        // StopSound3D
        // =========================================================

        void StopSound3D(
            SoundHandle sound
        )
        {
            if (!g_Initialized)
                return;


            std::lock_guard<std::mutex> lock(g_AudioMutex);


            auto it = g_Sounds.find(sound);


            if (it == g_Sounds.end())
                return;


            ma_sound* audio =
                it->second.sound.get();


            ma_sound_stop(audio);


            ma_sound_seek_to_pcm_frame(
                audio,
                0
            );
        }


        // =========================================================
        // SetVolume3D
        // =========================================================

        void SetVolume3D(
            SoundHandle sound,
            float volume
        )
        {
            SetVolume(
                sound,
                volume
            );
        }


        // =========================================================
        // SetPanning3D
        // =========================================================

        void SetPanning3D(
            SoundHandle sound,
            float panning
        )
        {
            SetPanning(
                sound,
                panning
            );
        }


	}
}
