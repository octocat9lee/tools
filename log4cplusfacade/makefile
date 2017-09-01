.PHONY : all clean

SUBDIRS := ./src ./test
all:
	@for subDir in $(SUBDIRS) ; do \
	(cd $$subDir && make ) || exit 1; \
	done

clean:
	@for subDir in $(SUBDIRS) ; do \
	(cd $$subDir && make clean ) || exit 1; \
	done

