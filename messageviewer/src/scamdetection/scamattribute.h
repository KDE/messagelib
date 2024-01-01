/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <Akonadi/Attribute>
#include <memory>
namespace MessageViewer
{
class ScamAttributePrivate;

class ScamAttribute : public Akonadi::Attribute
{
public:
    explicit ScamAttribute();
    ~ScamAttribute() override;

    [[nodiscard]] ScamAttribute *clone() const override;
    [[nodiscard]] QByteArray type() const override;
    [[nodiscard]] QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    [[nodiscard]] bool isAScam() const;
    void setIsAScam(bool b);

    [[nodiscard]] bool operator==(const ScamAttribute &other) const;

private:
    friend class ScamAttributePrivate;
    std::unique_ptr<ScamAttributePrivate> const d;
};
}
