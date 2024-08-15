/*
  SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <Akonadi/MDNStateAttribute>
#include <MessageComposer/MessageFactoryNG>
#include <QObject>

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT MDNAdviceHelper : public QObject
{
    Q_OBJECT
public:
    static MDNAdviceHelper *instance();

    /**
     * Checks the MDN headers to see if the user needs to be asked for any
     * confirmations. Will ask the user if action is required.
     *
     * Returns whether to send an MDN or not, and the sending mode for the MDN
     * to be created.
     *
     * Will also set the Akonadi::MDNStateAttribute on the given item
     * to what the user has selected.
     */
    [[nodiscard]] QPair<bool, KMime::MDN::SendingMode> checkAndSetMDNInfo(const Akonadi::Item &item, KMime::MDN::DispositionType d, bool forceSend = false);

    [[nodiscard]] static Akonadi::MDNStateAttribute::MDNSentState dispositionToSentState(KMime::MDN::DispositionType d);

    [[nodiscard]] QPair<QString, bool> mdnMessageText(const char *what);

private:
    MESSAGECOMPOSER_NO_EXPORT explicit MDNAdviceHelper(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    ~MDNAdviceHelper() override = default;

    [[nodiscard]] MESSAGECOMPOSER_NO_EXPORT int requestAdviceOnMDN(const char *what);
    MESSAGECOMPOSER_NO_EXPORT MessageComposer::MDNAdvice questionIgnoreSend(const QString &text, bool canDeny);

    static MDNAdviceHelper *s_instance;
};
}
