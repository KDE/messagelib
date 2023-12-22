#! /bin/sh
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
$EXTRACTRC `find . -name '*.ui' -or -name '*.kcfg'` >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/libtemplateparser6.pot
rm -f rc.cpp
