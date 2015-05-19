#include <iostream>
#include <ratio>

template<typename from, typename to>
class ratio_calculator
{
};

template<std::intmax_t from_num, std::intmax_t from_denum, std::intmax_t to_num, std::intmax_t to_denum>
class ratio_calculator<std::ratio<from_num, from_denum>, std::ratio<to_num, to_denum>>
{
    public:
    template<typename Arg>
    constexpr static Arg scale(Arg val)
    {
        if(from_denum == to_denum)
        {
            return val * ((Arg) from_num / to_num);
        }
        else if(from_num == to_num)
        {
            return val * ((Arg) to_denum / from_denum);
        }
        else if(from_num == 1)
        {
            return ratio_calculator<std::ratio<1,from_denum>,std::ratio<1, to_denum>>::scale(val) / to_num;
        }
        else
        {
            return ratio_calculator<std::ratio<1,from_denum>,std::ratio<to_num, to_denum>>::scale(val) * from_num;
        }
    }
};

enum class SI_unit : int
{
    // UNIT_LESS,
    METRE,
    GRAM,
    SECOND,
    AMPERE,
    // For temperature create another prefix than std::ratio<X,X>, and specialize the ratio_calculator
    KELVEN,
    MOLE
};

template<typename Unit1, typename Unit2>
class Unit_Division;

template<typename Unit1, typename Unit2>
class Unit_Multiplication;

template<typename Prefix, SI_unit unit, typename Arg = float>
class Unit
{
    Arg val;

    public:
    explicit constexpr Unit(Arg val = 0)
        : val(val)
    {
    }

    template<typename from_prefix, SI_unit from_unit>
    constexpr Unit(Unit<from_prefix, from_unit> o)
        : val(ratio_calculator<from_prefix, Prefix>::scale(o.getVal()))
    {
        static_assert(unit == from_unit, "Cannot convert between unrelated units");
    }

    template<typename Prefix_o, SI_unit unit_o>
    auto operator/(Unit<Prefix_o, unit_o> o) ->
        Unit_Division<Unit<Prefix, unit>, Unit<Prefix_o, unit_o>>
    {
        return Unit_Division<Unit<Prefix, unit>, Unit<Prefix_o, unit_o>>(getVal() / o.getVal());
    }

    template<typename Prefix_o, SI_unit unit_o>
    auto operator*(Unit<Prefix_o, unit_o> o) ->
        Unit_Multiplication<Unit<Prefix, unit>, Unit<Prefix_o, unit_o>>
    {
        return Unit_Multiplication<Unit<Prefix, unit>, Unit<Prefix_o, unit_o>>(getVal() * o.getVal());
    }

    constexpr Arg getVal() const
    {
        return val;
    }
};

template<typename Prefix, SI_unit unit>
class Unit<Prefix, unit, void>
{
};

template<typename ratio>
class gcd_calculator
{
};

template<std::intmax_t num, std::intmax_t denum>
class gcd_calculator<std::ratio<num,denum>>
{
    public:
    constexpr static intmax_t gcd = gcd_calculator<std::ratio<denum, num % denum>>::gcd;
    using type = std::ratio<num / gcd, denum / gcd>;
};

template<std::intmax_t num>
class gcd_calculator<std::ratio<num,0>>
{
    public:
    constexpr static intmax_t gcd = num;
};
/*
static_assert(std::is_same<gcd_calculator<std::ratio<2,4>>::type, std::ratio<1,2>>::value, "NOT THE SAME");
static_assert(std::is_same<gcd_calculator<std::ratio<22,11>>::type, std::ratio<2,1>>::value, "NOT THE SAME");
*/

template<std::intmax_t scale, typename ratio>
class multiplier
{
};

template<std::intmax_t scale, std::intmax_t num, std::intmax_t denum>
class multiplier<scale, std::ratio<num, denum>>
{
    public:
    using type = std::ratio<scale * num, denum>;
};

template<std::intmax_t scale, typename ratio>
class divider
{
};

template<std::intmax_t scale, std::intmax_t num, std::intmax_t denum>
class divider<scale, std::ratio<num, denum>>
{
    public:
    using type = std::ratio<num, scale * denum>;
};

namespace units
{
    using unit_ratio = std::ratio<1,1>;

    // Units of Length
    namespace length
    {
        // SI / Metric
        using nanometer     = Unit<std::nano, SI_unit::METRE>;
        using micrometer    = Unit<std::micro, SI_unit::METRE>;
        using micron        = micrometer;
        using millimeter    = Unit<std::milli, SI_unit::METRE>;
        using centimeter    = Unit<std::centi, SI_unit::METRE>;
        // Base unit
        using meter         = Unit<unit_ratio, SI_unit::METRE>;
        using kilometer     = Unit<std::kilo, SI_unit::METRE>;

