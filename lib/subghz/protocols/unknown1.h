#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_UNKNOWN1_NAME "Unknown1"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SubGhzProtocolDecoderUnknown1 SubGhzProtocolDecoderUnknown1;
typedef struct SubGhzProtocolEncoderUnknown1 SubGhzProtocolEncoderUnknown1;

extern const SubGhzProtocolDecoder subghz_protocol_unknown1_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_unknown1_encoder;
extern const SubGhzProtocol subghz_protocol_unknown1;

/**
 * Allocate SubGhzProtocolEncoderUnknown1.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderUnknown1* pointer to a SubGhzProtocolEncoderUnknown1 instance
 */
void* subghz_protocol_encoder_unknown1_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderUnknown1.
 * @param context Pointer to a SubGhzProtocolEncoderUnknown1 instance
 */
void subghz_protocol_encoder_unknown1_free(void* context);

/**
 * Deserialize and generating an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderUnknown1 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_encoder_unknown1_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderUnknown1 instance
 */
void subghz_protocol_encoder_unknown1_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderUnknown1 instance
 * @return LevelDuration 
 */
LevelDuration subghz_protocol_encoder_unknown1_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderUnknown1.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderUnknown1* pointer to a SubGhzProtocolDecoderUnknown1 instance
 */
void* subghz_protocol_decoder_unknown1_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderUnknown1.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 */
void subghz_protocol_decoder_unknown1_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderUnknown1.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 */
void subghz_protocol_decoder_unknown1_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_unknown1_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_unknown1_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderUnknown1.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return true On success
 */
bool subghz_protocol_decoder_unknown1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderUnknown1.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_unknown1_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderUnknown1 instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_unknown1_get_string(void* context, FuriString* output);

#ifdef __cplusplus
}
#endif
