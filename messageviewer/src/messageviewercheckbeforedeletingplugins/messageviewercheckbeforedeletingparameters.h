/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <QString>
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingParametersPrivate;
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingParameters
{
public:
    MessageViewerCheckBeforeDeletingParameters();
    MessageViewerCheckBeforeDeletingParameters(const MessageViewerCheckBeforeDeletingParameters &other);
    ~MessageViewerCheckBeforeDeletingParameters();

    enum DeleteType {
        Unknown = 0,
        MoveToTrash = 1,
        Delete = 2,
    };

    Q_REQUIRED_RESULT MessageViewerCheckBeforeDeletingParameters::DeleteType deleteType() const;
    void setDeleteType(MessageViewerCheckBeforeDeletingParameters::DeleteType type);

    MessageViewerCheckBeforeDeletingParameters &operator=(const MessageViewerCheckBeforeDeletingParameters &other);
    Q_REQUIRED_RESULT bool operator==(const MessageViewerCheckBeforeDeletingParameters &other) const;

private:
    MessageViewerCheckBeforeDeletingParametersPrivate *const d;
};
}
