#ifndef FixedElement_H
	#define FixedElement_H
	#include "element.h"

	class FixedElement : public Element {
		public:
		FixedElement(QGraphicsItem * = 0, Schema * = 0);
		int nbBornesMin() const;
		int nbBornesMax() const;
		virtual bool fromXml(QDomElement &, QHash<int, Terminal *>&);
		virtual int nbBornes() const = 0;
		virtual void paint(QPainter *, const QStyleOptionGraphicsItem *) = 0;
		virtual QString typeId() = 0;
		virtual QString  nom() = 0;
	};

#endif
