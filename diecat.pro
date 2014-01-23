#
#    diecat
#    A2L/HEX file reader.
#
#    File: diecat.pro
#
#    Copyright (C) 2013-2014 Artem Petrov <pa2311@gmail.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU General Public License for more details.
#    You should have received a copy of the GNU General Public License
#    along with this program. If not, see <http://www.gnu.org/licenses/>.
#

QT += core gui widgets

TARGET = diecat
TEMPLATE = app

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/a2l.cpp \
    src/ecuscalar.cpp \
    src/intelhex.cpp \
    src/labelinfodialog.cpp

HEADERS += src/mainwindow.hpp \
    src/constants.hpp \
    src/a2l.hpp \
    src/ecuscalar.hpp \
    src/intelhex.hpp \
    src/labelinfodialog.hpp

FORMS += forms/mainwindow.ui \
    forms/labelinfodialog.ui

RESOURCES = res/diecat.qrc
RC_FILE += res/diecat.rc

QMAKE_CXXFLAGS += -std=c++11 -W -pedantic

unix: {
    DESTDIR = build/unix/bin
    MOC_DIR = build/unix/moc
    RCC_DIR = build/unix/rc
    UI_DIR = build/unix/ui_h
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
    target.path = $$PREFIX/bin
    INSTALLS += target
}

win32: {
    DESTDIR = build/win/bin
    MOC_DIR = build/win/moc
    RCC_DIR = build/win/rc
    UI_DIR = build/win/ui_h
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/win/debug
    }
    else {
        OBJECTS_DIR = build/win/release
    }
}
