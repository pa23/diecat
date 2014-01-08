/*
    diecat
    Diesel engines calibration tool.

    File: intelhex.hpp

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

#ifndef INTELHEX_HPP
#define INTELHEX_HPP

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QStringList>
#include <QHash>

#include "ecuscalar.hpp"

class IntelHEX {

public:
    IntelHEX(const QString &);
    bool readValues(QVector< QSharedPointer<ECUScalar> > &);
    void clear();

private:
    QString m_hexpath;
    QStringList m_hexData;
    QHash<QString, ptrdiff_t> m_hexIndex; // address extension -> m_hexData index

    bool readHex();
    QString checksum(const QString &) const;
    bool readScalars(QVector< QSharedPointer<ECUScalar> > &) const;
    void writeScalarValue(QString &, QSharedPointer<ECUScalar> &) const;

};

#endif // INTELHEX_HPP
