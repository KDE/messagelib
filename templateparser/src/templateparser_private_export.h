/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEPARSERPRIVATE_EXPORT_H
#define TEMPLATEPARSERPRIVATE_EXPORT_H

#include "templateparser_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
# ifndef TEMPLATEPARSER_TESTS_EXPORT
#  define TEMPLATEPARSER_TESTS_EXPORT TEMPLATEPARSER_EXPORT
# endif
#else /* not compiling tests */
# define TEMPLATEPARSER_TESTS_EXPORT
#endif

#endif
