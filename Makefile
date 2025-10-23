.PHONY: all gc gc-clean gc-run gc-pgo-generate gc-pgo-optimize pgo-clean

all: gc

run: gc-run

clean: gc-clean

gc:
	$(MAKE) -f Makefile.gc

gc-clean:
	$(MAKE) -f Makefile.gc clean

gc-run: gc
	$(MAKE) -f Makefile.gc run

# PGO targets (GameCube)
gc-pgo-generate:
	$(MAKE) -f Makefile.gc pgo-generate

gc-pgo-optimize:
	$(MAKE) -f Makefile.gc pgo-optimize

pgo-clean:
	$(MAKE) -f Makefile.gc pgo-clean
