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

#ifndef POST_FILTER_H
#define POST_FILTER_H

class postFilter : public Base
{
public:
    postFilter(std::vector<Ray*> *tabRay) : _tabRay(tabRay) {}
    ~postFilter() {}

    /*!
     * \fn unsigned int Traite();
     * \brief apply a filter to the group of valid rays found by ray tracing
     * \return number of rays suppressed
     */
    virtual unsigned int Process() { return 0; }

public :
    std::vector<Ray*> *_tabRay;
};

#endif //POST_FILTER_H