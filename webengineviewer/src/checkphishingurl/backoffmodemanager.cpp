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

#include <KConfigGroup>
#include <KConfig>

#include <QTimer>

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
        mTimer = new QTimer(q);
        mTimer->setSingleShot(true);
        q->connect(mTimer, &QTimer::timeout, q, &BackOffModeManager::slotTimerFinished);
        load();
    }
    void save();
    void load();
    int calculateBackModeTime() const;
    void startOffMode();
    void exitBackOffMode();
    void updateTimer(int seconds);
    void slotTimerFinished();

    int mNumberOfHttpFailed;
    bool isInOffMode;
    QTimer *mTimer;
    BackOffModeManager *q;
};

void BackOffModeManagerPrivate::save()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("BackOffMode"));
    grp.writeEntry("Enabled", isInOffMode);
    if (isInOffMode) {
        int calculateTimeInSeconds = calculateBackModeTime();
        uint delay = QDateTime::currentDateTime().addSecs(calculateTimeInSeconds * 60).toTime_t();
        grp.writeEntry("Delay", delay);
        updateTimer(calculateTimeInSeconds);
    } else {
        grp.deleteEntry("Delay");
    }
    grp.sync();
}

void BackOffModeManagerPrivate::slotTimerFinished()
{
    qCDebug(WEBENGINEVIEWER_LOG) << "Existing from BlackOffMode";
    isInOffMode = false;
    save();
}

void BackOffModeManagerPrivate::updateTimer(int seconds)
{
    if (mTimer->isActive()) {
        mTimer->stop();
    }
    mTimer->setInterval(seconds * 1000);
    mTimer->start();
}

void BackOffModeManagerPrivate::load()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("BackOffMode"));
    isInOffMode = grp.readEntry("Enabled", false);
    if (isInOffMode) {
        const uint delay = grp.readEntry("Delay", 0);
        const uint now = QDateTime::currentDateTime().toTime_t();
        if (delay > now) {
            const int diff = (delay - now);
            updateTimer(diff);
        } else {
            //Disable mode.
            isInOffMode = false;
        }
    }
}

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
    save();
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

bool BackOffModeManager::isInBackOffMode() const
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

void BackOffModeManager::slotTimerFinished()
{
    d->slotTimerFinished();
}
