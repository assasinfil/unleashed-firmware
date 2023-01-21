#include "unknown1.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolUnknown1"

static const SubGhzBlockConst subghz_protocol_unknown1_const = {
    .te_short = 470,
    .te_long = 940,
    .te_delta = 200,
    .min_count_bit_for_found = 52,
};

struct SubGhzProtocolDecoderUnknown1 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    uint16_t header_count;
};

struct SubGhzProtocolEncoderUnknown1 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    Unknown1DecoderStepReset = 0,
    Unknown1DecoderStepFoundPreambula,
    Unknown1DecoderStepCheckPreambula,
    Unknown1DecoderStepStartDecode,
    Unknown1DecoderStepDecoderData,
} Unknown1DecoderStep;

const SubGhzProtocolDecoder subghz_protocol_unknown1_decoder = {
    .alloc = subghz_protocol_decoder_unknown1_alloc,
    .free = subghz_protocol_decoder_unknown1_free,

    .feed = subghz_protocol_decoder_unknown1_feed,
    .reset = subghz_protocol_decoder_unknown1_reset,

    .get_hash_data = subghz_protocol_decoder_unknown1_get_hash_data,
    .serialize = subghz_protocol_decoder_unknown1_serialize,
    .deserialize = subghz_protocol_decoder_unknown1_deserialize,
    .get_string = subghz_protocol_decoder_unknown1_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_unknown1_encoder = {
    .alloc = subghz_protocol_encoder_unknown1_alloc,
    .free = subghz_protocol_encoder_unknown1_free,

    .deserialize = subghz_protocol_encoder_unknown1_deserialize,
    .stop = subghz_protocol_encoder_unknown1_stop,
    .yield = subghz_protocol_encoder_unknown1_yield,
};

const SubGhzProtocol subghz_protocol_unknown1 = {
    .name = SUBGHZ_PROTOCOL_UNKNOWN1_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save,

    .decoder = &subghz_protocol_unknown1_decoder,
    .encoder = &subghz_protocol_unknown1_encoder,
};

void* subghz_protocol_encoder_unknown1_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderUnknown1* instance = malloc(sizeof(SubGhzProtocolEncoderUnknown1));

    instance->base.protocol = &subghz_protocol_unknown1;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 52; //max 24bit*2 + 2 (start, stop)
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_unknown1_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderUnknown1* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

bool subghz_protocol_encoder_unknown1_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderUnknown1* instance = context;
    bool res = false;
    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }

        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        // subghz_protocol_encoder_unknown1_get_upload(instance, instance->generic.btn);

        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        uint8_t key_data[sizeof(uint64_t)] = {0};
        for(size_t i = 0; i < sizeof(uint64_t); i++) {
            key_data[sizeof(uint64_t) - i - 1] = (instance->generic.data >> i * 8) & 0xFF;
        }
        if(!flipper_format_update_hex(flipper_format, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Unable to add Key");
            break;
        }

        instance->encoder.is_running = true;

        res = true;
    } while(false);

    return res;
}

void subghz_protocol_encoder_unknown1_stop(void* context) {
    SubGhzProtocolEncoderUnknown1* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_unknown1_yield(void* context) {
    SubGhzProtocolEncoderUnknown1* instance = context;

    if(instance->encoder.repeat == 0 || !instance->encoder.is_running) {
        instance->encoder.is_running = false;
        return level_duration_reset();
    }

    LevelDuration ret = instance->encoder.upload[instance->encoder.front];

    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }

    return ret;
}

void* subghz_protocol_decoder_unknown1_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderUnknown1* instance = malloc(sizeof(SubGhzProtocolDecoderUnknown1));
    instance->base.protocol = &subghz_protocol_unknown1;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_unknown1_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    free(instance);
}

void subghz_protocol_decoder_unknown1_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    instance->decoder.parser_step = Unknown1DecoderStepReset;
    // instance->last_data = 0;
}

