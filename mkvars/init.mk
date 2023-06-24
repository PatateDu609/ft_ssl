ifndef NAME
$(error "NAME is not defined")
endif

ifndef TYPE
$(error "TYPE is not defined")
endif

ifndef LANGUAGE
$(error "LANGUAGE is not defined")
endif

VERBOSE				?=	0

DEBUG				?=	0
RELEASE				?=	1

LANGEXTENSION		=

ifeq ($(TYPE),static)
	LIBEXTENTION	=	.a
else ifeq ($(TYPE),shared)
	LIBEXTENTION	=	.so
endif

ifeq ($(LANGUAGE),C)
	LANGEXTENSION	=	.c
else ifeq ($(LANGUAGE),C++)
	LANGEXTENSION	=	.cpp
endif

ifneq ($(TYPE),exec)
	NAME			:=	lib$(NAME)$(LIBEXTENTION)
endif

# These variables are editable by the user in the Makefile.
PATH_SRC			?=	src
PATH_OBJ			?=	obj
PATH_LIB			?=	lib
PATH_INC			?=	include

CC					?=	gcc
AS					?=	nasm
AR					?=	ar
MAKE				?=	make
ECHO				?=	/bin/echo
RM					?=	rm -f
MKDIR				?=	mkdir -p
STRIP				?=	strip

NCC					:= $(CC)
NAS					:= $(AS)
NAR					:= $(AR)
NECHO				:= $(ECHO)
NRM					:= $(RM)
NMKDIR				:= $(MKDIR)

ifeq ($(VERBOSE),0)
	CC				:=	@$(CC)
	AS				:=	@$(AS)
	AR				:=	@$(AR)
	ECHO			:=	@$(ECHO)
	RM				:=	@$(RM)
	MKDIR			:=	@$(MKDIR)
	MAKE			:=	@$(MAKE)
endif

STD					?=	-std=c11
OPT_CFLAGS			?=
CFLAGS				?=	-Wall -Wextra -Werror $(addprefix -I,$(PATH_INC)) $(STD) $(OPT_CFLAGS) -D_GNU_SOURCE
ARFLAGS				?=	rcs
ASFLAGS				?=

ifeq ($(DEBUG),1)
	CFLAGS			+=	-g3 -Og -DDEBUG -ggdb -fno-omit-frame-pointer
	ASFLAGS			+=	-g
endif

ifeq ($(RELEASE),1)
	CFLAGS			+=	-O2 -DRELEASE
endif
