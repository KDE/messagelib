/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <MessageCore/AttachmentLoadJob>

#include "messagecomposer_export.h"

#include <gpgme++/key.h>

namespace MessageComposer
{
// TODO I have no idea how to test this.  Have a fake keyring???
/**
 * @brief The AttachmentFromPublicKeyJob class
 */
class MESSAGECOMPOSER_EXPORT AttachmentFromPublicKeyJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT

public:
    explicit AttachmentFromPublicKeyJob(const GpgME::Key &key, QObject *parent = nullptr);
    ~AttachmentFromPublicKeyJob() override;

    [[nodiscard]] QString fingerprint() const;
    [[nodiscard]] GpgME::Key key() const;
    void setKey(const GpgME::Key &key);

protected Q_SLOTS:
    void doStart() override;

private:
    class AttachmentFromPublicKeyJobPrivate;
    friend class AttachmentFromPublicKeyJobPrivate;
    std::unique_ptr<AttachmentFromPublicKeyJobPrivate> const d;
};
} //
