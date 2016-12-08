/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "backoffmodemanager.h"
#include "checkphishingurlutil.h"
#include "webengineviewer_debug.h"

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(BackOffModeManager, s_backOffModeManager)

class WebEngineViewer::BackOffModeManagerPrivate
{
public:
    BackOffModeManagerPrivate(BackOffModeManager *qq)
        : mNumberOfHttpFailed(0),
          isInOffMode(false),
          q(qq)
    {

    }
    int calculateBackModeTime() const;
    void startOffMode();
    void exitBackOffMode();
    int mNumberOfHttpFailed;
    bool isInOffMode;
    BackOffModeManager *q;
};

int BackOffModeManagerPrivate::calculateBackModeTime() const
{
    return WebEngineViewer::CheckPhishingUrlUtil::generateRandomSecondValue(mNumberOfHttpFailed);
}

void BackOffModeManagerPrivate::startOffMode()
{
    if (!isInOffMode) {
        qCWarning(WEBENGINEVIEWER_LOG) << "starting back of mode";
        isInOffMode = true;
    }
    mNumberOfHttpFailed++;
    //TODO add timer here.
}

void BackOffModeManagerPrivate::exitBackOffMode()
{
    mNumberOfHttpFailed = 0;
}

BackOffModeManager::BackOffModeManager(QObject *parent)
    : QObject(parent),
      d(new BackOffModeManagerPrivate(this))
{

}

BackOffModeManager::~BackOffModeManager()
{
    delete d;
}

BackOffModeManager *BackOffModeManager::self()
{
    return s_backOffModeManager;
}

bool BackOffModeManager::isInOffMode() const
{
    return d->isInOffMode;
}

void BackOffModeManager::startOffMode()
{
    d->startOffMode();
}

int BackOffModeManager::numberOfHttpFailed() const
{
    return d->mNumberOfHttpFailed;
}
