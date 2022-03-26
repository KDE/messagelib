/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef WEBENGINEVIEWER_TESTS_EXPORT
#define WEBENGINEVIEWER_TESTS_EXPORT WEBENGINEVIEWER_EXPORT
#endif
#else /* not compiling tests */
#define WEBENGINEVIEWER_TESTS_EXPORT
#endif
