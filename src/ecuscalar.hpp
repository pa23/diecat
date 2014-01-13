/*
    diecat
    Diesel engines calibration tool.

    File: ecuscalar.hpp

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

#ifndef ECUSCALAR_HPP
#define ECUSCALAR_HPP

#include <QString>
#include <QVector>
#include <QStringList>

#include "constants.hpp"

class ECUScalar {

public:
    ECUScalar();
    void setName(const QString &);
    void setShortDescription(const QString &);
    void setType(ptrdiff_t);
    void setAddress(const QString &);
    void setNumType(const QString &);
    void setRangeSoft(double);
    void setCoefficients(const QVector<double> &);
    void setMinValueSoft(double);
    void setMaxValueSoft(double);
    void setPrecision(ptrdiff_t);
    void setMinValueHard(double);
    void setMaxValueHard(double);
    void setReadOnly(bool);
    void setDimension(const QString &);
    void setValue(QString);
    void setVTable(const QStringList &);

    QString name() const {
        return m_name;
    }
    QString shortDescription() const {
        return m_shortDescr;
    }
    ptrdiff_t type() const {
        return m_type;
    }
    QString address() const {
        return m_addr;
    }
    QString numType() const {
        return m_numType;
    }
    double rangeSoft() const {
        return m_rangeSoft;
    }
    QVector<double> coefficients() const {
        return m_coeff;
    }
    double minValueSoft() const {
        return m_minValSoft;
    }
    double maxValueSoft() const {
        return m_maxValSoft;
    }
    ptrdiff_t precision() const {
        return m_prec;
    }
    double minValueHard() const {
        return m_minValHard;
    }
    double maxValueHard() const {
        return m_maxValHard;
    }
    bool isReadOnly() const {
        return m_readOnly;
    }
    QString dimension() const {
        return m_dim;
    }
    QString value() const {
        return m_val;
    }
    QStringList vTable() const {
        return m_vtab;
    }

private:
    QString m_name;
    QString m_shortDescr;
    ptrdiff_t m_type = VARTYPE_SCALAR_NUM;
    QString m_addr;
    QString m_numType;
    double m_rangeSoft = 0;
    QVector<double> m_coeff;
    double m_minValSoft = 0;
    double m_maxValSoft = 0;
    ptrdiff_t m_prec = 6;
    double m_minValHard = 0;
    double m_maxValHard = 0;
    bool m_readOnly = false;
    QString m_dim;
    QString m_val;
    QStringList m_vtab;

};

#endif // ECUSCALAR_HPP
