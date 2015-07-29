#ifndef JOYSTICKRECT_H
#define JOYSTICKRECT_H

#include "remotecontroller.h"
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>


class joystickRect : public QGraphicsItem
{
public:
    joystickRect(remoteController * controllerJ);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;
    void setPosDirect(qreal x, qreal y);
    QPointF _location;

private:
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    QPointF _dragStart;

    remoteController *controller;
};

#endif // JOYSTICKRECT_H
