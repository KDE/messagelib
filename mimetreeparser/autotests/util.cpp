/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "util.h"

#include <QDebug>
#include <QFile>

using namespace Qt::Literals::StringLiterals;
std::shared_ptr<KMime::Message> readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1StringView(MAIL_DATA_DIR) + u'/' + mailFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossible to open" << file.fileName();
        return {};
    }
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    std::shared_ptr<KMime::Message> msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}
