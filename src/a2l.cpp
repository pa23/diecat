/*
    diecat
    Diesel engines calibration tool.

    File: a2l.cpp

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

#include "a2l.hpp"
#include "constants.hpp"

#include <QVector>
#include <QString>
#include <QFile>
#include <QRegExp>
#include <QSharedPointer>

A2L::A2L(const QString &path) {
    m_a2lpath = path;
}

bool A2L::readFile() {

    QFile a2lfile(m_a2lpath);

    if ( !a2lfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        return false;
    }

    QString str;
    QStringList strlst;
    QRegExp regexp("(.)+(_C)$");

    while ( !a2lfile.atEnd() ) {

        str = a2lfile.readLine().simplified();

        if ( str.isEmpty() ) {
            continue;
        }

        if ( str == "/begin CHARACTERISTIC" ) {

            for ( ptrdiff_t i=0; i<A2LCHARACTERISTIC; i++ ) {

                str = a2lfile.readLine().simplified();

                if ( str.isEmpty() ) {
                    continue;
                }

                if ( str == "/end CHARACTERISTIC" ) {
                    break;
                }

                strlst.push_back(str);
            }

            if ( regexp.exactMatch(strlst.first()) ) {
                m_scalarsInfo.push_back(strlst);
            }

            strlst.clear();
        }
        else if ( str == "/begin COMPU_METHOD" ) {

            for ( ptrdiff_t i=0; i<A2LCOMPUMETHOD; i++ ) {

                str = a2lfile.readLine().simplified();

                if ( str.isEmpty() ) {
                    continue;
                }

                if ( str == "/end COMPU_METHOD" ) {
                    break;
                }

                strlst.push_back(str);
            }

            m_compumethodsInfo.push_back(strlst);
            strlst.clear();
        }
    }

    a2lfile.close();

    if ( !checkVector(m_scalarsInfo, A2LSCALARINFO) ) {
        return false;
    }

    if ( !checkVector(m_compumethodsInfo, A2LCOMPUMETHODINFO) ) {
        return false;
    }

    return true;
}

void A2L::fillScalarsInfo(QVector< QSharedPointer<ECUScalar> > &scalars) const {

    for ( ptrdiff_t i=0; i<m_scalarsInfo.size(); i++ ) {

        QSharedPointer<ECUScalar> scal(new ECUScalar());
        scal->setName(m_scalarsInfo[i][0]);
        scal->setShortDescription(m_scalarsInfo[i][1]);
        scal->setAddress(m_scalarsInfo[i][3].split("x").last());
        scal->setCoefficients(getCoeff(findCompuMethod(m_scalarsInfo[i][6])));
        scal->setMinValue(m_scalarsInfo[i][7].toDouble());
        scal->setMaxValue(m_scalarsInfo[i][8].toDouble());

        scalars.push_back(scal);
    }
}

void A2L::clear() {

    m_scalarsInfo.clear();
    m_compumethodsInfo.clear();
}

bool A2L::checkVector(const QVector<QStringList> &v, ptrdiff_t n) const {

    for ( ptrdiff_t i=0; i<v.size(); i++ ) {

        if ( v[i].size() != n ) {
            return false;
        }
    }

    return true;
}

QString A2L::findCompuMethod(const QString &str) const {

    for ( ptrdiff_t i=0; i<m_compumethodsInfo.size(); i++ ) {

        if ( m_compumethodsInfo[i][0] == str ) {
            return m_compumethodsInfo[i][5];
        }
    }

    return "";
}

QVector<double> A2L::getCoeff(const QString &str) const {

    QStringList strlst;
    QVector<double> v;

    strlst = str.split(" ");

    if ( strlst.size() != (A2LCOEFFNUM + 1) ) {
        return v;
    }

    for ( ptrdiff_t i=0; i<strlst.size(); i++ ) {
        v.push_back(strlst[i].toDouble());
    }

    return v;
}
