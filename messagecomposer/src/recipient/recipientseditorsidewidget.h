/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    Refactored from earlier code by:
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "recipientseditor.h"

class QLabel;
class QPushButton;

namespace MessageComposer
{
class KWindowPositioner;

class RecipientsPicker;

class RecipientsEditorSideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecipientsEditorSideWidget(RecipientsEditor *editor, QWidget *parent);
    ~RecipientsEditorSideWidget() override;

    [[nodiscard]] MessageComposer::RecipientsPicker *picker() const;

public Q_SLOTS:
    void setTotal(int recipients, int lines);
    void setFocus();
    void updateTotalToolTip();
    void pickRecipient();

Q_SIGNALS:
    void pickedRecipient(const Recipient &, bool &);
    void saveDistributionList();

private:
    RecipientsEditor *const mEditor;
    QLabel *const mTotalLabel;
    QPushButton *const mDistributionListButton;
    QPushButton *const mSelectButton;
    /** The RecipientsPicker is lazy loaded, never access it directly,
      only through picker() */
    mutable MessageComposer::RecipientsPicker *mRecipientPicker = nullptr;
    /** lazy loaded, don't access directly, unless you've called picker() */
    mutable MessageComposer::KWindowPositioner *mPickerPositioner = nullptr;
};
}
