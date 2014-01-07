/*
    diecat
    Diesel engines calibration tool.

    File: intelhex.cpp

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

#include "intelhex.hpp"
#include "ecuscalar.hpp"

#include <QString>
#include <QVector>
#include <QStringList>
#include <QSharedPointer>
#include <QIODevice>
#include <QFile>

IntelHEX::IntelHEX(const QString &path) {
    m_hexpath = path;
}

bool IntelHEX::readValues(QVector<QSharedPointer<ECUScalar> > &scalars) {

    //

    return true;
}

bool IntelHEX::readHex() {

    QFile hexfile(m_hexpath);

    if ( !hexfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        return false;
    }

    m_hexData.clear();
    ptrdiff_t n = 0;

    while ( !hexfile.atEnd() ) {

        QString s(hexfile.readLine().simplified());

        if ( s.isEmpty() ) {
            continue;
        }

        s = s.right(s.size()-1);

        m_hexData.push_back(s);

        if ( s.contains("0200000400") && (s.size() == 14) ) {
            m_hexIndex.insert(s.mid(10, 2), n);
        }

        n++;
    }

    hexfile.close();

    return true;
}

QString IntelHEX::checksum(const QString &str) const {

    if ( str.size() < 2 ) {
        return "";
    }

    QVector<quint8> v;

    for ( ptrdiff_t i=1; i<str.size(); i+=2 ) {
        v.push_back(str.mid(i-1, 2).toUInt(0, 16));
    }

    quint8 cs = 0;

    for ( ptrdiff_t i=0; i<v.size(); i++ ) {
        cs += v[i];
    }

    cs = ~cs + 1;

    QString ret = QString::number(cs, 16);

    if ( ret.size() < 2 ) {
        ret = "0" + ret;
    }
    else if ( ret.size() > 2 ) {
        return "";
    }

    return ret.toUpper();
}
