#include <core/audio_system.h>
#include <util/logging_system.h>
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

void AudioSystem::LoadFromFile(const std::string_view& id, const std::string_view& fileName)
{
	// Skip loading the texture if it has already been loaded
	if (this->loadedAudio.find(id.data()) != this->loadedAudio.end())
	{
		LogSystem::GetInstance().OutputLog("Aborted audio loading process, there's already a loaded audio with the ID: " +
			std::string(id), Severity::WARNING);
		return;
	}
	else
	{
		for (const auto& audio : this->loadedAudio)
		{
			if (audio.second.first == fileName)
			{
				LogSystem::GetInstance().OutputLog("Aborted audio loading process, the audio (" + std::string(fileName) +
					") has already been loaded", Severity::WARNING);
				return;
			}
		}
	}

	// Construct the full path to the audio file
	const std::string filePath = std::string("audio/") + fileName.data();

	// Load the audio file
	irrklang::ISoundSource* loadedAudio = this->engine->addSoundSourceFromFile(filePath.c_str(), irrklang::ESM_AUTO_DETECT, true);
	this->loadedAudio[id.data()] = { fileName.data(), std::make_shared<Audio>(this->engine, loadedAudio)};
}

void AudioSystem::Free(const std::string_view& id)
{
	// Search through the unordered map for audio matching the given ID
	auto iterator = this->loadedAudio.find(id.data());
	if (iterator != this->loadedAudio.end())
		this->loadedAudio.erase(iterator);
	else
		LogSystem::GetInstance().OutputLog("No loaded audio has the given ID: " + std::string(id), Severity::WARNING);
}

const AudioPtr AudioSystem::GetAudio(const std::string_view& id) const
{
	// Search through the unordered map for a audio matching the given ID
	auto iterator = this->loadedAudio.find(id.data());
	if (iterator != this->loadedAudio.end())
		return iterator->second.second;

	// No audio has been found a matching ID
	LogSystem::GetInstance().OutputLog("No loaded audio has the given ID: " + std::string(id), Severity::WARNING);
	return nullptr;
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