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
#include <QRegExp>

IntelHEX::IntelHEX(const QString &path) {
    m_hexpath = path;
}

bool IntelHEX::readValues(QVector<QSharedPointer<ECUScalar> > &scalars) {

    if ( !readHex() ) {
        return false;
    }

    if ( !readScalars(scalars) ) {
        return false;
    }

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
            m_hexIndex.insert(s, n);
        }

        n++;
    }

    hexfile.close();

    return true;
}

void IntelHEX::clear() {

    m_hexpath.clear();
    m_hexData.clear();
    m_hexIndex.clear();
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

bool IntelHEX::readScalars(QVector<QSharedPointer<ECUScalar> > &scalars) const {

    for ( ptrdiff_t n=0; n<scalars.size(); n++ ) {

        const QString address = scalars[n]->address();
        const QString partAddrExt = "0200000400" + address.left(2);
        const QString addrExt = partAddrExt + checksum(partAddrExt);
        ptrdiff_t searchInd = 0;

        if ( m_hexIndex.contains(addrExt) ) {
            searchInd = m_hexIndex[addrExt];
        }
        else {
            return false;
        }

        //

        QVector<quint8> v;
        const QString nextStr = m_hexData[searchInd+1];

        for ( ptrdiff_t i=1; i<nextStr.size(); i+=2 ) {
            v.push_back(nextStr.mid(i-1, 2).toUInt(0, 16));
        }

        if ( v.isEmpty() ) {
            return false;
        }

        const ptrdiff_t hexDataLength = v[0];

        //

        const quint16 strtAddr(address.toUInt(0, 16) & (0xFFFF - hexDataLength + 1));
        const QRegExp regexpStrtAddr(R"(^)" + QString::number(hexDataLength, 16).toUpper() + QString::number(strtAddr, 16).toUpper() + R"(00.*)");
        ptrdiff_t beginStrNum = 0;

        for ( ptrdiff_t i=searchInd+1; i<m_hexData.size(); i++ ) {

            if ( regexpStrtAddr.exactMatch(m_hexData[i]) ) {
                beginStrNum = i;
                break;
            }
        }

        //

        const ptrdiff_t correctStrDataSize = 8 + hexDataLength * 2 + 2;

        if ( m_hexData[beginStrNum].size() != correctStrDataSize ) {
            return false;
        }

        //

        const ptrdiff_t firstByteInd = 8 + address.right(1).toUInt(0, 16) * 2;

        //

        QString tmpStr;
        const ptrdiff_t maxSize = scalars[n]->length() * 2;

        for ( ptrdiff_t j=firstByteInd; j<(m_hexData[beginStrNum].size()-2); j++ ) {

            if ( tmpStr.size() == maxSize ) {

                writeScalarValue(tmpStr, scalars[n]);
                break;
            }
            else {
                tmpStr.push_back(m_hexData.at(beginStrNum).at(j));
            }
        }

        if ( tmpStr.size() == maxSize ) {

            writeScalarValue(tmpStr, scalars[n]);
            continue;
        }

        for ( ptrdiff_t i=(beginStrNum+1); i<m_hexData.size(); i++ ) {

            if ( m_hexData[i].size() != correctStrDataSize ) {
                continue;
            }

            for ( ptrdiff_t j=8; j<(correctStrDataSize-2); j++ ) {

                if ( tmpStr.size() == maxSize ) {

                    writeScalarValue(tmpStr, scalars[n]);
                    break;
                }
                else {
                    tmpStr.push_back(m_hexData.at(i).at(j));
                }
            }

            if ( tmpStr.size() == maxSize ) {

                writeScalarValue(tmpStr, scalars[n]);
                break;
            }
        }
    }

    return true;
}

void IntelHEX::writeScalarValue(QString &str, QSharedPointer<ECUScalar> &scalar) const {

    if ( scalar->type() == VARTYPE_SCALAR_NUM ) {

        const ptrdiff_t rawVal = str.toUInt(0, 16);
        const QVector<double> coeff = scalar->coefficients();
        const double val = (coeff[5] * rawVal - coeff[2]) / (coeff[1] - coeff[4] * rawVal);

        scalar->setValue(QString::number(val, 'f', scalar->precision()));
    }
    else {
        return;
    }
}
