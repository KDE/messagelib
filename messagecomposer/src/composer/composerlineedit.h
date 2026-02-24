/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

  Based on kmail/kmlineeditspell.h/cpp
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <PimCommonAkonadi/AddresseeLineEdit>
namespace MessageComposer
{
/*!
 * \class MessageComposer::ComposerLineEdit
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/ComposerLineEdit
 * \brief The ComposerLineEdit class
 */
class MESSAGECOMPOSER_EXPORT ComposerLineEdit : public PimCommon::AddresseeLineEdit
{
    Q_OBJECT

public:
    /*! \brief Constructs a ComposerLineEdit.
        \param useCompletion Whether to enable address completion.
        \param parent The parent widget.
    */
    explicit ComposerLineEdit(bool useCompletion, QWidget *parent = nullptr);
    /*! \brief Destroys the ComposerLineEdit. */
    ~ComposerLineEdit() override;

Q_SIGNALS:
    /*! \brief Emitted when the user presses up/previous to focus the previous input. */
    void focusUp();
    /*! \brief Emitted when the user presses down/next to focus the next input. */
    void focusDown();

protected:
    /*! \brief Handles key press events for focus navigation.
        \param event The key press event.
    */
    void keyPressEvent(QKeyEvent *) override;
};
}
