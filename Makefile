# Critical include to remove predefined Makefile variables.
include mkvars/conf.mk

TYPE							=	exec
NAME							=	ft_ssl
LANGUAGE						=	C
COLORS							:=	256
DEBUG							:=	1
RELEASE							:=	0
LDFLAGS							:=	-Llibcrypto42 -Llibft		\
									-lft -lcrypto42 -lm

ifneq ($(shell uname),Darwin)
	LDFLAGS						+=	-lbsd
endif

SUBLIBS							:=	libcrypto42/libcrypto42.a libft/libft.a

PATH_INC						=	include libft/include libcrypto42/include

include mkvars/init.mk
include mkvars/colors.mk
include mkvars/SRC.mk
include mkvars/rules.mk

libcrypto42/libcrypto42.a:
	$(MAKE) -s -C libcrypto42
libft/libft.a:
	$(MAKE) -s -C libft

# File to be created if tests are needed.
# include mkvars/tests.mk
