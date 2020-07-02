/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOREPRIVATE_EXPORT_H
#define MESSAGECOREPRIVATE_EXPORT_H

#include "messagecore_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
# ifndef MESSAGECORE_TESTS_EXPORT
#  define MESSAGECORE_TESTS_EXPORT MESSAGECORE_EXPORT
# endif
#else /* not compiling tests */
# define MESSAGECORE_TESTS_EXPORT
#endif

#endif
