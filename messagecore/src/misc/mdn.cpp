/*  -*- c++ -*-
    kmime_mdn.cpp

    KMime, the KDE Internet mail/usenet news message library.
    SPDX-FileCopyrightText: 2002 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling @ref MIME data and
  provides functions for supporting Message Disposition Notifications (MDNs),
  also known as email return receipts.

  @brief
  Provides support for Message Disposition Notifications.

  @authors Marc Mutz \<mutz@kde.org\>
*/

#include "mdn.h"
#include "messagecore_debug.h"
#include "messagecore_version.h"

#include <KCodecs>

#include <QByteArray>
#include <QCoreApplication>
#include <array>

#ifdef Q_OS_WIN // gethostname
#include <winsock2.h>
#else
#include <unistd.h>
#endif

namespace MessageCore
{

namespace MDN
{

struct DispositionMetaData {
    DispositionType dispositionType;
    const char *string;
    const char *description;
};

static constexpr std::array<DispositionMetaData, 6> dispositionTypes = {
    DispositionMetaData{Displayed,
                        "displayed",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "The message sent on ${date} to ${to} with subject "
                                          "\"${subject}\" has been displayed. This is no guarantee that "
                                          "the message has been read or understood.")},
    DispositionMetaData{Deleted,
                        "deleted",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "The message sent on ${date} to ${to} with subject "
                                          "\"${subject}\" has been deleted unseen. This is no guarantee "
                                          "that the message will not be \"undeleted\" and nonetheless "
                                          "read later on.")},
    DispositionMetaData{Dispatched,
                        "dispatched",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "The message sent on ${date} to ${to} with subject "
                                          "\"${subject}\" has been dispatched. This is no guarantee "
                                          "that the message will not be read later on.")},
    DispositionMetaData{Processed,
                        "processed",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "The message sent on ${date} to ${to} with subject "
                                          "\"${subject}\" has been processed by some automatic means.")},
    DispositionMetaData{Denied,
                        "denied",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "The message sent on ${date} to ${to} with subject "
                                          "\"${subject}\" has been acted upon. The sender does not wish "
                                          "to disclose more details to you than that.")},
    DispositionMetaData{Failed,
                        "failed",
                        QT_TRANSLATE_NOOP("DispositionModifier",
                                          "Generation of a Message Disposition Notification for the "
                                          "message sent on ${date} to ${to} with subject \"${subject}\" "
                                          "failed. Reason is given in the Failure: header field below.")}};

static const char *stringFor(DispositionType d)
{
    for (size_t i = 0; i < dispositionTypes.size(); ++i) {
        if (dispositionTypes[i].dispositionType == d) {
            return dispositionTypes[i].string;
        }
    }
    return nullptr;
}

//
// disposition-modifier
//
struct DispositionModifierMetaData {
    DispositionModifier dispositionModifier;
    const char *string;
};

static constexpr std::array<DispositionModifierMetaData, 5> dispositionModifiers = {DispositionModifierMetaData{Error, "error"},
                                                                                    DispositionModifierMetaData{Warning, "warning"},
                                                                                    DispositionModifierMetaData{Superseded, "superseded"},
                                                                                    DispositionModifierMetaData{Expired, "expired"},
                                                                                    DispositionModifierMetaData{MailboxTerminated, "mailbox-terminated"}};

static const char *stringFor(DispositionModifier m)
{
    for (size_t i = 0; i < dispositionModifiers.size(); ++i) {
        if (dispositionModifiers[i].dispositionModifier == m) {
            return dispositionModifiers[i].string;
        }
    }
    return nullptr;
}

//
// action-mode (part of disposition-mode)
//

struct ActionModeMetaData {
    ActionMode actionMode;
    const char *string;
};

static constexpr std::array<ActionModeMetaData, 2> actionModes = {ActionModeMetaData{ManualAction, "manual-action"},
                                                                  ActionModeMetaData{AutomaticAction, "automatic-action"}};

static const char *stringFor(ActionMode a)
{
    for (size_t i = 0; i < actionModes.size(); ++i) {
        if (actionModes[i].actionMode == a) {
            return actionModes[i].string;
        }
    }
    return nullptr;
}

