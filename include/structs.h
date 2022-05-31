#ifndef STRUCTS_H
#define STRUCTS_H

#include <stddef.h>
#include <stdint.h>

typedef int (*fn_chk_opt_arg)(char *);

enum e_command_type
{
	STANDARD,
	DIGEST,
	CIPHER,
};

enum e_opt_type
{
	OPT_TYPE_GENERAL,
	OPT_TYPE_SPEC,
	OPT_TYPE_OUT,
	OPT_TYPE_IN,
	OPT_TYPE_ENC,
	OPT_TYPE_RAND,
	OPT_TYPE_PROVIDER,
	OPT_TYPE_PVK,
	OPT_TYPE_SIGNING,
};

enum e_param_type
{
	PARAM_TYPE_FILE,
	PARAM_TYPE_COMMAND,
};

struct s_param
{
	char *name;				 // name of the parameter
	uint8_t can_be_multiple; // can be repeated
	enum e_param_type type;	 // type of the parameter
	char *description;		 // description of the parameter
};

struct s_option
{
	char *name;			  // name of the option
	enum e_opt_type type; // type of the option
	uint64_t flag;		  // flag for the option
	char *desc;			  // description of the option

	int need_value;		// whether the option needs a value or not
	char *value;		// value of the option
	char *val_desc;		// small description of the value of the option
	fn_chk_opt_arg chk; // function to check the value of the option
};

struct s_env;
typedef int (*fn_command)(struct s_env *);
struct s_command
{
	char *name;				   // command name
	enum e_command_type type;  // command type
	struct s_option **options; // options of the command
	fn_command cmd;			   // command callback
	struct s_param *params;	   // parameters of the command
	size_t param_cnt;		   // number of parameters
};

struct s_env
{
	int ac;
	char **av;

	uint64_t opts;	// active options
	char **params;	// parameters for the command
	char *out_file; // output file
	char *in_file;	// input file

	struct s_command *cmd; // command executed
};

#endif
