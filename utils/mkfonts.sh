#!/bin/sh

rm -f ../fonts.c ../fonts.h

cd fonts

/bin/ls -1  *.bdf|while read f; do echo $f;../bdf2c.pl ' -~' 'a-z' 'A-Z' "$f"; done
/bin/ls -1  *.h  |while read f; do cat "$f" >> ../../fonts.h; done
/bin/ls -1  *.c  |while read f; do cat "$f" >> ../../fonts.c; done

rm -f *.c *.h

