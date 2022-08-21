#include "utils.h"
#include "commands.h"
#include "error.h"
#include <stdlib.h>

union u_input *ft_get_input(struct s_env *env, uint8_t use_str)
{
	union u_input *input = malloc(sizeof(union u_input));
	if (!input)
		throwe("Allocation failed", true);

	if (use_str && env->opts & MD5_FLAG_s)
		input->str = get_value(env, MD5_FLAG_s);
	else // if no string is given or its use is disabled duplicate params
		input->files = env->params;

	return input;
}
