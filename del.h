#ifndef DEL_H
	#define DEL_H
	#include "elementfixe.h"
	/**
	This class inherits from the Fixed Element class to define a
	Light emitting diode
	*/
	class DEL : public FixedElement {
		public:
		DEL(QGraphicsItem * = 0, Schema * = 0);
		virtual int nbBornes() const;
		void paint(QPainter *, const QStyleOptionGraphicsItem *);
		QString typeId();
		QString  nom() { return("DEL"); }
	};
#endif
