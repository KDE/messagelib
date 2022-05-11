/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composer/nearexpirychecker.h"
#include "composer/nearexpirychecker_p.h"

#include "messagecomposer_debug.h"

#include <Libkleo/Dn>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>

#include <gpgme++/keylistresult.h>

#include <KLocalizedString>

#include <QString>

using namespace MessageComposer;

NearExpiryChecker::NearExpiryChecker(int encrKeyNearExpiryThresholdDays,
                                     int signKeyNearExpiryThresholdDays,
                                     int encrRootCertNearExpiryThresholdDays,
                                     int signRootCertNearExpiryThresholdDays,
                                     int encrChainCertNearExpiryThresholdDays,
                                     int signChainCertNearExpiryThresholdDays)
    : d(new NearExpiryCheckerPrivate)
{
    d->encryptKeyNearExpiryWarningThreshold = encrKeyNearExpiryThresholdDays;
    d->signingKeyNearExpiryWarningThreshold = signKeyNearExpiryThresholdDays;
    d->encryptRootCertNearExpiryWarningThreshold = encrRootCertNearExpiryThresholdDays;
    d->signingRootCertNearExpiryWarningThreshold = signRootCertNearExpiryThresholdDays;
    d->encryptChainCertNearExpiryWarningThreshold = encrChainCertNearExpiryThresholdDays;
    d->signingChainCertNearExpiryWarningThreshold = signChainCertNearExpiryThresholdDays;
}

NearExpiryChecker::~NearExpiryChecker() = default;

int NearExpiryChecker::encryptKeyNearExpiryWarningThresholdInDays() const
{
    return d->encryptKeyNearExpiryWarningThreshold;
}

int NearExpiryChecker::signingKeyNearExpiryWarningThresholdInDays() const
{
    return d->signingKeyNearExpiryWarningThreshold;
}

int NearExpiryChecker::encryptRootCertNearExpiryWarningThresholdInDays() const
{
    return d->encryptRootCertNearExpiryWarningThreshold;
}

int NearExpiryChecker::signingRootCertNearExpiryWarningThresholdInDays() const
{
    return d->signingRootCertNearExpiryWarningThreshold;
}

int NearExpiryChecker::encryptChainCertNearExpiryWarningThresholdInDays() const
{
    return d->encryptChainCertNearExpiryWarningThreshold;
}

int NearExpiryChecker::signingChainCertNearExpiryWarningThresholdInDays() const
{
    return d->signingChainCertNearExpiryWarningThreshold;
}

QString formatOpenPGPMessage(const GpgME::Key& key, int secsTillExpiry, bool isOwnKey, bool isSigningKey)
{
    KLocalizedString msg;
    static const double secsPerDay = 24 * 60 * 60;
    const int daysTillExpiry = 1 + int(abs(secsTillExpiry) / secsPerDay);
    if (secsTillExpiry <= 0) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Key 0x" << key.shortKeyID() << " expired " << daysTillExpiry << " days ago";
        if (isSigningKey) {
            msg = ki18np("<p>Your OpenPGP signing key</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                            "<p>expired less than a day ago.</p>",
                            "<p>Your OpenPGP signing key</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                            "<p>expired %1 days ago.</p>");
        } else if (isOwnKey) {
            msg = ki18np("<p>Your OpenPGP encryption key</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expired less than a day ago.</p>",
                         "<p>Your OpenPGP encryption key</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expired %1 days ago.</p>");
        } else {
            msg = ki18np("<p>The OpenPGP key for</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expired less than a day ago.</p>",
                         "<p>The OpenPGP key for</p><p align=center><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expired %1 days ago.</p>");
        }
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Key 0x" << key.shortKeyID() << " expires in less than " << daysTillExpiry << " days";
        if (isSigningKey) {
            msg = ki18np("<p>Your OpenPGP signing key</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than a day.</p>",
                         "<p>Your OpenPGP signing key</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than %1 days.</p>");
        } else if (isOwnKey) {
            msg = ki18np("<p>Your OpenPGP encryption key</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than a day.</p>",
                         "<p>Your OpenPGP encryption key</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than %1 days.</p>");
        } else {
            msg = ki18np("<p>The OpenPGP key for</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than a day.</p>",
                         "<p>The OpenPGP key for</p><p align=\"center\"><b>%2</b> (KeyID 0x%3)</p>"
                         "<p>expires in less than %1 days.</p>");
        }
    }
    return msg.subs(daysTillExpiry)
                .subs(QString::fromUtf8(key.userID(0).id()))
                .subs(QString::fromLatin1(key.shortKeyID()))
                .toString();
}

