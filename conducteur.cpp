#include <QtDebug>
#include "conducteur.h"
#include "element.h"

/**
	Constructor
	@param p1 first terminal to which the driver is linked
	@param p2 second terminal to which the driver is linked
	@Param Parent Element of the driver (0 by default)
	@Param Scene QGraphicsscene in which the driver belongs
*/
Conducteur::Conducteur(Borne *p1, Borne* p2, Element *parent, QGraphicsScene *scene) : QGraphicsPathItem(parent, scene) {
	// terminals that the driver connects
	borne1 = p1;
	borne2 = p2;
	// Add the driver to the list of conductors of each of the two terminals
	bool ajout_p1 = borne1 -> addConducteur(this);
	bool ajout_p2 = borne2 -> addConducteur(this);
	// en cas d'echec de l'ajout (conducteur deja existant notamment)
	if (!ajout_p1 || !ajout_p2) return;
	destroyed = false;
	// the driver is represented by a fine line
	QPen t;
	t.setWidthF(1.0);
	setPen(t);
	// Calculation of driver rendering
	calculeConducteur();
}

/**
	Updates the graphical representation of the driver.
	@param rect rectangle to update
*/
void Conducteur::update(const QRectF &rect = QRectF()) {
	calculeConducteur();
	QGraphicsPathItem::update(rect);
}

/**
	Updates the graphical representation of the driver.
	@param x rectangle abscissa to update
	@param y ordinate rectangle to update
	@param width rectangle length to update
	@param height rectangle height to be updated
*/
void Conducteur::update(qreal x, qreal y, qreal width, qreal height) {
	calculeConducteur();
	QGraphicsPathItem::update(x, y, width, height);
}

/**
	Driver's destroyer.Before being destroyed, the driver removes terminals
	to which he is linked.
*/
/*Conducteur::~Conducteur() {

}*/

/**
	Update the qpainterpath constituting the driver to get
	A driver only composed of lines connecting the two terminals.
*/
void Conducteur::calculeConducteur() {
	QPainterPath t;
	
	QPointF p1 = borne1 -> amarrageConducteur();
	QPointF p2 = borne2 -> amarrageConducteur();
	
	QPointF depart, arrivee;
	Borne::Orientation ori_depart, ori_arrivee;
	// distinguishes the departure from the arrival: the trip is always from left to right
	if (p1.x() <= p2.x()) {
		depart      = mapFromScene(p1);
		arrivee     = mapFromScene(p2);
		ori_depart  = borne1 -> orientation();
		ori_arrivee = borne2 -> orientation();
	} else {
		depart      = mapFromScene(p2);
		arrivee     = mapFromScene(p1);
		ori_depart  = borne2 -> orientation();
		ori_arrivee = borne1 -> orientation();
	}
	
	// Beginning of the trip
	t.moveTo(depart);
	if (depart.y() < arrivee.y()) {
		// downhide
		if ((ori_depart == Borne::Nord && (ori_arrivee == Borne::Sud || ori_arrivee == Borne::Ouest)) || (ori_depart == Borne::Est && ori_arrivee == Borne::Ouest)) {
		// case � 3 �
			qreal ligne_inter_x = (depart.x() + arrivee.x()) / 2.0;
			t.lineTo(ligne_inter_x, depart.y());
			t.lineTo(ligne_inter_x, arrivee.y());
		} else if ((ori_depart == Borne::Sud && (ori_arrivee == Borne::Nord || ori_arrivee == Borne::Est)) || (ori_depart == Borne::Ouest && ori_arrivee == Borne::Est)) {
			// case � 4 �
			qreal ligne_inter_y = (depart.y() + arrivee.y()) / 2.0;
			t.lineTo(depart.x(), ligne_inter_y);
			t.lineTo(arrivee.x(), ligne_inter_y);
		} else if ((ori_depart == Borne::Nord || ori_depart == Borne::Est) && (ori_arrivee == Borne::Nord || ori_arrivee == Borne::Est)) {
			t.lineTo(arrivee.x(), depart.y()); // cas � 2 �
		} else t.lineTo(depart.x(), arrivee.y()); // cas � 1 �
	} else {
		// rising path
		if ((ori_depart == Borne::Ouest && (ori_arrivee == Borne::Est || ori_arrivee == Borne::Sud)) || (ori_depart == Borne::Nord && ori_arrivee == Borne::Sud)) {
		// case � 3 �
			qreal ligne_inter_y = (depart.y() + arrivee.y()) / 2.0;
			t.lineTo(depart.x(), ligne_inter_y);
			t.lineTo(arrivee.x(), ligne_inter_y);
		} else if ((ori_depart == Borne::Est && (ori_arrivee == Borne::Ouest || ori_arrivee == Borne::Nord)) || (ori_depart == Borne::Sud && ori_arrivee == Borne::Nord)) {
		// case � 4 �
			qreal ligne_inter_x = (depart.x() + arrivee.x()) / 2.0;
			t.lineTo(ligne_inter_x, depart.y());
			t.lineTo(ligne_inter_x, arrivee.y());
		} else if ((ori_depart == Borne::Ouest || ori_depart == Borne::Nord) && (ori_arrivee == Borne::Ouest || ori_arrivee == Borne::Nord)) {
			t.lineTo(depart.x(), arrivee.y()); // cas � 2 �
		} else t.lineTo(arrivee.x(), depart.y()); // cas � 1 �
	}
	// End of the journey
	t.lineTo(arrivee);
	setPath(t);
}

