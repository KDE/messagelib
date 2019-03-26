/*
   Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

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

#include "htmlblock.h"

#include "interfaces/htmlwriter.h"

#include <KMime/Content>

#include <QApplication>

using namespace MessageViewer;

HTMLBlock::HTMLBlock()
{
}

HTMLBlock::~HTMLBlock()
{
}

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
    return QString();
}

QString HTMLBlock::exit()
{
    if (entered) {
        entered = false;
        return exitString();
    }
    return QString();
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
    return QStringLiteral("<a name=\"att%1\"></a>").arg(index)
           + QStringLiteral("<div id=\"attachmentDiv%1\">\n").arg(index);
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
