/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QComboBox>

#include "messageviewer_private_export.h"
#include "remotecontentinfo.h"
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentStatusTypeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit RemoteContentStatusTypeComboBox(QWidget *parent = nullptr);
    ~RemoteContentStatusTypeComboBox() override;
    void setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus type);
    [[nodiscard]] RemoteContentInfo::RemoteContentInfoStatus status() const;
};
}
