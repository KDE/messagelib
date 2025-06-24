/*
   SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "htmlblock.h"
using namespace Qt::Literals::StringLiterals;

#include "interfaces/htmlwriter.h"

#include <KMime/Content>

#include <QApplication>

using namespace MessageViewer;

HTMLBlock::HTMLBlock() = default;

HTMLBlock::~HTMLBlock() = default;

QString HTMLBlock::dir() const
{
    return QApplication::isRightToLeft() ? u"rtl"_s : u"ltr"_s;
}

QString HTMLBlock::enter()
{
    if (!entered) {
        entered = true;
        return enterString();
    }
    return {};
}

QString HTMLBlock::exit()
{
    if (entered) {
        entered = false;
        return exitString();
    }
    return {};
}

AttachmentMarkBlock::AttachmentMarkBlock(HtmlWriter *writer, KMime::Content *node)
    : mNode(node)
    , mWriter(writer)
{
    internalEnter();
}

AttachmentMarkBlock::~AttachmentMarkBlock()
{
    internalExit();
}

void AttachmentMarkBlock::internalEnter()
{
    if (mWriter) {
        mWriter->write(enter());
    }
}

void AttachmentMarkBlock::internalExit()
{
    if (mWriter) {
        mWriter->write(exit());
    }
}

QString AttachmentMarkBlock::enterString() const
{
    const QString index = mNode->index().toString();
    return u"<a name=\"att%1\"></a>"_s.arg(index) + u"<div id=\"attachmentDiv%1\">\n"_s.arg(index);
}

QString AttachmentMarkBlock::exitString() const
{
    return u"</div>"_s;
}

RootBlock::RootBlock(HtmlWriter *writer)
    : mWriter(writer)
{
    internalEnter();
}

RootBlock::~RootBlock()
{
    internalExit();
}

void RootBlock::internalEnter()
{
    if (mWriter) {
        mWriter->write(enter());
    }
}

void RootBlock::internalExit()
{
    if (mWriter) {
        mWriter->write(exit());
    }
}

QString RootBlock::enterString() const
{
    return u"<div style=\"position: relative; word-wrap: break-word\">\n"_s;
}

QString RootBlock::exitString() const
{
    return u"</div>\n"_s;
}
