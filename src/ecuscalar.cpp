/*
    diecat
    Diesel engines calibration tool.

    File: ecuscalar.cpp

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

#include "ecuscalar.hpp"

ECUScalar::ECUScalar() {
}

void ECUScalar::setName(const QString &name) {
    m_name = name;
}

void ECUScalar::setShortDescription(const QString &shortDescr) {
    m_shortDescr = shortDescr;
}

void ECUScalar::setAddress(const QString &addr) {
    m_addr = addr;
}

void ECUScalar::setCoefficients(const QVector<double> &coeff) {
    m_coeff = coeff;
}

void ECUScalar::setMinValue(double minVal) {
    m_minVal = minVal;
}

void ECUScalar::setMaxValue(double maxVal) {
    m_maxVal = maxVal;
}

void ECUScalar::setReadOnly(bool ro) {
    m_readOnly = ro;
}

void ECUScalar::setSigned(bool s) {
    m_signed = s;
}

void ECUScalar::setDimension(const QString &dim) {
    m_dim = dim;
}

void ECUScalar::setLength(size_t l) {
    m_length = l;
}

void ECUScalar::setValue(double val) {
    m_val = val;
}
