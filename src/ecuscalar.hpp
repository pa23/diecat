/*
    diecat
    Diesel engines calibration tool.

    File: ecuscalar.hpp

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

#ifndef ECUSCALAR_HPP
#define ECUSCALAR_HPP

#include <QString>
#include <QVector>

class ECUScalar {

public:
    ECUScalar();
    void setName                 (const QString &);
    void setShortDescription     (const QString &);
    void setAddress              (const QString &);
    void setCoefficients         (const QVector<double> &);
    void setMinValue             (double);
    void setMaxValue             (double);
    QString name                 () const { return m_name;       }
    QString shortDescription     () const { return m_shortDescr; }
    QString address              () const { return m_addr;       }
    QVector<double> coefficients () const { return m_coeff;      }
    double  minValue             () const { return m_minVal;     }
    double  maxValue             () const { return m_maxVal;     }

private:
    QString m_name;
    QString m_shortDescr;
    QString m_addr;
    QVector<double> m_coeff;
    double m_minVal = 0;
    double m_maxVal = 0;

};

#endif // ECUSCALAR_HPP
