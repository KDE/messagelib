/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#ifndef TABLOCKEDWARNING_H
#define TABLOCKEDWARNING_H


#include <KMessageWidget>

namespace MessageViewer {
class TabLockedWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit TabLockedWarning(QWidget *parent = nullptr);
    ~TabLockedWarning() override;
};
}

#endif // TABLOCKEDWARNING_H
