/*  -*- c++ -*-
    stl_util.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

namespace MessageViewer
{
template<typename T>
struct DeleteAndSetToZero {
    void operator()(const T *&t)
    {
        delete t;
        t = nullptr;
    }
};
}
