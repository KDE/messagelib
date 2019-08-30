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

#ifndef MIMETREEPARSER_HTMLBLOCK_H
#define MIMETREEPARSER_HTMLBLOCK_H

#include "messageviewer_export.h"

#include <QString>
#include <QSharedPointer>

namespace KMime {
class Content;
}

namespace MessageViewer {
class HtmlWriter;
/**
 * @brief The HTMLBlock class
 */
class MESSAGEVIEWER_EXPORT HTMLBlock
{
public:
    typedef QSharedPointer<HTMLBlock> Ptr;

    HTMLBlock();

    virtual ~HTMLBlock();

    Q_REQUIRED_RESULT QString enter();
    Q_REQUIRED_RESULT QString exit();
protected:
    QString dir() const;
    virtual QString enterString() const = 0;
    virtual QString exitString() const = 0;

private:
    bool entered = false;
};

// The attachment mark is a div that is placed around the attachment. It is used for drawing
// a yellow border around the attachment when scrolling to it. When scrolling to it, the border
// color of the div is changed, see KMReaderWin::scrollToAttachment().
/**
 * @brief The AttachmentMarkBlock class
 */
class MESSAGEVIEWER_EXPORT AttachmentMarkBlock : public HTMLBlock
{
public:
    AttachmentMarkBlock(HtmlWriter *writer, KMime::Content *node);
    ~AttachmentMarkBlock() override;

protected:
    QString enterString() const override;
    QString exitString() const override;

private:
    void internalEnter();
    void internalExit();

    KMime::Content *mNode = nullptr;
    HtmlWriter *mWriter = nullptr;
};

// Make sure the whole content is relative, so that nothing is painted over the header
// if a malicious message uses absolute positioning.
// Also force word wrapping, which is useful for printing, see https://issues.kolab.org/issue3992.
class RootBlock : public HTMLBlock
{
public:
    explicit RootBlock(HtmlWriter *writer);
    ~RootBlock() override;

protected:
    QString enterString() const override;
    QString exitString() const override;

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter = nullptr;
};
}
#endif //__MIMETREEPARSER_HTMLBLOCK_H
