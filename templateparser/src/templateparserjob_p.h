/*
   SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparserextracthtmlinforesult.h"
using namespace Qt::Literals::StringLiterals;

#include "templateparserjob.h"

#include <KMime/Message>

namespace MimeTreeParser
{
class ObjectTreeParser;
class SimpleObjectTreeSource;
}

namespace KIdentityManagementCore
{
class IdentityManager;
}

namespace TemplateParser
{
class TemplateParserJobPrivate
{
public:
    TemplateParserJobPrivate(const std::shared_ptr<KMime::Message> &amsg, const TemplateParserJob::Mode amode);
    ~TemplateParserJobPrivate();
    void setAllowDecryption(const bool allowDecryption);

    std::shared_ptr<KMime::Message> mMsg; // Msg to write to
    std::shared_ptr<KMime::Message> mOrigMsg; // Msg to read from
    KIdentityManagementCore::IdentityManager *m_identityManager = nullptr;
    MimeTreeParser::ObjectTreeParser *mOtp = nullptr;
    MimeTreeParser::SimpleObjectTreeSource *mEmptySource = nullptr;
    QString mSelection;
    QString mQuoteString = u"> "_s;
    QString mTo;
    QString mCC;
    QString mHeadElement;
    TemplateParserJob::Quotes mQuotes = TemplateParserJob::ReplyAsOriginalMessage;
    TemplateParserJob::Mode mMode;
    TemplateParserExtractHtmlInfoResult mExtractHtmlInfoResult;
    qint64 mFolder = -1; // Used to find a template
    uint mIdentity = 0;
    int mColWrap = 80;
    bool mForceCursorPosition = false;
    bool mAllowDecryption = true;
    bool mDebug = false;
    bool mWrap = true;
    bool mReplyAsHtml = false;
};
}
