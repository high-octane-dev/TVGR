// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include "recompiled/ppc/ppc_context.h"
#include "utils/xbox.h"

#define XAUDIO_SAMPLES_HZ 48000
#define XAUDIO_NUM_CHANNELS 6
#define XAUDIO_SAMPLE_BITS 32

// Number of samples in a frame
#define XAUDIO_NUM_SAMPLES 256

void XAudioInitializeSystem();
void XAudioRegisterClient(PPCFunc* callback, uint32_t param);
void XAudioSubmitFrame(void* samples);
void XAudioConfigValueChangedCallback(class IConfigDef* configDef);

uint32_t XAudioRegisterRenderDriverClient(be<uint32_t>* callback, be<uint32_t>* driver);
uint32_t XAudioUnregisterRenderDriverClient(uint32_t driver);
uint32_t XAudioSubmitRenderDriverFrame(uint32_t driver, void* samples);