void subghz_protocol_decoder_unknown1_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;

    switch(instance->decoder.parser_step) {
    case Unknown1DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_short) <
                       subghz_protocol_unknown1_const.te_delta)) {
            //Found Preambula
            instance->decoder.parser_step = Unknown1DecoderStepFoundPreambula;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
            // instance->te = 0;
            FURI_LOG_D(TAG, "Reset");
        }
        break;
    case Unknown1DecoderStepFoundPreambula:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_short) <
                        subghz_protocol_unknown1_const.te_delta)) {
            instance->decoder.parser_step = Unknown1DecoderStepCheckPreambula;
            FURI_LOG_D(TAG, "Preambula found");
        } else {
            instance->header_count = 0;
            instance->decoder.parser_step = Unknown1DecoderStepReset;
        }
        break;
    case Unknown1DecoderStepCheckPreambula:
        if(level) {
            if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_short) <
                subghz_protocol_unknown1_const.te_delta)) {
                instance->decoder.parser_step = Unknown1DecoderStepFoundPreambula;
                instance->header_count++;
                FURI_LOG_D(TAG, "Preambula check");
            } else if(
                (instance->header_count > 1) &&
                (DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_long) <
                 subghz_protocol_unknown1_const.te_delta)) {
                instance->decoder.parser_step = Unknown1DecoderStepDecoderData;
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->header_count = 0;
            }
        }
        break;
    case Unknown1DecoderStepDecoderData:
        if(level) {
            if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_long) <
                subghz_protocol_unknown1_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.decode_count_bit++;
                FURI_LOG_D(TAG, "Decode 1");
            } else if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_short) <
                       subghz_protocol_unknown1_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.decode_count_bit++;
                FURI_LOG_D(TAG, "Decode 0");
            } else {
                instance->decoder.parser_step = Unknown1DecoderStepReset;
            }
        } else {
            if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_long) <
                subghz_protocol_unknown1_const.te_delta)) {
                // subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                // FURI_LOG_D(TAG, "Decode 1");
            } else if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_short) <
                       subghz_protocol_unknown1_const.te_delta)) {
                // subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                // FURI_LOG_D(TAG, "Decode 0");
            } else if((DURATION_DIFF(duration, subghz_protocol_unknown1_const.te_long * 8) <
                       subghz_protocol_unknown1_const.te_delta * 4)) {
                if(instance->decoder.decode_count_bit ==
                   subghz_protocol_unknown1_const.min_count_bit_for_found) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
            } else {
                instance->decoder.parser_step = Unknown1DecoderStepReset;
            }
        }
        break;
    }
}

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 */
static void subghz_protocol_unknown1_check_remote_controller(SubGhzBlockGeneric* instance) {
    instance->serial = instance->data >> 4;
    instance->btn = instance->data & 0xF;
}

uint8_t subghz_protocol_decoder_unknown1_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

bool subghz_protocol_decoder_unknown1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    bool res = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    // if(res && !flipper_format_write_uint32(flipper_format, "TE", &instance->te, 1)) {
    // FURI_LOG_E(TAG, "Unable to add TE");
    // res = false;
    // }
    return res;
}

bool subghz_protocol_decoder_unknown1_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    bool res = false;
    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(instance->generic.data_count_bit !=
           subghz_protocol_unknown1_const.min_count_bit_for_found) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            break;
        }
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        // if(!flipper_format_read_uint32(flipper_format, "TE", (uint32_t*)&instance->te, 1)) {
        // FURI_LOG_E(TAG, "Missing TE");
        // break;
        // }
        res = true;
    } while(false);

    return res;
}

void subghz_protocol_decoder_unknown1_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderUnknown1* instance = context;
    subghz_protocol_unknown1_check_remote_controller(&instance->generic);
    uint32_t data_rev = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%08lX\r\n"
        "Yek:0x%08lX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (uint32_t)(instance->generic.data & 0xFFFFFF),
        data_rev);
}
