/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"

#include <memory>
#include <type_traits>

namespace MimeTreeParser
{

class CompositeMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit CompositeMemento() = default;
    ~CompositeMemento() override;

    bool start() override;
    void exec() override;

    void addMemento(CryptoBodyPartMemento *memento);

    [[nodiscard]] QList<CryptoBodyPartMemento *> mementos() const
    {
        return mMementos;
    }

    [[nodiscard]] auto size() const
    {
        return mMementos.size();
    }

    template<typename T>
    T *memento() const
    {
        auto it = std::find_if(mMementos.begin(), mMementos.end(), [](auto *memento) {
            return qobject_cast<std::decay_t<T> *>(memento) != nullptr;
        });
        if (it != mMementos.cend()) {
            return static_cast<T *>(*it);
        }

        return nullptr;
    }

private:
    void subMementoFinished();
    QList<CryptoBodyPartMemento *> mMementos;
    int mRunningMementos = 0;
};

} // namespace
