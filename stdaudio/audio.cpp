#include "audio.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

std::experimental::audio::device::device()
{
	FMOD_RESULT result = FMOD::System_Create(&m_system);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));

	result = m_system->init(128, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));
}

std::experimental::audio::device::~device()
{
	m_system->release();
}

int std::experimental::audio::device::num_drivers() const
{
	int num_drivers = 0;
	m_system->getNumDrivers(&num_drivers);
	return num_drivers;
}

auto std::experimental::audio::device::get_driver(int index) const -> driver_info
{
	FMOD_GUID fmod_guid;
	char name[512];
	FMOD_RESULT result = m_system->getDriverInfo(index, name, 512, &fmod_guid, nullptr, nullptr, nullptr);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));

	name[511] = '\0';

	driver_info info;
	info.name.assign(name);
	info.guid = *reinterpret_cast<guid*>(&fmod_guid);

	return info;
}

void std::experimental::audio::device::set_driver(int index)
{
	FMOD_RESULT result = m_system->setDriver(index);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));
}

auto std::experimental::audio::device::load_sound(const std::experimental::filesystem::path& filepath) -> std::shared_ptr<sound>
{
	FMOD::Sound* fmod_sound = nullptr;
	FMOD_RESULT result = m_system->createSound(filepath.generic_u8string().c_str(), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_CREATESAMPLE, nullptr, &fmod_sound);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));

	fmod_sound->setLoopCount(0);

	return make_shared<sound>(this, fmod_sound, sound::constructor_tag{});
}

auto std::experimental::audio::device::play_sound(const std::shared_ptr<sound>& sound, bool paused) -> std::unique_ptr<voice>
{
	FMOD::Channel* fmod_channel = nullptr;
	FMOD_RESULT result = m_system->playSound(sound->m_sound, nullptr, paused, &fmod_channel);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));

	return make_unique<voice>(this, sound, fmod_channel, voice::constructor_tag{});
}

auto std::experimental::audio::voice::get_sound() const -> std::shared_ptr<sound>
{
	return m_sound.lock();
}

auto std::experimental::audio::voice::get_device() const -> device*
{
	return nullptr;
}

float std::experimental::audio::voice::get_audibility() const
{
	return 0.0f;
}

int std::experimental::audio::voice::get_frequency() const
{
	return 0;
}

unsigned long long std::experimental::audio::voice::get_playback_position(time_unit unit) const
{
	return 0;
}

int std::experimental::audio::voice::get_loop_count() const
{
	return 0;
}

float std::experimental::audio::voice::get_pitch() const
{
	return 0.0f;
}

float std::experimental::audio::voice::get_volume() const
{
	return 0.0f;
}

float std::experimental::audio::voice::get_pan() const
{
	return 0.0f;
}

bool std::experimental::audio::voice::is_muted() const
{
	return false;
}

bool std::experimental::audio::voice::is_paused() const
{
	return false;
}

bool std::experimental::audio::voice::is_playing() const
{
	bool playing = false;
	m_channel->isPlaying(&playing);
	return playing;
}

void std::experimental::audio::voice::set_loop_count(int count)
{
}

void std::experimental::audio::voice::set_pitch(float pitch)
{
}

void std::experimental::audio::voice::set_volume(float volume)
{
}

void std::experimental::audio::voice::set_pan(float pan)
{
}

void std::experimental::audio::voice::set_mute(bool mute)
{
}

void std::experimental::audio::voice::set_paused(bool pause)
{
}

void std::experimental::audio::voice::set_playback_position(unsigned long long position, time_unit unit)
{
}

void std::experimental::audio::voice::stop()
{
}

std::experimental::audio::voice::voice(device * device, const std::shared_ptr<sound>& sound, FMOD::Channel * channel, constructor_tag) :
	m_device(device),
	m_sound(sound),
	m_channel(channel)
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

auto std::experimental::audio::sound::get_device() const -> device*
{
	return m_device;
}

std::experimental::audio::sound::sound(device* device, FMOD::Sound* sound, constructor_tag) :
	m_device(device),
	m_sound(sound)
{
}
