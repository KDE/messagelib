/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkim-verify/dkimchecksignaturejob.h"
#include <QObject>

class CheckSignature : public QObject
{
    Q_OBJECT
public:
    explicit CheckSignature(const QString &fileName, QObject *parent = nullptr);
    ~CheckSignature() override;

private:
    void slotResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
};
