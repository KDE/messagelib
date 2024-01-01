/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "backoffmodemanager.h"
#include "checkphishingurlutil.h"
#include "webengineviewer_debug.h"

#include <KConfig>
#include <KConfigGroup>

#include <QTimer>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(BackOffModeManager, s_backOffModeManager)

class WebEngineViewer::BackOffModeManagerPrivate
{
public:
    BackOffModeManagerPrivate(BackOffModeManager *qq)
        : q(qq)
        , mTimer(new QTimer(q))
    {
        mTimer->setSingleShot(true);
        q->connect(mTimer, &QTimer::timeout, q, &BackOffModeManager::slotTimerFinished);
        load();
    }

    void save();
    void load();
    [[nodiscard]] int calculateBackModeTime() const;
    void startOffMode();
    void exitBackOffMode();
    void updateTimer(int seconds);
    void slotTimerFinished();

    BackOffModeManager *const q;
    QTimer *const mTimer;
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
            // Disable mode.
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

BackOffModeManager::~BackOffModeManager() = default;

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
