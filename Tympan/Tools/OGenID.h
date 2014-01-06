/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
 *
 *
 *
 *
 */

#ifndef __O_GEN_ID__
#define __O_GEN_ID__

#include <string>

#include <quuid.h>

// #include "Tympan/MetierSolver/DataManagerMetier/SolverDataModelBuilder.hpp"

namespace tympan {
class UuidAdapter;
} // namespace tympan

class OGenID
{
	friend size_t qHash(const OGenID& uid);
private:
    QUuid quid;
    friend class tympan::UuidAdapter;

public:
    OGenID();
    OGenID(const OGenID& other);
    OGenID(const QString& id);
    OGenID& operator =(const OGenID& other);

    ~OGenID();

    void GenUniqueID();
    bool isNull() const;

    const QString toString() const ;
    void FromString(const char* strUuid);
    void FromString(const QString& strUuid);

    bool operator ==(const OGenID& orig) const;
    bool operator !=(const OGenID& orig) const;
    bool operator <(const OGenID& other) const;
};

inline OGenID::OGenID() {}
inline OGenID::~OGenID() {}

inline OGenID::OGenID(const OGenID& other) :
    quid(other.quid) {}

inline OGenID::OGenID(const QString& id) :
    quid(id) {}

inline OGenID& OGenID::operator =(const OGenID& other)
{
    quid = other.quid;
    return *this;
}


inline void OGenID::GenUniqueID()
{
    quid = QUuid::createUuid();
}

inline bool OGenID::isNull() const
{
    return quid.isNull();
}

inline const QString OGenID::toString() const
{
    return quid.toString();
}

inline void OGenID::FromString(const char* strUuid)
{
    quid = QUuid(QString(strUuid));
}

inline void OGenID::FromString(const QString& strUuid)
{
    quid = QUuid(strUuid);
}

inline bool OGenID::operator ==(const OGenID& orig) const
{
    return quid == orig.quid;
}

inline bool OGenID::operator !=(const OGenID& orig) const
{
    return quid != orig.quid;
}

inline bool OGenID::operator <(const OGenID& other) const
{
    return quid < other.quid;
}

/**
 * @brief  Returns a hash of the QUuid
 * For old version of Qt, imported from
 * http://qt.gitorious.org/qt/qtbase/commit/55d68a1
 */
inline size_t qHash(const QUuid& uuid)
{
    return uuid.data1 ^ uuid.data2 ^ (uuid.data3 << 16)
     ^ ((uuid.data4[0] << 24) | (uuid.data4[1] << 16) | (uuid.data4[2] << 8) | uuid.data4[3])
     ^ ((uuid.data4[4] << 24) | (uuid.data4[5] << 16) | (uuid.data4[6] << 8) | uuid.data4[7]);
}


inline size_t qHash(const OGenID& uid)
{ return qHash( uid.quid); }

#endif // __O_GEN_ID__

