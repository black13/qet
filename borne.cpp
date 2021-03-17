#include "borne.h"
#include "schema.h"
#include "element.h"
#include "conducteur.h"

/**
	
Private function to initialize the terminal.
	
@param pf Position of the mooring point for a driver
@param the Orientation of La Borne: Qt :: Horizontal or Qt :: Vertical
	
*/
void Borne::initialise(QPointF pf, Borne::Orientation o) {
	// definition du pount d'amarrage pour un conducteur
// Definition of the mooring point for a driver
	amarrage_conducteur  = pf;
	
	// definition de l'orientation de la borne (par defaut : sud)
// Definition of the orientation of the terminal (by default: South)
	if (o < Borne::Nord || o > Borne::Ouest) sens = Borne::Sud;
	else sens = o;
	
	// calcul de la position du point d'amarrage a l'element
// Calculation of the position of the mooring point at the element
	amarrage_elmt = amarrage_conducteur;
	switch(sens) {
		case Borne::Nord  : amarrage_elmt += QPointF(0, TAILLE_BORNE);  break;
		case Borne::Est   : amarrage_elmt += QPointF(-TAILLE_BORNE, 0); break;
		case Borne::Ouest : amarrage_elmt += QPointF(TAILLE_BORNE, 0);  break;
		case Borne::Sud   :
		default           : amarrage_elmt += QPointF(0, -TAILLE_BORNE);
	}
	
	// par defaut : pas de conducteur
// by default: no driver
	
	// QRectF null
	br = new QRectF();
	borne_precedente = NULL;
	// divers
// divers
	setAcceptsHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	hovered = false;
	setToolTip("Borne");
	couleur_neutre   = QColor(Qt::blue);
	couleur_autorise = QColor(Qt::darkGreen);
	couleur_prudence = QColor("#ff8000");
	couleur_interdit = QColor(Qt::red);
	couleur_hovered  = couleur_neutre;
}

/**
	Default builder
*/
Borne::Borne() : QGraphicsItem(0, 0) {
	initialise(QPointF(0.0, 0.0), Borne::Sud);
	schema_scene = 0;
}

/**
	initializes a terminal
	
	@param pf Position of the mooring point for a driver
	@param the Orientation of La Borne: Qt :: Horizontal or Qt :: Vertical
	@param e element to which this terminal belongs
	@param s scene on which figure this terminal
*/
Borne::Borne(QPointF pf, Borne::Orientation o, Element *e, Schema *s) : QGraphicsItem(e, s) {
	initialise(pf, o);
	schema_scene = s;
}

/**
	initializes a terminal
	@param pf_x abscissa of the mooring point for a driver
	@param pf_y ordinate from the mooring point for a driver
	@param the Orientation of La Borne: Qt :: Horizontal or Qt :: Vertical
	@param e element to which this terminal belongs
	@param s scene on which figure this terminal
	@param s scene on which figure this terminal
*/
Borne::Borne(qreal pf_x, qreal pf_y, Borne::Orientation o, Element *e, Schema *s) : QGraphicsItem(e, s) {
	initialise(QPointF(pf_x, pf_y), o);
}

/**
	Destructive
*/
Borne::~Borne() {
	delete br;
}

/**
	Allows to know the orientation of the terminal.If the parent of the terminal
	is an element, this function returns the orientation compared to
	The scene of the terminal, taking into account the fact that the element could be
	Pivot.Otherwise it returns its normal sense.
	@return the current orientation of the terminal.
*/
Borne::Orientation Borne::orientation() const {
	//true pour une orientation verticale, false pour une orientation horizontale
	if (Element *elt = qgraphicsitem_cast<Element *>(parentItem())) {
		if (elt -> orientation()) return(sens);
		else {
			Borne::Orientation retour;
			switch(sens) {
				case Borne::Nord  : retour = Borne::Ouest; break;
				case Borne::Est   : retour = Borne::Nord;  break;
				case Borne::Ouest : retour = Borne::Sud;   break;
				case Borne::Sud   :
				default           : retour = Borne::Est;
			}
			return(retour);
		}
	} else return(sens);
}

/**
	Assigns a driver at the terminal
	@param f the driver to attach to this terminal
*/
bool Borne::addConducteur(Conducteur *f) {
	// pointer 0 refuses
	if (!f) return(false);
	
	// only one of the two terminals of the driver must be this
	Q_ASSERT_X((f -> borne1 == this ^ f -> borne2 == this), "Borne::addConducteur", "Le conducteur devrait etre relie exactement une fois a la borne en cours");
	
	// determines the other terminal at which this terminal will be connected thanks to the driver
	Borne *autre_borne = (f -> borne1 == this) ? f -> borne2 : f -> borne1;
	
	// check that the terminal is not already connected with the other terminal
	bool deja_liees = false;
	foreach (Conducteur* conducteur, liste_conducteurs) {
		if (conducteur -> borne1 == autre_borne || conducteur -> borne2 == autre_borne) deja_liees = true;
	}
	
	// if the two terminals are already connected, we refuse to add the driver
	if (deja_liees) return(false);
	
	// Otherwise we add the driver
	liste_conducteurs.append(f);
	return(true);
}

