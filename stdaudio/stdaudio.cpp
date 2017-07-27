#include "audio.h"
#include <thread>
#include <chrono>

using namespace std::experimental::audio;
using namespace std::literals::chrono_literals;

int main()
{
	device audio_device;
	auto tada = audio_device.load_sound(R"(C:\Windows\Media\tada.wav)");
	auto voice = audio_device.play_sound(tada);
	while (voice.get()->is_playing())
	{
		std::this_thread::sleep_for(100ms);
	}
    return 0;
}
