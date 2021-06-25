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
};

MessageViewerCheckBeforeDeletingParameters::MessageViewerCheckBeforeDeletingParameters()
    : d(new MessageViewer::MessageViewerCheckBeforeDeletingParametersPrivate)
{
}

MessageViewerCheckBeforeDeletingParameters::~MessageViewerCheckBeforeDeletingParameters()
{
}
