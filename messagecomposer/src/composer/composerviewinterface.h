/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <MessageComposer/ComposerAttachmentInterface>
#include <QString>

namespace MessageComposer
{
class ComposerViewBase;
/**
 * @brief The ComposerViewInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ComposerViewInterface
{
public:
    explicit ComposerViewInterface(ComposerViewBase *composerView);
    ~ComposerViewInterface();

    [[nodiscard]] QString subject() const;
    [[nodiscard]] QString to() const;
    [[nodiscard]] QString cc() const;
    [[nodiscard]] QString bcc() const;
    [[nodiscard]] QString from() const;
    [[nodiscard]] QString replyTo() const;

    [[nodiscard]] MessageComposer::ComposerAttachmentInterface attachments() const;

    [[nodiscard]] QString shortDate() const;
    [[nodiscard]] QString longDate() const;
    [[nodiscard]] QString shortTime() const;
    [[nodiscard]] QString longTime() const;
    [[nodiscard]] QString insertDayOfWeek() const;

private:
    ComposerViewBase *const mComposerView;
};
}
