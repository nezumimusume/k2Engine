#pragma once

#include "PhotonVoice-cpp/inc/VoiceAudio.h"

void initAudoIO(void);
void finitAudoIO(void);
ExitGames::Voice::IAudioPusher<float>* audioIOCreateMic(void);
ExitGames::Voice::ISyncAudioOut<float>* audioIOCreateOut(void);