QString formatSMIMEMessage(const GpgME::Key& key, const GpgME::Key& orig_key, int secsTillExpiry, bool isOwnKey, bool isSigningKey, bool ca)
{
    KLocalizedString msg;
    static const double secsPerDay = 24 * 60 * 60;
    const int daysTillExpiry = 1 + int(abs(secsTillExpiry) / secsPerDay);
    if (secsTillExpiry <= 0) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Key 0x" << key.shortKeyID() << " expired " << daysTillExpiry << " days ago";
        if (ca) {
            if (key.isRoot()) {
                if (isSigningKey) {
                    msg = ki18np("<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired less than a day ago.</p>",
                                 "<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired %1 days ago.</p>");
                } else if (isOwnKey) {
                    msg = ki18np("<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired less than a day ago.</p>",
                                 "<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired %1 days ago.</p>");
                } else {
                    msg = ki18np("<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired less than a day ago.</p>",
                                 "<p>The root certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired %1 days ago.</p>");
                }
            } else {
                if (isSigningKey) {
                    msg = ki18np("<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired less than a day ago.</p>",
                                 "<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                 "<p>expired %1 days ago.</p>");
                } else if (isOwnKey) {
                    msg =  ki18np("<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                  "<p>for your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                  "<p>expired less than a day ago.</p>",
                                  "<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                  "<p>for your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                  "<p>expired %1 days ago.</p>");
                } else {
                    msg =  ki18np("<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                  "<p>for S/MIME certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                  "<p>expired less than a day ago.</p>",
                                  "<p>The intermediate CA certificate</p><p align=center><b>%4</b></p>"
                                  "<p>for S/MIME certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                                  "<p>expired %1 days ago.</p>");
                }
            }
            return msg.subs(daysTillExpiry)
                       .subs(Kleo::DN(orig_key.userID(0).id()).prettyDN())
                       .subs(QString::fromLatin1(orig_key.issuerSerial()))
                       .subs(Kleo::DN(key.userID(0).id()).prettyDN())
                       .toString();
        } else {
            if (isSigningKey) {
                msg = ki18np("<p>Your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired less than a day ago.</p>",
                             "<p>Your S/MIME signing certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired %1 days ago.</p>");
            } else if (isOwnKey) {
                msg = ki18np("<p>Your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired less than a day ago.</p>",
                             "<p>Your S/MIME encryption certificate</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired %1 days ago.</p>");
            } else {
                msg = ki18np("<p>The S/MIME certificate for</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired less than a day ago.</p>",
                             "<p>The S/MIME certificate for</p><p align=center><b>%2</b> (serial number %3)</p>"
                             "<p>expired %1 days ago.</p>");
            }
            return msg.subs(daysTillExpiry)
                      .subs(Kleo::DN(key.userID(0).id()).prettyDN())
                      .subs(QString::fromLatin1(key.issuerSerial()))
                      .toString();
        }
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Key 0x" << key.shortKeyID() << " expires in less than " << daysTillExpiry << " days";
        if (ca) {
            if (key.isRoot()) {
                if (isSigningKey) {
                    msg = ki18np("<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3)</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");
                } else if (isOwnKey) {
                    msg = ki18np("<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");
                } else {
                    msg = ki18np("<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The root certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");

                }
            } else {
                if (isSigningKey) {
                    msg = ki18np("<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");
                } else if (isOwnKey) {
                    msg = ki18np("<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");
                } else {
                    msg = ki18np("<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than a day.</p>",
                                 "<p>The intermediate CA certificate</p><p align=\"center\"><b>%4</b></p>"
                                 "<p>for S/MIME certificate</p><p align=center><b>%2</b> (serial number %3);</p>"
                                 "<p>expires in less than %1 days.</p>");
                }
            }
            return msg.subs(daysTillExpiry)
                       .subs(Kleo::DN(orig_key.userID(0).id()).prettyDN())
                       .subs(QString::fromLatin1(orig_key.issuerSerial()))
                       .subs(Kleo::DN(key.userID(0).id()).prettyDN())
                       .toString();
        } else {
            if (isSigningKey) {
                msg = ki18np("<p>Your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3)</p>"
                             "<p>expires in less than a day.</p>",
                             "<p>Your S/MIME signing certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                             "<p>expires in less than %1 days.</p>");
            } else if (isOwnKey) {
                msg = ki18np("<p>Your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                             "<p>expires in less than a day.</p>",
                             "<p>Your S/MIME encryption certificate</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                             "<p>expires in less than %1 days.</p>");
            } else {
                msg = ki18np("<p>The S/MIME certificate for</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                             "<p>expires in less than a day.</p>",
                             "<p>The S/MIME certificate for</p><p align=\"center\"><b>%2</b> (serial number %3);</p>"
                             "<p>expires in less than %1 days.</p>");

            }
            return msg.subs(daysTillExpiry)
                      .subs(Kleo::DN(key.userID(0).id()).prettyDN())
                      .subs(QString::fromLatin1(key.issuerSerial()))
                      .toString();
        }
    }
}

double MessageComposer::NearExpiryChecker::calculateSecsTillExpiriy(const GpgME::Subkey& key) const
{
    if(d->testMode) {
        return d->difftime;
    }

    return ::difftime(key.expirationTime(), time(nullptr));
}


void NearExpiryChecker::checkKeyNearExpiry(const GpgME::Key& key,
                                           bool isOwnKey,
                                           bool isSigningKey,
                                           bool ca,
                                           int recur_limit,
                                           const GpgME::Key& orig_key) const
{
    if (recur_limit <= 0) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Key chain too long (>100 certs)";
        return;
    }
    const GpgME::Subkey subkey = key.subkey(0);
    if (d->alreadyWarnedFingerprints.count(subkey.fingerprint())) {
        return; // already warned about this one (and so about it's issuers)
    }

    if (subkey.neverExpires()) {
        return;
    }
    static const double secsPerDay = 24 * 60 * 60;
    const double secsTillExpiry = calculateSecsTillExpiriy(subkey);
    if (secsTillExpiry <= 0) {
        const QString msg = key.protocol() == GpgME::OpenPGP
            ? formatOpenPGPMessage(key, secsTillExpiry, isOwnKey, isSigningKey)
            : formatSMIMEMessage(key, orig_key, secsTillExpiry, isOwnKey, isSigningKey, ca);
        d->alreadyWarnedFingerprints.insert(subkey.fingerprint());
        Q_EMIT expiryMessage(key, msg, isOwnKey ? OwnKeyExpired : OtherKeyExpired);
    } else {
        const int daysTillExpiry = 1 + int(secsTillExpiry / secsPerDay);
        const int threshold = ca
            ? (key.isRoot() ? (isSigningKey ? signingRootCertNearExpiryWarningThresholdInDays() : encryptRootCertNearExpiryWarningThresholdInDays())
                            : (isSigningKey ? signingChainCertNearExpiryWarningThresholdInDays() : encryptChainCertNearExpiryWarningThresholdInDays()))
            : (isSigningKey ? signingKeyNearExpiryWarningThresholdInDays() : encryptKeyNearExpiryWarningThresholdInDays());
        if (threshold > -1 && daysTillExpiry <= threshold) {
            const QString msg = key.protocol() == GpgME::OpenPGP
                ? formatOpenPGPMessage(key, secsTillExpiry, isOwnKey, isSigningKey)
                : formatSMIMEMessage(key, orig_key, secsTillExpiry, isOwnKey, isSigningKey, ca);
            d->alreadyWarnedFingerprints.insert(subkey.fingerprint());
            Q_EMIT expiryMessage(key, msg, isOwnKey ? OwnKeyNearExpiry : OtherKeyNearExpiry);
        }
    }
    if (key.isRoot()) {
        return;
    } else if (key.protocol() != GpgME::CMS) {     // Key chaining is only possible in SMIME
        return;
    } else if (const char *chain_id = key.chainID()) {
        QGpgME::Protocol *p = QGpgME::smime();
        Q_ASSERT(p);
        std::unique_ptr<QGpgME::KeyListJob> job(p->keyListJob(false, false, true));
        if (job.get()) {
            std::vector<GpgME::Key> keys;
            job->exec(QStringList(QLatin1String(chain_id)), false, keys);
            if (!keys.empty()) {
                return checkKeyNearExpiry(keys.front(), isOwnKey, isSigningKey, true, recur_limit - 1, ca ? orig_key : key);
            }
        }
    }
}

void NearExpiryChecker::checkOwnSigningKey(const GpgME::Key& key) const
{
    checkKeyNearExpiry(key, /*isOwnKey*/ true, /*isSigningKey*/ true);
}

void NearExpiryChecker::checkOwnKey(const GpgME::Key& key) const
{
    checkKeyNearExpiry(key, /*isOwnKey*/ true, /*isSigningKey*/ false);
}

void NearExpiryChecker::checkKey(const GpgME::Key& key) const
{
    checkKeyNearExpiry(key, false, false);
}
