#include <irrKlang/irrKlang.h>

#include "SoundManager.h"
#include "Define.h"
#include "Logger.h"


SoundManager::SoundManager()
{
	m_EngineMusic = irrklang::createIrrKlangDevice();

	// if the user doesn t have any audio outputs
	if (m_EngineMusic == nullptr)
	{
		m_audioDevices = false;
		Logger::Warn("Failed during the soundManager creation. Please check your audio output devices");
	}
}

SoundManager::~SoundManager()
{
	// If no audio output devices, the drop() crashes
	if (!m_audioDevices)
		return;

	m_EngineMusic->drop(); // delete m_EngineMusicSound
}

void SoundManager::PlayMusic(std::string music, float volume)
{
	if (!m_audioDevices || music == "" || music == " ")
		return;

	std::string newmusic = ASSETS + music;

	m_Music = m_EngineMusic->play2D(newmusic.c_str(), true, false, true);

	Logger::Info("Load Music : " + music);

	m_Music->setVolume(volume);
}

void SoundManager::StopMusic()
{
	if (m_EngineMusic == nullptr || m_Music == nullptr)
		return;
	m_Music->stop();
}

irrklang::ISound* SoundManager::PlaySound(std::string sound, float volume)
{
	if (!m_audioDevices || sound == "" || sound == " ")
		return nullptr;

	std::string newsound = ASSETS + sound;

	irrklang::ISound* isound = m_EngineMusic->play2D(newsound.c_str(), false, true);
	isound->setVolume(volume);
	isound->setIsPaused(false);

	return isound;
}