        // Imperial
        using inch_ratio    = gcd_calculator<std::ratio<254,10000>>::type;

        using line_ratio    = gcd_calculator<divider<12, inch_ratio>::type>::type;
        using line          = Unit<line_ratio, SI_unit::METRE>;
        using thou_ratio    = gcd_calculator<divider<1000, inch_ratio>::type>::type;
        using thou          = Unit<thou_ratio, SI_unit::METRE>;
        using mil           = thou;
        using inch          = Unit<inch_ratio, SI_unit::METRE>;
        using foot_ratio    = gcd_calculator<multiplier<12, inch_ratio>::type>::type;
        using foot          = Unit<foot_ratio, SI_unit::METRE>;
        using yard_ratio    = gcd_calculator<multiplier<3, foot_ratio>::type>::type;
        using yard          = Unit<yard_ratio, SI_unit::METRE>;
        using mile_ratio    = gcd_calculator<multiplier<5280, foot_ratio>::type>::type;
        using mile          = Unit<mile_ratio, SI_unit::METRE>;
        using league_ratio  = gcd_calculator<multiplier<3, mile_ratio>::type>::type;
        using league        = Unit<league_ratio, SI_unit::METRE>;

        // Marine
        using fathom_ratio  = gcd_calculator<multiplier<2, yard_ratio>::type>::type;
        using fathom        = Unit<fathom_ratio, SI_unit::METRE>;
        using nautical_mile_ratio   = std::ratio<1852, 1>;
        using nautical_mile         = Unit<nautical_mile_ratio, SI_unit::METRE>;

        // Astronomy
        using astronomical_unit_ratio   = std::ratio<149597870700, 1>;
        using astronomical_unit         = Unit<astronomical_unit_ratio, SI_unit::METRE>; 
        using lightyear_ratio   = gcd_calculator<multiplier<632410, astronomical_unit_ratio>::type>::type;
        using lightyear         = Unit<lightyear_ratio, SI_unit::METRE>;

        // Horse racing
        using furlong_ratio = std::ratio<201, 1>;
        using furlong       = Unit<furlong_ratio, SI_unit::METRE>;
    }

    // Units of Mass
    namespace mass
    {
        using nanogram      = Unit<std::nano, SI_unit::GRAM>;
        using microgram     = Unit<std::micro, SI_unit::GRAM>;
        using milligram     = Unit<std::milli, SI_unit::GRAM>;
        using centigram     = Unit<std::centi, SI_unit::GRAM>;
        using decigram      = Unit<std::deci, SI_unit::GRAM>;
        using gram          = Unit<unit_ratio, SI_unit::GRAM>;
        using kilogram      = Unit<std::kilo, SI_unit::GRAM>;
        using megagram      = Unit<std::mega, SI_unit::GRAM>;
        using tonne         = megagram;

        using pound_ratio   = gcd_calculator<std::ratio<453592, 1000>>::type;
        using grain_ratio   = gcd_calculator<divider<7000, pound_ratio>::type>::type;
        using grain         = Unit<grain_ratio, SI_unit::GRAM>;
        using dram_ratio    = gcd_calculator<divider<256, pound_ratio>::type>::type;
        using dram          = Unit<dram_ratio, SI_unit::GRAM>;
        using ounce_ratio   = gcd_calculator<divider<16, pound_ratio>::type>::type;
        using ounce         = Unit<ounce_ratio, SI_unit::GRAM>;
        using pound         = Unit<pound_ratio, SI_unit::GRAM>;
        using short_ton_ratio = gcd_calculator<multiplier<2000, pound_ratio>::type>::type;
        using short_ton     = Unit<short_ton_ratio, SI_unit::GRAM>;
        using long_ton_ratio = gcd_calculator<multiplier<2240, pound_ratio>::type>::type;
        using long_ton      = Unit<long_ton_ratio, SI_unit::GRAM>;
    }

    // Units of Time
    namespace time
    {
        // SI / Metric
        using picosecond    = Unit<std::pico, SI_unit::SECOND>;
        using nanosecond    = Unit<std::nano, SI_unit::SECOND>;
        using microsecond   = Unit<std::micro, SI_unit::SECOND>;
        using millisecond   = Unit<std::milli, SI_unit::SECOND>;
        using centisecond   = Unit<std::centi, SI_unit::SECOND>;

        // Base unit
        using second_ratio  = unit_ratio;
        using second        = Unit<second_ratio, SI_unit::SECOND>;

