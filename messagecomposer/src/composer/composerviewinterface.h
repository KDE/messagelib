/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

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

    Q_REQUIRED_RESULT QString subject() const;
    Q_REQUIRED_RESULT QString to() const;
    Q_REQUIRED_RESULT QString cc() const;
    Q_REQUIRED_RESULT QString bcc() const;
    Q_REQUIRED_RESULT QString from() const;

    Q_REQUIRED_RESULT MessageComposer::ComposerAttachmentInterface attachments() const;

    Q_REQUIRED_RESULT QString shortDate() const;
    Q_REQUIRED_RESULT QString longDate() const;
    Q_REQUIRED_RESULT QString shortTime() const;
    Q_REQUIRED_RESULT QString longTime() const;
    Q_REQUIRED_RESULT QString insertDayOfWeek() const;

private:
    ComposerViewBase *const mComposerView;
};
}

