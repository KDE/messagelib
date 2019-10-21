/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dmarcinfo.h"
#include "messageviewer_dkimcheckerdebug.h"

using namespace MessageViewer;
DMARCInfo::DMARCInfo()
{

}

bool DMARCInfo::parseDMARC(const QString &key)
{
    if (key.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error: key empty";
        return false;
    }
    const QStringList items = key.split(QLatin1String("; "));
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1String("v="))) {
            //v: Version (plain-text; REQUIRED).  Identifies the record retrieved
            //      as a DMARC record.  It MUST have the value of "DMARC1".  The value
            //      of this tag MUST match precisely; if it does not or it is absent,
            //      the entire retrieved record MUST be ignored.  It MUST be the first
            //      tag in the list.
            mVersion = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("r="))) {
            //adkim:  (plain-text; OPTIONAL; default is "r".)  Indicates whether
            //   strict or relaxed DKIM Identifier Alignment mode is required by
            //   the Domain Owner.  See Section 3.1.1 for details.  Valid values
            //   are as follows:
            //      r: relaxed mode
            //      s: strict mode
            mAdkim = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("p="))) {
            //p: Requested Mail Receiver policy (plain-text; REQUIRED for policy
            //                    records).  Indicates the policy to be enacted by the Receiver at
            //                    the request of the Domain Owner.  Policy applies to the domain
            //                    queried and to subdomains, unless subdomain policy is explicitly
            //                    described using the "sp" tag.  This tag is mandatory for policy
            //                    records only, but not for third-party reporting records (see
            //                                                                             Section 7.1).  Possible values are as follows:

            //                none:  The Domain Owner requests no specific action be taken
            //                regarding delivery of messages.

            //                quarantine:  The Domain Owner wishes to have email that fails the
            //                DMARC mechanism check be treated by Mail Receivers as
            //                suspicious.  Depending on the capabilities of the Mail
            //                Receiver, this can mean "place into spam folder", "scrutinize
            //                with additional intensity", and/or "flag as suspicious".

            //                reject:  The Domain Owner wishes for Mail Receivers to reject
            //                email that fails the DMARC mechanism check.  Rejection SHOULD
            //                occur during the SMTP transaction.  See Section 10.3 for some
            //                discussion of SMTP rejection methods and their implications.
            mPolicy = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("ptc="))) {
            //pct:  (plain-text integer between 0 and 100, inclusive; OPTIONAL;
            //      default is 100).  Percentage of messages from the Domain Owner's
            //      mail stream to which the DMARC policy is to be applied.  However,
            //      this MUST NOT be applied to the DMARC-generated reports, all of
            //      which must be sent and received unhindered.  The purpose of the
            //      "pct" tag is to allow Domain Owners to enact a slow rollout
            //      enforcement of the DMARC mechanism.  The prospect of "all or
            //      nothing" is recognized as preventing many organizations from
            //      experimenting with strong authentication-based mechanisms.  See
            //      Section 6.6.4 for details.  Note that random selection based on
            //      this percentage, such as the following pseudocode, is adequate:

            //       if (random mod 100) < pct then
            //         selected = true
            //       else
            //         selected = false
            //TODO verify if it's a percentage
            mPercentage = elem.right(elem.length() - 4).toInt();
        } else if (elem.startsWith(QLatin1String("sp="))) {
            //sp:  Requested Mail Receiver policy for all subdomains (plain-text;
            //   OPTIONAL).  Indicates the policy to be enacted by the Receiver at
            //   the request of the Domain Owner.  It applies only to subdomains of
            //   the domain queried and not to the domain itself.  Its syntax is
            //   identical to that of the "p" tag defined above.  If absent, the
            //   policy specified by the "p" tag MUST be applied for subdomains.
            //   Note that "sp" will be ignored for DMARC records published on
            //   subdomains of Organizational Domains due to the effect of the
            //   DMARC policy discovery mechanism described in Section 6.6.3.
            mSubDomainPolicy = elem.right(elem.length() - 3);
        }
    }
    if (mAdkim.isEmpty() && mVersion == QLatin1String("DMARC1")) {
        mAdkim = QLatin1Char('r');
    }

    return true;
}

QString DMARCInfo::version() const
{
    return mVersion;
}

void DMARCInfo::setVersion(const QString &version)
{
    mVersion = version;
}

QString DMARCInfo::adkim() const
{
    return mAdkim;
}

void DMARCInfo::setAdkim(const QString &adkim)
{
    mAdkim = adkim;
}

QString DMARCInfo::policy() const
{
    return mPolicy;
}

void DMARCInfo::setPolicy(const QString &policy)
{
    mPolicy = policy;
}

int DMARCInfo::percentage() const
{
    return mPercentage;
}

void DMARCInfo::setPercentage(int percentage)
{
    mPercentage = percentage;
}

QString DMARCInfo::subDomainPolicy() const
{
    return mSubDomainPolicy;
}

void DMARCInfo::setSubDomainPolicy(const QString &subDomainPolicy)
{
    mSubDomainPolicy = subDomainPolicy;
}

bool DMARCInfo::operator==(const DMARCInfo &other) const
{
    return mVersion == other.version() &&
            mAdkim == other.adkim() &&
            mPolicy == other.policy() &&
            mSubDomainPolicy == other.subDomainPolicy() &&
            mPercentage == other.percentage();
}

QDebug operator <<(QDebug d, const DMARCInfo &t)
{
    d << " mVersion " << t.version();
    d << " mAdkim " << t.adkim();
    d << " mPolicy " << t.policy();
    d << " mSubDomainPolicy " << t.subDomainPolicy();
    d << " mPercentage " << t.percentage();
    return d;
}
