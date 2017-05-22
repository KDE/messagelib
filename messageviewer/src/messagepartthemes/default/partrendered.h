/*
   Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

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

#ifndef __MESSAGEVIEWER_PARTRENDERED_H__
#define __MESSAGEVIEWER_PARTRENDERED_H__

#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QVector>

namespace MimeTreeParser
{
    class MessagePart;
    typedef QSharedPointer<MessagePart> MessagePartPtr;
    class DefaultRendererPrivate;
}

namespace KMime
{
    class Content;
}

class CacheHtmlWriter;

class PartRendered
{
public:
    PartRendered();
    virtual ~PartRendered();

    inline QString alignText();
    virtual QString html() = 0;
    virtual QMap<QByteArray, QString> embededParts() = 0;
    virtual QString extraHeader() = 0;

protected:
    QVector<QSharedPointer<PartRendered>> renderSubParts(MimeTreeParser::MessagePartPtr mp);
};

class WrapperPartRendered : public PartRendered
{
public:
    WrapperPartRendered(CacheHtmlWriter *);
    virtual ~WrapperPartRendered();

    QString html() Q_DECL_OVERRIDE;
    QMap<QByteArray, QString> embededParts() Q_DECL_OVERRIDE;
    QString extraHeader() Q_DECL_OVERRIDE;

private:
    QString mHtml;
    QString mHead;
    QMap<QByteArray, QString> mEmbeded;
};

class HtmlOnlyPartRendered : public PartRendered
{
public:
    HtmlOnlyPartRendered(MimeTreeParser::MessagePartPtr part, const QString &html);
    virtual ~HtmlOnlyPartRendered();

    QString html() Q_DECL_OVERRIDE;
    QMap<QByteArray, QString> embededParts() Q_DECL_OVERRIDE;
    QString extraHeader() Q_DECL_OVERRIDE;

protected:
    void setHtml(const QString &html);

private:
    QString mHtml;
    bool mShowAttachmentBlock;
    KMime::Content *mAttachmentNode;
};

#endif