        // Astronomy
        using minute_ratio  = multiplier<60, second_ratio>::type;
        using minute        = Unit<minute_ratio, SI_unit::SECOND>;
        using hour_ratio    = multiplier<60, minute_ratio>::type;
        using hour          = Unit<hour_ratio, SI_unit::SECOND>;

        // Julian calender
        using day_ratio     = multiplier<24, hour_ratio>::type;
        using day           = Unit<day_ratio, SI_unit::SECOND>;
        using week_ratio    = multiplier<7, day_ratio>::type;
        using week          = Unit<week_ratio, SI_unit::SECOND>;
        using year_ratio    = multiplier<31557600, second_ratio>::type;
        using year          = Unit<year_ratio, SI_unit::SECOND>;
        using decade_ratio  = multiplier<10, year_ratio>::type;
        using decade        = Unit<decade_ratio, SI_unit::SECOND>;
        using century_ratio = multiplier<100, year_ratio>::type;
        using century       = Unit<century_ratio, SI_unit::SECOND>;
        using millennium_ratio  = multiplier<1000, year_ratio>::type;
        using millennium    = Unit<millennium_ratio, SI_unit::SECOND>;
    }

    // Units of Temperature
}

using namespace units;
using namespace units::length;
using namespace units::mass;
using namespace units::time;
/*
template<typename Unit1, int Exponent>
class Unit_Exponent
{
    Unit1 u;
};

template<typename... Units>
class Derived_Unit
{
};
*/
template<typename Unit1, typename Unit2>
class Unit_Division
{
};

template<typename Prefix1, SI_unit unit1, typename Arg1,
         typename Prefix2, SI_unit unit2, typename Arg2>
class Unit_Division<Unit<Prefix1, unit1, Arg1>,
                    Unit<Prefix2, unit2, Arg2>>
{
    public:
    Unit<Prefix1, unit1, Arg1> u1;
    Unit<Prefix2, unit2, Arg2> u2;

    public:
    explicit constexpr Unit_Division(float val)
        : u1(val), u2(1)
    {
    }

    template<typename Prefix3, SI_unit unit3, typename Arg3,
             typename Prefix4, SI_unit unit4, typename Arg4>
    constexpr Unit_Division(Unit_Division<Unit<Prefix3, unit3, Arg3>,
                                          Unit<Prefix4, unit4, Arg4>> u)
    {
        const Arg1 temp1 = ratio_calculator<Prefix3, Prefix1>::scale(u.u1.getVal());
        const Arg2 temp2 = ratio_calculator<Prefix4, Prefix2>::scale(u.u2.getVal()); 
        // Normalize
        u1 = Unit<Prefix1, unit1, Arg1>(temp1 / temp2);
        u2 = Unit<Prefix2, unit2, Arg2>(1);
    }

    auto operator*(Unit<Prefix2, unit2> o) ->
        Unit<Prefix1, unit1>
    {
        return Unit<Prefix1, unit1>(u1.getVal() * o.getVal());
    }
};

/*
template<typename Prefix1, SI_unit unit1, typename Arg1 = float, typename Prefix2, SI_unit unit2>
class Unit_Division<Unit<Prefix1, SI_unit1, 
{
    public:
    Unit1 u1;
    Unit2 u2;

    public:
    Unit_Division(float val)
        : u1(val)
    {
    }

    template<typename Unit3, typename Unit4>
    Unit_Division(Unit_Division<Unit3, Unit4> u)
        : u1(u.u1)
    {
        // Normalize
        u1 = (u1.getVal() * u2.getVal());
    }
};
*/

template<typename Unit1, typename Unit2>
class Unit_Multiplication
{
};

template<typename Prefix1, SI_unit unit1, typename Arg1,
         typename Prefix2, SI_unit unit2, typename Arg2>
class Unit_Multiplication<Unit<Prefix1, unit1, Arg1>,
                          Unit<Prefix2, unit2, Arg2>>
{
    public:
    Unit<Prefix1, unit1, Arg1> u1;
    Unit<Prefix2, unit2, Arg2> u2;

    public:
    explicit constexpr Unit_Multiplication(float val)
        : u1(val), u2(1)
    {
    }

    template<typename Prefix3, SI_unit unit3, typename Arg3,
             typename Prefix4, SI_unit unit4, typename Arg4>
    constexpr Unit_Multiplication(Unit_Multiplication<Unit<Prefix3, unit3, Arg3>,
                                                      Unit<Prefix4, unit4, Arg4>> u)
    {
        const Arg1 temp1 = ratio_calculator<Prefix3, Prefix1>::scale(u.u1.getVal());
        const Arg2 temp2 = ratio_calculator<Prefix4, Prefix2>::scale(u.u2.getVal()); 
        // Normalize
        u1 = Unit<Prefix1, unit1, Arg1>(temp1 * temp2);
        u2 = Unit<Prefix2, unit2, Arg2>(1);
    }

    auto operator/(Unit<Prefix2, unit2> o) ->
        Unit<Prefix1, unit1>
    {
        return Unit<Prefix1, unit1>(u1.getVal() / o.getVal());
    }
};

