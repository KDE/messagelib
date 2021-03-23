/*
  SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <PimCommon/PurposeMenuWidget>
namespace MessageViewer
{
class MailWebEngineView;
class MailfilterPurposeMenuWidget : public PimCommon::PurposeMenuWidget
{
    Q_OBJECT
public:
    explicit MailfilterPurposeMenuWidget(QWidget *parentWidget, QObject *parent = nullptr);
    ~MailfilterPurposeMenuWidget() override;

    Q_REQUIRED_RESULT QByteArray text() override;
    void setViewer(MailWebEngineView *viewer);

private:
    MailWebEngineView *mViewer = nullptr;
};
}

