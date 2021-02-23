#ifndef ENTREE_H
	#define ENTREE_H
	#include "elementfixe.h"
	/**
		This class inherits from the Fixed Element class to define a
		Entrance.
	*/
	class Entree : public FixedElement{
		public:
		Entree(QGraphicsItem * = 0, Schema * = 0);
		virtual int nbBornes() const;
		void paint(QPainter *, const QStyleOptionGraphicsItem *);
		QString typeId();
		QString nom() { return("Entr\351e"); }
	};
#endif
