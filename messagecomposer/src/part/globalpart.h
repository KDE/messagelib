/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagepart.h"

#include <QByteArray>
#include <memory>

namespace MessageComposer
{
/**
 * @brief The GlobalPart class
 */
class MESSAGECOMPOSER_EXPORT GlobalPart : public MessageComposer::MessagePart
{
    Q_OBJECT

public:
    explicit GlobalPart(QObject *parent = nullptr);
    ~GlobalPart() override;

    // default true
    [[nodiscard]] bool isGuiEnabled() const;
    void setGuiEnabled(bool enabled);
    [[nodiscard]] QWidget *parentWidgetForGui() const;
    void setParentWidgetForGui(QWidget *widget);

    // default is false
    [[nodiscard]] bool MDNRequested() const;
    void setMDNRequested(bool requestMDN);

    [[nodiscard]] bool requestDeleveryConfirmation() const;
    void setRequestDeleveryConfirmation(bool value);

private:
    class GlobalPartPrivate;
    std::unique_ptr<GlobalPartPrivate> const d;
};
} // namespace MessageComposer
