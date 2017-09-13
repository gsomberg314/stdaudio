#pragma once


class device;
class voice;
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

class device
{
public:
	device();
	~device();

	int num_drivers() const;
	driver_info get_driver(int index) const;
	void set_driver(int index);

	std::shared_ptr<sound> load_sound(const std::experimental::filesystem::path& filepath);
	std::unique_ptr<voice> play_sound(const std::shared_ptr<sound>& sound, bool paused = false);

private:
	FMOD::System* m_system;
};

enum class time_unit
{
	milliseconds,
	pcm_samples,
};

class voice
{
private:
	struct constructor_tag { explicit constructor_tag() = default; };

public:
	voice(device* device, const std::shared_ptr<sound>& sound, FMOD::Channel* channel, constructor_tag);

	std::shared_ptr<sound> get_sound() const;
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
	std::weak_ptr<sound> m_sound;
	FMOD::Channel* m_channel;
};

class sound
{
private:
	struct constructor_tag { explicit constructor_tag() = default; };

public:
	sound(device* device, FMOD::Sound* sound, constructor_tag);

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
};
