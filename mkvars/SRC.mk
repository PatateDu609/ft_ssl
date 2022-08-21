LANGEXTENSION			?=	.c

BASENAME				?=	main					\
							throw					\
							hash/sha256				\
							hash/sha512				\
							hash/sha384				\
							hash/sha224				\
							hash/sha512_224			\
							hash/sha512_256			\
							hash/md5				\
							utils/padding			\
							utils/ft_get_fd			\
							utils/ft_print_sp		\
							utils/free				\
							utils/stream			\
							utils/input				\
							utils/usage				\
							utils/process			\
							utils/init_cipher		\
							utils/bufferize_base64	\
							help/cmd				\
							commands/commands		\
							commands/options		\
							commands/setup_env		\
							cipher/base64			\
							cipher/des-ecb			\

SRC						:=	$(addprefix $(PATH_SRC)/,\
								$(addsuffix $(LANGEXTENSION), $(BASENAME)))


OBJS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .o, $(BASENAME)))
DEPS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .d, $(BASENAME)))

undefine BASENAME
undefine MD5_SRC_BASENAME
