/*
    diecat
    Diesel engines calibration tool.

    File: comboboxitemdelegate.cpp

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

#include "comboboxitemdelegate.hpp"

#include <QComboBox>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

ComboBoxItemDelegate::~ComboBoxItemDelegate() {
}

QWidget* ComboBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    // combobox is only in column 2

    if ( index.column() != 1 ) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    // create the combobox and populate it

    QComboBox *comboBox = new QComboBox(parent);
    return comboBox;
}

void ComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

    if ( QComboBox *comboBox = qobject_cast<QComboBox *>(editor) ) {

        // get the index of the text in the combobox, that matches the current value of the item

        QString currentText = index.data(Qt::EditRole).toString();
        int comboBoxIndex = comboBox->findText(currentText);

        // if it is valid, adjust the combobox

        if ( comboBoxIndex >= 0 ) {
            comboBox->setCurrentIndex(comboBoxIndex);
        }
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    if ( QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {

        // save the current text of the combo box as the current value of the item

        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
