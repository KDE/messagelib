/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef COMPOSERVIEWINTERFACE_H
#define COMPOSERVIEWINTERFACE_H
#include "messagecomposer_export.h"
#include <MessageComposer/ComposerAttachmentInterface>
#include <QString>

namespace MessageComposer {
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
    Q_REQUIRED_RESULT QString from() const;

    Q_REQUIRED_RESULT MessageComposer::ComposerAttachmentInterface attachments();

    Q_REQUIRED_RESULT QString convertVariableToString(const QString &originalText);
    Q_REQUIRED_RESULT QString shortDate() const;
    Q_REQUIRED_RESULT QString longDate() const;
    Q_REQUIRED_RESULT QString shortTime() const;
    Q_REQUIRED_RESULT QString longTime() const;
    Q_REQUIRED_RESULT QString insertDayOfWeek() const;
private:
    ComposerViewBase *mComposerView = nullptr;
};
}

#endif // COMPOSERVIEWINTERFACE_H