void Borne::removeConducteur(Conducteur *f) {
	int index = liste_conducteurs.indexOf(f);
	if (index == -1) return;
	liste_conducteurs.removeAt(index);
}

/**
	Terminal drawing function
	@param p the qpainter to use
	@Param Options Drawing options
	@Param Options Drawing options
	@param widget the widget on which we draw
*/
void Borne::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) {
	p -> save();
	
	//annulation des renderhints
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	
	// on travaille avec les coordonnees de l'element parent
	QPointF f = mapFromParent(amarrage_conducteur);
	QPointF e = mapFromParent(amarrage_elmt);
	
	QPen t;
	t.setWidthF(1.0);
	
	// drawing the terminal in red
	t.setColor(Qt::red);
	p -> setPen(t);
	p -> drawLine(f, e);
	
	// Drawing the driver mooring point in blue
	t.setColor(couleur_hovered);
	p -> setPen(t);
	p -> setBrush(couleur_hovered);
	if (hovered) p -> drawEllipse(((int)f.x())-2, ((int)f.y())-2, 5, 5);
	else p -> drawPoint(f);
	
	p -> restore();
}

/**
	@return The rectangle (in floating precision) delimiting the terminal and its surroundings.
*/
QRectF Borne::boundingRect() const {
	if (br -> isNull()) {
		qreal afx = amarrage_conducteur.x();
		qreal afy = amarrage_conducteur.y();
		qreal aex = amarrage_elmt.x();
		qreal aey = amarrage_elmt.y();
		QPointF origine;
		origine = (afx <= aex && afy <= aey ? amarrage_conducteur : amarrage_elmt);
		origine += QPointF(-3.0, -3.0);
		qreal w = qAbs((int)(afx - aex)) + 7;
		qreal h = qAbs((int)(afy - aey)) + 7;
		*br = QRectF(origine, QSizeF(w, h));
	}
	return(*br);
}

/**
	Gerts the entrance of the mouse over the area of the terminal.
*/
void Borne::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
	hovered = true;
	update();
}

/**
	the movements of the mouse over the area of the terminal.
*/
void Borne::hoverMoveEvent(QGraphicsSceneHoverEvent *) {
}

/**
	the fact that the mouse kind of the area of the terminal.
*/
void Borne::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
	hovered = false;
	update();
}

/**
	the fact that we push a button of the mouse over the terminal.
	@param e the corresponding mouse event
*/
void Borne::mousePressEvent(QGraphicsSceneMouseEvent *e) {
	if (Schema *s = qobject_cast<Schema *>(scene())) {
		s -> setDepart(mapToScene(QPointF(amarrage_conducteur)));
		s -> setArrivee(e -> scenePos());
		s -> poseConducteur(true);
		setCursor(Qt::CrossCursor);
	}
	//QGraphicsItem::mouseReleaseEvent(e);
}

/**
	Geres the fact that we move the mouse on the terminal.
	@param e the corresponding mouse event
*/
void Borne::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
	// during the installation of a driver, we adopt another cursor
	setCursor(Qt::CrossCursor);
	
	// from one movement to another, it is necessary to remove the hover effect from the preceding terminal
	if (borne_precedente != NULL) {
		if (borne_precedente == this) hovered = true;
		else borne_precedente -> hovered = false;
		borne_precedente -> couleur_hovered = borne_precedente -> couleur_neutre;
		borne_precedente -> update();
	}
	
	// If the scene is a schema, we update the driver's installer
	if (Schema *s = qobject_cast<Schema *>(scene())) s -> setArrivee(e -> scenePos());
	
	// we recover the list of qgi under the pointer
	QList<QGraphicsItem *> qgis = scene() -> items(e -> scenePos());
	
	/*  The highest QGI
	= the driver's installer
	= the first element of the list
	= the first element of the list
	= the list can not be empty
	= We take the second element of the list
	*/
	Q_ASSERT_X(!(qgis.isEmpty()), "Borne::mouseMoveEvent", "The Items list should not be empty");
	
	// If there is something other than the driver's installer in the list
	if (qgis.size() > 1) {
	// we take the second element of the list
		QGraphicsItem *qgi = qgis.at(1);
		// si le qgi est une borne...
		if (Borne *p = qgraphicsitem_cast<Borne *>(qgi)) {
			// ... it is applied to the appropriate Hover effect
			if (p == this) {
				// effect if one hover on the departure terminal
				couleur_hovered = couleur_interdit;
			} else if (p -> parentItem() == parentItem()) {
				// effect if one hover on a terminal of the same device
				if (((Element *)parentItem()) -> connexionsInternesAcceptees())
					p -> couleur_hovered = p -> couleur_autorise;
				else p -> couleur_hovered = p -> couleur_interdit;
			} else if (p -> nbConducteurs()) {
				// if the terminal has already a driver
				// check that this terminal is not already connected to the other terminal
				bool deja_reliee = false;
				foreach (Conducteur *f, liste_conducteurs) {
					if (f -> borne1 == p || f -> borne2 == p) {
						deja_reliee = true;
						break;
					}
				}
				// forbidden if the terminals are already connected, prudence otherwise
				p -> couleur_hovered = deja_reliee ? p -> couleur_interdit : p -> couleur_prudence;
			} else {
				// effect if you can install the driver
				p -> couleur_hovered = p -> couleur_autorise;
			}
			borne_precedente = p;
			p -> hovered = true;
			p -> update();
		}
	}
}

