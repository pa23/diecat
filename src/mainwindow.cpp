/*
    diecat
    Diesel engines calibration tool.

    File: mainwindow.cpp

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

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

#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_action_OpenProject_triggered() {
    //
}

void MainWindow::on_action_Quit_triggered() {
    close();
}

void MainWindow::on_action_About_triggered() {

    const QString str =
            "<b>" + QString(PROGNAME) + " v" + QString(PROGVER) + "</b> "
            + "(Date of build: " + QString(__DATE__) + ")<br>"
            + "Diesel engines calibration tool.<br><br>"
            "Copyright (C) 2013 Artem Petrov <a href= \"mailto:pa2311@gmail.com\" >pa2311@gmail.com</a><br><br>"
            "Source code hosting: <a href= \"https://github.com/pa23/diecat\">https://github.com/pa23/diecat</a><br>"
            "Author's blog (RU): <a href= \"http://pa2311.blogspot.com\">http://pa2311.blogspot.com</a><br><br>"
            "This program is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as "
            "published by the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.<br>"
            "This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
            "GNU General Public License for more details.<br>"
            "You should have received a copy of the GNU General Public License "
            "along with this program. If not, see <a href= \"http://www.gnu.org/licenses/\" >"
            "http://www.gnu.org/licenses/</a>.<br>";

    QMessageBox::about(this, tr("About diecat"), str);
}
