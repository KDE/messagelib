/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_private_export.h"

#include <KMessageWidget>

namespace MessageViewer
{
class ScamDetectionWarningWidgetPrivate;
class MESSAGEVIEWER_TESTS_EXPORT ScamDetectionWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit ScamDetectionWarningWidget(QWidget *parent = nullptr);
    ~ScamDetectionWarningWidget() override;

    void setUseInTestApps(bool b);

public Q_SLOTS:
    void slotShowWarning();

Q_SIGNALS:
    void showDetails();
    void moveMessageToTrash();
    void messageIsNotAScam();
    void addToWhiteList();

private:
    void slotShowDetails(const QString &content);
    void slotAddToWhiteList();
    void slotDisableScamDetection();
    void slotMessageIsNotAScam();

private:
    ScamDetectionWarningWidgetPrivate *const d;
};
}

