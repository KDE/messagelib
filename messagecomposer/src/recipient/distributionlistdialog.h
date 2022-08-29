/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <MessageComposer/Recipient>
#include <QDialog>

class KJob;
class QLineEdit;
class QTreeWidget;
class QPushButton;

namespace MessageComposer
{
class DistributionListDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DistributionListDialog(QWidget *parent);
    ~DistributionListDialog() override;
    void setRecipients(const Recipient::List &);

public Q_SLOTS:
    void slotUser1();
    void slotTitleChanged(const QString &);

private:
    void slotDelayedSetRecipients(KJob *);
    void slotDelayedUser1(KJob *);
    void slotContactGroupCreateJobResult(KJob *);
    void readConfig();
    void writeConfig();

private:
    QLineEdit *mTitleEdit = nullptr;
    QTreeWidget *mRecipientsList = nullptr;
    QPushButton *mUser1Button = nullptr;
};
}
