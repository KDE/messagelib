/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <KLocalizedString>
#include <QLineEdit>

using namespace MessageViewer;
RemoteContentWidget::RemoteContentWidget(QWidget *parent)
    : QWidget(parent)
    , mLineEdit(new QLineEdit(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});
    auto label = new QLabel(i18n("Url"), this);
    label->setObjectName(QStringLiteral("label"));
    mainLayout->addWidget(label);

    mLineEdit->setObjectName(QStringLiteral("mLineEdit"));
    mainLayout->addWidget(mLineEdit);
}

RemoteContentWidget::~RemoteContentWidget()
{

}

RemoteContentInfo RemoteContentWidget::info() const
{
    RemoteContentInfo info;
    info.setUrl(mLineEdit->text());
    //info.setStatus() TODO implement
    //TODO
    return info;
}

void RemoteContentWidget::setInfo(const RemoteContentInfo &info)
{
    mLineEdit->setText(info.url());
    //mLineEdit->setText(info.status());
    //TODO
}
