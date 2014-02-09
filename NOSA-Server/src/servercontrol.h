#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#if defined(__MINGW32__) && defined(_WIN32)
  typedef void * KSJACK_DESCRIPTION;
  typedef void * KSJACK_DESCRIPTION2;
  typedef void * KSJACK_SINK_INFORMATION;
#endif

#include <string>
#include "singleton.h"

#ifdef _WIN32
    #include <endpointvolume.h>
    typedef IAudioEndpointVolume VolumeController;
#else
    #include <alsa/asoundlib.h>
    #include <alsa/mixer.h>
    typedef snd_mixer_elem_t VolumeController;
#endif


class ServerControl
{
public:
    ServerControl();

    float getVolumeLevel();
    void setVolumeLevel(float volumeLevel);

private:
    VolumeController* getVolumeController();

    VolumeController* volumeController;

};

#define sPCControl Singleton<ServerControl>::getInstance()

#endif // SERVERCONTROL_H