/**
	 fact that we relaunch the mouse over the terminal.
	@param e the corresponding mouse event
*/
void Borne::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
	setCursor(Qt::ArrowCursor);
	borne_precedente = NULL;
	couleur_hovered  = couleur_neutre;
	// verify that the scene is a schema
	if (Schema *s = qobject_cast<Schema *>(scene())) {
		// We stop drawing the driver's overview
		s -> poseConducteur(false);
		// we recovered the element under the pointer during the mouseseesevent
		QGraphicsItem *qgi = s -> itemAt(e -> scenePos()); 
		// If there is nothing, we stop the
		if (!qgi) return;
		// idem if the element obtained is not a terminal
		Borne *p = qgraphicsitem_cast<Borne *>(qgi);
		if (!p) return;
		// We give the color of Hover to its default value
		p -> couleur_hovered = p -> couleur_neutre;
		// idem if it is the current terminal
		if (p == this) return;
		// idem if it is a terminal of the current element and that the element does not have the right to connect its own terminals
		bool cia = ((Element *)parentItem()) -> connexionsInternesAcceptees();
		if (!cia) foreach(QGraphicsItem *item, parentItem() -> children()) if (item == p) return;
		// last verification: check that this terminal is not already connected to the other terminal
		foreach (Conducteur *f, liste_conducteurs) if (f -> borne1 == p || f -> borne2 == p) return;
		// otherwise, we put a driver
		new Conducteur(this, (Borne *)qgi, 0, scene());
	}
}

/**
	Updates the eventual driver connects to the terminal.
*/
void Borne::updateConducteur() {
	if (scene()) {
		foreach (Conducteur *conducteur, liste_conducteurs) if (!conducteur -> isDestroyed()) conducteur -> update(QRectF()/*scene()->sceneRect()*/);
	}
}

/**
	@return the list of drivers related to this terminal
*/
QList<Conducteur *> Borne::conducteurs() const {
	return(liste_conducteurs);
}

/**
	Method of Export of  XML
	@param doc the XML document to use to create the XML element
	@return a qdomellation representing this terminal
*/
QDomElement Borne::toXml(QDomDocument &doc) {
	QDomElement qdo = doc.createElement("borne");
	qdo.setAttribute("x", amarrage_elmt.x());
	qdo.setAttribute("y", amarrage_elmt.y());
	qdo.setAttribute("orientation", sens);
	return(qdo);
}

/**
	Lets you know if an XML element represents a terminal
	@param e the QDomellation to Analyze
	@return true if the thommelement goes into parametre is a terminal, false otherwise
*/
bool Borne::valideXml(QDomElement &borne) {
	// verify the name of the tag
	if (borne.tagName() != "borne") return(false);
	
	// check the presence of minimum attributes
	if (!borne.hasAttribute("x")) return(false);
	if (!borne.hasAttribute("y")) return(false);
	if (!borne.hasAttribute("orientation")) return(false);
	
	bool conv_ok;
	// Parse the abscissa
	borne.attribute("x").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
	// Parse ordinate
	borne.attribute("y").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'id
	borne.attribute("id").toInt(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'orientation
	int borne_or = borne.attribute("orientation").toInt(&conv_ok);
	if (!conv_ok) return(false);
	if (borne_or != Borne::Nord && borne_or != Borne::Sud && borne_or != Borne::Est && borne_or != Borne::Ouest) return(false);
	
	// At this point, the terminal is syntactically correct
	return(true);
}

/**
	Lets you know if an XML element represents this terminal.Attention, the XML element is not verified
	@param e the QDomellation to Analyze
	@return true if the terminal "recognizes" (same coordinates, even orientation), FALSE otherwise
*/
bool Borne::fromXml(QDomElement &borne) {
	return (
		borne.attribute("x").toDouble() == amarrage_elmt.x() &&\
		borne.attribute("y").toDouble() == amarrage_elmt.y() &&\
		borne.attribute("orientation").toInt() == sens
	);
}
