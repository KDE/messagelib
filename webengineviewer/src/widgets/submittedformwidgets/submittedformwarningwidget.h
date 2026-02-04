/*
  SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
    /*! Constructs a SubmittedFormWarningWidget with the given parent. */
    explicit SubmittedFormWarningWidget(QWidget *parent = nullptr);
    /*! Destroys the SubmittedFormWarningWidget. */
    ~SubmittedFormWarningWidget() override;
    /*! Shows the warning. */
    void showWarning();
};
}
