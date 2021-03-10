/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidget.h"
#include "remotecontentstatustypecombobox.h"

#include <KLocalizedString>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace MessageViewer;
RemoteContentWidget::RemoteContentWidget(QWidget *parent)
    : QWidget(parent)
    , mLineEdit(new QLineEdit(this))
    , mStatusComboBox(new RemoteContentStatusTypeComboBox(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mLineEdit->setObjectName(QStringLiteral("mLineEdit"));
    mLineEdit->setClearButtonEnabled(true);
    auto label = new QLabel(i18n("Domain:"), this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(mLineEdit);

    mStatusComboBox->setObjectName(QStringLiteral("mStatusComboBox"));
    mainLayout->addWidget(mStatusComboBox);
    connect(mLineEdit, &QLineEdit::textChanged, this, &RemoteContentWidget::slotTextChanged);
}

RemoteContentWidget::~RemoteContentWidget()
{
}

void RemoteContentWidget::slotTextChanged(const QString &url)
{
    Q_EMIT updateOkButton(!url.trimmed().isEmpty());
}

RemoteContentInfo RemoteContentWidget::info() const
{
    RemoteContentInfo info;
    info.setUrl(mLineEdit->text());
    info.setStatus(mStatusComboBox->status());
    return info;
}

void RemoteContentWidget::setInfo(const RemoteContentInfo &info)
{
    mLineEdit->setText(info.url());
    mStatusComboBox->setStatus(info.status());
}
