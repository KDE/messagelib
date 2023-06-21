/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "recipientseditortest.h"
#include <MessageComposer/RecipientsEditor>

#include <QDebug>

#include <KMessageBox>

#include <QApplication>
#include <QCommandLineParser>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

using namespace MessageComposer;

Composer::Composer(QWidget *parent)
    : QWidget(parent)
    , mRecipients(new RecipientsEditor(this))
{
    auto topLayout = new QGridLayout(this);
    topLayout->setContentsMargins(4, 4, 4, 4);
    topLayout->setSpacing(4);

    auto label = new QLabel(QStringLiteral("From:"), this);
    topLayout->addWidget(label, 0, 0);
    auto edit = new QLineEdit(this);
    topLayout->addWidget(edit, 0, 1);

    topLayout->addWidget(mRecipients, 1, 0, 1, 2);

    qDebug() << "SIZEHINT:" << mRecipients->sizeHint();

    //  mRecipients->setFixedHeight( 10 );

    auto editor = new QTextEdit(this);
    topLayout->addWidget(editor, 2, 0, 1, 2);
    topLayout->setRowStretch(2, 1);

    auto button = new QPushButton(QStringLiteral("&Close"), this);
    topLayout->addWidget(button, 3, 0, 1, 2);
    connect(button, &QPushButton::clicked, this, &Composer::slotClose);
}

void Composer::slotClose()
{
#if 0
    QString text;

    text += "<qt>";

    Recipient::List recipients = mRecipients->recipients();
    Recipient::List::ConstIterator it;
    for (it = recipients.begin(); it != recipients.end(); ++it) {
        text += "<b>" + (*it).typeLabel() + ":</b> " + (*it).email() + "<br/>";
    }

    text += "</qt>";

    KMessageBox::information(this, text);
#endif

    close();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    QWidget *wid = new Composer(nullptr);

    wid->show();

    int ret = app.exec();

    delete wid;

    return ret;
}

#include "moc_recipientseditortest.cpp"
