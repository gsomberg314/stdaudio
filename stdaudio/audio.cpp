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

static FMOD_SOUND_FORMAT ConvertSoundFormat(std::experimental::audio::memory_buffer_format format)
{
	switch (format)
	{
	case std::experimental::audio::memory_buffer_format::pcm8:
		return FMOD_SOUND_FORMAT_PCM8;
	case std::experimental::audio::memory_buffer_format::pcm16:
		return FMOD_SOUND_FORMAT_PCM16;
	case std::experimental::audio::memory_buffer_format::pcm24:
		return FMOD_SOUND_FORMAT_PCM24;
	case std::experimental::audio::memory_buffer_format::pcm32:
		return FMOD_SOUND_FORMAT_PCM32;
	case std::experimental::audio::memory_buffer_format::pcmfloat:
		return FMOD_SOUND_FORMAT_PCMFLOAT;
	default:
		break;
	}
	return FMOD_SOUND_FORMAT_NONE;
}

auto std::experimental::audio::device::play_sound(const std::shared_ptr<source>& sound, bool paused) -> std::unique_ptr<voice>
{
	auto audio_data = sound->get_audio_data();
	if (audio_data.data.data == nullptr)
		return nullptr;

	FMOD_RESULT result;
	FMOD::Sound* fmod_sound = nullptr;
	FMOD_CREATESOUNDEXINFO ex_info = { 0 };
	ex_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	ex_info.format = ConvertSoundFormat(audio_data.description.format);
	ex_info.defaultfrequency = audio_data.description.frequency;
	ex_info.numchannels = audio_data.description.num_channels;
	ex_info.length = audio_data.data.size;
	result = m_system->createSound(
		reinterpret_cast<const char*>(audio_data.data.data),
		FMOD_OPENMEMORY_POINT | FMOD_LOOP_NORMAL | FMOD_2D | FMOD_OPENRAW,
		&ex_info,
		&fmod_sound);

	fmod_sound->setLoopCount(0);

	FMOD::Channel* fmod_channel = nullptr;
	result = m_system->playSound(fmod_sound, nullptr, paused, &fmod_channel);
	if (result != FMOD_OK)
		throw std::exception(FMOD_ErrorString(result));

	return make_unique<voice>(fmod_channel, fmod_sound, sound, voice::constructor_tag{});
}

auto std::experimental::audio::device::create_submix() -> std::unique_ptr<submix>
{
	FMOD::ChannelGroup* fmod_channelgroup = nullptr;
	m_system->createChannelGroup(nullptr, &fmod_channelgroup);
	return std::make_unique<submix>(fmod_channelgroup, submix::constructor_tag{});
}

std::experimental::audio::voice::voice(
	FMOD::Channel* channel,
	FMOD::Sound* fmod_sound,
	const std::shared_ptr<source>& sound,
	constructor_tag) :
	m_channel(channel),
	m_sound(fmod_sound),
	m_source(sound)
{
}

std::experimental::audio::voice::~voice()
{
	m_channel->stop();
	m_sound->release();
}

void std::experimental::audio::voice::stop()
{
	m_channel->stop();
}

void std::experimental::audio::voice::pause()
{
	m_channel->setPaused(true);
}

void std::experimental::audio::voice::resume()
{
	m_channel->setPaused(false);
}

void std::experimental::audio::voice::set_volume(float volume)
{
	m_channel->setVolume(volume);
}

void std::experimental::audio::voice::set_pitch(float pitch)
{
	m_channel->setPitch(pitch);
}

void std::experimental::audio::voice::set_pan(float pan_value)
{
	pan = pan_value;
	m_channel->setPan(pan);
}

float std::experimental::audio::voice::get_volume() const
{
	float volume = 0.0f;
	m_channel->getVolume(&volume);
	return volume;
}

float std::experimental::audio::voice::get_pitch() const
{
	float pitch = 0.0f;
	m_channel->getPitch(&pitch);
	return pitch;
}

float std::experimental::audio::voice::get_pan() const
{
	return pan;
}

bool std::experimental::audio::voice::is_playing() const
{
	bool playing = false;
	m_channel->isPlaying(&playing);
	return playing;
}

void std::experimental::audio::voice::assign_to_submix(submix& parent)
{
	m_channel->setChannelGroup(parent.m_channelgroup);
}

