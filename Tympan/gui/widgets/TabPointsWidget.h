#ifndef __TABPOINTSWIDGET_H__
#define __TABPOINTSWIDGET_H__

#include <QTableWidget.h>
#include "Tympan/models/business/TYDefines.h"

class TabPointsWidget : public QTableWidget
{
    Q_OBJECT

public:
	TabPointsWidget(TYTabPoint& listPoints, QWidget *parent=nullptr);
	
	virtual void update();
	virtual void apply();
	
public slots:
	void tabValueChanged(int, int);
    virtual void contextMenuEvent(QContextMenuEvent* e);
	
private : 
    void setAttributes(int row);
    void initRow(int row);
private:
	TYTabPoint& _listPoints;
	
};

#endif //__TABPOINTSWIDGET_H__

