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

CC					?=	cc
CXX					?=	c++
AS					?=	nasm
AR					?=	ar
MAKE				?=	make -s
PRINTF				?=	printf
RM					?=	rm -f
MKDIR				?=	mkdir -p
STRIP				?=	strip
BEAR				?=	bear
JQ					?= jq

NCC					:= $(CC)
NCXX				:= $(CXX)
NAS					:= $(AS)
NPRINTF				:= $(PRINTF)
NMAKE				:= $(MAKE)
NAR					:= $(AR)
NRM					:= $(RM)
NMKDIR				:= $(MKDIR)
NBEAR				?= $(BEAR)
NJQ					?= $(JQ)

STD					?=	-std=c11
CXX_STD				?=	-std=c++17
OPT_CFLAGS			?=
CFLAGS				?=	-Wall -Wextra -Werror $(addprefix -I,$(PATH_INC)) $(OPT_CFLAGS)
CXXFLAGS			?=	$(filter-out $(STD),$(CFLAGS)) $(CXX_STD)
ARFLAGS				?=	rcs
ASFLAGS				?=

ifeq (,$(shell which $(BEAR)))
	$(error "No $(BEAR) in $(PATH), consider doing apt-get install $(BEAR) (or appropriate command for your system)")
endif

ifeq (,$(shell which $(JQ)))
	$(error "No $(JQ) in $(PATH), consider doing apt-get install $(JQ) (or appropriate command for your system)")
endif

ifneq ($(findstring std,$(CFLAGS)),std)
	CFLAGS			+=	$(STD)
endif

C_COMPILER			:=	unknown
ifeq ($(shell $(NCC) --version | grep -o 'clang' | head -1 | tr -d '\n'),clang)
	CFLAGS			+=	-DHAVE_CLANG_COMPILER
	COMPILER		=	clang
else ifeq ($(shell $(NCC) --version | grep -oahr -m 1 'gcc' | head -1),gcc)
	CFLAGS			+=	-DHAVE_GCC_COMPILER
	COMPILER		=	gcc
else ifeq ($(shell readlink -f $(NCC) | grep -o gcc),gcc)
	CFLAGS			+=	-DHAVE_GCC_COMPILER
	COMPILER		=	gcc
else
	TARGET			!= $(error "unknown compiler, only clang and gcc are managed")
endif

CPP_COMPILER			:=	unknown
 ifeq ($(shell $(NCXX) --version | grep -o 'clang' | head -1 | tr -d '\n'),clang)
	CFXXLAGS			+=	-DHAVE_CLANG_COMPILER
	CPP_COMPILER		=	clang++
 else ifeq ($(shell $(NCXX) --version | grep -oahr -m 1 'g++' | head -1),g++)
	CXXFLAGS			+=	-DHAVE_GCC_COMPILER
	CPP_COMPILER		=	g++
 else ifeq ($(shell readlink -f $(NCXX) | grep -o g++),g++)
	CXXFLAGS			+=	-DHAVE_GCC_COMPILER
	CPP_COMPILER		=	g++
 else
	TARGET			!= $(error "unknown compiler, only clang and gcc are managed")
 endif


ifeq ($(shell uname),Darwin)
	CFLAGS			+=	-I/opt/homebrew/include
	CXXFLAGS		+=	-I/opt/homebrew/include
endif

ifeq ($(VERBOSE),0)
	CC				:=	@$(CC)
	CXX				:=	@$(CXX)
	AS				:=	@$(AS)
	AR				:=	@$(AR)
	PRINTF			:=	@$(PRINTF)
	RM				:=	@$(RM)
	MKDIR			:=	@$(MKDIR)
	MAKE			:=	@$(MAKE)
	BEAR			:=	@$(BEAR)
	JQ				:=	@$(JQ)
endif

ifeq ($(DEBUG),1)
	CFLAGS			+=	-O0 -DDEBUG
	CXXFLAGS		+=	-O0 -DDEBUG

	ifeq ($(COMPILER),gcc)
		CFLAGS			+=	-g3 -ggdb -fno-omit-frame-pointer -fdiagnostics-color=always
	else ifeq ($(COMPILER),clang)
		CFLAGS			+=	-g -glldb -fdebug-macro -fno-eliminate-unused-debug-types -fstandalone-debug
	endif

	ifeq ($(CPP_COMPILER),g++)
		CXXFLAGS		+=	-g3 -ggdb -fno-omit-frame-pointer -fdiagnostics-color=always
	else ifeq ($(CPP_COMPILER),clang++)
		CXXFLAGS		+=	-g -glldb -fdebug-macro -fno-eliminate-unused-debug-types -fstandalone-debug
	endif

	ASFLAGS			+=	-g
endif

ifeq ($(RELEASE),1)
	CFLAGS			+=	-O2 -DRELEASE
endif
