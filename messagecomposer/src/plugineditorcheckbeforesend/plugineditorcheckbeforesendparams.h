/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QString>
#include <memory>
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
    [[nodiscard]] QString subject() const;

    void setIdentity(uint currentIdentity);
    [[nodiscard]] uint identity() const;

    [[nodiscard]] bool isHtmlMail() const;
    void setHtmlMail(bool html);

    void setPlainText(const QString &text);
    [[nodiscard]] QString plainText() const;

    void setBccAddresses(const QString &lst);
    [[nodiscard]] QString bccAddresses() const;

    void setToAddresses(const QString &lst);
    [[nodiscard]] QString toAddresses() const;

    void setCcAddresses(const QString &lst);
    [[nodiscard]] QString ccAddresses() const;

    void setDefaultDomain(const QString &domain);
    [[nodiscard]] QString defaultDomain() const;

    [[nodiscard]] bool hasAttachment() const;
    void setHasAttachment(bool b);

    [[nodiscard]] int transportId() const;
    void setTransportId(int id);

    PluginEditorCheckBeforeSendParams &operator=(const PluginEditorCheckBeforeSendParams &other);
    [[nodiscard]] bool operator==(const PluginEditorCheckBeforeSendParams &other) const;

private:
    std::unique_ptr<PluginEditorCheckBeforeSendParamsPrivate> const d;
};
}
