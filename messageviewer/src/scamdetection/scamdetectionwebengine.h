/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <QVariant>
#include <memory>
class QWebEnginePage;
namespace MessageViewer
{
class ScamDetectionWebEnginePrivate;
/**
 * @brief The ScamDetectionWebEngine class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamDetectionWebEngine : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWebEngine(QObject *parent = nullptr);
    ~ScamDetectionWebEngine() override;

    void scanPage(QWebEnginePage *page);

public Q_SLOTS:
    void showDetails();

Q_SIGNALS:
    void messageMayBeAScam();
    void resultScanDetection(bool foundScam);

private:
    MESSAGEVIEWER_NO_EXPORT void handleScanPage(const QVariant &result);
    std::unique_ptr<ScamDetectionWebEnginePrivate> const d;
};
}
