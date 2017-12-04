/*
 * Copyright (c) 2003 Ingo Kloecker <kloecker@kde.org>
 * Copyright (c) 2008 Tom Albers <tomalbers@kde.nl>
 * Copyright (c) 2017 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
*/

#include "dictionarycombobox.h"
#include <SonnetCore/sonnet/speller.h>
#include "messagecomposer_debug.h"
#include <KLocalizedString>

using namespace MessageComposer;

//@cond PRIVATE
class MessageComposer::DictionaryComboBoxPrivate
{
public:
    explicit DictionaryComboBoxPrivate(DictionaryComboBox *combo) : q(combo) {}
    DictionaryComboBox *q;
    void slotDictionaryChanged(int idx);
};

void DictionaryComboBoxPrivate::slotDictionaryChanged(int idx)
{
    Q_EMIT q->dictionaryChanged(q->itemData(idx).toString());
    Q_EMIT q->dictionaryNameChanged(q->itemText(idx));
}
//@endcon

DictionaryComboBox::DictionaryComboBox(QWidget *parent)
    : QComboBox(parent), d(new DictionaryComboBoxPrivate(this))
{
    reloadCombo();
    connect(this, QOverload<int>::of(&QComboBox::activated),
        this, [this](int item) {d->slotDictionaryChanged(item);} );
}

DictionaryComboBox::~DictionaryComboBox()
{
    delete d;
}

QString DictionaryComboBox::currentDictionaryName() const
{
    return currentText();
}

QString DictionaryComboBox::currentDictionary() const
{
    return itemData(currentIndex()).toString();
}

bool DictionaryComboBox::assignDictionnaryName(const QString &name)
{
    if (name.isEmpty() || name == currentText()) {
        return false;
    }

    int idx = findText(name);
    if (idx == -1) {
        qCDebug(MESSAGECOMPOSER_LOG) << "name not found" << name;
        return false;
    }

    setCurrentIndex(idx);
    d->slotDictionaryChanged(idx);
    return true;
}

void DictionaryComboBox::setCurrentByDictionaryName(const QString &name)
{
    assignDictionnaryName(name);
    if (name != autoDeleteString()) {
        //TODO
    }
}

bool DictionaryComboBox::assignByDictionnary(const QString &dictionary)
{
    if (dictionary.isEmpty()) {
        return false;
    }
    if (dictionary == itemData(currentIndex()).toString()) {
        return true;
    }

    int idx = findData(dictionary);
    if (idx == -1) {
        qCDebug(MESSAGECOMPOSER_LOG) << "dictionary not found" << dictionary;
        return false;
    }

    setCurrentIndex(idx);
    d->slotDictionaryChanged(idx);
    return true;
}

void DictionaryComboBox::setCurrentByDictionary(const QString &dictionary)
{
    assignByDictionnary(dictionary);
}

void DictionaryComboBox::reloadCombo()
{
    clear();
    addItem(i18n("AutoDetect Language"), autoDeleteString());
    Sonnet::Speller *speller = new Sonnet::Speller();
    QMap<QString, QString> dictionaries = speller->availableDictionaries();
    QMapIterator<QString, QString> i(dictionaries);
    while (i.hasNext()) {
        i.next();
        addItem(i.key(), i.value());
    }
    delete speller;
}

QString DictionaryComboBox::autoDeleteString() const
{
    return QStringLiteral("autodetect");
}

#include "moc_dictionarycombobox.cpp"