/**
	Draw the driver without antialiasing.
	@param qp the qpainter to use to draw the driver
	@param QSogi style options for the driver
	@param qw the qwidget on which we draw
*/
void Conducteur::paint(QPainter *qp, const QStyleOptionGraphicsItem *qsogi, QWidget *qw) {
	qp -> save();
	qp -> setRenderHint(QPainter::Antialiasing,          false);
	qp -> setRenderHint(QPainter::TextAntialiasing,      false);
	qp -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	QGraphicsPathItem::paint(qp, qsogi, qw);
	qp -> restore();
}

/**
	Indicates whether two terminal orientations are on the same axis (vertical / horizontal).
	@param a La premiere orientation de Borne
	@param b the second orientation of terminal
	@return a boolean true if the two orientations of terminals are on the same axis
*/
bool Conducteur::surLeMemeAxe(Borne::Orientation a, Borne::Orientation b) {
	if ((a == Borne::Nord || a == Borne::Sud) && (b == Borne::Nord || b == Borne::Sud)) return(true);
	else if ((a == Borne::Est || a == Borne::Ouest) && (b == Borne::Est || b == Borne::Ouest)) return(true);
	else return(false);
}

/**
	Indicates whether a terminal orientation is horizontal (East / West).
	@Param has the orientation of terminal
	@return true if the terminal orientation is horizontal, false otherwise
*/
bool Conducteur::estHorizontale(Borne::Orientation a) {
	return(a == Borne::Est || a == Borne::Ouest);
}

/**
	Indicates whether a terminal orientation is vertical (North / South).
	@Param has the orientation of terminal
	@Param has the orientation of terminal
	@return True if the orientation of terminal is vertical, false otherwise
*/
bool Conducteur::estVerticale(Borne::Orientation a) {
	return(a == Borne::Nord || a == Borne::Sud);
}

/**
Method of preparation to the destruction of the driver;The driver is detached from his two terminals
*/
void Conducteur::destroy() {
	destroyed = true;
	borne1 -> removeConducteur(this);
	borne2 -> removeConducteur(this);
}

/**
	ELEMENT VALIDATION METHOD XML
	@param e an element XML Sense represents a driver
	@return True if the XML element represents a driver well;false otherwise
*/
bool Conducteur::valideXml(QDomElement &e){
	// verify the name of the tag
	if (e.tagName() != "conducteur") return(false);
	
	// check the presence of minimum attributes
	if (!e.hasAttribute("borne1")) return(false);
	if (!e.hasAttribute("borne2")) return(false);
	
	bool conv_ok;
	// Parse the abscissa
	e.attribute("borne1").toInt(&conv_ok);
	if (!conv_ok) return(false);
	

	// Parse ordinate
	e.attribute("borne2").toInt(&conv_ok);
	if (!conv_ok) return(false);
	return(true);
}
