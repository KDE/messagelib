/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamcheckshorturlmanager.h"
#include "scamcheckshorturl.h"

using namespace MessageViewer;

class MessageViewer::ScamCheckShortUrlManagerPrivate
{
public:
    ScamCheckShortUrlManagerPrivate() = default;

    ScamCheckShortUrl *mCheckShortUrl = nullptr;
};

ScamCheckShortUrlManager::ScamCheckShortUrlManager(QObject *parent)
    : QObject(parent)
    , d(new ScamCheckShortUrlManagerPrivate)
{
    d->mCheckShortUrl = new ScamCheckShortUrl(this);
}

ScamCheckShortUrlManager::~ScamCheckShortUrlManager() = default;

ScamCheckShortUrlManager *ScamCheckShortUrlManager::self()
{
    static ScamCheckShortUrlManager s_self;
    return &s_self;
}

ScamCheckShortUrl *ScamCheckShortUrlManager::scamCheckShortUrl() const
{
    return d->mCheckShortUrl;
}

#include "moc_scamcheckshorturlmanager.cpp"
