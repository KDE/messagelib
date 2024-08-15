#! /bin/sh
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
$EXTRACTRC `find . -name '*.ui' -or -name '*.rc' -or -name '*.kcfg'` >> rc.cpp || exit 11
$XGETTEXT -ktranslate `find -name '*.cpp' -o -name '*.h'` -o $podir/libmessagelist6.pot
rm -f rc.cpp
