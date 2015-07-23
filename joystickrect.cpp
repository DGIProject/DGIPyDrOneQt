#include "joystickrect.h"

joystickRect::joystickRect(remoteController *controllerJ)
{
    controller = controllerJ;

    this->setPos(70, 70);
    controller->updatePositionJoystick(this->pos());
}

QRectF joystickRect::boundingRect() const
{
    return QRectF(0,0,100,100);
}

void joystickRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget)
{
    QRectF rect = boundingRect();

    QPen pen(Qt::blue, 3);
    painter->setPen(pen);
    painter->drawRect(rect);
}

void joystickRect::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);

    this->setPos(70, 70);

    _location = QPointF(70, 70);

    controller->updatePositionJoystick(_location);
}


// for supporting moving the box across the scene
void joystickRect::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _dragStart = event->pos();
}


// for supporting moving the box across the scene
void joystickRect::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    QPointF newPos = event->pos();

    _location += (newPos - _dragStart);

    if(_location.x() >= 0)
    {
        if(_location.x() <= 140)
        {
            this->setPos(_location.x(), this->pos().y());
        }
        else
        {
            _location = QPointF(140, _location.y());
        }
    }
    else
    {
        _location = QPointF(0, _location.y());
    }

    if(_location.y() >= 0)
    {
        if(_location.y() <= 140)
        {
            this->setPos(this->pos().x(), _location.y());
        }
        else
        {
            _location = QPointF(_location.x(), 140);
        }
    }
    else
    {
        _location = QPointF(_location.x(), 0);
    }

    controller->updatePositionJoystick(_location);
}

