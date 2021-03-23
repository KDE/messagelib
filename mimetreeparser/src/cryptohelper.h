/*
    cryptohelper.h

    SPDX-FileCopyrightText: 2015 Sandro Knauß <knauss@kolabsys.com>
    SPDX-FileCopyrightText: 2001,2002 the KPGP authors
    See file AUTHORS.kpgp for details

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QByteArray>
#include <QVector>

namespace MimeTreeParser
{
enum PGPBlockType {
    UnknownBlock = -1, // BEGIN PGP ???
    NoPgpBlock = 0,
    PgpMessageBlock = 1, // BEGIN PGP MESSAGE
    MultiPgpMessageBlock = 2, // BEGIN PGP MESSAGE, PART X[/Y]
    SignatureBlock = 3, // BEGIN PGP SIGNATURE
    ClearsignedBlock = 4, // BEGIN PGP SIGNED MESSAGE
    PublicKeyBlock = 5, // BEGIN PGP PUBLIC KEY BLOCK
    PrivateKeyBlock = 6 // BEGIN PGP PRIVATE KEY BLOCK (PGP 2.x: ...SECRET...)
};

class Block
{
public:
    Block();
    Block(const QByteArray &m);

    Block(const QByteArray &m, PGPBlockType t);

    Q_REQUIRED_RESULT QByteArray text() const;
    Q_REQUIRED_RESULT PGPBlockType type() const;
    Q_REQUIRED_RESULT PGPBlockType determineType() const;

    QByteArray msg;
    PGPBlockType mType = UnknownBlock;
};

/** Parses the given message and splits it into OpenPGP blocks and
    Non-OpenPGP blocks.
*/
QVector<Block> prepareMessageForDecryption(const QByteArray &msg);
} // namespace MimeTreeParser

Q_DECLARE_TYPEINFO(MimeTreeParser::Block, Q_MOVABLE_TYPE);

