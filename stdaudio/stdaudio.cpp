#include "audio.h"
#include <thread>
#include <chrono>

using namespace std::experimental::audio;
using namespace std::literals::chrono_literals;

class LowPassFilter : public effect
{
public:
	void process(float* buffer_in, float* buffer_out, size_t length_samples, int num_channels) override
	{
		const float RC = 1.0f / (1000.0f * 2 * 3.14f);
		const float dt = 1.0f / 48000.0f;
		const float alpha = dt / (RC + dt);
		for (int i = 0; i < num_channels; i++)
		{
			buffer_out[i] = previous_entry[i] + (alpha*(buffer_in[i] - previous_entry[i]));
		}
		for (size_t i = num_channels; i < (length_samples * num_channels); i += num_channels)
		{
			for (int j = 0; j < num_channels; j++)
			{
				int current = i + j;
				int previous = i + j - num_channels;
				buffer_out[current] = buffer_out[previous] + (alpha*(buffer_in[current] - buffer_out[previous]));
			}
		}
		for (int i = 0; i < num_channels; i++)
		{
			previous_entry[i] = buffer_out[(length_samples - 1) * num_channels + i];
		}
	}

private:
	float previous_entry[2] = { 0.0f };
};

int main()
{
	device audio_device;
	auto master = audio_device.create_submix();

	auto sfx = audio_device.create_submix();
	auto music = audio_device.create_submix();
	auto ambience = audio_device.create_submix();
	auto vox = audio_device.create_submix();

	sfx->assign_to_submix(*master);
	music->assign_to_submix(*master);
	ambience->assign_to_submix(*master);
	vox->assign_to_submix(*master);

	sfx->set_volume(0.0625f);
	ambience->add_effect<LowPassFilter>();

	auto tada = load_from_disk(R"(C:\Windows\Media\tada.wav)");
	{
		auto voice = audio_device.play_sound(tada);
		voice->assign_to_submix(*sfx);
		while (voice->is_playing())
		{
			std::this_thread::sleep_for(100ms);
		}
	}
	{
		auto voice = audio_device.play_sound(tada);
		voice->assign_to_submix(*ambience);
		//voice->add_effect<LowPassFilter>();
		while (voice->is_playing())
		{
			std::this_thread::sleep_for(100ms);
		}
	}
	return 0;
}
