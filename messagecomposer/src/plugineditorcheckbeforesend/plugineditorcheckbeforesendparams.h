/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QString>

namespace MessageComposer
{
class PluginEditorCheckBeforeSendParamsPrivate;
/**
 * @brief The PluginEditorCheckBeforeSendParams class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendParams
{
public:
    PluginEditorCheckBeforeSendParams();
    PluginEditorCheckBeforeSendParams(const PluginEditorCheckBeforeSendParams &other);
    ~PluginEditorCheckBeforeSendParams();

    void setSubject(const QString &subject);
    Q_REQUIRED_RESULT QString subject() const;

    void setIdentity(uint currentIdentity);
    Q_REQUIRED_RESULT uint identity() const;

    Q_REQUIRED_RESULT bool isHtmlMail() const;
    void setHtmlMail(bool html);

    void setPlainText(const QString &text);
    Q_REQUIRED_RESULT QString plainText() const;

    void setBccAddresses(const QString &lst);
    Q_REQUIRED_RESULT QString bccAddresses() const;

    void setToAddresses(const QString &lst);
    Q_REQUIRED_RESULT QString toAddresses() const;

    void setCcAddresses(const QString &lst);
    Q_REQUIRED_RESULT QString ccAddresses() const;

    void setDefaultDomain(const QString &domain);
    Q_REQUIRED_RESULT QString defaultDomain() const;

    Q_REQUIRED_RESULT bool hasAttachment() const;
    void setHasAttachment(bool b);

    Q_REQUIRED_RESULT int transportId() const;
    void setTransportId(int id);

    PluginEditorCheckBeforeSendParams &operator=(const PluginEditorCheckBeforeSendParams &other);
    Q_REQUIRED_RESULT bool operator==(const PluginEditorCheckBeforeSendParams &other) const;

private:
    PluginEditorCheckBeforeSendParamsPrivate *const d;
};
}
