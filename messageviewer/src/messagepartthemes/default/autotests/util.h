/*
  Copyright (c) 2010 Thomas McGuire <thomas.mcguire@kdab.com>

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

#ifndef MESSAGEVIEWER_TESTS_UTIL_H
#define MESSAGEVIEWER_TESTS_UTIL_H

#include "interfaces/htmlwriter.h"
#include <MessageViewer/CSSHelperBase>

#include <KMime/Message>

namespace MessageViewer {
namespace Test {
class CSSHelper : public MessageViewer::CSSHelperBase
{
public:
    CSSHelper() : MessageViewer::CSSHelperBase(nullptr)
    {
        for (int i = 0; i < 3; ++i) {
            mQuoteColor[i] = QColor(0x00, 0x80 - i * 0x10, 0x00);
        }
    }

    virtual ~CSSHelper()
    {
    }

    QString nonQuotedFontTag() const
    {
        return QStringLiteral("<");
    }

    QString quoteFontTag(int) const
    {
        return QStringLiteral("<");
    }
};

KMime::Message::Ptr readAndParseMail(const QString &mailFile);
void compareFile(const QString &outFile, const QString &referenceFile);
}
}

#endif //__MESSAGEVIEWER_TESTS_UTIL_H
