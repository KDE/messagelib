/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cidschemehandler.h"
#include <QWebEngineUrlRequestJob>
using namespace MessageViewer;

CidSchemeHandler::CidSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

CidSchemeHandler::~CidSchemeHandler() = default;

void CidSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    // cid: urls for images are redirected by CidReferencesUrlInterceptor before reaching us.
    // Anything else can't be resolved: fail the job explicitly, otherwise it is never answered.
    job->fail(QWebEngineUrlRequestJob::UrlNotFound);
}

#include "moc_cidschemehandler.cpp"
