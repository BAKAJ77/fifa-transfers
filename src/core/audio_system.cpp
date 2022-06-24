#include <core/audio_system.h>
#include <util/logging_system.h>
#include <util/directory_system.h>

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioSystem::AudioSystem()
{
	this->engine = irrklang::createIrrKlangDevice();
	if (!this->engine)
		LogSystem::GetInstance().OutputLog("Failed to create IrrKlang device engine", Severity::FATAL);
}

AudioSystem::~AudioSystem()
{
	this->engine->drop();
}

void AudioSystem::SetMasterVolume(float volume)
{
	this->engine->setSoundVolume(volume);
}

AudioPtr AudioSystem::LoadAudioFromFile(const std::string_view& fileName)
{
	// Construct the full path to the audio file
	const std::string filePath = Util::GetAppDataDirectory() + "audio/" + fileName.data();

	// Load the audio file
	irrklang::ISoundSource* loadedAudio = this->engine->addSoundSourceFromFile(filePath.c_str(), irrklang::ESM_AUTO_DETECT, true);
	return std::make_shared<Audio>(this->engine, loadedAudio);
}

AudioSystem& AudioSystem::GetInstance()
{
	static AudioSystem instance;
	return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Audio::Audio(irrklang::ISoundEngine* engine, irrklang::ISoundSource* audio) :
	engine(engine), audio(audio), channel(nullptr)
{
	this->engine->grab(); // We get a pointer to the engine, so increase the ref count to the engine
}

Audio::~Audio()
{
	this->Stop();

	this->engine->removeSoundSource(this->audio);
	this->engine->drop();
}

void Audio::SetPlayPosition(uint32_t milliseconds)
{
	if (this->channel)
		this->channel->setPlayPosition(milliseconds);
}

void Audio::SetVolume(float volume)
{
	if (this->channel)
		this->channel->setVolume(std::clamp(volume, 0.0f, 1.0f));
}

void Audio::Play(bool loop)
{
	// Drop previously used sound channel
	if (this->channel)
	{
		this->channel->stop();
		this->channel->drop();
	}

	// Play the audio
	this->channel = this->engine->play2D(this->audio, loop, false, true, false);
}

void Audio::Stop()
{
	if (this->channel)
	{
		this->channel->stop();
		this->channel->drop();
	}
}

void Audio::Resume()
{
	if (this->channel)
		this->channel->setIsPaused(false);
}

void Audio::Pause()
{
	if (this->channel)
		this->channel->setIsPaused(true);
}

uint32_t Audio::GetPlayPosition() const
{
	if (this->channel)
		return this->channel->getPlayPosition();

	return 0;
}

float Audio::GetVolume() const
{
	if (this->channel)
		return this->channel->getVolume();

	return 1.0f;
}

bool Audio::isPaused() const
{
	if (this->channel)
		return this->channel->getIsPaused();

	return false;
}

bool Audio::isFinished() const
{
	if (this->channel)
		return this->channel->isFinished();

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////