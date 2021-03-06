#! /bin/sh
$EXTRACT_GRANTLEE_TEMPLATE_STRINGS `find . -name \*.html` >> rc.cpp
$EXTRACTRC `find . -name '*.ui' -or -name '*.rc' -or -name '*.kcfg' -or -name '*.kcfg.cmake'` >> rc.cpp || exit 11
$XGETTEXT `find . -name '*.cpp' -o -name '*.h' | grep -v '/tests/' | grep -v '/autotests/'` -o $podir/libmessageviewer.pot
rm -f rc.cpp
