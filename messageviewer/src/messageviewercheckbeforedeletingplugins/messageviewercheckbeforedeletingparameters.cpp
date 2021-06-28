/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingparameters.h"
using namespace MessageViewer;

class MessageViewer::MessageViewerCheckBeforeDeletingParametersPrivate
{
public:
    MessageViewerCheckBeforeDeletingParametersPrivate()
    {
    }
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

MessageViewerCheckBeforeDeletingParameters::~MessageViewerCheckBeforeDeletingParameters()
{
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
    }
    return *this;
}

bool MessageViewerCheckBeforeDeletingParameters::operator==(const MessageViewerCheckBeforeDeletingParameters &other) const
{
    return (d->mType == other.deleteType());
}
