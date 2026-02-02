/*
  SPDX-FileCopyrightText: 2022-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <KMessageWidget>
#include <KMime/MDN>

namespace MessageViewer
{
/*!
 * \class MessageViewer::MDNWarningWidget
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/MDNWarningWidget
 * \brief The MDNWarningWidget class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MDNWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    enum ResponseType : uint8_t {
        Ignore = 0,
        Send = 1,
        SendDeny = 2,
    };
    /*!
     * \brief Constructor
     * \param parent parent widget
     */
    explicit MDNWarningWidget(QWidget *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~MDNWarningWidget() override;

    /*!
     * \brief Sets whether the user can deny sending MDN
     * \param deny whether denial is allowed
     */
    void setCanDeny(bool deny);

    /*!
     * \brief Sets the information message
     * \param str information text
     */
    void setInformation(const QString &str);

    /*!
     * \brief Returns the sending mode
     * \return KMime MDN sending mode
     */
    [[nodiscard]] KMime::MDN::SendingMode sendingMode() const;
    /*!
     * \brief Sets the sending mode
     * \param newSendingMode the KMime MDN sending mode
     */
    void setSendingMode(KMime::MDN::SendingMode newSendingMode);

Q_SIGNALS:
    /*!
     * \brief Emitted when user responds to the MDN request
     * \param type response type
     * \param sendingMode MDN sending mode
     */
    void sendResponse(MessageViewer::MDNWarningWidget::ResponseType type, KMime::MDN::SendingMode sendingMode);

private:
    MESSAGEVIEWER_NO_EXPORT void slotSend();
    MESSAGEVIEWER_NO_EXPORT void slotIgnore();
    MESSAGEVIEWER_NO_EXPORT void slotSendDeny();
    QAction *const mIgnoreAction;
    QAction *const mSendAction;
    QAction *const mSendDenyAction;
    KMime::MDN::SendingMode mSendingMode = KMime::MDN::SentAutomatically;
};
}
