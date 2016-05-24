#! /bin/sh
scripts/extract_strings_gettext.py `find . -name \*.html` >> html.cpp
$XGETTEXT `find . -name '*.cpp' -o -name '*.h' | grep -v '/tests/' | grep -v '/autotests/'` -o $podir/libmessageviewer.pot
rm -f html.cpp
