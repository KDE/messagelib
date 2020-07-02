/*
    partnodebodypart.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#ifndef MIMETREEPARSER_PARTNODEBODYPART_H
#define MIMETREEPARSER_PARTNODEBODYPART_H

#include "mimetreeparser_export.h"
#include "mimetreeparser/bodypart.h"

namespace KMime {
class Content;
}

namespace MimeTreeParser {
class NodeHelper;
}

namespace MimeTreeParser {
/**
    @short an implementation of the BodyPart interface using KMime::Content's
*/
class MIMETREEPARSER_EXPORT PartNodeBodyPart : public Interface::BodyPart
{
public:
    explicit PartNodeBodyPart(ObjectTreeParser *otp, ProcessResult *result, KMime::Content *topLevelContent, KMime::Content *content, NodeHelper *nodeHelper);

    Interface::BodyPartMemento *memento() const override;
    KMime::Content *content() const override;
    KMime::Content *topLevelContent() const override;
    ProcessResult *processResult() const override;

    NodeHelper *nodeHelper() const override;
    ObjectTreeParser *objectTreeParser() const override;
    Interface::ObjectTreeSource *source() const override;
private:
    KMime::Content *mTopLevelContent = nullptr;
    KMime::Content *mContent = nullptr;
    NodeHelper *mNodeHelper = nullptr;
    ObjectTreeParser *mObjectTreeParser = nullptr;
    ProcessResult *mProcessResult = nullptr;
};
}

#endif // MIMETREEPARSER_PARTNODEBODYPART_H
