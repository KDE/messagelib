/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "../createphishingurldatabasejob.h"
#include <QWidget>
class QPlainTextEdit;
class QComboBox;
class CreatePhisingUrlDataBaseGui : public QWidget
{
    Q_OBJECT
public:
    explicit CreatePhisingUrlDataBaseGui(QWidget *parent = nullptr);
    ~CreatePhisingUrlDataBaseGui() override;
private Q_SLOTS:
    void slotResult(const QByteArray &data);
    void slotDownloadFullDatabase();
    void slotDebugJSon(const QByteArray &data);
    void slotDownloadPartialDatabase();
    void slotSaveResultToDisk();

private:
    WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType compressionType();
    void clear();
    QPlainTextEdit *mResult;
    QPlainTextEdit *mJson;
    QComboBox *mCompressionType;
};
