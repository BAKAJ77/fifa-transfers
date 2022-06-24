#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <irrKlang.h>
#include <string_view>
#include <memory>

class Audio
{
private:
	irrklang::ISoundEngine* engine;
	irrklang::ISoundSource* audio;
	irrklang::ISound* channel;
public:
	Audio(irrklang::ISoundEngine* engine, irrklang::ISoundSource* audio);
	~Audio();

	// Sets the current play position of the audio in milliseconds.
	void SetPlayPosition(uint32_t milliseconds);

	// Sets the volume of the audio when played.
	void SetVolume(float volume);

	// Plays the audio.
	void Play(bool loop = false);

	// Stops the audio.
	void Stop();

	// Resumes the audio.
	void Resume();

	// Pauses the audio.
	void Pause();

	// Returns the current play position of the audio in milliseconds.
	uint32_t GetPlayPosition() const;

	// Returns the volume of the audio.
	float GetVolume() const;
	
	// Returns TRUE if the audio is paused, else FALSE is returned.
	bool isPaused() const;

	// Returns TRUE if the audio has finished playing, else FALSE is returned.
	bool isFinished() const;
};

using AudioPtr = std::shared_ptr<Audio>;

class AudioSystem
{
private:
	irrklang::ISoundEngine* engine;
private:
	AudioSystem();
public:
	AudioSystem(const AudioSystem& other) = delete;
	AudioSystem(AudioSystem&& temp) noexcept = delete;
	~AudioSystem();

	AudioSystem& operator=(const AudioSystem& other) = delete;
	AudioSystem& operator=(AudioSystem&& temp) noexcept = delete;

	// Sets the master volume for all audios being played.
	void SetMasterVolume(float volume);

	// Returns a pointer to the audio that was loaded from file.
	AudioPtr LoadAudioFromFile(const std::string_view& fileName);

	// Returns singleton instance object of this class.
	static AudioSystem& GetInstance();
};

#endif
