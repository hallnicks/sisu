ifeq (,$(findstring clean,$(MAKECMDGOALS)))
 ifeq (,$(findstring static,$(MAKECMDGOALS)))
  ifeq (,$(findstring shared,$(MAKECMDGOALS)))
   ifeq (,$(findstring objects,$(MAKECMDGOALS)))
    ifeq (,$(findstring all,$(MAKECMDGOALS)))
     ifeq (,$(findstring exe,$(MAKECMDGOALS)))
      $(error Invalid make command goals. Valid commands are make exe, make shared, make static and make clean.)
     endif
    endif
   endif
  endif
 endif
endif

ifneq (,$(findstring verbose,$(MAKECMDGOALS)))
OUT :=#
else
OUT :=@#
endif
