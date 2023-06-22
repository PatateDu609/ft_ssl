#include "common.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

struct stream_ctx
{
	uint8_t buf[48];
	size_t pos;
	uint8_t line_pos;
};

// Static globals are initialized to 0
static struct stream_ctx encoder;
static struct stream_ctx decoder;

void stream_base64_enc(FILE *out, uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		encoder.buf[encoder.pos++] = buf[i];
		if (encoder.pos >= sizeof encoder.buf) // Buffer full -> flush
			stream_base64_enc_flush(out);
	}
}

void stream_base64_enc_flush(FILE *out)
{
	if (encoder.pos == 0)
		return;

	char *encoded = base64_encode(encoder.buf, encoder.pos);
	encoder.pos = 0;

	size_t len = strlen(encoded);
	size_t before_break = 64 - encoder.line_pos;
	encoder.line_pos += len;

	if (len < before_break)
		fwrite(encoded, 1, len, out);
	else
	{
		fwrite(encoded, 1, before_break, out);
		fwrite("\n", 1, 1, out);
		fwrite(encoded + before_break, 1, len - before_break, out);
		encoder.line_pos %= 64; // line length
	}
}

void stream_base64_dec(FILE *in, uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (!decoder.buf[0] && decoder.pos >= sizeof decoder.buf) {
			decoder.pos = 0;

			char __buf[65];
			size_t res = fread(__buf, sizeof __buf[0], sizeof __buf, in);
			if (res == 0) {
				//...
			}
			else if (res == sizeof __buf) {
				char *new_decoded;
				base64_decode(__buf, sizeof __buf);
			}
		}
		buf[i] = decoder.buf[decoder.pos++];
	}
}
