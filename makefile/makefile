.PHONY : all clean

SUBDIRS := ./static ./shared ./exe
all:
	@for subDir in $(SUBDIRS) ; do \
	(cd $$subDir && make ) || exit 1; \
	done

clean:
	@for subDir in $(SUBDIRS) ; do \
	(cd $$subDir && make clean ) || exit 1; \
	done

