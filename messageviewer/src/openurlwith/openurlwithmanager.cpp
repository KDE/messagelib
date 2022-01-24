/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanager.h"
using namespace MessageViewer;
OpenUrlWithManager::OpenUrlWithManager(QObject *parent)
    : QObject{parent}
{
}

OpenUrlWithManager::~OpenUrlWithManager()
{
}

OpenUrlWithManager *OpenUrlWithManager::self()
{
    static OpenUrlWithManager s_self;
    return &s_self;
}
