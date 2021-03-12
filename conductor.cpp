#include <QtDebug>
#include "conductor.h"
#include "element.h"
#include "debug.h"
/**
Builder
@param p1 First Terminal to which the driver is linked
@param p2 Second Terminal to which the driver is linked
@param parent Parent element of the driver (0 by default)
@param scene QGraphicsScene to which the conductor belongs
*/
Conductor::Conductor(Terminal *p1, Terminal* p2, Element *parent, QGraphicsScene *scene) : QGraphicsPathItem(parent) {
	trace_msg("");
	// terminals that the conductor connects
	terminal1 = p1;
	terminal2 = p2;
	// add the conductor to the list of conductors for each of the two terminals
	bool ajout_p1 = terminal1 -> addConducteur(this);
	bool ajout_p2 = terminal2 -> addConducteur(this);
	// in case of failure of the addition (driver already existing in particular)
	if (!ajout_p1 || !ajout_p2) return;
	destroyed = false;
 // the driver is represented by a thin line
	QPen t;
	t.setWidthF(1.0);
	setPen(t);
	// calcul du rendu du conducteur
	calculateConductor();
}

/**
	Updates the graphical representation of the driver.
	@param rect Rectangle to update
*/
void Conductor::update(const QRectF &rect = QRectF()) {
	calculateConductor();
	QGraphicsPathItem::update(rect);
}

/**
	Destroyer of the Conductor. Before being destroyed, the conductor unhooks from the terminals
	to which it is linked.
*/
void Conductor::update(qreal x, qreal y, qreal width, qreal height) {
	calculateConductor();
	QGraphicsPathItem::update(x, y, width, height);
}

 /**
 Destroyer of the Conductor. Before being destroyed, the conductor unhooks from the terminals
 to which it is linked.
 */
 /* Driver::~Driver () {

}*/

/**
Updates the QPainterPath constituting the conductor to obtain
a conductor only consists of straight lines connecting the two terminals.
*/
void Conductor::calculateConductor() {
	QPainterPath t;
	trace_msg("");
	QPointF p1 = terminal1 -> amarrageConducteur();
	QPointF p2 = terminal2 -> amarrageConducteur();
	
	QPointF depart, arrivee;
	Terminal::Orientation ori_depart, ori_arrivee;
	// distingue le depart de l'arrivee : le trajet se fait toujours de gauche a droite
	if (p1.x() <= p2.x()) {
		depart      = mapFromScene(p1);
		arrivee     = mapFromScene(p2);
		ori_depart  = terminal1 -> orientation();
		ori_arrivee = terminal2 -> orientation();
	} else {
		depart      = mapFromScene(p2);
		arrivee     = mapFromScene(p1);
		ori_depart  = terminal2 -> orientation();
		ori_arrivee = terminal1 -> orientation();
	}
	
	// debut du trajet
	t.moveTo(depart);
	if (depart.y() < arrivee.y()) {
 // downward path
		if ((ori_depart == Terminal::Nord && (ori_arrivee == Terminal::Sud || ori_arrivee == Terminal::Ouest)) || (ori_depart == Terminal::Est && ori_arrivee == Terminal::Ouest)) {
 // case   3  
			qreal ligne_inter_x = (depart.x() + arrivee.x()) / 2.0;
			t.lineTo(ligne_inter_x, depart.y());
			t.lineTo(ligne_inter_x, arrivee.y());
		} else if ((ori_depart == Terminal::Sud && (ori_arrivee == Terminal::Nord || ori_arrivee == Terminal::Est)) || (ori_depart == Terminal::Ouest && ori_arrivee == Terminal::Est)) {
 // case   4  
			qreal ligne_inter_y = (depart.y() + arrivee.y()) / 2.0;
			t.lineTo(depart.x(), ligne_inter_y);
			t.lineTo(arrivee.x(), ligne_inter_y);
		} else if ((ori_depart == Terminal::Nord || ori_depart == Terminal::Est) && (ori_arrivee == Terminal::Nord || ori_arrivee == Terminal::Est)) {
			t.lineTo(arrivee.x(), depart.y()); // cas « 2 »
		} else t.lineTo(depart.x(), arrivee.y()); // cas « 1 »
	} else {
 // upward journey
		if ((ori_depart == Terminal::Ouest && (ori_arrivee == Terminal::Est || ori_arrivee == Terminal::Sud)) || (ori_depart == Terminal::Nord && ori_arrivee == Terminal::Sud)) {
 // case   3  
			qreal ligne_inter_y = (depart.y() + arrivee.y()) / 2.0;
			t.lineTo(depart.x(), ligne_inter_y);
			t.lineTo(arrivee.x(), ligne_inter_y);
		} else if ((ori_depart == Terminal::Est && (ori_arrivee == Terminal::Ouest || ori_arrivee == Terminal::Nord)) || (ori_depart == Terminal::Sud && ori_arrivee == Terminal::Nord)) {
			// cas « 4 »
			qreal ligne_inter_x = (depart.x() + arrivee.x()) / 2.0;
			t.lineTo(ligne_inter_x, depart.y());
			t.lineTo(ligne_inter_x, arrivee.y());
		} else if ((ori_depart == Terminal::Ouest || ori_depart == Terminal::Nord) && (ori_arrivee == Terminal::Ouest || ori_arrivee == Terminal::Nord)) {
			t.lineTo(depart.x(), arrivee.y()); // cas « 2 »
		} else t.lineTo(arrivee.x(), depart.y()); // cas « 1 »
	}
 // end of trip
	t.lineTo(arrivee);
	setPath(t);
}

