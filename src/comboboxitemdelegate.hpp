/*
    diecat
    Diesel engines calibration tool.

    File: comboboxitemdelegate.hpp

    Copyright (C) 2014 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMBOBOXITEMDELEGATE_HPP
#define COMBOBOXITEMDELEGATE_HPP

#include <QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate {

    Q_OBJECT

public:
    ComboBoxItemDelegate(QObject *parent = 0);
    ~ComboBoxItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

#endif // COMBOBOXITEMDELEGATE_HPP
