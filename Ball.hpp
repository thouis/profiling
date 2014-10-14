#include <Point.hpp>
#include <Util.hpp>
#include <vector>
#include <math.h>

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

template <class T>
class Grid {
private:
    double _lo_x, _lo_y;
    double _bin_width, _bin_height;
    int _bins_x, _bins_y;

    // indexed by X, then Y, then number of T* in that grid
    std::vector<std::vector<std::vector<T> > > _bins;

    int clamp_i(int i) {
        return (i < 0) ? 0 : ((i < _bins_x) ? i : (_bins_x - 1));
    }

    int clamp_j(int j) {
        return (j < 0) ? 0 : ((j < _bins_y) ? j : (_bins_y - 1));
    }

    class NearIterator {
        friend class Grid<T>;

        int _lo_i, _hi_i, _lo_j, _hi_j, _cur_i, _cur_j, _idx;
        Grid<T> *_g;

        NearIterator(Grid<T> *g, double x, double y, double w, double h) : _g(g) {
            _lo_i = g->clamp_i((x - w - _g->_lo_x) / _g->_bin_width - 1);
            _lo_j = g->clamp_j((y - h - _g->_lo_y) / _g->_bin_height - 1);
            _hi_i = g->clamp_i((x + w - _g->_lo_x) / _g->_bin_width - 1);
            _hi_j = g->clamp_j((y + h - _g->_lo_y) / _g->_bin_height - 1);
            _cur_i = _lo_i;
            _cur_j = _lo_j;
            _idx = 0;
            fix();
        }

        NearIterator() {
            _idx = -1;
        }

        void fix()
        {
            while (_cur_i <= _hi_i) {
                while (_cur_j <= _hi_j) {
                    if (_idx < (int) _g->_bins[_cur_i][_cur_j].size())
                        return;
                    ++_cur_j;
                    _idx = 0;
                }
                ++_cur_i;
                _cur_j = _lo_j;
                _idx = 0;
            }
            // we've iterated over everything
            _idx = -1;
        }

        friend class NearIterator;

        void set_to_end() {
            _idx = -1;
        }

    public:
        NearIterator &operator++() {
            ++_idx;
            fix();
            return *this;
        }

        T & (operator*)() {
            return _g->_bins[_cur_i][_cur_j][_idx];
        }

        bool operator==(const NearIterator &other) {
            return ((_g == other._g) &&
                    (_lo_i == other._lo_i) &&
                    (_lo_j == other._lo_j) &&
                    (_hi_i == other._hi_i) &&
                    (_hi_j == other._hi_j) &&
                    (_idx == other._idx));
        }

        bool operator!=(const NearIterator &other) {
            return !(*this == other);
        }
    };

public:
    Grid(double lo_x, double lo_y,
         double hi_x, double hi_y,
         double bin_width, double bin_height) :
        _lo_x(lo_x), _lo_y(lo_y),
        _bin_width(bin_width), _bin_height(bin_height) {
        _bins_x = ceil((hi_x - lo_x) / bin_width);
        _bins_y = ceil((hi_y - lo_y) / bin_height);

        for (int i = 0; i < _bins_x; ++i)
            _bins.push_back(std::vector<std::vector<T> >(_bins_y));
    }

    void remove(double x, double y, T v)
    {
        double i = clamp_i((x - _lo_x) / _bin_width);
        double j = clamp_j((y - _lo_y) / _bin_height);
        auto& vec = _bins[i][j];
        vec.erase(std::remove(vec.begin(), vec.end(), v), vec.end());
    }

    void add(double x, double y, T v)
    {
        double i = clamp_i((x - _lo_x) / _bin_width);
        double j = clamp_j((y - _lo_y) / _bin_height);
        _bins[i][j].push_back(v);
    }

    NearIterator near_begin(double x, double y,
                            double width, double height)
    {
        return NearIterator(this, x, y, width, height);
    }

    NearIterator near_end(double x, double y,
                          double width, double height)
    {
        auto it = NearIterator(this, x, y, width, height);
        it.set_to_end();
        return it;
    }
        
            
};
