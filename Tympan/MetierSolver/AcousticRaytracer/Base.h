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

#ifndef BASE_H
#define BASE_H

//#include "BaseGraphic.h"
//#include "BaseWidget.h"

#include <string>

//#ifdef USE_QT
//
//#define GRAPHIC_DECL(classname) \
//  friend class classname##Graphic; \
//  public: \
//      virtual BaseGraphic* getGraphicBase() { \
//          if (!graphicBase)   graphicBase = new classname##Graphic(this); \
//          return graphicBase; \
//      }
//
//#define WIDGET_DECL(classname) \
//  friend class classname##Widget; \
//  public: \
//      virtual BaseWidget* getEditWidget() { \
//          return new classname##Widget(this); \
//      }
//
//#endif

class Base
{

public:
    Base()
    {
        //graphicBase = NULL;
        //widgetBase = NULL;

        name = "unkown element";
    }
    Base(const Base& other)
    {
        //graphicBase = other.graphicBase;
        //widgetBase = other.widgetBase;
        name = other.name;
    }

    virtual ~Base()
    {
        //if(graphicBase) delete graphicBase;
        //if(widgetBase) delete widgetBase;
    }

    std::string getName() { return name; }

    void setName(const std::string& _name) { name = _name; }


protected:
    //BaseGraphic* graphicBase;
    //BaseWidget* widgetBase;

    std::string name;
};

#endif
