/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once
#include "webengineviewer_export.h"
#include <KMessageWidget>

namespace WebEngineViewer
{
class SubmittedFormWarningWidgetPrivate;
class WEBENGINEVIEWER_EXPORT SubmittedFormWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit SubmittedFormWarningWidget(QWidget *parent = nullptr);
    ~SubmittedFormWarningWidget() override;

public Q_SLOTS:
    void showWarning();
};
}
