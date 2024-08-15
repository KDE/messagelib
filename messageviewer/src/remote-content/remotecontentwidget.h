/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include "remotecontentinfo.h"
#include <QWidget>
class QLineEdit;
namespace MessageViewer
{
class RemoteContentStatusTypeComboBox;
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentWidget(QWidget *parent = nullptr);
    ~RemoteContentWidget() override;

    [[nodiscard]] RemoteContentInfo info() const;
    void setInfo(const RemoteContentInfo &info);

Q_SIGNALS:
    void updateOkButton(bool enabled);

private:
    MESSAGEVIEWER_NO_EXPORT void slotTextChanged(const QString &url);
    QLineEdit *const mLineEdit;
    RemoteContentStatusTypeComboBox *const mStatusComboBox;
};
}
