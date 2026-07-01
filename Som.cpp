#include "Som.h"
#include <windows.h>
#include <mmsystem.h>

void Som::tocarMusica() {
    PlaySound(
        TEXT("audio_SWjogo.wav"),
        NULL,
        SND_FILENAME | SND_ASYNC | SND_LOOP
    );
}

void Som::pararMusica() {
    PlaySound(NULL, NULL, 0);
}