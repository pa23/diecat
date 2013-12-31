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
    QRegExp regexp_C("(.)+(_C)$");

    while ( !a2lfile.atEnd() ) {

        str = a2lfile.readLine().simplified();

        if ( str.isEmpty() ) {
            continue;
        }

        if ( str == "/begin CHARACTERISTIC" ) {

            while ( !a2lfile.atEnd() ) {

                str = a2lfile.readLine().simplified();

                if ( str.isEmpty() ) {
                    continue;
                }

                if ( str == "/end CHARACTERISTIC" ) {
                    break;
                }

                strlst.push_back(str);
            }

            if ( strlst.size() > 12 ) {

                if ( regexp_C.exactMatch(strlst[0]) && (strlst[2] == "VALUE") ){
                    m_scalarsInfo.push_back(strlst);
                }
            }

            strlst.clear();
        }
        else if ( str == "/begin COMPU_METHOD" ) {

            while ( !a2lfile.atEnd() ) {

                str = a2lfile.readLine().simplified();

                if ( str.isEmpty() ) {
                    continue;
                }

                if ( str == "/end COMPU_METHOD" ) {
                    break;
                }

                strlst.push_back(str);
            }

            if ( strlst.size() == A2LCOMPUMETHODSIZE ) {
                m_compumethodsInfo.push_back(strlst);
            }

            strlst.clear();
        }
    }

    a2lfile.close();

    return true;
}

void A2L::fillScalarsInfo(QVector< QSharedPointer<ECUScalar> > &scalars) const {

    for ( ptrdiff_t i=0; i<m_scalarsInfo.size(); i++ ) {

        QSharedPointer<ECUScalar> scal(new ECUScalar());

        ptrdiff_t compuMethodInd = findCompuMethod(m_scalarsInfo[i][6]);

        scal->setName(m_scalarsInfo[i][0]);
        scal->setShortDescription(m_scalarsInfo[i][1]);
        scal->setAddress(m_scalarsInfo[i][3].split("x").last());
        scal->setCoefficients(getCoeff(compuMethodInd));
        scal->setMinValue(m_scalarsInfo[i][7].toDouble());
        scal->setMaxValue(m_scalarsInfo[i][8].toDouble());
        scal->setReadOnly(isReadOnly(i));
        //scal->setSigned...
        scal->setDimension(m_compumethodsInfo[compuMethodInd][4]);
        //scal->setLength...

        scalars.push_back(scal);
    }
}

void A2L::clear() {

    m_scalarsInfo.clear();
    m_compumethodsInfo.clear();
}

ptrdiff_t A2L::findCompuMethod(const QString &str) const {

    for ( ptrdiff_t i=0; i<m_compumethodsInfo.size(); i++ ) {

        if ( m_compumethodsInfo[i][0] == str ) {
            return i;
        }
    }

    return -1;
}

QVector<double> A2L::getCoeff(ptrdiff_t ind) const {

    QVector<double> v(A2LCOEFFNUM);

    if ( ind < 0 ) {
        return v;
    }

    QStringList strlst = m_compumethodsInfo[ind][5].split(" ");

    if ( strlst.size() != (A2LCOEFFNUM + 1) ) {
        return v;
    }

    for ( ptrdiff_t i=1; i<strlst.size(); i++ ) {
        v[i-1] = strlst[i].toDouble();
    }

    return v;
}

bool A2L::isReadOnly(ptrdiff_t ind) const {

    for ( ptrdiff_t i=0; i<m_scalarsInfo[ind].size(); i++ ) {

        if ( m_scalarsInfo[ind][i] == "READ_ONLY" ) {
            return true;
        }
    }

    return false;
}
