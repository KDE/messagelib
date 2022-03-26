/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <MimeTreeParser/MessagePart>

#include <map>
#include <memory>

#include <QSharedPointer>

namespace MessageViewer
{
class CSSHelperBase;
class HtmlWriter;
/**
 * @brief The RenderContext class
 */
class MESSAGEVIEWER_EXPORT RenderContext
{
public:
    virtual ~RenderContext();

    virtual CSSHelperBase *cssHelper() const = 0;
    template<typename T>
    inline bool renderWithFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
    {
        return renderWithFactory(&T::staticMetaObject, msgPart, writer);
    }

    inline bool renderWithFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
    {
        return renderWithFactory(msgPart->metaObject(), msgPart, writer);
    }

    virtual void renderSubParts(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) = 0;

    Q_REQUIRED_RESULT virtual bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) = 0;
    Q_REQUIRED_RESULT virtual MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) = 0;
    Q_REQUIRED_RESULT virtual bool showEmoticons() const = 0;
    Q_REQUIRED_RESULT virtual bool isPrinting() const = 0;
    Q_REQUIRED_RESULT virtual bool htmlLoadExternal() const = 0;
    Q_REQUIRED_RESULT virtual bool showExpandQuotesMark() const = 0;
    Q_REQUIRED_RESULT virtual bool showOnlyOneMimePart() const = 0;
    Q_REQUIRED_RESULT virtual bool showSignatureDetails() const = 0;
    Q_REQUIRED_RESULT virtual bool showEncryptionDetails() const = 0;
    Q_REQUIRED_RESULT virtual int levelQuote() const = 0;

protected:
    Q_REQUIRED_RESULT virtual bool renderWithFactory(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer) = 0;
};
/**
 * @brief The MessagePartRendererBase class
 */
class MESSAGEVIEWER_EXPORT MessagePartRendererBase
{
public:
    MessagePartRendererBase();
    virtual ~MessagePartRendererBase();
    Q_REQUIRED_RESULT virtual bool render(const MimeTreeParser::MessagePart::Ptr &, HtmlWriter *htmlWriter, RenderContext *context) const = 0;
};
}
