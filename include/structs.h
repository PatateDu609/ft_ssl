#ifndef STRUCTS_H
#define STRUCTS_H

typedef int (*fn_command)(int, char **);

enum e_command_type
{
	STANDARD,
	DIGEST,
	CIPHER,
};

struct s_command
{
	char *name;
	enum e_command_type type;
	fn_command cmd;
};

#endif
