/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagelist_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef MESSAGELIST_TESTS_EXPORT
#define MESSAGELIST_TESTS_EXPORT MESSAGELIST_EXPORT
#endif
#else /* not compiling tests */
#define MESSAGELIST_TESTS_EXPORT
#endif
