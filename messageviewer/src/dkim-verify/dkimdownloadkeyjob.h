/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
class QDnsLookup;
namespace MessageViewer
{
/*!
 * \class MessageViewer::DKIMDownloadKeyJob
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/DKIMDownloadKeyJob
 * \brief The DKIMDownloadKeyJob class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMDownloadKeyJob : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit DKIMDownloadKeyJob(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~DKIMDownloadKeyJob() override;

    /*!
     * \brief Starts the DNS lookup
     * \return true if lookup started successfully
     */
    [[nodiscard]] bool start();

    /*!
     * \brief Checks if the job can be started
     * \return true if job can start
     */
    [[nodiscard]] bool canStart() const;

    /*!
     */
    /*!
     * \brief Returns the domain name
     * \return domain name string
     */
    [[nodiscard]] QString domainName() const;
    /*!
     * \brief Sets the domain name
     * \param domainName domain name to set
     */
    void setDomainName(const QString &domainName);

    /*!
     */
    /*!
     * \brief Returns the selector name
     * \return selector name string
     */
    [[nodiscard]] QString selectorName() const;
    /*!
     * \brief Sets the selector name
     * \param selectorName selector name to set
     */
    void setSelectorName(const QString &selectorName);

    /*!
     * \brief Returns the resolved DNS value
     * \return DNS value string
     */
    [[nodiscard]] QString resolvDnsValue() const;

    /*!
     * \brief Returns the DNS lookup object
     * \return QDnsLookup pointer
     */
    [[nodiscard]] QDnsLookup *dnsLookup() const;

Q_SIGNALS:
    /*!
     * \brief Emitted when an error occurs
     * \param error error message
     */
    void error(const QString &error);
    /*!
     * \brief Emitted when DNS lookup succeeds
     * \param domain domain name
     * \param selector selector name
     */
    void success(const QList<QByteArray> &, const QString &domain, const QString &selector);

private:
    MESSAGEVIEWER_NO_EXPORT void resolvDnsDone();
    QString mDomainName;
    QString mSelectorName;
    QDnsLookup *mDnsLookup = nullptr;
};
}
