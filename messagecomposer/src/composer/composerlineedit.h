/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

  Based on kmail/kmlineeditspell.h/cpp
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_COMPOSERLINEEDIT_H
#define MESSAGECOMPOSER_COMPOSERLINEEDIT_H

#include "messagecomposer_export.h"
#include <PimCommonAkonadi/AddresseeLineEdit>
namespace MessageComposer
{
/**
 * @brief The ComposerLineEdit class
 */
class MESSAGECOMPOSER_EXPORT ComposerLineEdit : public PimCommon::AddresseeLineEdit
{
    Q_OBJECT

public:
    explicit ComposerLineEdit(bool useCompletion, QWidget *parent = nullptr);
    ~ComposerLineEdit() override;

Q_SIGNALS:
    void focusUp();
    void focusDown();

protected:
    void keyPressEvent(QKeyEvent *) override;
};
}

#endif
