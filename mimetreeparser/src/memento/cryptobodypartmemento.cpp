/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cryptobodypartmemento.h"

using namespace GpgME;
using namespace MimeTreeParser;

CryptoBodyPartMemento::CryptoBodyPartMemento()
    : QObject(nullptr)
    , Interface::BodyPartMemento()
{
}

CryptoBodyPartMemento::~CryptoBodyPartMemento() = default;

bool CryptoBodyPartMemento::isRunning() const
{
    return m_running;
}

void CryptoBodyPartMemento::setAuditLog(const Error &err, const QString &log)
{
    m_auditLogError = err;
    m_auditLog = log;
}

void CryptoBodyPartMemento::setRunning(bool running)
{
    m_running = running;
}

void CryptoBodyPartMemento::detach()
{
    disconnect(this, SIGNAL(update(MimeTreeParser::UpdateMode)), nullptr, nullptr);
}

#include "moc_cryptobodypartmemento.cpp"
