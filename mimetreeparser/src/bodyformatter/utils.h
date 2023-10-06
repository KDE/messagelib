/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "interfaces/bodypart.h"
#include "messagepart.h"

#include <KMime/Content>

namespace MimeTreeParser
{
/**
  1. Create a new partNode using 'content' data and Content-Description
     found in 'cntDesc'.
  2. Parse the 'node' to display the content.
*/
[[nodiscard]] MimeMessagePart::Ptr createAndParseTempNode(Interface::BodyPart &part, KMime::Content *parentNode, const char *content, const char *cntDesc);

[[nodiscard]] KMime::Content *findTypeInDirectChilds(KMime::Content *content, const QByteArray &mimeType);

[[nodiscard]] MessagePart::Ptr toplevelTextNode(MessagePart::Ptr messageTree);
}
