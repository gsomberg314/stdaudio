#include "audio.h"
#include <thread>
#include <chrono>

using namespace std::experimental::audio;
using namespace std::literals::chrono_literals;

class LowPassFilter : public effect
{
	void process(float* buffer_in, float* buffer_out, size_t length_samples, int num_channels) override
	{
		const float RC = 1.0f / (1000.0f * 2 * 3.14f);
		const float dt = 1.0f / 48000.0f;
		const float alpha = dt / (RC + dt);
		for (int i = 0; i < num_channels; i++)
		{
			buffer_out[i] = buffer_in[i];
		}
		for (int i = num_channels; i < (length_samples * num_channels); i += num_channels)
		{
			for (int j = 0; j < num_channels; j++)
			{
				int current = i + j;
				int previous = i + j - num_channels;
				buffer_out[current] = buffer_out[previous] + (alpha*(buffer_in[current] - buffer_out[previous]));
			}
		}
	}
};

int main()
{
	device audio_device;
	auto tada = load_from_disk(R"(C:\Windows\Media\tada.wav)");
	{
		auto voice = audio_device.play_sound(tada);
		while (voice->is_playing())
		{
			std::this_thread::sleep_for(100ms);
		}
	}
	{
		auto voice = audio_device.play_sound(tada);
		voice->add_effect<LowPassFilter>();
		while (voice->is_playing())
		{
			std::this_thread::sleep_for(100ms);
		}
	}
	return 0;
}
