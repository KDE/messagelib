/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#pragma once

#include "messagelist_private_export.h"
#include <KMessageWidget>

namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT TabLockedWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit TabLockedWarning(QWidget *parent = nullptr);
    ~TabLockedWarning() override;
Q_SIGNALS:
    void unlockTabRequested();

private:
    void slotLinkActivated(const QString &contents);
};
}
}

