/*
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  SPDX-FileContributor: Tobias Koenig <tokoe@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailcommon_export.h"

#include <MessageViewer/Viewer>

#include <QObject>

namespace MailCommon
{
class IKernel;

/**
 * @short A viewer handler to send MDN for viewed messages.
 */
class MAILCOMMON_EXPORT SendMdnHandler : public QObject, public MessageViewer::AbstractMessageLoadedHandler
{
    Q_OBJECT

public:
    /**
     * Creates a new send mnd handler.
     *
     * @param kernel The mail kernel object that will be used.
     * @param parent The parent object.
     */
    explicit SendMdnHandler(IKernel *kernel, QObject *parent = nullptr);

    /**
     * Destroys the send mdn handler.
     */
    ~SendMdnHandler() override;

    /**
     * @copydoc AbstractMessageLoadedHandler::setItem()
     */
    void setItem(const Akonadi::Item &item) override;

private:
    //@cond PRIVATE
    class SendMdnHandlerPrivate;
    std::unique_ptr<SendMdnHandlerPrivate> const d;
    //@endcond
};
}
