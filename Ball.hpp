#include <Point.hpp>
#include <Util.hpp>

class Ball {
private:
    Point _center;
    Point _velocity;
    double _color[3];
    double _radius;

public:
    Ball() : _center(CS207::random(-1.0, 1.0), CS207::random(-1.0, 1.0), -10),
             _velocity(CS207::random(-1.0, 1.0), CS207::random(-1.0, 1.0), 0),
             _radius(0.02) {
        _color[0] = CS207::random(0.5, 1);
        _color[1] = CS207::random(0.5, 1);
        _color[2] = CS207::random(0.5, 1);
    }

    Point &center() {
        return _center;
    }
    double &radius() {
        return _radius;
    }
    double &color() {
        return _color[0];
    }

    int near(Ball &other)
    {
        Point vec = other._center - _center;
        // check for overlap
        return (norm(vec) < (_radius + other._radius));
    }

    void collide(Ball &other)
    {
        Point vec = other._center - _center;

        // collide if they overlap, and relative velocity is moving
        // them together
        if (norm(vec) < (_radius + other._radius)) {
            vec /= norm(vec);
            if (dot(_velocity, vec) > 0) 
                _velocity -= 2 * dot(_velocity, vec) * vec;
            vec = -vec;
            if (dot(other._velocity, vec) > 0) 
                other._velocity -= 2 * dot(other._velocity, vec) * vec;
        }
    }

    void collide_walls()
    {
        // min X
        if ((_center.x - _radius < -1) && (_velocity.x < 0))
            _velocity.x *= -1.0;
        // max X
        if ((_center.x + _radius > 1) && (_velocity.x > 0))
            _velocity.x *= -1.0;
        
        // min Y
        if ((_center.y - _radius < -1) && (_velocity.y < 0))
            _velocity.y *= -1.0;
        // may Y
        if ((_center.y + _radius > 1) && (_velocity.y > 0))
            _velocity.y *= -1.0;
    }

    void step(double t)
    {
        _center += t * _velocity;
    }
};