// Speed
using meters_per_second         = Unit_Division<meter, second>;
using kilometers_per_second     = Unit_Division<kilometer, second>;
using kilometers_per_hour       = Unit_Division<kilometer, hour>;
using miles_per_hour            = Unit_Division<mile, hour>;
using knot                      = Unit_Division<nautical_mile, hour>;
using feet_per_second           = Unit_Division<foot, second>;

// Area
using meters_squared            = Unit_Multiplication<meter, meter>;
using kilometers_squared        = Unit_Multiplication<kilometer, kilometer>;

void print_distance(mile m)
{
    std::cout << "fly " << m.getVal() << " miles north" << std::endl;
}

meter operator ""_meter(long double d)
{
    return meter(d);
}

meter operator ""_meter(unsigned long long int d)
{
    return meter(d);
}

second operator ""_second(long double d)
{
    return second(d);
}

second operator ""_second(unsigned long long int d)
{
    return second(d);
}

meters_per_second operator "" _mps(long double d)
{
    return meters_per_second(d);
}

meters_per_second operator "" _mps(unsigned long long int d)
{
    return meters_per_second(d);
}

int main()
{
    meter distance = 100_meter;
    kilometer km_distance = distance;
    millimeter mm_distance = distance;

    std::cout << "meters: " << distance.getVal() << std::endl;
    std::cout << "kilometers: " << km_distance.getVal() << std::endl;
    std::cout << "millimeters: " << mm_distance.getVal() << std::endl;

    std::cout << distance.getVal() << "meters is;" << std::endl;
    std::cout << ((lightyear) distance).getVal() << std::endl;
    std::cout << ((furlong) distance).getVal() << std::endl;
    std::cout << ((fathom) distance).getVal() << std::endl;
    std::cout << ((league) distance).getVal() << std::endl;
    std::cout << ((line) distance).getVal() << std::endl;
    std::cout << ((nanometer) distance).getVal() << std::endl;

    print_distance(distance);

    std::cout << "1 inch is: " << ((centimeter) inch(1)).getVal() << " centimeter" << std::endl;

    second seconds = 400_second;
    millisecond mil_second = seconds;
    minute min_second = seconds;
    hour hour_second = seconds;

    std::cout << "seconds: " << seconds.getVal() << std::endl;
    std::cout << "milliseconds: " << mil_second.getVal() << std::endl;
    std::cout << "minutes: " << min_second.getVal() << std::endl;
    std::cout << "hours: " << hour_second.getVal() << std::endl;

    // Uncomment: Compile time error
    // second sec = distance;

    meters_per_second speed = 22_mps;
    // 0.0022
    kilometers_per_second km_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << km_speed.u1.getVal() << "km/s" << std::endl;
    // 79.2
    kilometers_per_hour kph_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << kph_speed.u1.getVal() << "km/h" << std::endl;

    miles_per_hour mph_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << mph_speed.u1.getVal() << "mph" << std::endl;

    knot knot_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << knot_speed.u1.getVal() << "knots" << std::endl;

    feet_per_second fps_speed = speed;
    std::cout << speed.u1.getVal() << "m/s is " << fps_speed.u1.getVal() << "ft/s" << std::endl;

    // Division unit
    meters_per_second calc_speed = distance / seconds;
    std::cout << "Going " << distance.getVal() << " meters in " << seconds.getVal() << " seconds means a speed of: " << calc_speed.u1.getVal() << "m/s" << std::endl;
    meter distance2 = calc_speed * second(4);
    std::cout << "Going " << calc_speed.u1.getVal() << "m/s for 4 seconds, means traveling: " << distance2.getVal() << " meters" << std::endl;

    // Multiplication unit
    meters_squared area = distance * meter(1000);
    std::cout << "A square " << distance.getVal() << "meters times 1000meters has an area of: " << area.u1.getVal() << " m^2" << std::endl;
    kilometers_squared area_km = area;
    std::cout << "\t" << "which equals " << area_km.u1.getVal() << " km^2" << std::endl;
}
static_assert(std::is_same<decltype(meter() / second()), meters_per_second>::value, "");

