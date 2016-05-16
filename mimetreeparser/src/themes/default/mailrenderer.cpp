/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#include "mailrenderer.h"

#include "interfaces/htmlwriter.h"

#include "viewer/htmlblock.h"
#include "viewer/messagepart.h"
#include "viewer/objecttreeparser.h"

using namespace MimeTreeParser;

HtmlRenderer::HtmlRenderer(MessagePartPtr msgPart)
    : mMsgPart(msgPart)
{
    render();
}

const QString& HtmlRenderer::html()
{
    return mHtml;
}

class TestHtmlWriter : public MimeTreeParser::HtmlWriter
{
public:
    explicit TestHtmlWriter(MimeTreeParser::HtmlWriter *baseWriter)
    : mBaseWriter(baseWriter)
    {}
    virtual ~TestHtmlWriter() {}

    void begin(const QString &text) Q_DECL_OVERRIDE {mBaseWriter->begin(text);}
    void write(const QString &str) Q_DECL_OVERRIDE
    {
        html.append(str);
    }
    void end() Q_DECL_OVERRIDE {mBaseWriter->end();}
    void reset() Q_DECL_OVERRIDE {mBaseWriter->reset();}
    void queue(const QString &str) Q_DECL_OVERRIDE
    {
        html.append(str);
    }
    void flush() Q_DECL_OVERRIDE {mBaseWriter->flush();}
    void embedPart(const QByteArray &contentId, const QString &url) Q_DECL_OVERRIDE {mBaseWriter->embedPart(contentId, url);}
    void extraHead(const QString &extra) Q_DECL_OVERRIDE {mBaseWriter->extraHead(extra);}

    QString html;
    MimeTreeParser::HtmlWriter *mBaseWriter;
};

void HtmlRenderer::render()
{
    const auto oldWriter = mMsgPart->htmlWriter();
    TestHtmlWriter htmlWriter(oldWriter);

    if (oldWriter) {
        mMsgPart->setHtmlWriter(&htmlWriter);
        mMsgPart->html(false);
        mMsgPart->setHtmlWriter(oldWriter);
    }

    mHtml = htmlWriter.html;
}
