/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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
        : q(qq)
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

    BackOffModeManager *q;
    QTimer *mTimer = nullptr;
    int mNumberOfHttpFailed = 0;
    bool isInOffMode = false;
};

void BackOffModeManagerPrivate::save()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("BackOffMode"));
    grp.writeEntry("Enabled", isInOffMode);
    if (isInOffMode) {
        const int calculateTimeInSeconds = calculateBackModeTime();
        const qint64 delay = QDateTime::currentDateTime().addSecs(calculateTimeInSeconds).toSecsSinceEpoch();
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
    exitBackOffMode();
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
        const qint64 delay = grp.readEntry("Delay", 0);
        const qint64 now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
        if (delay > now) {
            const qint64 diff = (delay - now);
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
    mNumberOfHttpFailed++;
    if (isInOffMode) {
        qCWarning(WEBENGINEVIEWER_LOG) << "New failed" << mNumberOfHttpFailed;
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "starting back of mode";
        isInOffMode = true;
    }
    save();
}

void BackOffModeManagerPrivate::exitBackOffMode()
{
    isInOffMode = false;
    mNumberOfHttpFailed = 0;
}

BackOffModeManager::BackOffModeManager(QObject *parent)
    : QObject(parent)
    , d(new BackOffModeManagerPrivate(this))
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
