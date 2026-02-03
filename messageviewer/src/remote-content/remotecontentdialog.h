/*
   SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDialog>

#include "messageviewer_private_export.h"
#include "remote-content/remotecontentinfo.h"
namespace MessageViewer
{
class RemoteContentWidget;
/**
 * \class MessageViewer::RemoteContentDialog
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/RemoteContentDialog
 * \brief The RemoteContentDialog class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoteContentDialog(QWidget *parent = nullptr);
    ~RemoteContentDialog() override;

    [[nodiscard]] RemoteContentInfo info() const;
    void setInfo(const RemoteContentInfo &info);

private:
    RemoteContentWidget *const mRemoveContentWidget;
};
}
