#include "audio.h"

std::experimental::audio::device::~device()
{
}

int std::experimental::audio::device::num_drivers() const
{
	return 0;
}

auto std::experimental::audio::device::get_driver(int index) const -> driver_info
{
	return driver_info();
}

void std::experimental::audio::device::set_driver(int index)
{
}

auto std::experimental::audio::channel::get_submix() const -> std::weak_ptr<submix>
{
	return std::weak_ptr<submix>();
}

auto std::experimental::audio::channel::get_sound() const -> std::weak_ptr<sound>
{
	return std::weak_ptr<sound>();
}

auto std::experimental::audio::channel::get_device() const -> std::weak_ptr<device>
{
	return weak_ptr<device>();
}

float std::experimental::audio::channel::get_audibility() const
{
	return 0.0f;
}

int std::experimental::audio::channel::get_frequency() const
{
	return 0;
}

unsigned long long std::experimental::audio::channel::get_playback_position(time_unit unit) const
{
	return 0;
}

int std::experimental::audio::channel::get_loop_count() const
{
	return 0;
}

float std::experimental::audio::channel::get_pitch() const
{
	return 0.0f;
}

float std::experimental::audio::channel::get_volume() const
{
	return 0.0f;
}

float std::experimental::audio::channel::get_pan() const
{
	return 0.0f;
}

bool std::experimental::audio::channel::is_muted() const
{
	return false;
}

bool std::experimental::audio::channel::is_paused() const
{
	return false;
}

void std::experimental::audio::channel::set_loop_count(int count)
{
}

void std::experimental::audio::channel::set_pitch(float pitch)
{
}

void std::experimental::audio::channel::set_volume(float volume)
{
}

void std::experimental::audio::channel::set_pan(float pan)
{
}

void std::experimental::audio::channel::set_mute(bool mute)
{
}

void std::experimental::audio::channel::set_paused(bool pause)
{
}

void std::experimental::audio::channel::set_playback_position(unsigned long long position, time_unit unit)
{
}

void std::experimental::audio::channel::stop()
{
}

int std::experimental::audio::submix::num_channels() const
{
	return 0;
}

auto std::experimental::audio::submix::get_channel(int index) const -> std::weak_ptr<channel>
{
	return std::weak_ptr<channel>();
}

int std::experimental::audio::submix::num_submixes() const
{
	return 0;
}

auto std::experimental::audio::submix::get_submix(int index) const -> std::weak_ptr<submix>
{
	return std::weak_ptr<submix>();
}

auto std::experimental::audio::submix::get_parent() const -> std::weak_ptr<submix>
{
	return std::weak_ptr<submix>();
}

auto std::experimental::audio::submix::get_device() const -> std::weak_ptr<device>
{
	return weak_ptr<device>();
}

float std::experimental::audio::submix::get_audibility() const
{
	return 0.0f;
}

float std::experimental::audio::submix::get_pitch() const
{
	return 0.0f;
}

float std::experimental::audio::submix::get_volume() const
{
	return 0.0f;
}

bool std::experimental::audio::submix::is_muted() const
{
	return false;
}

bool std::experimental::audio::submix::is_paused() const
{
	return false;
}

void std::experimental::audio::submix::set_pitch(float pitch)
{
}

void std::experimental::audio::submix::set_volume(float volume)
{
}

void std::experimental::audio::submix::set_mute(bool mute)
{
}

void std::experimental::audio::submix::set_paused(bool pause)
{
}

void std::experimental::audio::submix::set_playback_position(unsigned long long position, time_unit unit)
{
}

void std::experimental::audio::submix::release()
{
}

int std::experimental::audio::sound::get_frequency() const
{
	return 0;
}

int std::experimental::audio::sound::get_channels() const
{
	return 0;
}

auto std::experimental::audio::sound::get_format() const -> format
{
	return format();
}

auto std::experimental::audio::sound::get_device() const -> std::weak_ptr<device>
{
	return std::weak_ptr<device>();
}
