#pragma once

#include <string>
#include <memory>
#include <filesystem>

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
			struct guid
			{
				unsigned int data1;
				unsigned short data2;
				unsigned short data3;
				unsigned char data4[8];
			};

			struct driver_info
			{
				int id;
				std::string name;
				guid guid;
			};

			class channel;
			class submix;
			class sound;

			class device
			{
			public:
				device();
				~device();

				int num_drivers() const;
				driver_info get_driver(int index) const;
				void set_driver(int index);

				std::weak_ptr<sound> load_sound(const std::experimental::filesystem::path& filepath);
				std::weak_ptr<channel> play_sound(const std::shared_ptr<sound>& sound, const std::shared_ptr<submix>& submix = nullptr, bool paused = false);

			private:
				FMOD::System* m_system;
			};

			enum class time_unit
			{
				milliseconds,
				pcm_samples,
			};

			class channel
			{
			public:
				std::weak_ptr<submix> get_submix() const;
				std::weak_ptr<sound> get_sound() const;
				std::weak_ptr<device> get_device() const;

				float get_audibility() const;
				int get_frequency() const;
				unsigned long long get_playback_position(time_unit unit) const;

				int get_loop_count() const;
				float get_pitch() const;
				float get_volume() const;
				float get_pan() const;
				bool is_muted() const;
				bool is_paused() const;

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
				channel();
			};

			class submix
			{
			public:
				int num_channels() const;
				std::weak_ptr<channel> get_channel(int index) const;

				int num_submixes() const;
				std::weak_ptr<submix> get_submix(int index) const;
				std::weak_ptr<submix> get_parent() const;
				std::weak_ptr<device> get_device() const;

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
				std::weak_ptr<device> get_device() const;

			private:
				friend class device;
				sound();
			};
		}
	}
}
