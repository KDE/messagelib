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
    void calculateBackModeTime();
    void startOffMode();
    void exitBackOffMode();
    int mNumberOfHttpFailed;
    bool isInOffMode;
    BackOffModeManager *q;
};

void BackOffModeManagerPrivate::calculateBackModeTime()
{
    //Random between 0-1
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    //MIN((2**N-1 * 15 minutes) * (RAND + 1), 24 hours)
    int seconds = static_cast<int>(qMin(((2*mNumberOfHttpFailed) - 1) * (15 * 60) * r, static_cast<float>(24 * 60 * 60)));
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
