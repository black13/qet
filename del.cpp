#include "del.h"
#include <QPen>
#include <QGraphicsTextItem>

/**
Manufacturer
@Param parent The parent QObject of the element.
@Param scene the scene on which the element is displays
 */
DEL::DEL(QGraphicsItem *parent, Schema *scene) : ElementFixe(parent, scene) {
// size and hotspot
	setSize(30, 70);
	setHotspot(QPoint(15, 5));
	
// adding two terminals to the element
	new Borne(0,  0, Borne::Nord, this, scene);
	new Borne(0, 60, Borne::Sud,  this, scene);
	
	peut_relier_ses_propres_bornes = true;
}

/**
@return the current number of element terminals
*/
int DEL::nbBornes() const {
	return(2);
}

/**
Function that makes the graphic rendering of the LED
@param p the qpainter to use to draw the element
@param o drawing options
*/
void DEL::paint(QPainter *p, const QStyleOptionGraphicsItem *) {
// Black color traits
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.0);
	p -> setPen(t);
	
// a circle with white background
	p -> setBrush(QBrush(Qt::white, Qt::SolidPattern));
	p -> drawEllipse(-10, 20, 20, 20);
	p -> setBrush(Qt::NoBrush);
// two lines eventually antialiasis
	p -> drawLine(-7, 23,  7, 37);
	p -> drawLine( 7, 23, -7, 37);
// two lines ever antialiasis
	p -> save();
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	p -> drawLine(0,  0, 0, 20);
	p -> drawLine(0, 40, 0, 60);
	p -> restore();
}

/**
@return l'ID du type "DEL"
*/
QString DEL::typeId() {
	return(QString("1"));
}
