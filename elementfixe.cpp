#include "elementfixe.h"
/**
	Constructeur
*/
FixedElement::FixedElement(QGraphicsItem *parent, Schema *scene) : Element(parent, scene) {
}

/**
	@return Le nombre minimal de bornes que l'element peut avoir
*/
int FixedElement::nbBornesMin() const {
	return(nbBornes());
}

/**
	@return Le nombre maximal de bornes que l'element peut avoir
*/
int FixedElement::nbBornesMax() const {
	return(nbBornes());
}

/**
 XML import method. This method is called when importing XML content (pasting, importing, opening a file, etc.) so that the element can manage the import of its terminals itself. Here, as this class is characterized by a fixed number of bounds, the implementation requires to find exactly its bounds in the XML file.
 @param e The XML element to analyze.
 @param table_id_adr Reference to the correspondence table between the IDs of the XML file and the addresses in memory. If the import is successful, you have to add the right pairs (id, address).
 @return true if the import was successful, false otherwise
	
 */
bool FixedElement::fromXml(QDomElement &e, QHash<int, Terminal *> &table_id_adr) {
/*
 the terminals will now be listed to associate their id to their real address
 this census will be used when placing the wires
*/
	
	QList<QDomElement> liste_bornes;
 	// course of the children of the element
	for (QDomNode enfant = e.firstChild() ; !enfant.isNull() ; enfant = enfant.nextSibling()) {
	 // we are interested in the XML element "limits"
		QDomElement bornes = enfant.toElement();
		if (bornes.isNull() || bornes.tagName() != "bornes") continue;
		// browse the children of the "terminals" XML element
		for (QDomNode node_borne = bornes.firstChild() ; !node_borne.isNull() ; node_borne = node_borne.nextSibling()) {
		// we are interested in the "bound" XML element
			QDomElement borne = node_borne.toElement();
			if (!borne.isNull() && Terminal::valideXml(borne)) liste_bornes.append(borne);
		}
	}
	
	QHash<int, Terminal *> priv_id_adr;
	int bornes_non_trouvees = 0;
	foreach(QGraphicsItem *qgi, childItems()) {
		if (Terminal *p = qgraphicsitem_cast<Terminal *>(qgi)) {
			bool borne_trouvee = false;
			foreach(QDomElement qde, liste_bornes) {
				if (p -> fromXml(qde)) {
					priv_id_adr.insert(qde.attribute("id").toInt(), p);
					borne_trouvee = true;
					break;
				}
			}
			if (!borne_trouvee) ++ bornes_non_trouvees;
		}
	}
	
	if (bornes_non_trouvees > 0) {
		return(false);
	} else {
	// check that the id / adr associations do not conflict with table_id_adr
		foreach(int id_trouve, priv_id_adr.keys()) {
			if (table_id_adr.contains(id_trouve)) {
	 		// this element has an id which is already referenced (= conflict)
				return(false);
			}
		}
		// copy of id / adr associations
		foreach(int id_trouve, priv_id_adr.keys()) {
			table_id_adr.insert(id_trouve, priv_id_adr.value(id_trouve));
		}
	}
	
	return(true);
}
