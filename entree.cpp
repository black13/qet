#include "entree.h"

/**
	Constructor 
	@Param parent The parent QObject of the element.
	@Param scene the scene on which the element is displays
*/
Entree::Entree(QGraphicsItem *parent, Schema *scene) : ElementFixe(parent, scene) {
	// size and hotspot
	setSize(20, 40);
	setHotspot(QPoint(10, 15));
	
	// Add a terminal to the element
	new Borne(0,  15, Borne::Sud, this, scene);
}

/**
	@return the current number of element terminals
*/
int Entree::nbBornes() const {
	return(1);
}

/**
	Function that makes the graphic rendering of the contactor
	@param p the qpainter to use to draw the element
	@param o drawing options
*/
void Entree::paint(QPainter *p, const QStyleOptionGraphicsItem *) {
	// Black color traits
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.0);
	t.setJoinStyle(Qt::MiterJoin);
	p -> setPen(t);
	p -> setBrush(Qt::black);
	
	// Triangle drawing
	static const QPointF points[3] = {
		QPointF(-7.5, -13),
		QPointF( 7.5, -13),
		QPointF( 0.0, 0.0)
	};
	
	p -> drawPolygon(points, 3);
	p -> setBrush(Qt::NoBrush);
	
	// a line ever antialiased (cancellation of renderehints)
	p -> save();
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	p -> drawLine(0, 0, 0, 13);
	p -> restore();
}

/**
	@return the ID of the type "Contactor"
*/
QString Entree::typeId() {
	return(QString("2"));
}