//
// sending-mode (part of disposition-mode)
//

struct SendingModeMetaData {
    SendingMode sendingMode;
    const char *string;
};

static constexpr std::array<SendingModeMetaData, 2> sendingModes = {SendingModeMetaData{SentManually, "MDN-sent-manually"},
                                                                    SendingModeMetaData{SentAutomatically, "MDN-sent-automatically"}};

static const char *stringFor(SendingMode s)
{
    for (size_t i = 0; i < sendingModes.size(); ++i) {
        if (sendingModes[i].sendingMode == s) {
            return sendingModes[i].string;
        }
    }
    return nullptr;
}

static QByteArray dispositionField(DispositionType d, ActionMode a, SendingMode s, const QList<DispositionModifier> &m)
{
    // mandatory parts: Disposition: foo/baz; bar
    QByteArray result = "Disposition: ";
    result += stringFor(a);
    result += '/';
    result += stringFor(s);
    result += "; ";
    result += stringFor(d);

    // optional parts: Disposition: foo/baz; bar/mod1,mod2,mod3
    bool first = true;
    for (QList<DispositionModifier>::const_iterator mt = m.begin(); mt != m.end(); ++mt) {
        if (first) {
            result += '/';
            first = false;
        } else {
            result += ',';
        }
        result += stringFor(*mt);
    }
    return result + '\n';
}

static QByteArray finalRecipient(const QString &recipient)
{
    if (recipient.isEmpty()) {
        return {};
    } else {
        return "Final-Recipient: rfc822; " + KCodecs::encodeRFC2047String(recipient, "utf-8") + '\n';
    }
}

static QByteArray orginalRecipient(const QByteArray &recipient)
{
    if (recipient.isEmpty()) {
        return {};
    } else {
        return "Original-Recipient: " + recipient + '\n';
    }
}

static QByteArray originalMessageID(const QByteArray &msgid)
{
    if (msgid.isEmpty()) {
        return {};
    } else {
        return "Original-Message-ID: " + msgid + '\n';
    }
}

static QByteArray reportingUAField()
{
    char hostName[256];
    if (gethostname(hostName, 255)) {
        hostName[0] = '\0'; // gethostname failed: pretend empty string
    } else {
        hostName[255] = '\0'; // gethostname may have returned 255 chars (man page)
    }
    return QByteArray("Reporting-UA: ") + QByteArray(hostName) + QByteArray("; MessageCore " MESSAGECORE_VERSION_STRING "\n");
}

QByteArray dispositionNotificationBodyContent(const QString &r,
                                              const QByteArray &o,
                                              const QByteArray &omid,
                                              DispositionType d,
                                              ActionMode a,
                                              SendingMode s,
                                              const QList<DispositionModifier> &m,
                                              const QString &special)
{
    // in Perl: chomp(special)
    QString spec;
    if (special.endsWith(QLatin1Char('\n'))) {
        spec = special.left(special.length() - 1);
    } else {
        spec = special;
    }

    // std headers:
    QByteArray result = reportingUAField();
    result += orginalRecipient(o);
    result += finalRecipient(r);
    result += originalMessageID(omid);
    result += dispositionField(d, a, s, m);

    // headers that are only present for certain disposition {types,modifiers}:
    if (d == Failed) {
        result += "Failure: " + KCodecs::encodeRFC2047String(spec, "utf-8") + '\n';
    } else if (m.contains(Error)) {
        result += "Error: " + KCodecs::encodeRFC2047String(spec, "utf-8") + '\n';
    } else if (m.contains(Warning)) {
        result += "Warning: " + KCodecs::encodeRFC2047String(spec, "utf-8") + '\n';
    }

    return result;
}

QString descriptionFor(DispositionType d, const QList<DispositionModifier> &)
{
    for (size_t i = 0; i < dispositionTypes.size(); ++i) {
        if (dispositionTypes[i].dispositionType == d) {
            return QCoreApplication::translate("DispositionModifier", dispositionTypes[i].description);
        }
    }
    qCWarning(MESSAGECORE_LOG) << "KMime::MDN::descriptionFor(): No such disposition type:" << static_cast<int>(d);
    return {};
}

} // namespace MDN
} // namespace KMime
