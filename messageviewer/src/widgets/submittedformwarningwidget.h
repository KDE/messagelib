/*
  SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef SUBMITTEDFORMWARNINGWIDGET_H
#define SUBMITTEDFORMWARNINGWIDGET_H

#include <KMessageWidget>

namespace MessageViewer {
class SubmittedFormWarningWidgetPrivate;
class SubmittedFormWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit SubmittedFormWarningWidget(QWidget *parent = nullptr);
    ~SubmittedFormWarningWidget() override;

public Q_SLOTS:
    void showWarning();
};
}

#endif // SUBMITTEDFORMWARNINGWIDGET_H
