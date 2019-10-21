/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#ifndef DKIMUTIL_H
#define DKIMUTIL_H
#include <QString>
#include <QCryptographicHash>
#include "messageviewer_private_export.h"
namespace MessageViewer {
namespace DKIMUtil {
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString bodyCanonizationRelaxed(QString body);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString bodyCanonizationSimple(QString body);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QByteArray generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString headerCanonizationSimple(const QString &headerName, const QString &headerValue);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString headerCanonizationRelaxed(const QString &headerName, const QString &headerValue);
MESSAGEVIEWER_TESTS_EXPORT QString cleanString(QString str);
MESSAGEVIEWER_TESTS_EXPORT QString emailDomain(const QString &emailDomain);
MESSAGEVIEWER_TESTS_EXPORT QString emailSubDomain(const QString &emailDomain);
}
}

#endif // DKIMUTIL_H
