/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef MESSAGEVIEWER_TESTS_EXPORT
#define MESSAGEVIEWER_TESTS_EXPORT MESSAGEVIEWER_EXPORT
#endif
#else /* not compiling tests */
#define MESSAGEVIEWER_TESTS_EXPORT
#endif
