#include "contacteur.h"

/**
	Constructor
	@Param parent The parent QObject of the element.
	@Param scene the scene on which the element is displays
*/
Contacteur::Contacteur(QGraphicsItem *parent, Schema *scene) : ElementFixe(parent, scene) {
	// size and hotspot
	setSize(15, 70);
	setHotspot(QPoint(10, 5));
	
	// ajout de deux bornes a l'element
	new Borne(0,  0, Borne::Nord, this, scene);
	new Borne(0, 60, Borne::Sud,  this, scene);
}

/**
	@return the current number of element terminals
*/
int Contacteur::nbBornes() const {
	return(2);
}

/**
	Function that makes the graphic rendering of the contactor
	@param p the qpainter to use to draw the element
	@param o drawing options
*/
void Contacteur::paint(QPainter *p, const QStyleOptionGraphicsItem *) {
// Black color traits
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.0);
	t.setJoinStyle(Qt::MiterJoin);
	p -> setPen(t);
	
// An eventually antialiasis line
	p -> drawLine(-5, 19, 0, 40);
	
// two lines ever antialiasis (cancellation of renderehints)
	p -> save();
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	p -> drawLine(0, 0,  0, 20);
	p -> drawLine(0, 40, 0, 60);
	p -> restore();
}

/**
@return the ID of the type "Contactor"
*/
QString Contacteur::typeId() {
	return(QString("0"));
}
