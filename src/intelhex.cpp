/*
    diecat
    A2L/HEX file reader.

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

#include <cstdio>
#include <limits>

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
        QString hexStrtAddr = QString::number(strtAddr, 16).toUpper();

        ptrdiff_t addsymbnum = 4 - hexStrtAddr.size();
        if ( hexStrtAddr.size() < 4 ) {
            for ( ptrdiff_t i=0; i<addsymbnum; i++ ) {
                hexStrtAddr.insert(0, "0");
            }
        }

        const QRegExp regexpStrtAddr(R"(^)" + QString::number(hexDataLength, 16).toUpper() + hexStrtAddr + R"(00.*)");
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
        const ptrdiff_t maxSize = getLength(scalars[n]->numType()) * 2;

        for ( ptrdiff_t j=firstByteInd; j<(m_hexData[beginStrNum].size()-2); j++ ) {

            tmpStr.push_back(m_hexData.at(beginStrNum).at(j));

            if ( tmpStr.size() == maxSize ) {
                writeScalarValue(tmpStr, scalars[n]);
                break;
            }
        }

        if ( tmpStr.size() == maxSize ) {
            continue;
        }

        for ( ptrdiff_t i=(beginStrNum+1); i<m_hexData.size(); i++ ) {

            if ( m_hexData[i].size() != correctStrDataSize ) {
                continue;
            }

            for ( ptrdiff_t j=8; j<(correctStrDataSize-2); j++ ) {

                tmpStr.push_back(m_hexData.at(i).at(j));

                if ( tmpStr.size() == maxSize ) {
                    writeScalarValue(tmpStr, scalars[n]);
                    break;
                }
            }

            if ( tmpStr.size() == maxSize ) {
                break;
            }
        }
    }

    return true;
}

void IntelHEX::writeScalarValue(QString &str, QSharedPointer<ECUScalar> &scalar) const {

    if ( scalar->type() == VARTYPE_SCALAR_NUM ) {

        QString numtype = scalar->numType();
        size_t rawVal = str.toULongLong(0, 16);
        double preVal = 0;
        double val = 0;

        if ( numtype == "Ws8" ) {

            ptrdiff_t i = 0;

            if ( rawVal > CHAR_MAX ) {
                i = rawVal - UCHAR_MAX - 1;
            }
            else {
                i = rawVal;
            }

            preVal = static_cast<double>(i);
        }
        else if ( numtype == "Ws16" ) {

            ptrdiff_t i = 0;

            if ( rawVal > SHRT_MAX ) {
                i = rawVal - USHRT_MAX - 1;
            }
            else {
                i = rawVal;
            }

            preVal = static_cast<double>(i);
        }
        else if ( numtype == "Ws32" ) {

            ptrdiff_t i = 0;

            if ( rawVal > INT_MAX ) {
                i = rawVal - UINT_MAX - 1;
            }
            else {
                i = rawVal;
            }

            preVal = static_cast<double>(i);
        }
        else if ( numtype == "Wr32" ) {

            float f = 0;
            sscanf(str.toLatin1().data(), "%x", (int*)&f);
            preVal = static_cast<double>(f);
        }
        else {
            preVal = static_cast<double>(rawVal);
        }

        const QVector<double> coeff = scalar->coefficients();
        val = (coeff[5] * preVal - coeff[2]) / (coeff[1] - coeff[4] * preVal);

        scalar->setValue(QString::number(val, 'f', scalar->precision()));
    }
    else if ( scalar->type() == VARTYPE_SCALAR_VTAB ) {
        scalar->setValue(QString::number(str.toUInt(0, 16)));
    }
    else {
        return;
    }
}

size_t IntelHEX::getLength(const QString &type) const {

    if ( type.size() == 3 ) {
        return type.right(1).toUInt() / 8;
    }
    else if ( type.size() == 4 ) {
        return type.right(2).toUInt() / 8;
    }

    return 0;
}
