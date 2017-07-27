#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <cstdint>

namespace FMOD
{
	class System;
	class Channel;
	class ChannelGroup;
	class Sound;
}

namespace std
{
	namespace experimental
	{
		namespace audio
		{
			class device;
			class voice;
			class submix;
			class sound;

			struct guid
			{
				unsigned int data1;
				unsigned short data2;
				unsigned short data3;
				unsigned char data4[8];
			};

			struct driver_info
			{
				std::string name;
				guid guid;
			};

			template<typename T>
			class handle
			{
			public:
				handle() : 
					m_lookup(&default_lookup),
					m_key(0)
				{}

				T* get()
				{
					auto found = m_lookup->find(m_key);
					if (found != m_lookup->end())
					{
						return &found->second;
					}
					else
					{
						return nullptr;
					}
				}

				const T* get() const
				{
					return const_cast<handle<T>*>(this)->get();
				}

			private:
				static std::unordered_map<int, T> default_lookup;

				friend class device;
				handle(
					std::unordered_map<int, T>* lookup,
					int key) :
					m_lookup(lookup),
					m_key(key)
				{
				}

				std::unordered_map<int, T>* m_lookup;
				int m_key;
			};

			class device
			{
			public:
				device();
				~device();

				int num_drivers() const;
				driver_info get_driver(int index) const;
				void set_driver(int index);

				handle<sound> load_sound(const std::experimental::filesystem::path& filepath);
				handle<voice> play_sound(const handle<sound>& sound, const handle<submix>& submix = handle<std::experimental::audio::submix>{}, bool paused = false);

			private:
				FMOD::System* m_system;
				std::unordered_map<int, sound> m_sounds;
				std::unordered_map<int, voice> m_channels;
				int m_next_id = 0;
			};

			enum class time_unit
			{
				milliseconds,
				pcm_samples,
			};

			class voice
			{
			public:
				handle<submix> get_submix() const;
				handle<sound> get_sound() const;
				device* get_device() const;

				float get_audibility() const;
				int get_frequency() const;
				unsigned long long get_playback_position(time_unit unit) const;

				int get_loop_count() const;
				float get_pitch() const;
				float get_volume() const;
				float get_pan() const;
				bool is_muted() const;
				bool is_paused() const;
				bool is_playing() const;

				void set_loop_count(int count);
				void set_pitch(float pitch);
				void set_volume(float volume);
				void set_pan(float pan);
				void set_mute(bool mute);
				void set_paused(bool pause);
				void set_playback_position(unsigned long long position, time_unit unit);

				void stop();

			private:
				friend class device;
				device* m_device;
				FMOD::Channel* m_channel;
				voice(device* device, FMOD::Channel* channel);
			};

			class submix
			{
			public:
				int num_channels() const;
				handle<voice> get_channel(int index) const;

				int num_submixes() const;
				handle<submix> get_submix(int index) const;
				handle<submix> get_parent() const;
				device* get_device() const;

				float get_audibility() const;
				float get_pitch() const;
				float get_volume() const;
				bool is_muted() const;
				bool is_paused() const;

				void set_pitch(float pitch);
				void set_volume(float volume);
				void set_mute(bool mute);
				void set_paused(bool pause);

				void release();

			private:
				friend class device;
				submix();
			};

			class sound
			{
			public:
				enum class format
				{
					unknown,
					pcm_8,
					pcm_16,
					pcm_24,
					pcm_32,
					pcm_float,
				};

				int get_frequency() const;
				int get_channels() const;
				format get_format() const;
				device* get_device() const;

			private:
				friend class device;
				device* m_device;
				FMOD::Sound* m_sound;
				sound(device* device, FMOD::Sound* sound);
			};
		}
	}
}
