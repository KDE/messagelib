/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on Akonadi code by:
  SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef QTEST_MESSAGECORE_H
#define QTEST_MESSAGECORE_H

/**
 * Runs a KJob synchronously and aborts if the job failed.
 * Similar to QVERIFY( job->exec() ) but includes the job error message
 * in the output in case of a failure.
 */
#define VERIFYEXEC(job) \
    QVERIFY2(job->exec(), job->errorString().toUtf8().constData())

#endif
