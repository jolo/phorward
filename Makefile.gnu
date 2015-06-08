all: README.md phorward.man
	cd src; make -f Makefile.gnu
	cd tools; make -f Makefile.gnu
	cd DEMO; make -f Makefile.gnu

clean:
	cd src; make -f Makefile.gnu clean
	cd tools; make -f Makefile.gnu clean
	cd DEMO; make -f Makefile.gnu clean

make_install:
	cp Makefile.gnu Makefile
	cp src/Makefile.gnu src/Makefile
	cp tools/Makefile.gnu tools/Makefile
	cp DEMO/Makefile.gnu DEMO/Makefile

make_update:
	cp Makefile Makefile.gnu
	cp src/Makefile src/Makefile.gnu
	cp tools/Makefile tools/Makefile.gnu
	cp DEMO/Makefile DEMO/Makefile.gnu

make_uninstall:
	-rm Makefile
	-rm src/Makefile
	-rm tools/Makefile
	-rm DEMO/Makefile

README.md: doc/readme.t2t
	-rm -f $@
	txt2tags -t txt -H -o - $? | sed -E -n '1h;1!H;$${;g;s/ +([-A-Z ]+)\n +(=+)/\1\n\2/g;p;}' | sed 's/^  /    /g' >$@

phorward.man: doc/readme.t2t
	txt2tags -o $@ -t man $?

