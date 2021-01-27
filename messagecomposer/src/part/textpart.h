/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_TEXTPART_H
#define MESSAGECOMPOSER_TEXTPART_H

#include <QString>

#include "messagecomposer_export.h"
#include "messagepart.h"
#include <KPIMTextEdit/RichTextComposerImages>

namespace MessageComposer
{
/**
 * @brief The TextPart class
 */
class MESSAGECOMPOSER_EXPORT TextPart : public MessagePart
{
    Q_OBJECT

public:
    explicit TextPart(QObject *parent = nullptr);
    ~TextPart() override;

    // default true
    Q_REQUIRED_RESULT bool isWordWrappingEnabled() const;
    void setWordWrappingEnabled(bool enabled);
    // default true
    Q_REQUIRED_RESULT bool warnBadCharset() const;
    void setWarnBadCharset(bool warn);

    Q_REQUIRED_RESULT QString cleanPlainText() const;
    void setCleanPlainText(const QString &text);
    Q_REQUIRED_RESULT QString wrappedPlainText() const;
    void setWrappedPlainText(const QString &text);

    Q_REQUIRED_RESULT bool isHtmlUsed() const;
    Q_REQUIRED_RESULT QString cleanHtml() const;
    void setCleanHtml(const QString &text);

    Q_REQUIRED_RESULT bool hasEmbeddedImages() const;
    Q_REQUIRED_RESULT KPIMTextEdit::ImageList embeddedImages() const;
    void setEmbeddedImages(const KPIMTextEdit::ImageList &images);

private:
    class Private;
    Private *const d;
};
} // namespace MessageComposer

#endif // MESSAGECOMPOSER_TEXTPART_H
