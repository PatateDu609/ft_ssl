ifndef SRC
$(error "SRC is not defined")
endif

ifndef OBJS
$(error "OBJS is not defined")
endif

ifndef DEPS
$(error "DEPS is not defined")
endif

ifndef NAME
$(error "NAME is not defined")
endif

all:							$(NAME)

$(PATH_OBJ)/%.o:			$(PATH_SRC)/%$(LANGEXTENSION) mkvars/*.mk Makefile
	$(MKDIR) $(dir $@)
	$(ECHO) -e " $(BOLD)$(BLUE)$(GREATER)$(NORMAL)   Compiling $(ITALIC)$(subst $(PATH_SRC)/,,$<)$(TRESET)"
	$(CC) $(CFLAGS) -c -MMD $< -o $@

-include $(DEPS)

$(NAME):						$(OBJS) $(SUBLIBS)
ifeq ($(TYPE),exec)

	$(ECHO) -e " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Linking $(ITALIC)$(subst $(PATH_OBJ)/,,$@)$(TRESET)"
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

else ifeq ($(TYPE),static)

	$(ECHO) -e " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Creating $(ITALIC)$(subst $(PATH_LIB)/,,$@)$(TRESET)"
	$(AR) $(ARFLAGS) $@ $(OBJS)

else ifeq ($(TYPE),shared)

	$(ECHO) -e " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Creating $(ITALIC)$(subst $(PATH_LIB)/,,$@)$(TRESET)"
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)

endif

re:								fclean all

fclean:							clean
	$(RM) $(NAME)
	$(RM) -r $(PATH_OBJ)

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)

info:							test_colors info_vars

test_colors:
	$(ECHO) "Testing colors..."
	$(ECHO) -e "$(RED)RED$(TRESET)"
	$(ECHO) -e "$(GREEN)GREEN$(TRESET)"
	$(ECHO) -e "$(YELLOW)YELLOW$(TRESET)"
	$(ECHO) -e "$(BLUE)BLUE$(TRESET)"
	$(ECHO) -e "$(MAGENTA)MAGENTA$(TRESET)"
	$(ECHO) -e "$(CYAN)CYAN$(TRESET)"

	$(ECHO) -e "$(UNDERLINE)UNDERLINE$(TRESET)"
	$(ECHO) -e "$(BOLD)BOLD$(TRESET)\n"


info_vars:
	$(ECHO) "Displaying variables..."
	@($(NECHO) "CC:$(CC)";																\
	$(NECHO) "AS:$(AS)";																\
	$(NECHO) "AR:$(AR)";																\
	$(NECHO) "RM:$(RM)";																\
	$(NECHO) "MKDIR:$(MKDIR)";															\
	$(NECHO) "ECHO:$(ECHO)";															\
	$(NECHO);																			\
	\
	$(NECHO) "NCC:$(NCC)";																\
	$(NECHO) "NAS:$(NAS)";																\
	$(NECHO) "NAR:$(NAR)";																\
	$(NECHO) "NRM:$(NRM)";																\
	$(NECHO) "NMKDIR:$(NMKDIR)";														\
	$(NECHO) "NECHO:$(NECHO)";															\
	$(NECHO);																			\
	\
	$(NECHO) "PATH_SRC:$(PATH_SRC)";													\
	$(NECHO) "PATH_OBJ:$(PATH_OBJ)";													\
	$(NECHO) "PATH_LIB:$(PATH_LIB)";													\
	$(NECHO) "PATH_INC:$(PATH_INC)";													\
	$(NECHO);																			\
	\
	$(NECHO) "CFLAGS:$(CFLAGS)";														\
	$(NECHO) "ARFLAGS:$(ARFLAGS)";														\
	$(NECHO) "ASFLAGS:$(ASFLAGS)";														\
	$(NECHO);																			\
	\
	$(NECHO) "NAME:$(NAME)";															\
	$(NECHO) "TYPE:$(TYPE)";															\
	$(NECHO) "LANGUAGE:$(LANGUAGE)";													\
	$(NECHO) "COLORS:$(COLORS)";														\
	$(NECHO);																			\
	\
	$(NECHO) "VERBOSE:$(VERBOSE)";														\
	$(NECHO) "DEBUG:$(DEBUG)";															\
	$(NECHO) "RELEASE:$(RELEASE)";														\
	$(NECHO);																			\
	\
	$(NECHO) "SRC:$(SRC)";																\
	$(NECHO) "OBJS:$(OBJS)";															\
	$(NECHO) "DEPS:$(DEPS)"																\
	) | awk '																			\
		{																				\
			input = $$0;																\
			split(input, x, ":");														\
			printf "$(BOLD)$(RED)""%-15s""$(CRESET)""%s""$(CRESET)\n", x[1], x[2]		\
		}'

.PHONY: all info info_vars test_colors clean fclean re
