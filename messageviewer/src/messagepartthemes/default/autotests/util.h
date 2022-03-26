/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "interfaces/htmlwriter.h"
#include <MessageViewer/CSSHelper>
#include <MessageViewer/CSSHelperBase>

#include <KMime/Message>

namespace MessageViewer
{
namespace Test
{
class CSSHelper : public MessageViewer::CSSHelper
{
public:
    CSSHelper()
        : MessageViewer::CSSHelper(nullptr)
    {
        for (int i = 0; i < 3; ++i) {
            mQuoteColor[i] = QColor(0x00, 0x80 - i * 0x10, 0x00);
        }
    }

    ~CSSHelper() override = default;

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
