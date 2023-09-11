# contrib/pg_pathlist_hook_demo/Makefile

MODULE_big	= pg_pathlist_hook_demo
OBJS		= pg_pathlist_hook.o pg_custom_pathlist_hook.o fpga_qual.o hlog.o query.o shmem_lib.o msg_client.o $(WIN32RES)

EXTENSION = pg_pathlist_hook_demo

REGRESS = page btree brin gin hash

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_pathlist_hook_demo
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
