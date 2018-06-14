/*
  Copyright (c) 2013-2018 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#ifndef SCAMATTRIBUTE_H
#define SCAMATTRIBUTE_H

#include <AkonadiCore/attribute.h>
namespace MessageViewer {
class ScamAttributePrivate;

class ScamAttribute : public Akonadi::Attribute
{
public:
    explicit ScamAttribute();
    ~ScamAttribute() override;

    Q_REQUIRED_RESULT ScamAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    Q_REQUIRED_RESULT bool isAScam() const;
    void setIsAScam(bool b);

    Q_REQUIRED_RESULT bool operator==(const ScamAttribute &other) const;

private:
    friend class ScamAttributePrivate;
    ScamAttributePrivate *const d;
};
}

#endif // SCAMATTRIBUTE_H
