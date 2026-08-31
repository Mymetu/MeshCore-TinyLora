#pragma once

#define RADIOLIB_STATIC_ONLY 1
#include <RadioLib.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include "TinyLoraC3V222SBoard.h"
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include <helpers/radiolib/CustomSX1268Wrapper.h>
#include <helpers/radiolib/CustomLLCC68Wrapper.h>
#include <helpers/radiolib/CustomSX126xAutoWrapper.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/SensorManager.h>
#include <variant.h>

extern TinyLora_C3_V2_22S_Board board;
extern CustomSX126xAutoWrapper radio_driver;
extern const char* g_radio_chip_name;
extern AutoDiscoverRTCClock rtc_clock;
extern SensorManager sensors;

bool radio_init();
uint32_t radio_get_rng_seed();
void radio_set_params(float freq, float bw, uint8_t sf, uint8_t cr);
void radio_set_tx_power(int8_t dbm);
mesh::LocalIdentity radio_new_identity();
