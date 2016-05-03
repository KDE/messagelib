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

#ifndef __MIMETREEPARSER_HTMLBLOCK_H__
#define __MIMETREEPARSER_HTMLBLOCK_H__

#include "partmetadata.h"
#include <Libkleo/CryptoBackend>

#include <QString>
#include <QSharedPointer>
#include <QVector>

class QTextCodec;

namespace GpgME
{
class ImportResult;
}

namespace KMime
{
class Content;
}

namespace MimeTreeParser
{
class ObjectTreeParser;
class HtmlWriter;
class NodeHelper;

namespace Interface
{
class ObjectTreeSource;
}

class HTMLBlock
{
public:
    typedef QSharedPointer<HTMLBlock> Ptr;

    HTMLBlock()
        : entered(false)
    { }

    virtual ~HTMLBlock();

protected:
    QString dir() const;
    bool entered;
};

class EncapsulatedRFC822Block : public HTMLBlock
{
public:
    EncapsulatedRFC822Block(MimeTreeParser::HtmlWriter *writer, const MimeTreeParser::NodeHelper *nodeHelper, KMime::Content *node);
    virtual ~EncapsulatedRFC822Block();

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter;
    const NodeHelper *mNodeHelper;
    KMime::Content *mNode;
};

class EncryptedBlock : public HTMLBlock
{
public:
    EncryptedBlock(MimeTreeParser::HtmlWriter *writer, const PartMetaData &block);
    virtual ~EncryptedBlock();

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter;
    const PartMetaData &mBlock;
};

class SignedBlock : public HTMLBlock
{
public:
    SignedBlock(MimeTreeParser::HtmlWriter *writer, const PartMetaData &block,
                const Kleo::CryptoBackend::Protocol *cryptoProto,
                Interface::ObjectTreeSource *source,
                QString fromAddress, bool printing);
    virtual ~SignedBlock();

private:
    QString simpleHeader();
    void internalEnter();
    void internalExit();

    const PartMetaData &mBlock;
    HtmlWriter *mWriter;
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    Interface::ObjectTreeSource *mSource;
    QString mClass;
    QString mFromAddress;
    bool mPrinting;
};

class CryptoBlock: public HTMLBlock
{
public:
    CryptoBlock(MimeTreeParser::HtmlWriter *writer,
                PartMetaData *block,
                const Kleo::CryptoBackend::Protocol *cryptoProto,
                Interface::ObjectTreeSource *source,
                const QString &fromAddress);
    virtual ~CryptoBlock();

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter;
    PartMetaData *mMetaData;
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    Interface::ObjectTreeSource *mSource;
    QString mFromAddress;
    QVector<HTMLBlock::Ptr> mInteralBlocks;
};

// The attachment mark is a div that is placed around the attchment. It is used for drawing
// a yellow border around the attachment when scrolling to it. When scrolling to it, the border
// color of the div is changed, see KMReaderWin::scrollToAttachment().
class AttachmentMarkBlock : public HTMLBlock
{
public:
    AttachmentMarkBlock(MimeTreeParser::HtmlWriter *writer, KMime::Content *node);
    virtual ~AttachmentMarkBlock();

private:
    void internalEnter();
    void internalExit();

    KMime::Content *mNode;
    HtmlWriter *mWriter;
};

class TextBlock : public HTMLBlock
{
public:
    TextBlock(MimeTreeParser::HtmlWriter *writer, MimeTreeParser::NodeHelper *nodeHelper, KMime::Content *node, bool link);
    virtual ~TextBlock();
private:
    void internalEnter();
    void internalExit();
private:
    HtmlWriter *mWriter;
    NodeHelper *mNodeHelper;
    KMime::Content *mNode;
    QString mFileName;
    bool mLink;
};

class HTMLWarnBlock : public HTMLBlock
{
public:
    HTMLWarnBlock(MimeTreeParser::HtmlWriter *writer, const QString &msg);
    virtual ~HTMLWarnBlock();
private:
    void internalEnter();
    void internalExit();
private:
    HtmlWriter *mWriter;
    const QString &mMsg;
};

// Make sure the whole content is relative, so that nothing is painted over the header
// if a malicious message uses absolute positioning.
// Also force word wrapping, which is useful for printing, see https://issues.kolab.org/issue3992.
class RootBlock : public HTMLBlock
{
public:
    RootBlock(MimeTreeParser::HtmlWriter *writer);
    virtual ~RootBlock();

private:
    void internalEnter();
    void internalExit();

    HtmlWriter *mWriter;
};

}
#endif //__MIMETREEPARSER_HTMLBLOCK_H__
