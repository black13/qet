#include "terminal.h"
#include "schema.h"
#include "element.h"
#include "conductor.h"
#include "debug.h"
/**
Private function to initialize the terminal.
@param pf position of the mooring point for a driver
@param o terminal orientation: Qt :: Horizontal or Qt :: Vertical
*/
void Terminal::initialise(QPointF pf, Terminal::Orientation o) {
	// definition du pount d'amarrage pour un conducteur
	amarrage_conducteur  = pf;
	
	// definition de l'orientation de la borne (par defaut : sud)
	if (o < Terminal::Nord || o > Terminal::Ouest) sens = Terminal::Sud;
	else sens = o;
	
	// calcul de la position du point d'amarrage a l'element
	amarrage_elmt = amarrage_conducteur;
	switch(sens) {
		case Terminal::Nord  : amarrage_elmt += QPointF(0, TAILLE_BORNE);  break;
		case Terminal::Est   : amarrage_elmt += QPointF(-TAILLE_BORNE, 0); break;
		case Terminal::Ouest : amarrage_elmt += QPointF(TAILLE_BORNE, 0);  break;
		case Terminal::Sud   :
		default           : amarrage_elmt += QPointF(0, -TAILLE_BORNE);
	}
	
	// par defaut : pas de conducteur
	
	// QRectF null
	br = new QRectF();
	terminal_precedente = NULL;
	// divers
	//setAcceptsHoverEvents(true);
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	hovered = false;
	setToolTip("Terminal");
	couleur_neutre   = QColor(Qt::blue);
	couleur_autorise = QColor(Qt::darkGreen);
	couleur_prudence = QColor("#ff8000");
	couleur_interdit = QColor(Qt::red);
	couleur_hovered  = couleur_neutre;
}

/**
	Constructeur par defaut
*/
Terminal::Terminal() : QGraphicsItem() {
	initialise(QPointF(0.0, 0.0), Terminal::Sud);
	schema_scene = 0;
}

/**
initializes a terminal
@param pf position of the mooring point for a conductor
@param o terminal orientation: Qt :: Horizontal or Qt :: Vertical
@param e Element to which this terminal belongs
@param s Scene on which this terminal appears
*/
Terminal::Terminal(QPointF pf, Terminal::Orientation o, Element *e, Schema *s) : QGraphicsItem(e) {
	initialise(pf, o);
	schema_scene = s;
}

/**

initializes a terminal
@param pf_x Abscissa of the mooring point for a conductor
@param pf_y Order of the mooring point for a driver
@param o terminal orientation: Qt :: Horizontal or Qt :: Vertical
@param e Element to which this terminal belongs
@param s Scene on which this terminal appears
*/
Terminal::Terminal(qreal pf_x, qreal pf_y, Terminal::Orientation o, Element *e, Schema *s) : QGraphicsItem(e) {
	initialise(QPointF(pf_x, pf_y), o);
}

/**
	Destructeur
*/
Terminal::~Terminal() {
	delete br;
}

/**

Used to find out the orientation of the terminal. If the parent of the terminal
is indeed an Element, this function returns the orientation with respect to
the scene of the terminal, taking into account that the element could have been
swivels. Otherwise it returns its normal meaning.
@return The current orientation of the Terminal.
*/
Terminal::Orientation Terminal::orientation() const {
	//true pour une orientation verticale, false pour une orientation horizontale
	if (Element *elt = qgraphicsitem_cast<Element *>(parentItem())) {
		if (elt -> orientation()) return(sens);
		else {
			Terminal::Orientation retour;
			switch(sens) {
				case Terminal::Nord  : retour = Terminal::Ouest; break;
				case Terminal::Est   : retour = Terminal::Nord;  break;
				case Terminal::Ouest : retour = Terminal::Sud;   break;
				case Terminal::Sud   :
				default           : retour = Terminal::Est;
			}
			return(retour);
		}
	} else return(sens);
}

/**
Assign a conductor to the terminal
@param f The conductor to be attached to this terminal
*/
bool Terminal::addConducteur(Conductor *f) {
	trace_msg("");
	// pointer 0 refuse
	if (!f) return(false);
	// only one of the two terminals of the conductor must be this
	Q_ASSERT_X((f -> terminal1 == this ^ f -> terminal2 == this), "Terminal::addConducteur", "The conductor should be connected exactly once to the current terminal");
	
	// determines the other terminal to which this terminal will be connected thanks to the conductor
	Terminal *autre_borne = (f -> terminal1 == this) ? f -> terminal2 : f -> terminal1;
	
	// check that the terminal is not already connected with the other terminal
	bool deja_liees = false;
	foreach (Conductor* conducteur, liste_conducteurs) {
		if (conducteur -> terminal1 == autre_borne || conducteur -> terminal2 == autre_borne) 
			deja_liees = true;
	}

	// if the two terminals are already connected, we refuse to add the conductor
	if (deja_liees) return(false);
	
	// otherwise we add the driver
	liste_conducteurs.append(f);
	return(true);
}

