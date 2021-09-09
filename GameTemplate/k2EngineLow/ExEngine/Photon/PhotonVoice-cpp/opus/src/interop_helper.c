// C# interop helper
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "opus.h"

#ifdef __cplusplus
extern "C" {
#endif
OPUS_EXPORT int opus_encoder_ctl_set(OpusEncoder *st, int request, int value) {
	return opus_encoder_ctl(st, request, value);
}
OPUS_EXPORT int opus_encoder_ctl_get(OpusEncoder *st, int request, int* value) {
	return opus_encoder_ctl(st, request, value);
}
OPUS_EXPORT int opus_decoder_ctl_set(OpusDecoder *st, int request, int value) {
	return opus_decoder_ctl(st, request, value);
}
OPUS_EXPORT int opus_decoder_ctl_get(OpusDecoder *st, int request, int* value) {
	return opus_decoder_ctl(st, request, value);
}

#ifdef __cplusplus
}
#endif
