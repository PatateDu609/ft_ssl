#include "crypto.h"
#include "ft_ssl.h"
#include "defines.h"
#include "usage.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void ft_process_hash(struct s_env *e, char *alg_pretty_name,
	char *(*hash_str)(const char *),
	char *(hash_filename)(const char *),
	char *(hash_descriptor)(int))
{
	union u_input *input = ft_get_input(e, true);
	char *hash = NULL;

	if (e->opts & DIGEST_FLAG_s)
		hash = hash_str(input->str);
	else if (input->files[0])
	{
		for (int i = 0; input->files[i]; i++)
		{
			hash = hash_filename(input->files[i]);
			if (!hash)
			{
				fprintf(stderr, "ft_ssl: %s: %s\n", input->files[i], strerror(errno));
				continue;
			}
			if (e->opts & DIGEST_FLAG_q)
				printf("%s\n", hash);
			else if (e->opts & DIGEST_FLAG_r)
				printf("%s *%s\n", hash, input->files[i]);
			else
				printf("%s(%s)= %s\n", alg_pretty_name, input->files[i], hash);
			free(hash);
			hash = NULL;
		}
		goto end;
	}
	else
		hash = hash_descriptor(STDIN_FILENO);

	if (!hash)
	{
		fprintf(stderr, "ft_ssl: %s\n", strerror(errno));
		goto end;
	}
	if (e->opts & DIGEST_FLAG_q)
		printf("%s\n", hash);
	else if (e->opts & DIGEST_FLAG_r && !(e->opts & DIGEST_FLAG_s))
		printf("%s *%s\n", hash, "stdin");
	else if (e->opts & DIGEST_FLAG_p && e->opts & DIGEST_FLAG_s )
		printf("%s(%s)= %s\n", alg_pretty_name, input->str, hash);
	else
		printf("%s(%s)= %s\n", alg_pretty_name, "stdin", hash);

end:
	free(hash);
	free(input);
}
