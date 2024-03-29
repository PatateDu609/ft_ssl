#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ft_math.h"

typedef bool (*fn_chk_opt_arg)(char *);

enum e_command_type {
	STANDARD,
	DIGEST,
	CIPHER,
};

enum e_opt_type {
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

enum e_param_type {
	PARAM_TYPE_FILE,
	PARAM_TYPE_COMMAND,
};

enum e_sha2_type {
	SHA2_TYPE_SHA256,
	SHA2_TYPE_SHA512,
	SHA2_TYPE_SHA224,
	SHA2_TYPE_SHA384,
	SHA2_TYPE_SHA512_224,// SHA512/224
	SHA2_TYPE_SHA512_256 // SHA512/256
};

struct s_param {
	char             *name;           // name of the parameter
	char             *def;            // default value
	uint8_t           can_be_multiple;// can be repeated
	uint8_t           is_required;    // is required
	enum e_param_type type;           // type of the parameter
	char             *description;    // description of the parameter
};

struct s_option {
	char           *name;      // name of the option
	enum e_opt_type type;      // type of the option
	uint64_t        flag;      // flag for the option
	char           *desc;      // description of the option

	int             need_value;// whether the option needs a value or not
	char           *value;     // value of the option
	char           *val_desc;  // small description of the value of the option
	fn_chk_opt_arg  chk;       // function to check the value of the option
};

struct s_env;
typedef int (*fn_command)(struct s_env *);
struct s_command {
	char                 *name;      // command name
	char                 *alias;     // Alias to the command
	enum e_command_type   type;      // command type
	struct s_option      *options;   // Array of all the available options for this command
	size_t                opts_count;// Count of options in the corresponding array
	fn_command            cmd;       // command callback
	const struct s_param *params;    // parameters of the command
	size_t                param_cnt; // number of parameters
};

union u_input {
	char  *str;
	char **files;
};

struct s_env {
	int                     ac;
	char                  **av;

	struct s_option        *options;
	size_t                  option_cnt;

	uint64_t                opts;    // active options
	char                  **params;  // parameters for the command
	char                   *out_file;// output file
	char                   *in_file; // input file

	const struct s_command *cmd;     // command executed
};

struct s_blocks {
	size_t   nb;        // number of blocks
	uint8_t *data;      // data of the blocks
	size_t   block_size;// size of each block in bytes
};

struct s_msg {
	uint8_t *data;      // data of the message
	size_t   len;       // length of the message
	int128_t len_128;   // length of the message in 128 bits
	size_t   bits;      // number of bits in the message
	size_t   block_size;// true block size in bytes
};

#endif
