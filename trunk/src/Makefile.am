NULL =

INCLUDES =						\
	-DG_LOG_DOMAIN=\"Nautilus-Batch-Rename\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	-Werror                                         \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(NAUTILUS_CFLAGS)

nautilus_extensiondir=$(NAUTILUS_EXTENSION_DIR)

nautilus_extension_LTLIBRARIES=libnautilus-batch-rename.la

libnautilus_batch_rename_la_SOURCES =	\
	nautilus-batch-rename-extension.c		\
	nautilus-batch-rename-extension.h		\
	nautilus-batch-rename.c		\
	nautilus-batch-rename.h		\
	$(NULL)

libnautilus_batch_rename_la_LDFLAGS = -module -avoid-version
libnautilus_batch_rename_la_LIBADD  = $(NAUTILUS_LIBS)

dist_pkgdata_DATA = \
	nautilus-batch-rename.glade \
	$(NULL)
