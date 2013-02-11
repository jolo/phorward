all: README phorward.man
	cd src; make -f Makefile.gnu
	cd DEMO; make -f Makefile.gnu

clean:
	cd src; make -f Makefile.gnu clean
	cd DEMO; make -f Makefile.gnu clean

config:
	cp Makefile.gnu Makefile
	cp src/Makefile.gnu src/Makefile
	cp DEMO/Makefile.gnu DEMO/Makefile
	
unconfig:
	-rm Makefile
	-rm src/Makefile
	-rm DEMO/Makefile

README: doc/readme.t2t
	-rm -f $@
	txt2tags -t txt -H -o - $? | sed -E -n '1h;1!H;$${;g;s/ +([-A-Z ]+)\n +(=+)/\2==\n \1 \n\2==/g;p;}' >$@.tmp
	cat doc/readme.hdr $@.tmp >>$@
	rm -f $@.tmp


phorward.man: doc/readme.t2t
	txt2tags -o $@ -t man $?