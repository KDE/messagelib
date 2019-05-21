/*
   Copyright (C) 2017 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TEMPLATEPARSERJOB_P_H
#define TEMPLATEPARSERJOB_P_H

#include "templateparserjob.h"
#include "templateparserextracthtmlinforesult.h"

#include <KMime/Message>

namespace MimeTreeParser {
class ObjectTreeParser;
class SimpleObjectTreeSource;
}

namespace KIdentityManagement {
class IdentityManager;
}

namespace MessageCore {
class ImageCollector;
}

namespace TemplateParser {
class TemplateParserJobPrivate
{
public:
    TemplateParserJobPrivate(const KMime::Message::Ptr &amsg, const TemplateParserJob::Mode amode);
    ~TemplateParserJobPrivate();
    void setAllowDecryption(const bool allowDecryption);

    KMime::Message::Ptr mMsg;             // Msg to write to
    KMime::Message::Ptr mOrigMsg;         // Msg to read from
    KIdentityManagement::IdentityManager *m_identityManager = nullptr;
    MimeTreeParser::ObjectTreeParser *mOtp = nullptr;
    MimeTreeParser::SimpleObjectTreeSource *mEmptySource = nullptr;
    QString mSelection;
    QString mQuoteString = QStringLiteral("> ");
    QString mTo;
    QString mCC;
    QString mHeadElement;
    QStringList mCharsets;
    TemplateParserJob::Quotes mQuotes = TemplateParserJob::ReplyAsOriginalMessage;
    TemplateParserJob::Mode mMode;
    TemplateParserExtractHtmlInfoResult mExtractHtmlInfoResult;
    qint64 mFolder = -1;          //Used to find a template
    uint mIdentity = 0;
    int mColWrap = 80;
    bool mForceCursorPosition = false;
    bool mAllowDecryption = true;
    bool mDebug = false;
    bool mWrap = true;
};
}

#endif
