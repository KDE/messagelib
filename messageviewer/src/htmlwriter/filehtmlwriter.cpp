/*  -*- c++ -*-
    filehtmlwriter.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filehtmlwriter.h"
#include "messageviewer_debug.h"

using namespace MessageViewer;

FileHtmlWriter::FileHtmlWriter(const QString &filename)
    : mFile(filename.isEmpty() ? QStringLiteral("filehtmlwriter.out") : filename)
{
}

FileHtmlWriter::~FileHtmlWriter()
{
    if (mFile.isOpen()) {
        qCWarning(MESSAGEVIEWER_LOG) << "FileHtmlWriter: file still open!";
        HtmlWriter::end();
        mFile.close();
    }
}

void FileHtmlWriter::begin()
{
    if (mFile.isOpen()) {
        qCWarning(MESSAGEVIEWER_LOG) << "FileHtmlWriter: file still open!";
        mFile.close();
    }
    if (!mFile.open(QIODevice::WriteOnly)) {
        qCWarning(MESSAGEVIEWER_LOG) << "FileHtmlWriter: Cannot open file" << mFile.fileName();
    }
    HtmlWriter::begin();
}

void FileHtmlWriter::end()
{
    HtmlWriter::end();
    mFile.close();
}

void FileHtmlWriter::reset()
{
    HtmlWriter::reset();
    if (mFile.isOpen()) {
        mFile.close();
    }
}

QIODevice *FileHtmlWriter::device() const
{
    return const_cast<QFile *>(&mFile);
}

void FileHtmlWriter::embedPart(const QByteArray &contentId, const QString &url)
{
    *stream() << "<!-- embedPart(contentID=" << contentId << ", url=" << url << ") -->" << Qt::endl;
}

void FileHtmlWriter::setExtraHead(const QString &)
{
}

void FileHtmlWriter::setStyleBody(const QString &)
{
}
