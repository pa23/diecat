/*
    diecat
    A2L/HEX file reader.

    File: labelinfodialog.cpp

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

#include "labelinfodialog.hpp"
#include "ui_labelinfodialog.h"

LabelInfoDialog::LabelInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabelInfoDialog) {

    ui->setupUi(this);

    ui->tableWidget_Description->setRowCount(11);
    ui->tableWidget_Description->setItem(0, 0, new QTableWidgetItem("Name"));
    ui->tableWidget_Description->setItem(0, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(1, 0, new QTableWidgetItem("Description"));
    ui->tableWidget_Description->setItem(1, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(2, 0, new QTableWidgetItem("Address"));
    ui->tableWidget_Description->setItem(2, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(3, 0, new QTableWidgetItem("Numeric type"));
    ui->tableWidget_Description->setItem(3, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(4, 0, new QTableWidgetItem("Scalar type"));
    ui->tableWidget_Description->setItem(4, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(5, 0, new QTableWidgetItem("Min value (soft)"));
    ui->tableWidget_Description->setItem(5, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(6, 0, new QTableWidgetItem("Max value (soft)"));
    ui->tableWidget_Description->setItem(6, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(7, 0, new QTableWidgetItem("Min value (hard)"));
    ui->tableWidget_Description->setItem(7, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(8, 0, new QTableWidgetItem("Max value (hard)"));
    ui->tableWidget_Description->setItem(8, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(9, 0, new QTableWidgetItem("Read only"));
    ui->tableWidget_Description->setItem(9, 1, new QTableWidgetItem(""));
    ui->tableWidget_Description->setItem(10, 0, new QTableWidgetItem("Dimension"));
    ui->tableWidget_Description->setItem(10, 1, new QTableWidgetItem(""));

    for ( ptrdiff_t i=0; i<ui->tableWidget_Description->rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<ui->tableWidget_Description->columnCount(); j++ ) {
            ui->tableWidget_Description->item(i, j)->
                    setFlags(ui->tableWidget_Description->item(i, j)->flags() ^ Qt::ItemIsEditable);
        }
    }

    ui->tableWidget_Description->resizeRowsToContents();
    ui->tableWidget_Description->resizeColumnsToContents();
}

LabelInfoDialog::~LabelInfoDialog() {

    delete ui;
}
