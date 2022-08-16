LANGEXTENSION			?=	.c

BASENAME				?=	hash/sha256/cmd			\
							hash/sha512/cmd			\
							hash/sha384/cmd			\
							hash/sha512_224/cmd		\
							hash/sha512_256/cmd		\
							hash/md5/init			\
							hash/md5/cmd			\
							hash/md5/update			\
							hash/sha224/cmd			\
							main					\
							utils/padding			\
							utils/ft_get_fd			\
							utils/ft_print_sp		\
							utils/free				\
							utils/stream			\
							utils/input				\
							utils/usage				\
							help/cmd				\
							commands/commands		\
							commands/options		\
							commands/setup_env		\
							throw					\
							generic/sha2/consts		\
							generic/sha2/sha2		\
							generic/sha2/final		\
							generic/sha2/update		\

SRC						:=	$(addprefix $(PATH_SRC)/,\
								$(addsuffix $(LANGEXTENSION), $(BASENAME)))


OBJS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .o, $(BASENAME)))
DEPS					:=	$(addprefix $(PATH_OBJ)/, $(addsuffix .d, $(BASENAME)))

undefine BASENAME
undefine MD5_SRC_BASENAME
