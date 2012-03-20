morfenc = MorfEnc.pm morfeusz_encoding.txt

all: tab-unicode.cc tab-latin2.cc tab-cp1250.cc tab-cp852.cc \
	latin2-tab.pl char-ranges.hh unicode-tab.pl

tab-unicode.cc: gentable-unicode $(morfenc)
	./gentable-unicode > $@

unicode-tab.pl: gentable-unicode-perl $(morfenc)
	./gentable-unicode-perl > $@

tab-latin2.cc: 8859-2.txt gentable-8bit $(morfenc)
	./gentable-8bit latin2 $< > $@

tab-cp1250.cc: cp1250.txt gentable-8bit $(morfenc)
	./gentable-8bit cp1250 $< > $@

tab-cp852.cc: cp852.txt gentable-8bit $(morfenc)
	./gentable-8bit cp852 $< > $@

latin2-tab.pl: 8859-2.txt gentable-perl $(morfenc)
	./gentable-perl  $< > $@

char-ranges.hh: dump-ranges $(morfenc)
	./dump-ranges >$@

.PHONY: all