void Terminal::removeConducteur(Conductor *f) {
	int index = liste_conducteurs.indexOf(f);
	if (index == -1) return;
	liste_conducteurs.removeAt(index);
}

/**
Terminal drawing function
@param p The QPainter to use
@param options The drawing options
@param widget The widget we are drawing on
*/
void Terminal::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) {
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
	
	// dessin de la borne en rouge
	t.setColor(Qt::red);
	p -> setPen(t);
	p -> drawLine(f, e);
	
	// dessin du point d'amarrage au conducteur en bleu
	t.setColor(couleur_hovered);
	p -> setPen(t);
	p -> setBrush(couleur_hovered);
	if (hovered) p -> drawEllipse(((int)f.x())-2, ((int)f.y())-2, 5, 5);
	else p -> drawPoint(f);
	
	p -> restore();
}

/**
	@return Le rectangle (en precision flottante) delimitant la borne et ses alentours.
*/
QRectF Terminal::boundingRect() const 
{

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
 Manages the entry of the mouse on the area of ??the Terminal.
*/
void Terminal::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
	hovered = true;
	update();
}

/**
 Manages mouse movements on the Terminal area.
*/
void Terminal::hoverMoveEvent(QGraphicsSceneHoverEvent *) {
}

/**
 Handles the fact that the mouse goes out of the Kiosk area.
*/
void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
	hovered = false;
	update();
}

/**
 Handles the fact that a mouse button is pressed on the Terminal.
	@param e L'evenement souris correspondant
*/
void Terminal::mousePressEvent(QGraphicsSceneMouseEvent *e) {
	if (Schema *s = qobject_cast<Schema *>(scene())) {
		s -> setDepart(mapToScene(QPointF(amarrage_conducteur)));
		s -> setArrivee(e -> scenePos());
		s -> poseConducteur(true);
		setCursor(Qt::CrossCursor);
	}
	//QGraphicsItem::mouseReleaseEvent(e);
}

/**
	Handles the fact that we move the mouse on the Terminal.
	@param e The corresponding mouse event
*/
void Terminal::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
	// during the installation of a conductor, we adopt another cursor
	setCursor(Qt::CrossCursor);
	
	// from one movement to another, we must remove the hover effect from the previous bound
	if (terminal_precedente != NULL) {
		if (terminal_precedente == this) hovered = true;
		else terminal_precedente -> hovered = false;
		terminal_precedente -> couleur_hovered = terminal_precedente -> couleur_neutre;
		terminal_precedente -> update();
	}
	
	// if the scene is a Schema, we update the driver installer
	if (Schema *s = qobject_cast<Schema *>(scene())) s -> setArrivee(e -> scenePos());
	
	// we get the list of qgi under the pointer
	QList<QGraphicsItem *> qgis = scene() -> items(e -> scenePos());
	
	/* the highest qgi
		= the driver installer
		= the first element of the list
		= the list cannot be empty
		= we take the second element of the list
	*/
	Q_ASSERT_X(!(qgis.isEmpty()), "Terminal::mouseMoveEvent", "The list of items should not be empty");
	
	// if there is something other than the driver setter in the list
	if (qgis.size() > 1) {
		// we take the second element of the list
		QGraphicsItem *qgi = qgis.at(1);
		// if the qgi is a bound ...
		if (Terminal *p = qgraphicsitem_cast<Terminal *>(qgi)) {
		// we apply the appropriate hover effect
			if (p == this) {
			// effect if we hover on the starting terminal
				couleur_hovered = couleur_interdit;
			} else if (p -> parentItem() == parentItem()) {
			// effect if we hover on a terminal of the same device
				if (((Element *)parentItem()) -> connexionsInternesAcceptees())
					p -> couleur_hovered = p -> couleur_autorise;
				else p -> couleur_hovered = p -> couleur_interdit;
			} else if (p -> nbConducteurs()) {
			// if the terminal already has a conductor
			// check that this terminal is not already linked to the other terminal
				bool deja_reliee = false;
				foreach (Conductor *f, liste_conducteurs) {
					if (f -> terminal1 == p || f -> terminal2 == p) {
						deja_reliee = true;
						break;
					}
				}
				// forbidden if the terminals are already connected, caution otherwise
				p -> couleur_hovered = deja_reliee ? p -> couleur_interdit : p -> couleur_prudence;
			} else {
			// effect if we can put the conductor
				p -> couleur_hovered = p -> couleur_autorise;
			}
			terminal_precedente = p;
			p -> hovered = true;
			p -> update();
		}
	}
}

