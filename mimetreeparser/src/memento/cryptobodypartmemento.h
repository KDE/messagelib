/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_CRYPTOBODYPARTMEMENTO_H
#define MIMETREEPARSER_CRYPTOBODYPARTMEMENTO_H

#include <gpgme++/error.h>

#include <QObject>
#include <QString>

#include "interfaces/bodypart.h"
#include "enums.h"

namespace MimeTreeParser {
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

    GpgME::Error auditLogError() const
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
    bool m_running;
    QString m_auditLog;
    GpgME::Error m_auditLogError;
};
}
#endif // MIMETREEPARSER_CRYPTOBODYPARTMEMENTO_H
