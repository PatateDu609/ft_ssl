#include "common.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

struct stream_ctx {
	uint8_t buf[48];
	size_t  pos;
	uint8_t ref;///< May be the line position if the ctx is in encoder mode, or how much there is
			///< to copy in the buffer if the ctx is in decryption mode
};

// Static globals are initialized to 0
static struct stream_ctx encoder;
static struct stream_ctx decoder;

void                     stream_base64_enc(FILE *out, const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        encoder.buf[encoder.pos++] = buf[i];
        if (encoder.pos >= sizeof encoder.buf)// Buffer full -> flush
            stream_base64_enc_flush(out);
    }
}

void stream_base64_enc_flush(FILE *out) {
	if (encoder.pos == 0)
		return;

	char *encoded       = base64_encode(encoder.buf, encoder.pos);
	encoder.pos         = 0;

	size_t len          = strlen(encoded);
	size_t before_break = 64 - encoder.ref;
	encoder.ref += len;

	if (len < before_break)
		fwrite(encoded, 1, len, out);
	else {
		fwrite(encoded, 1, before_break, out);
		fwrite("\n", 1, 1, out);
		fwrite(encoded + before_break, 1, len - before_break, out);
		encoder.ref %= 64;// line length
	}
}

static void stream_base64_dec_update_buf(char *__buf) {
	size_t   decoded_len;
	uint8_t *decoded = base64_decode(__buf, &decoded_len);

	if (decoded_len > sizeof decoder.buf)
		throwe("internal error: got bad length in decoded string", false);

	memcpy(decoder.buf, decoded, decoded_len);
	decoder.ref = decoded_len;
}

size_t stream_base64_dec(FILE *in, uint8_t *buf, size_t len) {
	memset(buf, 0, len);

	if (feof(in) && decoder.pos >= decoder.ref)
		return 0;

	size_t result;

	for (result = 0; result < len; result++) {
		if (!decoder.buf[0] || decoder.pos >= sizeof decoder.buf) {
			decoder.pos = 0;

			char   __buf[65];
			size_t res = fread(__buf, sizeof __buf[0], sizeof __buf, in);

			if (res == sizeof __buf || feof(in)) {
				__buf[res - 1] = 0;
				stream_base64_dec_update_buf(__buf);
			} else// error...
				throwe("couldn't read stream", true);
		}
		buf[result] = decoder.buf[decoder.pos];
		if (decoder.pos >= decoder.ref && feof(in))
			return result;
		decoder.pos++;
	}

	return result;
}
