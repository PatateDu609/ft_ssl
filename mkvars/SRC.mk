LANGEXTENSION			?=	.c

BASENAME				?=	main							\
							throw							\
							help/cmd						\
							\
							hash/sha256						\
							hash/sha512						\
							hash/sha384						\
							hash/sha224						\
							hash/sha512_224					\
							hash/sha512_256					\
							hash/md5						\
							\
							utils/padding					\
							utils/cipher					\
							utils/ft_get_fd					\
							utils/ft_print_sp				\
							utils/free						\
							utils/stream					\
							utils/input						\
							utils/usage						\
							utils/process					\
							utils/init_cipher				\
							utils/helpers					\
							\
							commands/commands				\
							commands/exec					\
							commands/sanitize				\
							commands/setup_env				\
							commands/utils					\
							cipher/base64					\
							\
							cipher/des/ecb					\
							cipher/des/cbc					\
							cipher/des/cfb					\
							cipher/des/ofb					\
							\
							cipher/aes/ecb					\
							cipher/aes/cbc					\
							cipher/aes/cfb					\
							cipher/aes/ofb					\
							cipher/aes/ctr					\
							\
							cipher/3des_ede3/cbc			\
							cipher/3des_ede3/cfb			\
							cipher/3des_ede3/ecb			\
							cipher/3des_ede3/ofb			\
							\
							cipher/3des_ede2/cbc			\
							cipher/3des_ede2/cfb			\
							cipher/3des_ede2/ecb			\
							cipher/3des_ede2/ofb			\

SRC						:=	$(addprefix $(PATH_SRC)/,\
								$(addsuffix $(LANGEXTENSION), $(BASENAME)))


OBJS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .o, $(BASENAME)))
DEPS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .d, $(BASENAME)))

undefine BASENAME
undefine MD5_SRC_BASENAME
