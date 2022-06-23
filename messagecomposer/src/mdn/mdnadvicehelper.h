/*
  SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <MessageComposer/MDNStateAttribute>
#include <MessageComposer/MessageFactoryNG>
#include <QObject>

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT MDNAdviceHelper : public QObject
{
    Q_OBJECT
public:
    static MDNAdviceHelper *instance()
    {
        if (!s_instance) {
            s_instance = new MDNAdviceHelper;
        }

        return s_instance;
    }

    /**
     * Checks the MDN headers to see if the user needs to be asked for any
     * confirmations. Will ask the user if action is required.
     *
     * Returns whether to send an MDN or not, and the sending mode for the MDN
     * to be created.
     *
     * Will also set the MailCommon::MDNStateAttribute on the given item
     * to what the user has selected.
     */
    Q_REQUIRED_RESULT QPair<bool, KMime::MDN::SendingMode> checkAndSetMDNInfo(const Akonadi::Item &item, KMime::MDN::DispositionType d, bool forceSend = false);

    Q_REQUIRED_RESULT MessageComposer::MDNStateAttribute::MDNSentState dispositionToSentState(KMime::MDN::DispositionType d);

    Q_REQUIRED_RESULT QPair<QString, bool> mdnMessageText(const char *what);

private:
    explicit MDNAdviceHelper(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    ~MDNAdviceHelper() override = default;

    Q_REQUIRED_RESULT int requestAdviceOnMDN(const char *what);
    MessageComposer::MDNAdvice questionIgnoreSend(const QString &text, bool canDeny);

    static MDNAdviceHelper *s_instance;
};
}
