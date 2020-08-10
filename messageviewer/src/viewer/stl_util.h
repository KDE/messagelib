/*  -*- c++ -*-
    stl_util.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDEPIM_MESSAGEVIEWER_STL_UTIL_H
#define KDEPIM_MESSAGEVIEWER_STL_UTIL_H

namespace MessageViewer {
template<typename T>
struct DeleteAndSetToZero {
    void operator()(const T * &t)
    {
        delete t;
        t = nullptr;
    }
};
}

#endif // KDEPIM__MESSAGEVIEWER__STL_UTIL_H
