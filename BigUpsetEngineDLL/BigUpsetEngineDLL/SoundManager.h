#pragma once

#include <string>

namespace irrklang
{
	class ISoundEngine;
	class ISound;
}

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	// Play Music (only mp3 file)
	void PlayMusic(std::string music, float volume);
	// Stop Music
	void StopMusic();
	// Play Sound (only mp3 file)
	irrklang::ISound* PlaySound(std::string sound, float volume);

	bool m_audioDevices = true;

private:
	irrklang::ISoundEngine* m_EngineMusic;
	irrklang::ISound* m_Music;
};