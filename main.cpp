#include <iostream>

enum class SI_prefix : int
{
    GIGA    =  9,  // = 1000000000,
    MEGA    =  6,  // = 1000000,
    KILO    =  3,  // = 1000,
    HECTO   =  2,  // = 100,
    DECA    =  1,  // = 10,
    UNIT    =  0,  // = 1,
    DECI    = -1,  // = 0.1
    CENTI   = -2,  // = 0.01
    MILLI   = -3,  // = 0.001
    MICRO   = -6,  // = 0.000001
    NANO    = -9   // = 0.000000001
};

constexpr float ipow_10(int exp) {
  return exp == 0 ? 1 : 
         exp > 0 ? ipow_10(exp-1) * 10 :
                   ipow_10(exp+1) / 10;
}

constexpr float SI_prefix_scale(SI_prefix to, SI_prefix from)
{
    return ipow_10(((int)from - (int)to));
}

constexpr float SI_prefix_calculate(SI_prefix to, SI_prefix from, float value)
{
    return SI_prefix_scale(to, from) * value;
}

enum class SI_unit : int
{
    UNIT_LESS,
    METRE,
    GRAM,
    SECOND,
    AMPERE,
    KELVEN,
    MOLE
};

template<SI_prefix prefix, SI_unit unit>
class SIUnit
{
    float val;

    public:
    constexpr SIUnit(float val)
        : val(val)
    {
    }

    template<SI_prefix from_prefix, SI_unit from_unit>
    constexpr SIUnit(SIUnit<from_prefix, from_unit> o)
        : val(SI_prefix_calculate(prefix, from_prefix, o.getVal()))
    {
        static_assert(unit == from_unit, "Cannot convert between unrelated units");
    }

    constexpr float getVal() const
    {
        return val;
    }
};

template<typename Unit1, typename Unit2>
class SIUnit_Division
{
    public:
    Unit1 u1;
    Unit2 u2;

    public:
    SIUnit_Division(float val)
        : u1(val), u2(1)
    {
    }

    template<typename Unit3, typename Unit4>
    SIUnit_Division(SIUnit_Division<Unit3, Unit4> u)
        : u1(u.u1), u2(u.u2)
    {
        // Normalize
        u1 = (u1.getVal() * u2.getVal());
        u2 = 1;
    }
};

using unit_meter = SIUnit<SI_prefix::UNIT, SI_unit::METRE>;
using kilo_meter = SIUnit<SI_prefix::KILO, SI_unit::METRE>;
using mill_meter = SIUnit<SI_prefix::MILLI, SI_unit::METRE>;

using unit_second = SIUnit<SI_prefix::UNIT, SI_unit::SECOND>;
using mill_second = SIUnit<SI_prefix::MILLI, SI_unit::SECOND>;

using meters_per_second = SIUnit_Division<unit_meter, unit_second>;
using kilometers_per_second = SIUnit_Division<kilo_meter, unit_second>;
using kilometers_per_mill_second = SIUnit_Division<kilo_meter, mill_second>;

int main()
{
    unit_meter distance = 100;
    kilo_meter km_distance = distance;
    mill_meter mm_distance = distance;
    
    std::cout << "meters: " << distance.getVal() << std::endl;
    std::cout << "kilometers: " << km_distance.getVal() << std::endl;
    std::cout << "millimeters: " << mm_distance.getVal() << std::endl;

    // Uncomment: Compile time error
    // unit_second sec = distance;

    meters_per_second speed = 22;
    kilometers_per_second km_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << km_speed.u1.getVal() << "km/s" << std::endl;

    kilometers_per_mill_second kmms_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << kmms_speed.u1.getVal() << "km/ms" << std::endl;
}
