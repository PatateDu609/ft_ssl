#include "common.h"
#include "defines.h"
#include "error.h"
#include "ft_ssl.h"
#include "usage.h"

#include <stdio.h>

static int ft_base64_encode(struct s_env *e) {
	FILE *in = e->in_file ? fopen(e->in_file, "rb") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "wb") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	uint8_t buf[1024];
	size_t  len = sizeof buf / sizeof *buf;
	while (!feof(in)) {
		size_t ret = fread(buf, sizeof *buf, len, in);

		if (ret != len && ferror(in))
			throwe("couldn't read from input file", true);

		stream_base64_enc(out, buf, ret);
	}

	stream_base64_enc_flush(out);

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);
	return (0);
}

static int ft_base64_decode(struct s_env *e) {
	FILE *in = e->in_file ? fopen(e->in_file, "rb") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "wb") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	uint8_t buf[1024];
	size_t  len = sizeof buf / sizeof *buf;
	size_t  ret;
	do {
		ret = stream_base64_dec(in, buf, len);
		fwrite(buf, sizeof *buf, ret, out);
	} while (!feof(in));

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);

	return 0;
}

int ft_base64(struct s_env *e) {
	if (e->opts & FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	stream_base64_reset_all();

	if (e->opts & BASE64_FLAG_d)
		return (ft_base64_decode(e));
	else
		return (ft_base64_encode(e));
}
