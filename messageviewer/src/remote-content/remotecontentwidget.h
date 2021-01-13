/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTWIDGET_H
#define REMOTECONTENTWIDGET_H

#include <QWidget>
#include "messageviewer_private_export.h"
#include "remotecontentinfo.h"
class QLineEdit;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentWidget(QWidget *parent = nullptr);
    ~RemoteContentWidget() override;

    Q_REQUIRED_RESULT RemoteContentInfo info() const;
    void setInfo(const RemoteContentInfo &info);
private:
    QLineEdit *const mLineEdit;
};
}
#endif // REMOTECONTENTWIDGET_H
