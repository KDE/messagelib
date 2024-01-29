/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailman.h"

#include "utils.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const MailmanBodyPartFormatter *MailmanBodyPartFormatter::self;

const Interface::BodyPartFormatter *MailmanBodyPartFormatter::create()
{
    if (!self) {
        self = new MailmanBodyPartFormatter();
    }
    return self;
}

bool MailmanBodyPartFormatter::isMailmanMessage(KMime::Content *curNode) const
{
    if (!curNode || curNode->head().isEmpty()) {
        return false;
    }
    if (curNode->hasHeader("X-Mailman-Version")) {
        return true;
    }
    if (KMime::Headers::Base *header = curNode->headerByType("X-Mailer")) {
        if (header->asUnicodeString().contains(QLatin1StringView("MAILMAN"), Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

MessagePart::Ptr MailmanBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *curNode = part.content();

    if (!isMailmanMessage(curNode)) {
        return {};
    }

    // Latin1 or utf8 ?
    const QString str = QString::fromLatin1(curNode->decodedContent());

    // ###
    const QLatin1StringView delim1("--__--__--\n\nMessage:");
    const QLatin1StringView delim2("--__--__--\r\n\r\nMessage:");
    const QLatin1StringView delimZ2("--__--__--\n\n_____________");
    const QLatin1StringView delimZ1("--__--__--\r\n\r\n_____________");
    QString partStr;
    QString digestHeaderStr;
    int thisDelim = str.indexOf(delim1, Qt::CaseInsensitive);
    if (thisDelim == -1) {
        thisDelim = str.indexOf(delim2, Qt::CaseInsensitive);
    }
    if (thisDelim == -1) {
        return {};
    }

    int nextDelim = str.indexOf(delim1, thisDelim + 1, Qt::CaseInsensitive);
    if (-1 == nextDelim) {
        nextDelim = str.indexOf(delim2, thisDelim + 1, Qt::CaseInsensitive);
    }
    if (-1 == nextDelim) {
        nextDelim = str.indexOf(delimZ1, thisDelim + 1, Qt::CaseInsensitive);
    }
    if (-1 == nextDelim) {
        nextDelim = str.indexOf(delimZ2, thisDelim + 1, Qt::CaseInsensitive);
    }
    if (nextDelim < 0) {
        return {};
    }

    // if ( curNode->mRoot )
    //  curNode = curNode->mRoot;

    // at least one message found: build a mime tree
    digestHeaderStr = QStringLiteral("Content-Type: text/plain\nContent-Description: digest header\n\n");
    digestHeaderStr += QStringView(str).mid(0, thisDelim);

    MessagePartList::Ptr mpl(new MessagePartList(part.objectTreeParser()));
    mpl->appendSubPart(createAndParseTempNode(part, part.topLevelContent(), digestHeaderStr.toLatin1().constData(), "Digest Header"));
    // mReader->queueHtml("<br><hr><br>");
    // temporarily change current node's Content-Type
    // to get our embedded RfC822 messages properly inserted
    curNode->contentType()->setMimeType("multipart/digest");
    while (-1 < nextDelim) {
        int thisEoL = str.indexOf(QLatin1StringView("\nMessage:"), thisDelim, Qt::CaseInsensitive);
        if (-1 < thisEoL) {
            thisDelim = thisEoL + 1;
        } else {
            thisEoL = str.indexOf(QLatin1StringView("\n_____________"), thisDelim, Qt::CaseInsensitive);
            if (-1 < thisEoL) {
                thisDelim = thisEoL + 1;
            }
        }
        thisEoL = str.indexOf(QLatin1Char('\n'), thisDelim);
        if (-1 < thisEoL) {
            thisDelim = thisEoL + 1;
        } else {
            thisDelim = thisDelim + 1;
        }
        // while( thisDelim < cstr.size() && '\n' == cstr[thisDelim] )
        //  ++thisDelim;

        partStr = QStringLiteral("Content-Type: message/rfc822\nContent-Description: embedded message\n\n");
        partStr += QStringView(str).mid(thisDelim, nextDelim - thisDelim);
        QString subject = QStringLiteral("embedded message");
        QString subSearch = QStringLiteral("\nSubject:");
        int subPos = partStr.indexOf(subSearch, 0, Qt::CaseInsensitive);
        if (-1 < subPos) {
            subject = partStr.mid(subPos + subSearch.length());
            thisEoL = subject.indexOf(QLatin1Char('\n'));
            if (-1 < thisEoL) {
                subject.truncate(thisEoL);
            }
        }
        qCDebug(MIMETREEPARSER_LOG) << "        embedded message found: \"" << subject;
        mpl->appendSubPart(createAndParseTempNode(part, part.topLevelContent(), partStr.toLatin1().constData(), subject.toLatin1().constData()));
        // mReader->queueHtml("<br><hr><br>");
        thisDelim = nextDelim + 1;
        nextDelim = str.indexOf(delim1, thisDelim, Qt::CaseInsensitive);
        if (-1 == nextDelim) {
            nextDelim = str.indexOf(delim2, thisDelim, Qt::CaseInsensitive);
        }
        if (-1 == nextDelim) {
            nextDelim = str.indexOf(delimZ1, thisDelim, Qt::CaseInsensitive);
        }
        if (-1 == nextDelim) {
            nextDelim = str.indexOf(delimZ2, thisDelim, Qt::CaseInsensitive);
        }
    }
    // reset current node's Content-Type
    curNode->contentType()->setMimeType("text/plain");
    int thisEoL = str.indexOf(QLatin1StringView("_____________"), thisDelim);
    if (-1 < thisEoL) {
        thisDelim = thisEoL;
        thisEoL = str.indexOf(QLatin1Char('\n'), thisDelim);
        if (-1 < thisEoL) {
            thisDelim = thisEoL + 1;
        }
    } else {
        thisDelim = thisDelim + 1;
    }
    partStr = QStringLiteral("Content-Type: text/plain\nContent-Description: digest footer\n\n");
    partStr += QStringView(str).mid(thisDelim);
    mpl->appendSubPart(createAndParseTempNode(part, part.topLevelContent(), partStr.toLatin1().constData(), "Digest Footer"));
    return mpl;
}