/**

Draw the driver without antialiasing.
@param qp The QPainter to use to draw the conductor
@param qsogi Style options for the driver
@param qw The QWidget on which we draw
*/
void Conductor::paint(QPainter *qp, const QStyleOptionGraphicsItem *qsogi, QWidget *qw) {
	trace_msg("");
	qp -> save();
	qp -> setRenderHint(QPainter::Antialiasing,          false);
	qp -> setRenderHint(QPainter::TextAntialiasing,      false);
	qp -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	QGraphicsPathItem::paint(qp, qsogi, qw);
	qp -> restore();
}

/**
Indicates whether two Terminal orientations are on the same axis (Vertical / Horizontal).
@param a Terminal's first orientation
@param b The second orientation of Terminal
@return A boolean is true if both terminal orientations are on the same axis
*/
bool Conductor::surLeMemeAxe(Terminal::Orientation a, Terminal::Orientation b) {
	if ((a == Terminal::Nord || a == Terminal::Sud) && (b == Terminal::Nord || b == Terminal::Sud)) return(true);
	else if ((a == Terminal::Est || a == Terminal::Ouest) && (b == Terminal::Est || b == Terminal::Ouest)) return(true);
	else return(false);
}

/**
Indicates whether a terminal orientation is horizontal (East / West).
@param a Terminal orientation
@return True if the terminal orientation is horizontal, false otherwise
*/
bool Conductor::estHorizontale(Terminal::Orientation a) {
	return(a == Terminal::Est || a == Terminal::Ouest);
}

/**
Indicates whether a terminal orientation is vertical (North / South).
@param a Terminal orientation
@return True if the terminal orientation is vertical, false otherwise
*/
bool Conductor::estVerticale(Terminal::Orientation a) {
	return(a == Terminal::Nord || a == Terminal::Sud);
}

/**
 Method of preparation for the destruction of the conductor; the conductor is detached from its two terminals
*/
void Conductor::destroy() {
	destroyed = true;
	terminal1 -> removeConducteur(this);
	terminal2 -> removeConducteur(this);
}

/**
 XML element validation method
 @param e A sense XML element represents a Driver
 @return true if the XML element does represent a Driver; false otherwise
*/
bool Conductor::valideXml(QDomElement &e){
 // check the name of the tag
	if (e.tagName() != "conductor") return(false);
	
 // check the presence of minimal attributes
	if (!e.hasAttribute("terminal1")) return(false);
	if (!e.hasAttribute("terminal2")) return(false);
	
	bool conv_ok;
 // parse the abscissa
	e.attribute("terminal1").toInt(&conv_ok);
	if (!conv_ok) return(false);
	
 // parse the order
	e.attribute("terminal2").toInt(&conv_ok);
	if (!conv_ok) return(false);
	return(true);
}
