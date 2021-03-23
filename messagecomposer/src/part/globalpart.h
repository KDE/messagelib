/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagepart.h"

#include <QByteArray>

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
    Q_REQUIRED_RESULT bool isGuiEnabled() const;
    void setGuiEnabled(bool enabled);
    Q_REQUIRED_RESULT QWidget *parentWidgetForGui() const;
    void setParentWidgetForGui(QWidget *widget);

    Q_REQUIRED_RESULT bool isFallbackCharsetEnabled() const;
    void setFallbackCharsetEnabled(bool enabled);
    Q_REQUIRED_RESULT QVector<QByteArray> charsets(bool forceFallback = false) const;
    void setCharsets(const QVector<QByteArray> &charsets);

    Q_REQUIRED_RESULT bool is8BitAllowed() const;
    void set8BitAllowed(bool allowed);

    // default is false
    Q_REQUIRED_RESULT bool MDNRequested() const;
    void setMDNRequested(bool requestMDN);

    Q_REQUIRED_RESULT bool requestDeleveryConfirmation() const;
    void setRequestDeleveryConfirmation(bool value);

private:
    class Private;
    Private *const d;
};
} // namespace MessageComposer

