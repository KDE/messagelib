/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSERPRIVATE_EXPORT_H
#define MESSAGECOMPOSERPRIVATE_EXPORT_H

#include "messagecomposer_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
# ifndef MESSAGECOMPOSER_TESTS_EXPORT
#  define MESSAGECOMPOSER_TESTS_EXPORT MESSAGECOMPOSER_EXPORT
# endif
#else /* not compiling tests */
# define MESSAGECOMPOSER_TESTS_EXPORT
#endif

#endif
