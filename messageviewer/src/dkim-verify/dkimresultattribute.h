/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_private_export.h"
#include <AkonadiCore/attribute.h>
namespace MessageViewer
{
class DKIMResultAttributePrivate;

class MESSAGEVIEWER_TESTS_EXPORT DKIMResultAttribute : public Akonadi::Attribute
{
public:
    DKIMResultAttribute();
    ~DKIMResultAttribute() override;
    Q_REQUIRED_RESULT DKIMResultAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    Q_REQUIRED_RESULT bool operator==(const DKIMResultAttribute &other) const;

    void setError(int err);
    Q_REQUIRED_RESULT int error() const;

    void setWarning(int err);
    Q_REQUIRED_RESULT int warning() const;

    void setStatus(int err);
    Q_REQUIRED_RESULT int status() const;

private:
    friend class DKIMResultAttributePrivate;
    DKIMResultAttributePrivate *const d;
};
}

