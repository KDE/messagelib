/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/
#pragma once

#include <QWidget>

namespace MessageComposer
{
class RecipientsEditor;
}

class Composer : public QWidget
{
    Q_OBJECT
public:
    explicit Composer(QWidget *parent);

public Q_SLOTS:
    void slotClose();

private:
    MessageComposer::RecipientsEditor *const mRecipients;
};
