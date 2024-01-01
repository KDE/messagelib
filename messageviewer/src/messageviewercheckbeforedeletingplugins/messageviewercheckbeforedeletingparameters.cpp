/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingparameters.h"
using namespace MessageViewer;

class MessageViewer::MessageViewerCheckBeforeDeletingParametersPrivate
{
public:
    MessageViewerCheckBeforeDeletingParametersPrivate() = default;
    Akonadi::Item::List mList;
    MessageViewerCheckBeforeDeletingParameters::DeleteType mType = MessageViewerCheckBeforeDeletingParameters::Unknown;
};

MessageViewerCheckBeforeDeletingParameters::MessageViewerCheckBeforeDeletingParameters()
    : d(new MessageViewer::MessageViewerCheckBeforeDeletingParametersPrivate)
{
}

MessageViewerCheckBeforeDeletingParameters::MessageViewerCheckBeforeDeletingParameters(const MessageViewerCheckBeforeDeletingParameters &other)
    : d(new MessageViewer::MessageViewerCheckBeforeDeletingParametersPrivate)
{
    (*this) = other;
}

MessageViewerCheckBeforeDeletingParameters::~MessageViewerCheckBeforeDeletingParameters() = default;

Akonadi::Item::List MessageViewerCheckBeforeDeletingParameters::items() const
{
    return d->mList;
}

void MessageViewerCheckBeforeDeletingParameters::setItems(const Akonadi::Item::List &items)
{
    d->mList = items;
}

MessageViewerCheckBeforeDeletingParameters::DeleteType MessageViewerCheckBeforeDeletingParameters::deleteType() const
{
    return d->mType;
}

void MessageViewerCheckBeforeDeletingParameters::setDeleteType(DeleteType type)
{
    d->mType = type;
}

MessageViewerCheckBeforeDeletingParameters &MessageViewerCheckBeforeDeletingParameters::operator=(const MessageViewerCheckBeforeDeletingParameters &other)
{
    if (this != &other) {
        d->mType = other.deleteType();
        d->mList = other.items();
    }
    return *this;
}

bool MessageViewerCheckBeforeDeletingParameters::operator==(const MessageViewerCheckBeforeDeletingParameters &other) const
{
    return (d->mType == other.deleteType()) && (d->mList == other.items());
}

bool MessageViewerCheckBeforeDeletingParameters::isValid() const
{
    return !d->mList.isEmpty() && (d->mType != Unknown);
}
