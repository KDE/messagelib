/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithjob.h"
using namespace MessageViewer;
OpenUrlWithJob::OpenUrlWithJob(QObject *parent)
    : QObject{parent}
{
}

OpenUrlWithJob::~OpenUrlWithJob()
{
}

bool OpenUrlWithJob::canStart() const
{
    // TODO
    return false;
}

void OpenUrlWithJob::start()
{
    // TODO
}
