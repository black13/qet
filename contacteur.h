#ifndef CONTACTEUR_H
	#define CONTACTEUR_H
	#include "elementfixe.h"
	/**
	This HERITE class of the fixed class to define a
	contactor
	*/
	class Contacteur : public ElementFixe {
		public:
		Contacteur(QGraphicsItem * = 0, Schema * = 0);
		virtual int nbBornes() const;
		void paint(QPainter *, const QStyleOptionGraphicsItem *);
		QString typeId();
		QString  nom() { return("Contacteur"); }
	};
#endif
