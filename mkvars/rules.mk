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

compiledb:
	$(BEAR)	-- $(NMAKE) re
	$(MAKE) -s -C libcrypto42 compiledb
	$(JQ) -s add libcrypto42/compile_commands.json ./compile_commands.json > compile_commands.tmp.json
	mv compile_commands.tmp.json compile_commands.json

$(PATH_OBJ)/%.o:			$(PATH_SRC)/%.c
	$(MKDIR) $(dir $@)

	$(PRINTF) " $(BOLD)$(if $(filter $(shell echo $< | grep 'tests/' ; echo $$?),1),$(if $(filter $(shell echo $< | grep -E '\.test\.c' ; echo $$?),0),$(MAGENTA),$(BLUE)),$(CYAN))"
	$(PRINTF) "$(GREATER)$(NORMAL)   Compiling $(ITALIC)$(subst $(PATH_SRC)/,,$<)$(TRESET)\n"

	$(CC) $(CFLAGS) -c -MMD $< -o $@

$(PATH_OBJ)/%.o:			$(PATH_SRC)/%.cc
	$(MKDIR) $(dir $@)

	$(PRINTF) " $(BOLD)$(if $(filter $(shell echo $< | grep 'tests/' ; echo $$?),1),$(if $(filter $(shell echo $< | grep -E '\.test\.c' ; echo $$?),0),$(MAGENTA),$(BLUE)),$(CYAN))"
	$(PRINTF) "$(GREATER)$(NORMAL)   Compiling $(ITALIC)$(subst $(PATH_SRC)/,,$<)$(TRESET)\n"

	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@

-include $(DEPS)

$(NAME):						$(OBJS) $(SUBLIBS)
ifeq ($(TYPE),exec)

	$(PRINTF) " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Linking $(ITALIC)$(subst $(PATH_OBJ)/,,$@)$(TRESET)\n"
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

else ifeq ($(TYPE),static)

	$(PRINTF) " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Creating $(ITALIC)$(subst $(PATH_LIB)/,,$@)$(TRESET)\n"
	$(AR) $(ARFLAGS) $@ $(OBJS)

else ifeq ($(TYPE),shared)

	$(PRINTF) " $(BOLD)$(GREEN)$(BIGGREATER)$(NORMAL)   Creating $(ITALIC)$(subst $(PATH_LIB)/,,$@)$(TRESET)\n"
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
	$(PRINTF) "Testing colors...\n"
	$(PRINTF) "$(RED)RED$(TRESET)\n"
	$(PRINTF) "$(GREEN)GREEN$(TRESET)\n"
	$(PRINTF) "$(GREEN_42)GREEN_42$(TRESET)\n"
	$(PRINTF) "$(GREEN_79)GREEN_79$(TRESET)\n"
	$(PRINTF) "$(YELLOW)YELLOW$(TRESET)\n"
	$(PRINTF) "$(ORANGE)ORANGE$(TRESET)\n"
	$(PRINTF) "$(BLUE)BLUE$(TRESET)\n"
	$(PRINTF) "$(MAGENTA)MAGENTA$(TRESET)\n"
	$(PRINTF) "$(MAGENTA_129)MAGENTA_129$(TRESET)\n"
	$(PRINTF) "$(CYAN)CYAN$(TRESET)\n"

	$(PRINTF) "$(UNDERLINE)UNDERLINE$(TRESET)\n"
	$(PRINTF) "$(BOLD)BOLD$(TRESET)\n\n"


info_vars:
	$(PRINTF) "Displaying variables...\n"
	@($(NPRINTF) "CC:$(CC)\n";																\
	$(NPRINTF) "CXX:$(CXX)\n";																\
	$(NPRINTF) "AS:$(AS)\n";																\
	$(NPRINTF) "AR:$(AR)\n";																\
	$(NPRINTF) "RM:$(RM)\n";																\
	$(NPRINTF) "MKDIR:$(MKDIR)\n";															\
	$(NPRINTF) "PRINTF:$(PRINTF)\n\n";															\
	\
	$(NPRINTF) "NCC:$(NCC)\n";																\
	$(NPRINTF) "NCXX:$(NCXX)\n";																\
	$(NPRINTF) "NAS:$(NAS)\n";																\
	$(NPRINTF) "NAR:$(NAR)\n";																\
	$(NPRINTF) "NRM:$(NRM)\n";																\
	$(NPRINTF) "NMKDIR:$(NMKDIR)\n";														\
	$(NPRINTF) "NPRINTF:$(NPRINTF)\n\n";															\
	\
	$(NPRINTF) "PATH_SRC:$(PATH_SRC)\n";													\
	$(NPRINTF) "PATH_OBJ:$(PATH_OBJ)\n";													\
	$(NPRINTF) "PATH_LIB:$(PATH_LIB)\n";													\
	$(NPRINTF) "PATH_INC:$(PATH_INC)\n\n";													\
	\
	$(NPRINTF) "CFLAGS:$(CFLAGS)\n";														\
	$(NPRINTF) "CXXFLAGS:$(CXXFLAGS)\n";														\
	$(NPRINTF) "ARFLAGS:$(ARFLAGS)\n";														\
	$(NPRINTF) "ASFLAGS:$(ASFLAGS)\n\n";														\
	\
	$(NPRINTF) "NAME:$(NAME)\n";															\
	$(NPRINTF) "TYPE:$(TYPE)\n";															\
	$(NPRINTF) "LANGUAGE:$(LANGUAGE)\n";													\
	$(NPRINTF) "COLORS:$(COLORS)\n\n";														\
	\
	$(NPRINTF) "VERBOSE:$(VERBOSE)\n";														\
	$(NPRINTF) "DEBUG:$(DEBUG)\n";															\
	$(NPRINTF) "RELEASE:$(RELEASE)\n\n";														\
	\
	$(NPRINTF) "SRC:$(SRC)\n";																\
	$(NPRINTF) "OBJS:$(OBJS)\n";															\
	$(NPRINTF) "DEPS:$(DEPS)\n"																\
	) | awk '																			\
		{																				\
			input = $$0;																\
			split(input, x, ":");														\
			printf "$(BOLD)$(RED)""%-15s""$(CRESET)""%s""$(CRESET)\n", x[1], x[2]		\
		}'
	$(PRINTF) "$(TRESET)"

.PHONY: all info info_vars test_colors clean fclean re $(SUBLIBS)
