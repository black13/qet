#include "element.h"
#include "schema.h"
#include <QtDebug>

/*** Public methods ***/

/**
Builder for an element without a scene or parent
*/
Element::Element(QGraphicsItem *parent, Schema *scene) : QGraphicsItem(parent, scene) {
	sens = true;
	peut_relier_ses_propres_bornes = false;
}

/**
	Main method of drawing of the element
	@param painter the qpainter uses to draw the element
	@param Options Style options to take into account
	@param widget the widget on which we draw
*/
void Element::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *) {
// drawing the element itself
	paint(painter, options);
	
// drawing the selection frame if necessary
	if (isSelected()) drawSelection(painter, options);
}

/**
@return the rectangle delimiting the outline of the element
*/
QRectF Element::boundingRect() const {
	return(QRectF(QPointF(-hotspot_coord.x(), -hotspot_coord.y()), dimensions));
}

/**
	Defines the size of the element on the schema.Sizes must be
	multiples of 10;If this is not the case, the dimensions indicated
	will be arriving with the superior dozens.
	@param wid Width of the element
	@param here today the l'item
	@return the final size of the element
*/
QSize Element::setSize(int wid, int hei) {
	prepareGeometryChange();
	// each dimension indicated is rounded to the ten superior
	while (wid % 10) ++ wid;
	while (hei % 10) ++ hei;
	// the final dimensions are preserved and returned
	return(dimensions = QSize(wid, hei));
}

/**
	Defines the hotspot of the element with respect to the left area of its deimitant rectangle.
	Necessity that the size has already been defined
	@param HSX hotspot abscissa
	@param hsy ordinate hotspot
*/
QPoint Element::setHotspot(QPoint hs) {
	// the size must have been defined
	prepareGeometryChange();
	if (dimensions.isNull()) hotspot_coord = QPoint(0, 0);
	else {
		// The coordinates indicated should not exceed the dimensions of the element
		int hsx = hs.x() > dimensions.width() ? dimensions.width() : hs.x();
		int hsy = hs.y() > dimensions.height() ? dimensions.height() : hs.y();
		hotspot_coord = QPoint(hsx, hsy);
	}
	return(hotspot_coord);
}

/**
@return the current hotspot of the element
*/
QPoint Element::hotspot() const {
	return(hotspot_coord);
}

/**
SELECT THE ELEMENT
*/
void Element::select() {
	setSelected(true);
}

/**
Deselectionne l'element
Deselectionne l'element
*/
void Element::deselect() {
	setSelected(false);
}

/**
@return The Pixmap of the item
*/
QPixmap Element::pixmap() {
	if (apercu.isNull()) updatePixmap(); // on genere la pixmap si ce n'est deja fait
	return(apercu);
}

/**
@todo distinguish the terminals with a dynamic cast
*/
QVariant Element::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemSelectedChange) {
		foreach(QGraphicsItem *qgi, children()) {
			if (Borne *p = qgraphicsitem_cast<Borne *>(qgi)) p -> updateConducteur();
		}
	}
	return(QGraphicsItem::itemChange(change, value));
}

/**
@return The current orientation of the element: True for a vertical orientation, false for a horizontal orientation
*/
bool Element::orientation() const {
	return(sens);
}

/**
Inverts the orientation of the element
@return The new orientation: True for a vertical orientation, false for horizontal orientation
*/
bool Element::invertOrientation() {
	sens = !sens;
// reversing meaning
// Temporarily hide the element to avoid a graphic bug
	hide();
// rotation in consequence and refreshment of the graphic element
	rotate(sens ? 90.0 : -90.0);
// We refine the element, we do it reselect and we refresh it
	show();
	select();
	update();
	return(sens);
}

/*** Methods proteges ***/

/**
Draws a small repere (x and y axes) relating to the element
@param painter the qpainter to use to draw the axes
@param Options Style options to take into account
*/
void Element::drawAxes(QPainter *painter, const QStyleOptionGraphicsItem *) {
	painter -> setPen(Qt::blue);
	painter -> drawLine(0, 0, 10, 0);
	painter -> drawLine(7,-3, 10, 0);
	painter -> drawLine(7, 3, 10, 0);
	painter -> setPen(Qt::red);
	painter -> drawLine(0,  0, 0, 10);
	painter -> drawLine(0, 10,-3,  7);
	painter -> drawLine(0, 10, 3,  7);
}

