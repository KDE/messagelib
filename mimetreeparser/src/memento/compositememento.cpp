/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "compositememento.h"
#include <QEventLoop>

using namespace MimeTreeParser;

CompositeMemento::~CompositeMemento()
{
    for (auto *memento : mMementos) {
        memento->detach();
        delete memento;
    }
    mMementos.clear();
}

bool CompositeMemento::start()
{
    mRunningMementos = 0;
    for (auto *memento : mMementos) {
        connect(memento, &CryptoBodyPartMemento::update, this, &CompositeMemento::subMementoFinished);
        ++mRunningMementos;
        memento->start();
    }

    return true;
}

void CompositeMemento::exec()
{
    int counter = 0;
    QEventLoop el;
    for (auto *memento : mMementos) {
        connect(memento, &CryptoBodyPartMemento::update, this, [&el, &counter]() {
            --counter;
            if (counter == 0) {
                el.quit();
            }
        });
        memento->start();
        ++counter;
    }
    el.exec();
}

void CompositeMemento::addMemento(CryptoBodyPartMemento *memento)
{
    mMementos.push_back(memento);
}

void CompositeMemento::subMementoFinished()
{
    --mRunningMementos;
    if (mRunningMementos == 0) {
        notify();
    }
}

#include "moc_compositememento.cpp"
