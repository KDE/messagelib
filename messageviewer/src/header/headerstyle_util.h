/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HEADERSTYLE_UTIL_H
#define HEADERSTYLE_UTIL_H

#include <QString>
#include <KTextToHTML>

#include <kmime/kmime_message.h>
#include <kmime/kmime_dateformatter.h>
#include "messageviewer/spamheaderanalyzer.h"

#include "messageviewer/headerstyle.h"
#include "messageviewer/viewer.h"
#include "messageviewer_export.h"
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT HeaderStyleUtil
{
public:
    struct xfaceSettings {
        xfaceSettings()
            : photoWidth(60),
              photoHeight(60)
        {
        }

        QString photoURL;
        int photoWidth;
        int photoHeight;
    };


    static QString directionOf(const QString &str);

    static QString strToHtml(const QString &str, KTextToHTML::Options flags = KTextToHTML::PreserveSpaces);

    static QString dateString(KMime::Message *message, bool printing, bool shortDate);

    static QString subjectString(KMime::Message *message, KTextToHTML::Options flags = KTextToHTML::PreserveSpaces);

    static QString subjectDirectionString(KMime::Message *message);

    static QString drawSpamMeter(SpamError spamError, double percent, double confidence,
                                 const QString &filterHeader, const QString &confidenceHeader);

    static QString imgToDataUrl(const QImage &image);

    static QString mailAction(MessageViewer::Viewer::MailAction action);

    static QString spamStatus(KMime::Message *message);

    static QString dateStr(const QDateTime &dateTime);

    static QString dateShortStr(const QDateTime &dateTime);

    static QVector<KMime::Types::Mailbox> resentFromList(KMime::Message *message);
    static QVector<KMime::Types::Mailbox> resentToList(KMime::Message *message);


    static xfaceSettings xface(const HeaderStyle *style, KMime::Message *message);
    static void updateXFaceSettings(QImage photo, xfaceSettings &settings);
};
}


#endif // HEADERSTYLE_UTIL_H
