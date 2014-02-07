#include <iostream>

#include "servercontrol.h"

#ifdef _WIN32
    #include <windows.h>
    #include <mmdeviceapi.h>
    #include <Tchar.h>
    #include <PowrProf.h>
#else

#endif

ServerControl::ServerControl()
    : volumeController(NULL)
{
}

float ServerControl::getVolumeLevel()
{
    VolumeController* endpoint = getVolumeController();

#ifdef _WIN32
    float volumeLevel = 0.f;
    endpoint->GetMasterVolumeLevelScalar(&volumeLevel);
    return volumeLevel;
#else
    long levelMin, levelMax, levelCur;
    snd_mixer_selem_get_playback_volume_range(endpoint, &levelMin, &levelMax);
    snd_mixer_selem_get_playback_volume(endpoint, SND_MIXER_SCHN_FRONT_LEFT, &levelCur);
    return float(levelCur - levelMin) / (levelMax - levelMin);

#endif
}


void ServerControl::setVolumeLevel(float volumeLevel)
{
    VolumeController* endpoint = getVolumeController();

#ifdef _WIN32
    GUID guid = GUID_NULL;
    CoCreateGuid(&guid);

    endpoint->SetMasterVolumeLevelScalar(volumeLevel, &guid);
#else
    long levelMin, levelMax;
    snd_mixer_selem_get_playback_volume_range(endpoint, &levelMin, &levelMax);
    long val = ((levelMax - levelMin) * volumeLevel) + levelMin;
    snd_mixer_selem_set_playback_volume_all(endpoint, val);
#endif
}

VolumeController* ServerControl::getVolumeController()
{
    if (!volumeController)
    {
    #ifdef _WIN32
        HRESULT hr = S_OK;
        IMMDeviceEnumerator *pEnumerator = NULL;
        IMMDevice *pDevice = NULL;

        CoInitialize(NULL);

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),NULL, CLSCTX_INPROC_SERVER,
                              __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);


        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&volumeController);
    #else
        snd_mixer_t *handle;
        snd_mixer_selem_id_t *sid;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        volumeController = snd_mixer_find_selem(handle, sid);
    #endif
    }

    return volumeController;
}
