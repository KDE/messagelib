/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingParametersPrivate;
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingParameters
{
public:
    MessageViewerCheckBeforeDeletingParameters();
    ~MessageViewerCheckBeforeDeletingParameters();

private:
    MessageViewerCheckBeforeDeletingParametersPrivate *const d;
};
}
