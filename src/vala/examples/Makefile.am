NULL=

bin_PROGRAMS = \
	       example1 \
	       example2 \
	       $(NULL)

BUILT_SOURCES = \
		example1.vala.stamp \
		example2.vala.stamp \
		$(NULL)

example1_VALASOURCES = example1.vala
example2_VALASOURCES = example2.vala

example1_SOURCES = \
		   $(example1_VALASOURCES:.vala=.c) \
		   $(NULL)

example1.vala.stamp: $(example1_VALASOURCES)
	$(VALAC) -X -I/usr/local/include/midgard3/midgard -X -L/usr/local/lib -X -lmidgardcr --includedir=/usr/local/include/ --save-temps --pkg MidgardCR-3.0 $^
	touch $@

example2.vala.stamp: $(example2_VALASOURCES)
$(VALAC) -X -I/usr/local/include/midgard3/midgard -X -L/usr/local/lib -X -lmidgardcr --includedir=/usr/local/include/ --save-temps --pkg MidgardCR-3.0 $^
touch $@
