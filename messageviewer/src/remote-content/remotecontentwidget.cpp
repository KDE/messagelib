/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

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
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mLineEdit->setObjectName(QLatin1StringView("mLineEdit"));
    mLineEdit->setClearButtonEnabled(true);
    auto label = new QLabel(i18nc("@label:textbox", "Domain:"), this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(mLineEdit);

    mStatusComboBox->setObjectName(QLatin1StringView("mStatusComboBox"));
    mainLayout->addWidget(mStatusComboBox);
    connect(mLineEdit, &QLineEdit::textChanged, this, &RemoteContentWidget::slotTextChanged);
}

RemoteContentWidget::~RemoteContentWidget() = default;

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

#include "moc_remotecontentwidget.cpp"
