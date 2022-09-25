/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartalternative.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

using namespace MimeTreeParser;

const MultiPartAlternativeBodyPartFormatter *MultiPartAlternativeBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartAlternativeBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartAlternativeBodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr MultiPartAlternativeBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    if (node->contents().isEmpty()) {
        return {};
    }

    auto preferredMode = part.source()->preferredMode();
    AlternativeMessagePart::Ptr mp(new AlternativeMessagePart(part.objectTreeParser(), node, preferredMode));
    if (mp->childParts().isEmpty()) {
        MimeMessagePart::Ptr _mp(new MimeMessagePart(part.objectTreeParser(), node->contents().at(0), false));
        return _mp;
    }

    KMime::Content *dataIcal = mp->childParts().contains(Util::MultipartIcal) ? mp->childParts()[Util::MultipartIcal]->content() : nullptr;
    KMime::Content *dataHtml = mp->childParts().contains(Util::MultipartHtml) ? mp->childParts()[Util::MultipartHtml]->content() : nullptr;
    KMime::Content *dataPlain = mp->childParts().contains(Util::MultipartPlain) ? mp->childParts()[Util::MultipartPlain]->content() : nullptr;

    // Make sure that in default ical is preferred over html and plain text
    if (dataIcal && ((preferredMode != Util::MultipartHtml && preferredMode != Util::MultipartPlain))) {
        if (dataHtml) {
            part.nodeHelper()->setNodeProcessed(dataHtml, false);
        }
        if (dataPlain) {
            part.nodeHelper()->setNodeProcessed(dataPlain, false);
        }
        preferredMode = Util::MultipartIcal;
    } else if ((dataHtml && (preferredMode == Util::MultipartHtml || preferredMode == Util::Html)) || (dataHtml && dataPlain && dataPlain->body().isEmpty())) {
        if (dataPlain) {
            part.nodeHelper()->setNodeProcessed(dataPlain, false);
        }
        preferredMode = Util::MultipartHtml;
    } else if (!(preferredMode == Util::MultipartHtml) && dataPlain) {
        part.nodeHelper()->setNodeProcessed(dataHtml, false);
        preferredMode = Util::MultipartPlain;
    }

    //    qDebug() << " MessagePart::Ptr MultiPartAlternativeBodyPartFormatter::process(Interface::BodyPart &part) const";
    //    for (int i = 0; i < mp->availableModes().count(); ++i) {
    //        qDebug() << "MultiPartAlternativeBodyPartFormatter::processed Modes " << MimeTreeParser::Util::htmlModeToString(mp->availableModes().at(i));
    //    }
    //    qDebug() << "MultiPartAlternativeBodyPartFormatter::process preferred " << MimeTreeParser::Util::htmlModeToString(preferredMode);

    part.source()->setHtmlMode(preferredMode, mp->availableModes());
    mp->setPreferredMode(preferredMode);
    return mp;
}
