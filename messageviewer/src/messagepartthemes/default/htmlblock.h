/*
   SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QSharedPointer>
#include <QString>

namespace KMime
{
class Content;
}

namespace MessageViewer
{
class HtmlWriter;
/**
 * @brief The HTMLBlock class
 */
class MESSAGEVIEWER_EXPORT HTMLBlock
{
public:
    using Ptr = QSharedPointer<HTMLBlock>;

    HTMLBlock();

    virtual ~HTMLBlock();

    [[nodiscard]] QString enter();
    [[nodiscard]] QString exit();

protected:
    [[nodiscard]] QString dir() const;
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
    [[nodiscard]] QString enterString() const override;
    [[nodiscard]] QString exitString() const override;

private:
    void internalEnter();
    void internalExit();

    KMime::Content *const mNode;
    HtmlWriter *const mWriter;
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
    [[nodiscard]] QString enterString() const override;
    [[nodiscard]] QString exitString() const override;

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter = nullptr;
};
}
