/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H
#include <QObject>
#include "messagecomposer_export.h"

class KActionCollection;
namespace MessageComposer
{
class PluginEditorInterface;
class PluginEditorPrivate;
class MESSAGECOMPOSER_EXPORT PluginEditor : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditor(QObject *parent = Q_NULLPTR);
    ~PluginEditor();
    void setOrder(int order);

    virtual PluginEditorInterface *createInterface(KActionCollection *ac, QObject *parent = Q_NULLPTR) = 0;
    int order() const;
private:
    PluginEditorPrivate *const d;
};
}
#endif // PLUGINEDITOR_H
