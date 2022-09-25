/*
    partnodebodypart.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "partnodebodypart.h"
#include "nodehelper.h"
#include "objecttreeparser.h"
#include <KMime/Content>

using namespace MimeTreeParser;

PartNodeBodyPart::PartNodeBodyPart(ObjectTreeParser *otp,
                                   ProcessResult *result,
                                   KMime::Content *topLevelContent,
                                   KMime::Content *content,
                                   NodeHelper *nodeHelper)
    : Interface::BodyPart()
    , mTopLevelContent(topLevelContent)
    , mContent(content)
    , mNodeHelper(nodeHelper)
    , mObjectTreeParser(otp)
    , mProcessResult(result)
{
}

Interface::BodyPartMemento *PartNodeBodyPart::memento() const
{
    /*TODO(Andras) Volker suggests to use a ContentIndex->Mememnto mapping
    Also review if the reader's bodyPartMemento should be returned or the NodeHelper's one
    */
    return mNodeHelper->bodyPartMemento(mContent, "__plugin__");
}

Interface::ObjectTreeSource *PartNodeBodyPart::source() const
{
    return mObjectTreeParser->mSource;
}

KMime::Content *PartNodeBodyPart::content() const
{
    return mContent;
}

KMime::Content *PartNodeBodyPart::topLevelContent() const
{
    return mTopLevelContent;
}

NodeHelper *PartNodeBodyPart::nodeHelper() const
{
    return mNodeHelper;
}

ObjectTreeParser *PartNodeBodyPart::objectTreeParser() const
{
    return mObjectTreeParser;
}

ProcessResult *PartNodeBodyPart::processResult() const
{
    return mProcessResult;
}
