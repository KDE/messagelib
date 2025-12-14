/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "util.h"
using namespace Qt::Literals::StringLiterals;

#include <QFile>

std::shared_ptr<KMime::Message> MessageViewer::Test::readAndParseMail(const QString &mailFile)
{
    QFile file(QStringLiteral(MAIL_DATA_DIR) + u'/' + mailFile);
    const bool openFile = file.open(QIODevice::ReadOnly);
    Q_ASSERT(openFile);
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    std::shared_ptr<KMime::Message> msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}
