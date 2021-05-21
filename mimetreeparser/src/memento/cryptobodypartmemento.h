/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <gpgme++/error.h>

#include <QObject>
#include <QString>

#include "enums.h"
#include "interfaces/bodypart.h"

namespace MimeTreeParser
{
class CryptoBodyPartMemento : public QObject, public Interface::BodyPartMemento
{
    Q_OBJECT
public:
    CryptoBodyPartMemento();
    ~CryptoBodyPartMemento() override;

    virtual bool start() = 0;
    virtual void exec() = 0;
    Q_REQUIRED_RESULT bool isRunning() const;

    const QString &auditLogAsHtml() const
    {
        return m_auditLog;
    }

    Q_REQUIRED_RESULT GpgME::Error auditLogError() const
    {
        return m_auditLogError;
    }

    void detach() override;

Q_SIGNALS:
    void update(MimeTreeParser::UpdateMode);

protected Q_SLOTS:
    void notify()
    {
        Q_EMIT update(MimeTreeParser::Force);
    }

protected:
    void setAuditLog(const GpgME::Error &err, const QString &log);
    void setRunning(bool running);

private:
    bool m_running = false;
    QString m_auditLog;
    GpgME::Error m_auditLogError;
};
}
