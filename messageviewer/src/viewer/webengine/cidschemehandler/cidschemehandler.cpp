/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cidschemehandler.h"
using namespace MessageViewer;

CidSchemeHandler::CidSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

CidSchemeHandler::~CidSchemeHandler()
{
}

void CidSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    Q_UNUSED(job)
}
