/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_private_export.h"
#include <Akonadi/Attribute>
#include <memory>
namespace MessageViewer
{
class DKIMResultAttributePrivate;

class MESSAGEVIEWER_TESTS_EXPORT DKIMResultAttribute : public Akonadi::Attribute
{
public:
    DKIMResultAttribute();
    ~DKIMResultAttribute() override;
    [[nodiscard]] DKIMResultAttribute *clone() const override;
    [[nodiscard]] QByteArray type() const override;
    [[nodiscard]] QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    [[nodiscard]] bool operator==(const DKIMResultAttribute &other) const;

    void setError(int err);
    [[nodiscard]] int error() const;

    void setWarning(int err);
    [[nodiscard]] int warning() const;

    void setStatus(int err);
    [[nodiscard]] int status() const;

private:
    friend class DKIMResultAttributePrivate;
    std::unique_ptr<DKIMResultAttributePrivate> const d;
};
}
