/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHECKSIGNATURE_H
#define CHECKSIGNATURE_H

#include "dkim-verify/dkimchecksignaturejob.h"
#include <QObject>
#include <QtCrypto>

class CheckSignature : public QObject
{
    Q_OBJECT
public:
    explicit CheckSignature(const QString &fileName, QObject *parent = nullptr);
    ~CheckSignature();

private:
    void slotResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    QCA::Initializer *mQcaInitializer = nullptr;
};

#endif // CHECKSIGNATURE_H