/**
	Handles the fact that the mouse is released on the Terminal.
	@param e The corresponding mouse event
*/
void Terminal::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
	trace_msg("");
	setCursor(Qt::ArrowCursor);
	terminal_precedente = NULL;
	couleur_hovered  = couleur_neutre;
 // check that the scene is indeed a Schema
	if (Schema *s = qobject_cast<Schema *>(scene())) {

 // we stop drawing the driver preview
		s -> poseConducteur(false);
 // we get the element under the pointer during the MouseReleaseEvent
		QGraphicsItem *qgi = s -> itemAt(e -> scenePos(),QTransform() );
 // if there is nothing, we stop it
		if (!qgi) return;
 // same if the obtained element is not a bound
		Terminal *p = qgraphicsitem_cast<Terminal *>(qgi);
		if (!p) return;
 // we reset the hover color to its default value
		p -> couleur_hovered = p -> couleur_neutre;
 // same if it is the current terminal
		if (p == this) return;
 // same if it is a terminal of the current element and the element does not have the right to connect its own terminals
		bool cia = ((Element *)parentItem()) -> connexionsInternesAcceptees();
		if (!cia) foreach(QGraphicsItem *item, parentItem() -> childItems()) if (item == p) return;
 // last check: check that this terminal is not already linked to the other terminal
		foreach (Conductor *f, liste_conducteurs) if (f -> terminal1 == p || f -> terminal2 == p) return;
 // otherwise, we put a conductor
		new Conductor(this, (Terminal *)qgi, 0, scene());
	}
}

/**
	Updates the possible driver connected to the Terminal.
*/
void Terminal::updateConducteur() {
	if (scene()) {
		foreach(Conductor * conductor, liste_conducteurs) { 
			if (!conductor->isDestroyed()) {
				conductor->update(QRectF()/*scene()->sceneRect()*/);
			}
		}
	}
}

/**
	@return The list of conductors linked to this terminal
*/
QList<Conductor *> Terminal::conducteurs() const {
	return(liste_conducteurs);
}

/**

	XML export method
	@param doc The XML Document to use to create the XML element
	@return a QDomElement representing this bound
*/
QDomElement Terminal::toXml(QDomDocument &doc) {
	QDomElement qdo = doc.createElement("borne");
	qdo.setAttribute("x", amarrage_elmt.x());
	qdo.setAttribute("y", amarrage_elmt.y());
	qdo.setAttribute("orientation", sens);
	return(qdo);
}

/**

	Used to find out if an XML element represents a terminal
	@param e The QDomElement to analyze
	@return true if the QDomElement is a bound parameter, false otherwise
*/
bool Terminal::valideXml(QDomElement &borne) {
	// check the name of the tag
	if (borne.tagName() != "borne") 
		return(false);
	
	// check the presence of minimal attributes
	if (!borne.hasAttribute("x")) return(false);
	if (!borne.hasAttribute("y")) return(false);
	if (!borne.hasAttribute("orientation")) return(false);
	
	bool conv_ok;
	// parse the abscissa
	borne.attribute("x").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
 // parse the order
	borne.attribute("y").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
 // parse the id
	borne.attribute("id").toInt(&conv_ok);
	if (!conv_ok) return(false);
	
 // parse orientation
	int borne_or = borne.attribute("orientation").toInt(&conv_ok);
	if (!conv_ok) return(false);
	if (borne_or != Terminal::Nord && borne_or != Terminal::Sud && borne_or != Terminal::Est && borne_or != Terminal::Ouest) return(false);
	
 // at this stage, the terminal is syntactically correct
	return(true);
}

/**
	Used to find out if an XML element represents this terminal. Attention, 
	the XML element is not checked
	@param e The QDomElement to analyze
	@return true if the terminal "recognizes itself" (same coordinates, 
	same orientation), false otherwise
*/
bool Terminal::fromXml(QDomElement &borne) {
	return (
		borne.attribute("x").toDouble() == amarrage_elmt.x() &&\
		borne.attribute("y").toDouble() == amarrage_elmt.y() &&\
		borne.attribute("orientation").toInt() == sens
	);
}
