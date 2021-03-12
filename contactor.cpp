#include "contactor.h"

 /**
 Manufacturer
 @param parent The parent QObject of the element.
 @param scene The scene on which the element is displayed
 */
 Contactor :: Contactor (QGraphicsItem * parent, Schema * scene): FixedElement(parent, scene) {
 // size and hotspot
 setSize (15, 70);
 setHotspot (QPoint (10, 5));
	
 // add two bounds to the element
	new Terminal(0,  0, Terminal::Nord, this, scene);
	new Terminal(0, 60, Terminal::Sud,  this, scene);
}

/**
	@return Le nombre actuel de bornes de l'element
*/
int Contactor::nbBornes() const {
	return(2);
}

/**
 Function that performs the graphic rendering of the contactor
 @param p The QPainter to use to draw the element
 @param o The drawing options
*/
void Contactor::paint(QPainter *p, const QStyleOptionGraphicsItem *) {
 // black lines
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.0);
	t.setJoinStyle(Qt::MiterJoin);
	p -> setPen(t);
	
 // a possibly anti-aliasing line
	p -> drawLine(-5, 19, 0, 40);
	
 // two lines NEVER anti-aliases (cancellation of renderhints)
	p -> save();
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	p -> drawLine(0, 0,  0, 20);
	p -> drawLine(0, 40, 0, 60);
	p -> restore();
}

/**
 @return the ID of the "Contactor" type
*/
QString Contactor::typeId() {
	return(QString("0"));
}
