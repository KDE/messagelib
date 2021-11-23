/*
   SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "htmlblock.h"

#include "interfaces/htmlwriter.h"

#include <KMime/Content>

#include <QApplication>

using namespace MessageViewer;

HTMLBlock::HTMLBlock() = default;

HTMLBlock::~HTMLBlock() = default;

QString HTMLBlock::dir() const
{
    return QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
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
    return QStringLiteral("<a name=\"att%1\"></a>").arg(index) + QStringLiteral("<div id=\"attachmentDiv%1\">\n").arg(index);
}

QString AttachmentMarkBlock::exitString() const
{
    return QStringLiteral("</div>");
}

RootBlock::RootBlock(HtmlWriter *writer)
    : HTMLBlock()
    , mWriter(writer)
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
    return QStringLiteral("<div style=\"position: relative; word-wrap: break-word\">\n");
}

QString RootBlock::exitString() const
{
    return QStringLiteral("</div>\n");
}
