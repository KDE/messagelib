/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcinfo.h"
#include <QDebug>

#include "messageviewer_dkimcheckerdebug.h"

using namespace Qt::Literals::StringLiterals;
using namespace MessageViewer;
DMARCInfo::DMARCInfo() = default;

bool DMARCInfo::parseDMARC(const QString &key)
{
    if (key.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error: key empty";
        return false;
    }

    // Reset state so parsing a second record on the same instance does not
    // leak values from the first one.
    mVersion.clear();
    mAdkim.clear();
    mPolicy.clear();
    mSubDomainPolicy.clear();
    mPercentage = -1;

    QString cleanKey = key;
    cleanKey.replace(QLatin1StringView("; "), QLatin1StringView(";"));
    const QStringList items = cleanKey.split(u';', Qt::SkipEmptyParts);
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1StringView("v="))) {
            // v: Version (plain-text; REQUIRED).  Identifies the record retrieved
            //      as a DMARC record.  It MUST have the value of "DMARC1".  The value
            //      of this tag MUST match precisely; if it does not or it is absent,
            //      the entire retrieved record MUST be ignored.  It MUST be the first
            //      tag in the list.
            mVersion = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1StringView("adkim="))) {
            // adkim:  (plain-text; OPTIONAL; default is "r".)  Indicates whether
            //   strict or relaxed DKIM Identifier Alignment mode is required by
            //   the Domain Owner.  See Section 3.1.1 for details.  Valid values
            //   are as follows:
            //      r: relaxed mode
            //      s: strict mode
            const QString adkimValue = QStringView(elem).mid(6).toString();
            if (adkimValue != QLatin1StringView("r") && adkimValue != QLatin1StringView("s")) {
                return false;
            }
            mAdkim = adkimValue;
        } else if (elem.startsWith(QLatin1StringView("p="))) {
            // p: Requested Mail Receiver policy (plain-text; REQUIRED for policy
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
            const QString policyValue = QStringView(elem).mid(2).toString();
            if (policyValue != QLatin1StringView("none") && policyValue != QLatin1StringView("quarantine") && policyValue != QLatin1StringView("reject")) {
                return false;
            }
            mPolicy = policyValue;
        } else if (elem.startsWith(QLatin1StringView("pct="))) {
            // pct:  (plain-text integer between 0 and 100, inclusive; OPTIONAL;
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
            bool ok = false;
            mPercentage = QStringView(elem).mid(4).toInt(&ok);
            if (!ok) {
                return false;
            }
        } else if (elem.startsWith(QLatin1StringView("sp="))) {
            // sp:  Requested Mail Receiver policy for all subdomains (plain-text;
            //   OPTIONAL).  Indicates the policy to be enacted by the Receiver at
            //   the request of the Domain Owner.  It applies only to subdomains of
            //   the domain queried and not to the domain itself.  Its syntax is
            //   identical to that of the "p" tag defined above.  If absent, the
            //   policy specified by the "p" tag MUST be applied for subdomains.
            //   Note that "sp" will be ignored for DMARC records published on
            //   subdomains of Organizational Domains due to the effect of the
            //   DMARC policy discovery mechanism described in Section 6.6.3.
            const QString subDomainPolicy = QStringView(elem).mid(3).toString();
            if (subDomainPolicy != QLatin1StringView("none") && subDomainPolicy != QLatin1StringView("quarantine")
                && subDomainPolicy != QLatin1StringView("reject")) {
                return false;
            }
            mSubDomainPolicy = subDomainPolicy;
        }
    }
    if (mAdkim.isEmpty() && mVersion == QLatin1StringView("DMARC1")) {
        mAdkim = u'r';
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
    return mVersion == other.version() && mAdkim == other.adkim() && mPolicy == other.policy() && mSubDomainPolicy == other.subDomainPolicy()
        && mPercentage == other.percentage();
}

QDebug operator<<(QDebug d, const DMARCInfo &t)
{
    d << " mVersion " << t.version();
    d << " mAdkim " << t.adkim();
    d << " mPolicy " << t.policy();
    d << " mSubDomainPolicy " << t.subDomainPolicy();
    d << " mPercentage " << t.percentage();
    return d;
}