/*** PRIVATE METHODS ***/

/**
	Draws the selection frame of the systematically non-antialiased manner.
	@param QP The qpainter to use to draw the terminals.
	@param Options Style options to take into account
 */
void Element::drawSelection(QPainter *painter, const QStyleOptionGraphicsItem *) {
	painter -> save();
// cancellation of renderehints
	painter -> setRenderHint(QPainter::Antialiasing,          false);
	painter -> setRenderHint(QPainter::TextAntialiasing,      false);
	painter -> setRenderHint(QPainter::SmoothPixmapTransform, false);
// drawing the selection frame in gray
	QPen t;
	t.setColor(Qt::gray);
	t.setStyle(Qt::DashDotLine);
	painter -> setPen(t);
	// Le dessin se fait a partir du rectangle delimitant
	painter -> drawRoundRect(boundingRect(), 10, 10);
	painter -> restore();
}

/**
Initializing and drawing the Pixmap of the element.
*/
void Element::updatePixmap() {
// transparent pixmap making the basic size of the element
	apercu = QPixmap(dimensions);
	apercu.fill(QColor(255, 255, 255, 0));
// qpainter on the pixmap, with antialiasing
	QPainter p(&apercu);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
// Translation of the origin of the repere of the pixmap
	p.translate(hotspot_coord);
// the element draws on the pixmap
	paint(&p, 0);
}

/**
Change the position of the element by ensuring that the element
remains on the scheme grid to which it belongs.
@Param p New coordinates of the element
*/
void Element::setPos(const QPointF &p) {
	if (p == pos()) return;
	// not worth positioning on the grid if the element is not on a schema
	if (scene()) {
	// round up the abscissa at 10 px near
		int p_x = qRound(p.x() / 10.0) * 10;
		// round up the ordinate to 10 px near
		int p_y = qRound(p.y() / 10.0) * 10;
		QGraphicsItem::setPos(p_x, p_y);
	} else QGraphicsItem::setPos(p);
	// update the terminals / drivers
	// update the terminals / drivers
	foreach(QGraphicsItem *qgi, children()) {
		if (Borne *p = qgraphicsitem_cast<Borne *>(qgi)) p -> updateConducteur();
	}
}

/**
	Change the position of the element by ensuring that the element
	remains on the scheme grid to which it belongs.
	@param x new abscissa element
	@param y new ordinate element
*/
void Element::setPos(qreal x, qreal y) {
	setPos(QPointF(x, y));
}

/**
	Managing the mouse movements in the element, in particular
*/
void Element::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
	/*&& (flags() & ItemIsMovable)*/ // on le sait qu'il est movable
	if (e -> buttons() & Qt::LeftButton) {
		QPointF oldPos = pos();
		setPos(mapToParent(e->pos()) - matrix().map(e->buttonDownPos(Qt::LeftButton)));
		QPointF diff = pos() - oldPos;
		
		// RECEPE THE LIST OF SELECTING ELEMENTS
		QList<QGraphicsItem *> selectedItems;
		if (scene()) {
			selectedItems = scene() -> selectedItems();
		} else if (QGraphicsItem *parent = parentItem()) {
			while (parent && parent->isSelected()) selectedItems << parent;
		}
		
		// DEPLACE ALL SELECTION ELEMENTS
		foreach (QGraphicsItem *item, selectedItems) {
			if (!item->parentItem() || !item->parentItem()->isSelected())
				if (item != this) item->setPos(item->pos() + diff);
		}
	} else e -> ignore();
}

/**
Lets you know if an XML element (QDomellation) represents an element
@param by qdomelement of painude
@return true if the XML element is an element, false otherwise
*/
bool Element::valideXml(QDomElement &e) {
	// verifie le nom du tag
	// verify the name of the tag
	if (e.tagName() != "element") return(false);
	
	// check the presence of minimum attributes
	if (!e.hasAttribute("type")) return(false);
	if (!e.hasAttribute("x"))    return(false);
	if (!e.hasAttribute("y"))    return(false);
	
	bool conv_ok;
	// Parse the abscissa
	e.attribute("x").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'ordonnee
	e.attribute("y").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	return(true);
}
