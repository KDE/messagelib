/*  -*- c++ -*-
    filehtmlwriter.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
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
    *stream() << "<!-- embedPart(contentID=" << contentId << ", url=" << url << ") -->"
             #if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
              << endl
             #else
              << Qt::endl
             #endif
                 ;
}

void FileHtmlWriter::extraHead(const QString &)
{
}
