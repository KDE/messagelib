/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <Akonadi/Item>
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingParametersPrivate;
/** @brief The MessageViewerCheckBeforeDeletingParameters class
 * @author Laurent Montel <montel@kde.org>
 */
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

    [[nodiscard]] MessageViewerCheckBeforeDeletingParameters::DeleteType deleteType() const;
    void setDeleteType(MessageViewerCheckBeforeDeletingParameters::DeleteType type);

    MessageViewerCheckBeforeDeletingParameters &operator=(const MessageViewerCheckBeforeDeletingParameters &other);
    [[nodiscard]] bool operator==(const MessageViewerCheckBeforeDeletingParameters &other) const;

    [[nodiscard]] Akonadi::Item::List items() const;
    void setItems(const Akonadi::Item::List &items);

    [[nodiscard]] bool isValid() const;

private:
    MessageViewerCheckBeforeDeletingParametersPrivate *const d;
};
}
