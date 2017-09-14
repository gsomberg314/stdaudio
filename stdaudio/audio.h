#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <cstdint>
#include <cstddef>
#include <variant>
#include <vector>

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
			class source;
			class buffer;
			class submix;
			class dsp;

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

			class device
			{
			public:
				device();
				~device();

				int num_drivers() const;
				driver_info get_driver(int index) const;
				void set_driver(int index);

				std::unique_ptr<voice> play_sound(const std::shared_ptr<source>& sound, bool paused = false);
				std::unique_ptr<submix> create_submix();

			private:
				FMOD::System* m_system;
			};

			class voice
			{
			private:
				struct constructor_tag {};
			public:
				voice(FMOD::Channel* channel, FMOD::Sound* fmod_sound, const std::shared_ptr<source>& sound, constructor_tag);
				~voice();
				void stop();
				void pause();
				void resume();
				void set_volume(float volume);
				void set_pitch(float pitch);
				void set_pan(float pan);

				float get_volume() const;
				float get_pitch() const;
				float get_pan() const;

				bool is_playing() const;

				void assign_to_submix(submix& parent);

			private:
				friend class device;
				FMOD::Channel* m_channel;
				FMOD::Sound* m_sound;
				std::shared_ptr<source> m_source;
				float pan = 0.0f;
			};

			struct memory_buffer
			{
				memory_buffer() = default;
				memory_buffer(const std::byte* indata, size_t insize) : data(indata), size(insize) {}
				const std::byte* data = nullptr;
				size_t size = 0;
			};

			enum class memory_buffer_format
			{
				pcm8,
				pcm16,
				pcm24,
				pcm32,
				pcmfloat,
			};

			struct memory_buffer_description
			{
				memory_buffer_format format;
				unsigned int num_channels;
				unsigned int frequency;
			};

			struct memory_buffer_data
			{
				memory_buffer data;
				memory_buffer_description description;
			};

			class source
			{
			public:
				virtual ~source() {}
				virtual memory_buffer_data get_audio_data() const = 0;
			};

			class buffer : public source
			{
			public:
				memory_buffer_data get_audio_data() const override;

			private:
				friend std::shared_ptr<buffer> load_from_disk(const std::experimental::filesystem::path&);
				friend std::shared_ptr<buffer> load_from_memory(const memory_buffer&, const memory_buffer_description&, bool);
				std::variant<std::vector<std::byte>, memory_buffer> m_data;
				memory_buffer_description m_description;
			};

			std::shared_ptr<buffer> load_from_disk(const std::experimental::filesystem::path& filepath);
			std::shared_ptr<buffer> load_from_memory(const memory_buffer& buffer, const memory_buffer_description& description, bool copy = true);

			class submix
			{
			private:
				struct constructor_tag {};
			public:
				submix(FMOD::ChannelGroup* channelgroup, constructor_tag);

				float get_volume() const;

				void set_volume(float volume);

				void assign_to_submix(submix& parent);

			private:
				friend class device;
				friend class voice;
				FMOD::ChannelGroup* m_channelgroup;
			};
		}
	}
}
