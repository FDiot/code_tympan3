#include <qmenu.h>
#include <QStringList>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geometry/TYPoint.h"
#include "Tympan/gui/widgets/TabPointsWidget.h"

#define TR(id) OLocalizator::getString("TabPointsWidget", (id))

TabPointsWidget::TabPointsWidget(TYTabPoint& listPoints, QWidget *parent) : QTableWidget(parent), _listPoints(listPoints)
{
    setColumnCount( 3 );

	// Set horizontal header
    QStringList stringList;
    stringList.append("X");
    stringList.append("Y");
    stringList.append("Z");

    setHorizontalHeaderLabels(stringList);
	setRowCount( _listPoints.size() );

    setSelectionBehavior(QAbstractItemView::SelectRows);
	
	connect ( this, SIGNAL( cellChanged(int, int) ), this, SLOT(tabValueChanged(int, int)) );
}

void TabPointsWidget::update()
{
	// Filling the table
    disconnect ( this, SIGNAL( cellChanged(int, int) ), this, SLOT(tabValueChanged(int, int)) );

    for (size_t row = 0; row < _listPoints.size(); row++)
    {
        setItem(row, 0, new QTableWidgetItem((QString().setNum(_listPoints[row]._x, 'f', 2))));
        setItem(row, 1, new QTableWidgetItem((QString().setNum(_listPoints[row]._y, 'f', 2))));
        setItem(row, 2, new QTableWidgetItem((QString().setNum(_listPoints[row]._z, 'f', 2))));
        item(row, 0)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item(row, 1)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item(row, 2)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
		
   	connect ( this, SIGNAL( cellChanged(int, int) ), this, SLOT(tabValueChanged(int, int)) );
}

void TabPointsWidget::apply()
{
	bool ok(false);
	double x(0.), y(0.), z(0.);
	
	// Filling the list
	_listPoints.clear();
    for (size_t row=0; row < static_cast<size_t>(rowCount()) ; row++)
    {
        x = item(row, 0)->text().toDouble(&ok);
		y = item(row, 1)->text().toDouble(&ok);
		z = item(row, 2)->text().toDouble(&ok);
		
        if (ok) { _listPoints.push_back( TYPoint(x, y, z) ); }
    }	
}

void TabPointsWidget::tabValueChanged(int row, int col)
{
	// Translate input value to double to see if it's a correct number 
	bool ok; 
	item(row, col)->text().toDouble(&ok);

	// If value is not a valid number, get the original value in points list
	if (!ok)
	{
		item(row, col)->setText(QString().setNum(0.0, 'f', 2));
	}
}

void TabPointsWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QPoint point = mapFrom(this, e->pos());
    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= width()) && (point.y() <= height()))
    {
        QPoint resPoint = QPoint( point.x(), point.y() );
        QTableWidgetItem* element = itemAt(resPoint);
        if (element)
        {
            int row = indexAt(resPoint).row();
            row = row >= 0 ? row : 0; // Securite
            row = row < rowCount() ? row : rowCount()-1;

            int col = indexAt(resPoint).column();
            col = col >= 0 ? col : 0; // Securite
            col = col < columnCount() ? col : columnCount()-1;

            QAction *insertLine = NULL, *deleteLine = NULL;

            QMenu* pPopup = new QMenu(this);

            insertLine = pPopup->addAction( TR("id_insert_row_item") );
            deleteLine = pPopup->addAction( TR("id_delete_row_item") );

            QAction* ret = pPopup->exec(mapToGlobal(point));

            if (ret == insertLine)
            {
                insertRow(row);
                setItem(row, 0, new QTableWidgetItem((QString().setNum(0.0, 'f', 2))));
                setItem(row, 1, new QTableWidgetItem((QString().setNum(0.0, 'f', 2))));
                setItem(row, 2, new QTableWidgetItem((QString().setNum(0.0, 'f', 2))));
                
                item(row, 0)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		        item(row, 1)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		        item(row, 2)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
            else if (ret == deleteLine)
            {
                removeRow(row);
            }
        }
    }
}