/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TEMPLATECONVERTCOMMANDJOB_H
#define TEMPLATECONVERTCOMMANDJOB_H

#include <QObject>
#include "templateparser_export.h"
#include <KMime/Message>

namespace TemplateParser {
/**
 * @brief The TemplateConvertCommandJob class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateConvertCommandJob : public QObject
{
    Q_OBJECT
public:
    explicit TemplateConvertCommandJob(QObject *parent = nullptr);
    ~TemplateConvertCommandJob();
    QString convertText();

    QString currentText() const;
    void setCurrentText(const QString &currentText);

    KMime::Message::Ptr originalMessage() const;
    void setOriginalMessage(const KMime::Message::Ptr &originalMessage);

private:
    QString mCurrentText;
    KMime::Message::Ptr mOriginalMessage;
};
}

#endif // TEMPLATECONVERTCOMMANDJOB_H
