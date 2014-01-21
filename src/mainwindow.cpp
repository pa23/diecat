/*
    diecat
    A2L/HEX file reader.

    File: mainwindow.cpp

    Copyright (C) 2013-2014 Artem Petrov <pa2311@gmail.com>

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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "constants.hpp"
#include "a2l.hpp"
#include "ecuscalar.hpp"
#include "intelhex.hpp"
#include "labelinfodialog.hpp"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QColor>
#include <QComboBox>
#include <QItemDelegate>
#include <QTableWidget>
#include <QTableWidgetSelectionRange>
#include <QTime>
#include <QDateTime>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>

A2L *globA2L = 0;
IntelHEX *globIHEX = 0;
QVector< QSharedPointer<ECUScalar> > *globScalars = 0;

void parseA2L() {

    if ( !globA2L->readFile() ) {
        QMessageBox::critical(0, QString(PROGNAME) + ": error", "Error occured during a2l file parsing!");
        globA2L->clear();
        return;
    }

    globA2L->fillScalarsInfo(*globScalars);
    globA2L->clear();
}

void readHEX() {

    if ( !globIHEX->readValues(*globScalars) ) {
        QMessageBox::critical(0, QString(PROGNAME) + ": error", "Error occured during hex file reading!");
    }

    globIHEX->clear();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_labelInfoDialog(new LabelInfoDialog(this)),
    m_progSettings("pa23software", PROGNAME) {

    ui->setupUi(this);

    //

    QCoreApplication::setApplicationName(QString(PROGNAME));
    QCoreApplication::setApplicationVersion(QString(PROGVER));

    this->setWindowTitle(QString(PROGNAME) + " v" + QString(PROGVER));

    //

    readProgramSettings();

    //

    connect(ui->lineEdit_QuickSearch, SIGNAL(textChanged(QString)), this, SLOT(searchTemplChanged(QString)));
}

MainWindow::~MainWindow() {

    writeProgramSettings();

    delete ui;
}

void MainWindow::on_action_OpenProject_triggered() {

    const QString a2lFileName(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open a2l file..."),
                    m_lastA2LPath,
                    QString::fromLatin1("a2l files (*.a2l);;All files (*)"),
                    0, 0)
                );

    if ( a2lFileName.isEmpty() ) {
        return;
    }

    const QFileInfo a2lFileInfo(a2lFileName);
    m_lastA2LPath = a2lFileInfo.absolutePath();

    const QString hexFileName(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open hex file..."),
                    m_lastHEXPath,
                    QString::fromLatin1("hex files (*.hex);;All files (*)"),
                    0, 0)
                );

    if ( hexFileName.isEmpty() ) {
        return;
    }

    const QFileInfo hexFileInfo(hexFileName);
    m_lastHEXPath = hexFileInfo.absolutePath();

    //

    ui->lineEdit_QuickSearch->clear();
    ui->tableWidget_Labels->setRowCount(0);
    ui->tableWidget_Scalars->setRowCount(0);
    m_scalars.clear();

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //

    QTime timer;

    timer.start();
    readA2LInfo(a2lFileName);
    ui->plainTextEdit_log->appendPlainText(
                QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                + " Parsing " + a2lFileName + ": "
                + QString::number(timer.elapsed()) + " ms"
                );

    timer.restart();
    readHEXData(hexFileName);
    ui->plainTextEdit_log->appendPlainText(
                QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                + " Reading " + hexFileName + ": "
                + QString::number(timer.elapsed()) + " ms"
                );

    timer.restart();
    showLabels();
    ui->plainTextEdit_log->appendPlainText(
                QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                + " Displaying labels: "
                + QString::number(timer.elapsed()) + " ms"
                );

    ui->groupBox_Labels->setTitle("Labels (" + QString::number(m_scalars.size()) + ")");
}

void MainWindow::on_action_OpenA2L_triggered() {

    const QString a2lFileName(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open a2l file..."),
                    m_lastA2LPath,
                    QString::fromLatin1("a2l files (*.a2l);;All files (*)"),
                    0, 0)
                );

    if ( a2lFileName.isEmpty() ) {
        return;
    }

    const QFileInfo a2lFileInfo(a2lFileName);
    m_lastA2LPath = a2lFileInfo.absolutePath();

    //

    ui->lineEdit_QuickSearch->clear();
    ui->tableWidget_Labels->setRowCount(0);
    ui->tableWidget_Scalars->setRowCount(0);
    m_scalars.clear();

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //

    QTime timer;

    timer.start();
    readA2LInfo(a2lFileName);
    ui->plainTextEdit_log->appendPlainText(
                QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                + " Parsing " + a2lFileName + ": "
                + QString::number(timer.elapsed()) + " ms"
                );

    timer.restart();
    showLabels();
    ui->plainTextEdit_log->appendPlainText(
                QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                + " Displaying labels: "
                + QString::number(timer.elapsed()) + " ms"
                );

    ui->groupBox_Labels->setTitle("Labels (" + QString::number(m_scalars.size()) + ")");
}

void MainWindow::on_action_SearchLine_triggered() {

    ui->lineEdit_QuickSearch->setFocus();
    ui->lineEdit_QuickSearch->selectAll();
}

void MainWindow::on_action_Select_triggered() {

    QTime timer;
    timer.start();

    //

    QTableWidgetSelectionRange selectedRange;

    for ( ptrdiff_t n=0; n<ui->tableWidget_Labels->selectedRanges().size(); n++ ) {

        selectedRange = ui->tableWidget_Labels->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            ui->tableWidget_Labels->
                    item(selectedRange.topRow()+i, selectedRange.leftColumn())->
                    setTextColor(QColor(Qt::red));
            addParameterToTable(selectedRange.topRow()+i);
        }
    }

    ptrdiff_t ind = ui->tableWidget_Labels->currentRow();

    if ( ind != (ui->tableWidget_Labels->rowCount()-1) ) {
        ui->tableWidget_Labels->setCurrentCell(ind+1, 0);
    }

    ui->tableWidget_Scalars->resizeRowsToContents();
    ui->tableWidget_Scalars->resizeColumnsToContents();

    //

    ui->plainTextEdit_log->appendPlainText(
                    QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                    + " Displaying values: "
                    + QString::number(timer.elapsed()) + " ms"
                    );
}

void MainWindow::on_action_Unselect_triggered() {

    QTime timer;
    timer.start();

    //

    QTableWidgetSelectionRange selectedRange;

    for ( ptrdiff_t n=0; n<ui->tableWidget_Labels->selectedRanges().size(); n++ ) {

        selectedRange = ui->tableWidget_Labels->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            ui->tableWidget_Labels->
                    item(selectedRange.topRow()+i, selectedRange.leftColumn())->
                    setTextColor(QColor(Qt::black));
            deleteParameterFromTable(selectedRange.topRow()+i);
        }
    }

    ptrdiff_t ind = ui->tableWidget_Labels->currentRow();

    if ( ind != (ui->tableWidget_Labels->rowCount()-1) ) {
        ui->tableWidget_Labels->setCurrentCell(ind+1, 0);
    }

    ui->tableWidget_Scalars->resizeRowsToContents();
    ui->tableWidget_Scalars->resizeColumnsToContents();

    //

    ui->plainTextEdit_log->appendPlainText(
                    QDateTime::currentDateTime().toString("[yyyy-MM-dd_hh-mm-ss]")
                    + " Cleaning values table: "
                    + QString::number(timer.elapsed()) + " ms"
                    );
}

void MainWindow::on_action_LabelInfo_triggered() {

    QTableWidget *tableWidget_Description =
            m_labelInfoDialog->findChild<QTableWidget *>("tableWidget_Description");

    ptrdiff_t currItemInd = ui->tableWidget_Labels->currentRow();

    if ( currItemInd < 0 ) {
        return;
    }

    tableWidget_Description->item(0, 1)->setText(m_scalars[currItemInd]->name());
    tableWidget_Description->item(1, 1)->setText(m_scalars[currItemInd]->shortDescription());
    tableWidget_Description->item(2, 1)->setText(m_scalars[currItemInd]->address());
    tableWidget_Description->item(3, 1)->setText(m_scalars[currItemInd]->numType());

    if ( m_scalars[currItemInd]->type() == VARTYPE_SCALAR_NUM ) {
        tableWidget_Description->item(4, 1)->setText("Numeric");
    }
    else if ( m_scalars[currItemInd]->type() == VARTYPE_SCALAR_VTAB ) {
        tableWidget_Description->item(4, 1)->setText("VTable");
    }

    tableWidget_Description->item(5, 1)->setText(
                QString::number(m_scalars[currItemInd]->minValueSoft(), 'f', m_scalars[currItemInd]->precision())
                );
    tableWidget_Description->item(6, 1)->setText(
                QString::number(m_scalars[currItemInd]->maxValueSoft(), 'f', m_scalars[currItemInd]->precision())
                );
    tableWidget_Description->item(7, 1)->setText(
                QString::number(m_scalars[currItemInd]->minValueHard(), 'f', m_scalars[currItemInd]->precision())
                );
    tableWidget_Description->item(8, 1)->setText(
                QString::number(m_scalars[currItemInd]->maxValueHard(), 'f', m_scalars[currItemInd]->precision())
                );

    if ( m_scalars[currItemInd]->isReadOnly() ) {
        tableWidget_Description->item(9, 1)->setText("true");
    }
    else {
        tableWidget_Description->item(9, 1)->setText("false");
    }

    tableWidget_Description->item(10, 1)->setText(m_scalars[currItemInd]->dimension());

    tableWidget_Description->resizeColumnsToContents();

    //

    m_labelInfoDialog->show();
}

void MainWindow::on_action_About_triggered() {

    const QString str =
            "<b>" + QString(PROGNAME) + " v" + QString(PROGVER) + "</b> "
            + "[" + QString(__DATE__) + " " + QString(__TIME__) + "]<br>"
            + "A2L/HEX file reader.<br><br>"
            "Copyright (C) 2013-2014 Artem Petrov "
            "<a href=\"mailto:pa2311@gmail.com\">pa2311@gmail.com</a><br><br>"
            "Source code hosting: "
            "<a href=\"https://github.com/pa23/diecat\">https://github.com/pa23/diecat</a><br>"
            "Author's blog (RU): "
            "<a href=\"http://pa2311.blogspot.com\">http://pa2311.blogspot.com</a><br><br>"
            "This program is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as "
            "published by the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.<br>"
            "This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
            "GNU General Public License for more details.<br>"
            "You should have received a copy of the GNU General Public License "
            "along with this program. If not, see <a href=\"http://www.gnu.org/licenses/\">"
            "http://www.gnu.org/licenses/</a>.<br>";

    QMessageBox::about(this, "About " + QString(PROGNAME), str);
}

void MainWindow::searchTemplChanged(QString templ) {

    for ( ptrdiff_t i=0; i<ui->tableWidget_Labels->rowCount(); i++ ) {

        if ( ui->tableWidget_Labels->item(i, 0)->text().startsWith(templ, Qt::CaseInsensitive) ) {
            ui->tableWidget_Labels->setCurrentCell(i, 0);
            break;
        }
    }
}

void MainWindow::writeProgramSettings() {

    m_progSettings.beginGroup("/settings");
    m_progSettings.setValue("/window_geometry", geometry());
    m_progSettings.setValue("/last_a2l_path", m_lastA2LPath);
    m_progSettings.setValue("/last_hex_path", m_lastHEXPath);
    m_progSettings.endGroup();
}

void MainWindow::readProgramSettings() {

    m_progSettings.beginGroup("/settings");
    setGeometry(m_progSettings.value("/window_geometry", QRect(20, 40, 0, 0)).toRect());
    m_lastA2LPath = m_progSettings.value("/last_a2l_path", QDir::currentPath()).toString();
    m_lastHEXPath = m_progSettings.value("/last_hex_path", QDir::currentPath()).toString();
    m_progSettings.endGroup();
}

void MainWindow::addParameterToTable(ptrdiff_t ind) {

    if ( m_scalarsInTable[ind] ) {
        return;
    }

    ptrdiff_t varType = m_scalars[ind]->type();

    ptrdiff_t tblRow = ui->tableWidget_Scalars->rowCount();
    ui->tableWidget_Scalars->setRowCount(tblRow + 1);

    if ( varType == VARTYPE_SCALAR_NUM ) {

        ui->tableWidget_Scalars->setItem(tblRow, 0, new QTableWidgetItem(m_scalars[ind]->name()));
        ui->tableWidget_Scalars->item(tblRow, 0)->
                setFlags(ui->tableWidget_Scalars->item(tblRow, 0)->flags() ^ Qt::ItemIsEditable);

        ui->tableWidget_Scalars->setItem(tblRow, 1, new QTableWidgetItem(m_scalars[ind]->value()));
        ui->tableWidget_Scalars->item(tblRow, 1)->setTextColor(QColor(Qt::blue));

        ui->tableWidget_Scalars->setItem(tblRow, 2, new QTableWidgetItem(m_scalars[ind]->dimension()));
        ui->tableWidget_Scalars->item(tblRow, 2)->
                setFlags(ui->tableWidget_Scalars->item(tblRow, 2)->flags() ^ Qt::ItemIsEditable);

        m_scalarsInTable[ind] = true;
    }
    else if ( varType == VARTYPE_SCALAR_VTAB ) {

        ui->tableWidget_Scalars->setItem(tblRow, 0, new QTableWidgetItem(m_scalars[ind]->name()));
        ui->tableWidget_Scalars->item(tblRow, 0)->
                setFlags(ui->tableWidget_Scalars->item(tblRow, 0)->flags() ^ Qt::ItemIsEditable);

        m_comboBox_vTable = new QComboBox(ui->tableWidget_Scalars);
        m_comboBox_vTable->setMinimumWidth(230);
        ui->tableWidget_Scalars->setCellWidget(tblRow, 1, m_comboBox_vTable);

        m_comboBox_vTable->addItems(m_scalars[ind]->vTable());
        m_comboBox_vTable->setCurrentIndex(m_scalars[ind]->value().toInt());

        ui->tableWidget_Scalars->setItem(tblRow, 2, new QTableWidgetItem(m_scalars[ind]->dimension()));
        ui->tableWidget_Scalars->item(tblRow, 2)->
                setFlags(ui->tableWidget_Scalars->item(tblRow, 2)->flags() ^ Qt::ItemIsEditable);

        m_scalarsInTable[ind] = true;
    }
}

void MainWindow::deleteParameterFromTable(ptrdiff_t ind) {

    for ( ptrdiff_t i=0; i<ui->tableWidget_Scalars->rowCount(); i++ ) {

        if ( ui->tableWidget_Scalars->item(i, 0)->text() ==
             ui->tableWidget_Labels->item(ind, 0)->text() ) {

            ui->tableWidget_Scalars->removeRow(i);
            m_scalarsInTable[ind] = false;
        }
    }
}

void MainWindow::readA2LInfo(const QString &filepath) {

    ui->statusBar->showMessage("Parsing a2l file. It may take a long time. Please wait...");
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //

    QSharedPointer<A2L> a2l(new A2L(filepath));
    globA2L = a2l.data();
    globScalars = &m_scalars;

    QFuture<void> sepThrFun = QtConcurrent::run(parseA2L);
    sepThrFun.waitForFinished();

    globA2L = 0;
    globScalars = 0;

    //

    ui->statusBar->clearMessage();
}

void MainWindow::readHEXData(const QString &filepath) {

    ui->statusBar->showMessage("Reading hex file. It may take a long time. Please wait...");
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //

    QSharedPointer<IntelHEX> ihex(new IntelHEX(filepath));
    globIHEX = ihex.data();
    globScalars = &m_scalars;

    QFuture<void> sepThrFun = QtConcurrent::run(readHEX);
    sepThrFun.waitForFinished();

    globIHEX = 0;
    globScalars = 0;

    //

    ui->statusBar->clearMessage();
}

void MainWindow::showLabels() {

    ui->statusBar->showMessage("Displaying labels. Please wait...");
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //

    m_scalarsInTable = QVector<bool>(m_scalars.size());
    ui->tableWidget_Labels->setRowCount(m_scalars.size());

    for ( ptrdiff_t i=0; i<m_scalars.size(); i++ ) {
        ui->tableWidget_Labels->setItem(i, 0, new QTableWidgetItem(m_scalars[i]->name()));
        ui->tableWidget_Labels->item(i, 0)->
                setFlags(ui->tableWidget_Labels->item(i, 0)->flags() ^ Qt::ItemIsEditable);
    }

    ui->tableWidget_Labels->resizeRowsToContents();
    ui->tableWidget_Labels->resizeColumnsToContents();

    //

    ui->statusBar->clearMessage();
}
