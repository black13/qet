#include "elementfixe.h"
/**
Manufacturer
*/
ElementFixe::ElementFixe(QGraphicsItem *parent, Schema *scene) : Element(parent, scene) {
}

/**
	@return the minimum number of terminals that the element can have
*/
int ElementFixe::nbBornesMin() const {
	return(nbBornes());
}

/**
	@return the maximum number of terminals that the element can have
*/
int ElementFixe::nbBornesMax() const {
	return(nbBornes());
}

/**
	XML Import Method.This method is called during the import of XML content (paste, import, file opening ...) so that the element can manage itself the importation of its terminals.Here, as this class is characterized by a fixed number of terminals, the implementation requires to find exactly its terminals in the XML file.
	@param e XML element to analyze.
	@Param table_id_adr reference to the correspondence table between the IDS of the XML file and the addresses in memory.If import succeeds, you have to add the good couples (id, address).
	@return true if the import has managed, false otherwise
	
*/
bool ElementFixe::fromXml(QDomElement &e, QHash<int, Borne *> &table_id_adr) {
	/*
	the terminals will now be listed to associate their ID with their real address
	This census will serve when setting up
	*/
	
	QList<QDomElement> liste_bornes;
	// Course of children of the element
	for (QDomNode enfant = e.firstChild() ; !enfant.isNull() ; enfant = enfant.nextSibling()) {
	// on s'interesse a l'element XML "bornes"
		QDomElement bornes = enfant.toElement();
		if (bornes.isNull() || bornes.tagName() != "bornes") continue;
		// COURSES OF CHILDREN OF THE ELEMENT XML "BORNES"
		for (QDomNode node_borne = bornes.firstChild() ; !node_borne.isNull() ; node_borne = node_borne.nextSibling()) {
		// on s'interesse a l'element XML "borne"
			QDomElement borne = node_borne.toElement();
			if (!borne.isNull() && Borne::valideXml(borne)) liste_bornes.append(borne);
		}
	}
	
	QHash<int, Borne *> priv_id_adr;
	int bornes_non_trouvees = 0;
	foreach(QGraphicsItem *qgi, children()) {
		if (Borne *p = qgraphicsitem_cast<Borne *>(qgi)) {
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
		// verify that ID / ADR associations do not conflict with table_id_adr
		foreach(int id_trouve, priv_id_adr.keys()) {
			if (table_id_adr.contains(id_trouve)) {
			// This element has an ID that is already referring (= conflict)
				return(false);
			}
		}
		// copy associations ID / ADR
		foreach(int id_trouve, priv_id_adr.keys()) {
			table_id_adr.insert(id_trouve, priv_id_adr.value(id_trouve));
		}
	}
	
	return(true);
}
