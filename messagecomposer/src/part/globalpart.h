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
/*!
 \class MessageComposer::GlobalPart
 \inmodule MessageComposer
 \inheaderfile MessageComposer/GlobalPart

 The GlobalPart class.
 */
class MESSAGECOMPOSER_EXPORT GlobalPart : public MessageComposer::MessagePart
{
    Q_OBJECT

public:
    /*! \brief Constructs a GlobalPart.
        \param parent The parent object.
    */
    explicit GlobalPart(QObject *parent = nullptr);
    /*! \brief Destroys the GlobalPart. */
    ~GlobalPart() override;

    // default true
    /*! \brief Returns whether the GUI is enabled for user interaction. */
    [[nodiscard]] bool isGuiEnabled() const;
    /*! \brief Sets whether the GUI is enabled.
        \param enabled True to enable GUI.
    */
    void setGuiEnabled(bool enabled);
    /*! \brief Returns the parent widget for GUI operations. */
    [[nodiscard]] QWidget *parentWidgetForGui() const;
    /*! \brief Sets the parent widget for GUI operations.
        \param widget The parent widget.
    */
    void setParentWidgetForGui(QWidget *widget);

    // default is false
    /*! \brief Returns whether a message delivery notification is requested. */
    [[nodiscard]] bool MDNRequested() const;
    /*! \brief Sets whether a message delivery notification is requested.
        \param requestMDN True to request MDN.
    */
    void setMDNRequested(bool requestMDN);

    /*! \brief Returns whether a delivery confirmation is requested. */
    [[nodiscard]] bool requestDeleveryConfirmation() const;
    /*! \brief Sets whether a delivery confirmation is requested.
        \param value True to request delivery confirmation.
    */
    void setRequestDeleveryConfirmation(bool value);

private:
    class GlobalPartPrivate;
    std::unique_ptr<GlobalPartPrivate> const d;
};
} // namespace MessageComposer