auto std::experimental::audio::buffer::get_audio_data() const -> memory_buffer_data
{
	memory_buffer_data return_value;
	return_value.description = m_description;
	if (auto v = std::get_if<std::vector<std::byte>>(&m_data))
	{
		return_value.data = memory_buffer(v->data(), v->size());
	}
	if (auto v = std::get_if<memory_buffer>(&m_data))
	{
		return_value.data = *v;
	}
	return return_value;
}

static std::experimental::audio::memory_buffer_format ConvertSoundFormat(FMOD_SOUND_FORMAT format)
{
	switch (format)
	{
	case FMOD_SOUND_FORMAT_PCM8:
		return std::experimental::audio::memory_buffer_format::pcm8;
	case FMOD_SOUND_FORMAT_PCM16:
		return std::experimental::audio::memory_buffer_format::pcm16;
	case FMOD_SOUND_FORMAT_PCM24:
		return std::experimental::audio::memory_buffer_format::pcm24;
	case FMOD_SOUND_FORMAT_PCM32:
		return std::experimental::audio::memory_buffer_format::pcm32;
	case FMOD_SOUND_FORMAT_PCMFLOAT:
		return std::experimental::audio::memory_buffer_format::pcmfloat;
	default:
		break;
	}
	throw std::exception("Unkown format");
}

std::shared_ptr<std::experimental::audio::buffer> std::experimental::audio::load_from_disk(const std::experimental::filesystem::path& filepath)
{
	// TODO: This is pretty horrible.  We create an entire FMOD System object just to decompress a single sound.
	// This would be okay for a sound or two, but at scale it will explode horribly.
	FMOD::System* pLoadSystem = nullptr;
	FMOD::System_Create(&pLoadSystem);
	pLoadSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND_NRT);
	pLoadSystem->init(1, FMOD_INIT_STREAM_FROM_UPDATE | FMOD_INIT_MIX_FROM_UPDATE | FMOD_INIT_THREAD_UNSAFE, nullptr);

	FMOD::Sound* pSound = nullptr;
	pLoadSystem->createSound(filepath.generic_u8string().c_str(), FMOD_OPENONLY, nullptr, &pSound);

	FMOD_SOUND_FORMAT fmod_format = FMOD_SOUND_FORMAT_NONE;
	int num_channels = 0;
	pSound->getFormat(nullptr, &fmod_format, &num_channels, nullptr);

	float frequency;
	pSound->getDefaults(&frequency, nullptr);

	unsigned int lengthbytes = 0;
	pSound->getLength(&lengthbytes, FMOD_TIMEUNIT_PCMBYTES);

	auto return_value = std::make_shared<std::experimental::audio::buffer>();
	return_value->m_description.format = ConvertSoundFormat(fmod_format);
	return_value->m_description.frequency = static_cast<unsigned int>(frequency);
	return_value->m_description.num_channels = static_cast<unsigned int>(num_channels);

	std::vector<std::byte> data(lengthbytes);
	unsigned int bytes_read = 0;
	pSound->readData(data.data(), lengthbytes, &bytes_read);
	data.resize(bytes_read);
	return_value->m_data = std::move(data);

	pSound->release();
	pLoadSystem->release();

	return return_value;
}

std::shared_ptr<std::experimental::audio::buffer> std::experimental::audio::load_from_memory(const memory_buffer& buffer, const memory_buffer_description& description, bool copy)
{
	auto return_value = std::make_shared<std::experimental::audio::buffer>();
	return_value->m_description = description;
	if (copy)
	{
		std::vector<std::byte> copied_buffer(buffer.size);
		std::copy(buffer.data, buffer.data + buffer.size, copied_buffer.begin());
	}
	else
	{
		return_value->m_data = buffer;
	}
	return return_value;
}

std::experimental::audio::submix::submix(FMOD::ChannelGroup* channelgroup, constructor_tag) :
	m_channelgroup(channelgroup)
{
}

float std::experimental::audio::submix::get_volume() const
{
	float volume = 0.0f;
	m_channelgroup->getVolume(&volume);
	return volume;
}

void std::experimental::audio::submix::set_volume(float volume)
{
	m_channelgroup->setVolume(volume);
}

void std::experimental::audio::submix::assign_to_submix(submix& parent)
{
	parent.m_channelgroup->addGroup(m_channelgroup);
}
