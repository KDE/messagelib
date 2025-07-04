/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/optionset.h"
using namespace Qt::Literals::StringLiterals;

#include <ctime> // for ::time( time_t * )

#include <QDataStream>
#include <QIODevice>

static const int gOptionSetInitialMarker = 0xcafe; // don't change
static const int gOptionSetFinalMarker = 0xbabe; // don't change

static const int gOptionSetWithReadOnLyModeVersion = 0x1002;

using namespace MessageList::Core;

OptionSet::OptionSet()
{
    generateUniqueId();
}

OptionSet::OptionSet(const OptionSet &set) = default;

OptionSet::OptionSet(const QString &name, const QString &description, bool readOnly)
    : mName(name)
    , mDescription(description)
    , mReadOnly(readOnly)
{
    generateUniqueId();
}

OptionSet::~OptionSet() = default;

void OptionSet::generateUniqueId()
{
    static int nextUniqueId = 0;
    nextUniqueId++;
    mId = u"%1-%2"_s.arg((unsigned int)::time(nullptr)).arg(nextUniqueId);
}

QString OptionSet::saveToString() const
{
    QByteArray raw;

    {
        QDataStream s(&raw, QIODevice::WriteOnly);

        s << gOptionSetInitialMarker;
        s << gOptionSetWithReadOnLyModeVersion;
        s << mId;
        s << mName;
        s << mDescription;
        s << mReadOnly;

        save(s);

        s << gOptionSetFinalMarker;
    }

    return QString::fromLatin1(raw.toHex());
}

bool OptionSet::loadFromString(const QString &data)
{
    QByteArray raw = QByteArray::fromHex(data.toLatin1());

    QDataStream s(&raw, QIODevice::ReadOnly);

    int marker;

    s >> marker;

    if (marker != gOptionSetInitialMarker) {
        return false; // invalid configuration
    }

    int currentVersion;

    s >> currentVersion;

    if (currentVersion > gOptionSetWithReadOnLyModeVersion) {
        return false; // invalid configuration
    }

    s >> mId;

    if (mId.isEmpty()) {
        return false; // invalid configuration
    }

    s >> mName;

    if (mName.isEmpty()) {
        return false; // invalid configuration
    }

    s >> mDescription;

    bool readOnly = false;
    if (currentVersion == gOptionSetWithReadOnLyModeVersion) {
        s >> readOnly;
    }
    mReadOnly = readOnly;

    if (!load(s)) {
        return false; // invalid configuration
    }

    s >> marker;

    if (marker != gOptionSetFinalMarker) {
        return false; // invalid configuration
    }

    return true;
}
