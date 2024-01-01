/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
    void showWarning();
};
}
