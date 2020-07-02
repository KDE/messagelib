/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCONVERTTEXT_H
#define PLUGINEDITORCONVERTTEXT_H

#include <QObject>
#include "messagecomposer_export.h"
namespace MessageComposer {
class PluginEditorConverttextPrivate;
class PluginEditorConvertTextInterface;
/**
 * @brief The PluginEditorConvertText class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertText : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorConvertText(QObject *parent = nullptr);
    ~PluginEditorConvertText();

    virtual PluginEditorConvertTextInterface *createInterface(QObject *parent) = 0;

    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void emitConfigChanged();

    Q_REQUIRED_RESULT virtual QString description() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

    Q_REQUIRED_RESULT virtual bool canWorkOnHtml() const;

    Q_REQUIRED_RESULT virtual bool hasStatusBarSupport() const;

    Q_REQUIRED_RESULT virtual bool hasPopupMenuSupport() const;

    Q_REQUIRED_RESULT virtual bool hasToolBarSupport() const;

Q_SIGNALS:
    void configChanged();

private:
    PluginEditorConverttextPrivate *const d;
};
}
#endif // PLUGINEDITORCONVERTTEXT_H
