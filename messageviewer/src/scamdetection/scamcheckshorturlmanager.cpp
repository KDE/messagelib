/*
  SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamcheckshorturl.h"
#include "scamcheckshorturlmanager.h"

using namespace MessageViewer;

class MessageViewer::ScamCheckShortUrlManagerPrivate
{
public:
    ScamCheckShortUrlManagerPrivate()
    {
    }

    ScamCheckShortUrl *mCheckShortUrl = nullptr;
};

ScamCheckShortUrlManager::ScamCheckShortUrlManager(QObject *parent)
    : QObject(parent)
    , d(new ScamCheckShortUrlManagerPrivate)
{
    d->mCheckShortUrl = new ScamCheckShortUrl(this);
}

ScamCheckShortUrlManager::~ScamCheckShortUrlManager()
{
    delete d;
}

ScamCheckShortUrlManager *ScamCheckShortUrlManager::self()
{
    static ScamCheckShortUrlManager s_self;
    return &s_self;
}

ScamCheckShortUrl *ScamCheckShortUrlManager::scamCheckShortUrl() const
{
    return d->mCheckShortUrl;
}
