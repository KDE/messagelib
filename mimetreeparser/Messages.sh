#! /bin/sh
$EXTRACT_GRANTLEE_TEMPLATE_STRINGS `find . -name \*.html` >> html.cpp
$XGETTEXT `find . -name '*.cpp' -o -name '*.h' | grep -v '/tests/' | grep -v '/autotests/'` -o $podir/libmessageviewer.pot
rm -f html.cpp
