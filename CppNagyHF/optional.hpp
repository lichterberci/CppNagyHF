#pragma once

namespace std {

    template <typename T>
    struct optional {
    private:
        bool _has_value;
        T _value;
    public:
        optional() : _has_value{ false }, _value{} {}
        optional(T v) : _has_value{ true }, _value{ v } {}
        bool has_value() const { return _has_value; }
        T value() const {
            if (_has_value) return _value;
            throw "std::bad_optional_access()";
        }
        T value_or(T def) const {
            return _has_value ? _value : def;
        }
        optional<T>& operator=(T v) {
            _has_value = true;
            _value = v;
            return *this;
        }
        void reset() { _has_value = false; }
    };

}
