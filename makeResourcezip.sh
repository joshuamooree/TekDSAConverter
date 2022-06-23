#!/bin/sh

rm Resources.zip

zip -r -9 Resources.zip Documentation/*.htm Documentation/*.html Documentation/*.hhp Documentation/*.hhk Documentation/*.hhc \
	 Documentation/*.png Documentation/*.jpg Documentation/*jpeg Documentation/*.gif icons/*.png

tools/bin2c Resources.zip Resources.zip.h

# rm Resources.zip
