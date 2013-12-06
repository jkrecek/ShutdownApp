#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#ifdef __MINGW32__
  typedef void * KSJACK_DESCRIPTION;
  typedef void * KSJACK_DESCRIPTION2;
  typedef void * KSJACK_SINK_INFORMATION;
#endif

#include <string>
#include <endpointvolume.h>

enum Command {
    NONE,
    POWER_OFF,
    RESTART,
    LOCK,
    SLEEP
};

class ServerControl
{
public:
    ServerControl();

    const char* execute(Command command);
    Command getCommand(std::string string);

    float getVolumeLevel();
    void setVolumeLevel(float volumeLevel);

private:
    IAudioEndpointVolume* getAudioEndpoint();

    IAudioEndpointVolume* audioEndpoint;

};

#endif // SERVERCONTROL_H
