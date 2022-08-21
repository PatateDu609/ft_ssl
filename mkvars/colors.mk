ifneq ($(COLORS),no)

UNDERLINE		=	\033[4m
BOLD			=	\033[1m
DIM				=	\033[2m
ITALIC			=	\033[3m

NORMAL			=	\033[21;22;24;23m
TRESET			=	\033[0m

ifeq ($(COLORS),256)
	CRESET		=	\033[38;5;248m
	RED			=	\033[38;5;160m
	GREEN		=	\033[38;5;46m
	YELLOW		=	\033[38;5;190m
	BLUE		=	\033[38;5;21m
	MAGENTA		=	\033[38;5;126m
	CYAN		=	\033[38;5;86m

else ifeq ($(COLORS),16)
	CRESET		=	\033[0m
	RED			=	\033[31m
	GREEN		=	\033[32m
	YELLOW		=	\033[33m
	BLUE		=	\033[34m
	MAGENTA		=	\033[35m
	CYAN		=	\033[36m
endif

endif

GREATER			=	⪢
BIGGREATER		=	⫸
