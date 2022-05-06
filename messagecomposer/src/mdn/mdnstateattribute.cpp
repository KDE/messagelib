/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
 *  SPDX-FileCopyrightText: 2010 KDAB
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 *
 *****************************************************************************/

#include "mdnstateattribute.h"

#include <Akonadi/AttributeFactory>

#include <QByteArray>

using namespace MailCommon;

/**
 *  @internal
 */
class Q_DECL_HIDDEN MDNStateAttribute::MDNStateAttributePrivate
{
public:
    MDNSentState dataToState(const QByteArray &data)
    {
        MDNSentState state = MDNStateUnknown;

        switch (data.at(0)) {
        case 'N':
            state = MDNNone;
            break;
        case 'I':
            state = MDNIgnore;
            break;
        case 'R':
            state = MDNDisplayed;
            break;
        case 'D':
            state = MDNDeleted;
            break;
        case 'F':
            state = MDNDispatched;
            break;
        case 'P':
            state = MDNProcessed;
            break;
        case 'X':
            state = MDNDenied;
            break;
        case 'E':
            state = MDNFailed;
            break;
        case 'U':
        default:
            state = MDNStateUnknown;
            break;
        }

        return state;
    }

    QByteArray stateToData(MDNSentState state)
    {
        QByteArray data = "U"; // Unknown

        switch (state) {
        case MDNNone:
            data = "N";
            break;
        case MDNIgnore:
            data = "I";
            break;
        case MDNDisplayed:
            data = "R";
            break;
        case MDNDeleted:
            data = "D";
            break;
        case MDNDispatched:
            data = "F";
            break;
        case MDNProcessed:
            data = "P";
            break;
        case MDNDenied:
            data = "X";
            break;
        case MDNFailed:
            data = "E";
            break;
        case MDNStateUnknown:
            data = "U";
            break;
        }

        return data;
    }

    QByteArray mSentState;
};

MDNStateAttribute::MDNStateAttribute(MDNSentState state)
    : d(new MDNStateAttributePrivate)
{
    d->mSentState = d->stateToData(state);
}

MDNStateAttribute::MDNStateAttribute(const QByteArray &stateData)
    : d(new MDNStateAttributePrivate)
{
    d->mSentState = stateData;
}

MDNStateAttribute::~MDNStateAttribute() = default;

MDNStateAttribute *MDNStateAttribute::clone() const
{
    return new MDNStateAttribute(d->mSentState);
}

QByteArray MDNStateAttribute::type() const
{
    static const QByteArray sType("MDNStateAttribute");
    return sType;
}

QByteArray MDNStateAttribute::serialized() const
{
    return d->mSentState;
}

void MDNStateAttribute::deserialize(const QByteArray &data)
{
    d->mSentState = data;
}

void MDNStateAttribute::setMDNState(MDNSentState state)
{
    d->mSentState = d->stateToData(state);
}

MDNStateAttribute::MDNSentState MDNStateAttribute::mdnState() const
{
    return d->dataToState(d->mSentState);
}

bool MDNStateAttribute::operator==(const MDNStateAttribute &other) const
{
    return mdnState() == other.mdnState();
}

// Register the attribute when the library is loaded.
namespace
{
bool dummyMDNStateAttribute()
{
    using namespace MailCommon;
    Akonadi::AttributeFactory::registerAttribute<MDNStateAttribute>();
    return true;
}

const bool registeredMDNStateAttribute = dummyMDNStateAttribute();
}
