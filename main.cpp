#include <iostream>
#include <ratio>
#include <tuple>

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

template<typename Prefix, SI_unit unit>
class Base_Unit
{
    public:
        using prefix = Prefix;
        static constexpr SI_unit u = unit;
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

namespace units_base
{
    using unit_ratio = std::ratio<1,1>;

    // Units of Length
    namespace length
    {
        // SI / Metric
        using nanometer_base     = Base_Unit<std::nano, SI_unit::METRE>;
        using micrometer_base    = Base_Unit<std::micro, SI_unit::METRE>;
        using micron_base        = micrometer_base;
        using millimeter_base    = Base_Unit<std::milli, SI_unit::METRE>;
        using centimeter_base    = Base_Unit<std::centi, SI_unit::METRE>;
        // Base unit
        using meter_base         = Base_Unit<unit_ratio, SI_unit::METRE>;
        using kilometer_base     = Base_Unit<std::kilo, SI_unit::METRE>;

        // Imperial
        using inch_ratio         = gcd_calculator<std::ratio<254,10000>>::type;

        using line_ratio         = gcd_calculator<divider<12, inch_ratio>::type>::type;
        using line_base          = Base_Unit<line_ratio, SI_unit::METRE>;
        using thou_ratio         = gcd_calculator<divider<1000, inch_ratio>::type>::type;
        using thou_base          = Base_Unit<thou_ratio, SI_unit::METRE>;
        using mil_base           = thou_base;
        using inch_base          = Base_Unit<inch_ratio, SI_unit::METRE>;
        using foot_ratio         = gcd_calculator<multiplier<12, inch_ratio>::type>::type;
        using foot_base          = Base_Unit<foot_ratio, SI_unit::METRE>;
        using yard_ratio         = gcd_calculator<multiplier<3, foot_ratio>::type>::type;
        using yard_base          = Base_Unit<yard_ratio, SI_unit::METRE>;
        using mile_ratio         = gcd_calculator<multiplier<5280, foot_ratio>::type>::type;
        using mile_base          = Base_Unit<mile_ratio, SI_unit::METRE>;
        using league_ratio       = gcd_calculator<multiplier<3, mile_ratio>::type>::type;
        using league_base        = Base_Unit<league_ratio, SI_unit::METRE>;

        // Marine
        using fathom_ratio       = gcd_calculator<multiplier<2, yard_ratio>::type>::type;
        using fathom_base        = Base_Unit<fathom_ratio, SI_unit::METRE>;
        using nautical_mile_ratio       = std::ratio<1852, 1>;
        using nautical_mile_base        = Base_Unit<nautical_mile_ratio, SI_unit::METRE>;

        // Astronomy
        using astronomical_unit_ratio   = std::ratio<149597870700, 1>;
        using astronomical_unit_base    = Base_Unit<astronomical_unit_ratio, SI_unit::METRE>; 
        using lightyear_ratio    = gcd_calculator<multiplier<632410, astronomical_unit_ratio>::type>::type;
        using lightyear_base     = Base_Unit<lightyear_ratio, SI_unit::METRE>;

        // Horse racing
        using furlong_ratio      = std::ratio<201, 1>;
        using furlong_base       = Base_Unit<furlong_ratio, SI_unit::METRE>;
    }

    // Units of Mass
    namespace mass
    {
        using nanogram_base      = Base_Unit<std::nano, SI_unit::GRAM>;
        using microgram_base     = Base_Unit<std::micro, SI_unit::GRAM>;
        using milligram_base     = Base_Unit<std::milli, SI_unit::GRAM>;
        using centigram_base     = Base_Unit<std::centi, SI_unit::GRAM>;
        using decigram_base      = Base_Unit<std::deci, SI_unit::GRAM>;
        using gram_base          = Base_Unit<unit_ratio, SI_unit::GRAM>;
        using kilogram_base      = Base_Unit<std::kilo, SI_unit::GRAM>;
        using megagram_base      = Base_Unit<std::mega, SI_unit::GRAM>;
        using tonne_base         = megagram_base;

        using pound_ratio        = gcd_calculator<std::ratio<453592, 1000>>::type;
        using grain_ratio        = gcd_calculator<divider<7000, pound_ratio>::type>::type;
        using grain_base         = Base_Unit<grain_ratio, SI_unit::GRAM>;
        using dram_ratio         = gcd_calculator<divider<256, pound_ratio>::type>::type;
        using dram_base          = Base_Unit<dram_ratio, SI_unit::GRAM>;
        using ounce_ratio        = gcd_calculator<divider<16, pound_ratio>::type>::type;
        using ounce_base         = Base_Unit<ounce_ratio, SI_unit::GRAM>;
        using pound_base         = Base_Unit<pound_ratio, SI_unit::GRAM>;
        using short_ton_ratio    = gcd_calculator<multiplier<2000, pound_ratio>::type>::type;
        using short_ton_base     = Base_Unit<short_ton_ratio, SI_unit::GRAM>;
        using long_ton_ratio     = gcd_calculator<multiplier<2240, pound_ratio>::type>::type;
        using long_ton_base      = Base_Unit<long_ton_ratio, SI_unit::GRAM>;
    }

    // Units of Time
    namespace time
    {
        // SI / Metric
        using picosecond_base    = Base_Unit<std::pico, SI_unit::SECOND>;
        using nanosecond_base    = Base_Unit<std::nano, SI_unit::SECOND>;
        using microsecond_base   = Base_Unit<std::micro, SI_unit::SECOND>;
        using millisecond_base   = Base_Unit<std::milli, SI_unit::SECOND>;
        using centisecond_base   = Base_Unit<std::centi, SI_unit::SECOND>;

        // Base unit
        using second_ratio       = unit_ratio;
        using second_base        = Base_Unit<second_ratio, SI_unit::SECOND>;

        // Astronomy
        using minute_ratio       = multiplier<60, second_ratio>::type;
        using minute_base        = Base_Unit<minute_ratio, SI_unit::SECOND>;
        using hour_ratio         = multiplier<60, minute_ratio>::type;
        using hour_base          = Base_Unit<hour_ratio, SI_unit::SECOND>;

        // Julian calender
        using day_ratio          = multiplier<24, hour_ratio>::type;
        using day_base           = Base_Unit<day_ratio, SI_unit::SECOND>;
        using week_ratio         = multiplier<7, day_ratio>::type;
        using week_base          = Base_Unit<week_ratio, SI_unit::SECOND>;
        using year_ratio         = multiplier<31557600, second_ratio>::type;
        using year_base          = Base_Unit<year_ratio, SI_unit::SECOND>;
        using decade_ratio       = multiplier<10, year_ratio>::type;
        using decade_base        = Base_Unit<decade_ratio, SI_unit::SECOND>;
        using century_ratio = multiplier<100, year_ratio>::type;
        using century_base       = Base_Unit<century_ratio, SI_unit::SECOND>;
        using millennium_ratio  = multiplier<1000, year_ratio>::type;
        using millennium_base    = Base_Unit<millennium_ratio, SI_unit::SECOND>;
    }

    // Units of Temperature
}

template<typename Unit1, std::intmax_t Exponent>
class Unit_Exponent
{
    public:
        static constexpr std::intmax_t exponent = Exponent;
        using u = Unit1;
};

template<typename... Units>
class Sorted
{
};

template<typename car>
class Sorted<car>
{
    public:
    using sorted_type = std::true_type;
    static constexpr bool value = true;
};

template<typename car1, typename car2, typename... cdr>
class Sorted<car1, car2, cdr...>
{
    public:
    static constexpr SI_unit car1_tu = car1::u::u;
    static constexpr SI_unit car2_tu = car2::u::u;
  
    using sorted_type = typename std::conditional<car1_tu <= car2_tu,
          typename Sorted<car2, cdr...>::sorted_type,
          std::false_type>::type;

    static constexpr bool value = sorted_type::value;
};

/*
// WORKING SORTER BELOW
template<int id, typename... Units>
class find_smallest_id
{
};

template<int id, typename T>
class find_smallest_id<id, T>
{
    public:
    using smallest = T;
    static constexpr int _id = id;
};

template<int id, typename T, typename... Units>
class find_smallest_id<id, T, Units...>
{
    public:
    using recursive = typename find_smallest_id<id+1, Units...>::smallest;
    static constexpr int recursive_id = find_smallest_id<id+1, Units...>::_id;
    static constexpr SI_unit recursive_value = recursive::u::u;
    static constexpr SI_unit T_value = T::u::u;

    using smallest = typename std::conditional<recursive_value <= T_value, recursive, T>::type;
    static constexpr int _id = recursive_value <= T_value ? recursive_id : id;
};

template<int idremove, int id, typename... Units>
class tuple_remove
{
};

template<int idremove, int id, typename T>
class tuple_remove<idremove, id, T>
{
    public:
    using type = typename std::conditional<idremove == id, 
          std::tuple<>,
          std::tuple<T>>::type;
};

template<int idremove, int id, typename T, typename... Units>
class tuple_remove<idremove, id, T, Units...>
{
    public:
    using type = typename std::conditional<idremove == id, 
          typename tuple_remove<idremove, id+1, Units...>::type,
          decltype(std::tuple_cat(std::tuple<T>(), typename tuple_remove<idremove, id+1, Units...>::type()))>::type;
};

template<typename... Units>
class TemplateSorter
{
};

template<typename T>
class TemplateSorter<std::tuple<T>>
{
    public:
    using sorted = std::tuple<T>;
};

template<typename... Units>
class TemplateSorter<std::tuple<Units...>>
{
    public:
    static constexpr int id = find_smallest_id<0, Units...>::_id;
    using smallest = typename std::tuple_element<id, std::tuple<Units...>>::type;
    using smallest_removed = typename tuple_remove<id, 0, Units...>::type;

    using sorted = decltype(std::tuple_cat(std::tuple<smallest>(), typename TemplateSorter<smallest_removed>::sorted()));
};
*/

template<typename... Units>
class ZeroExponentChecker
{
};

template<>
class ZeroExponentChecker<>
{
    public:
    static constexpr bool value = true;
};

template<typename T, typename... Units>
class ZeroExponentChecker<T, Units...>
{
    public:
    static constexpr int exponent = T::exponent;
    static constexpr bool value = (exponent == 0) ? false : ZeroExponentChecker<Units...>::value;
};

template<typename NumericalType, typename I, typename O>
class ConversionRate
{
};

template<typename NumericalType>
class ConversionRate<NumericalType, std::tuple<>, std::tuple<>>
{
    public:
    constexpr static NumericalType scale = 1;
};

template<typename NumericalType>
constexpr NumericalType power(NumericalType x, std::intmax_t n)
{
    if(n == 0) return 1;
    else return x * power(x, n-1);
}

constexpr std::intmax_t abs(std::intmax_t n)
{
    if(n < 0) return -n;
    else return n;
}

template<typename NumericalType, typename unit_i, typename... Units_i, typename unit_o, typename... Units_o>
class ConversionRate<NumericalType, std::tuple<unit_i, Units_i...>, std::tuple<unit_o, Units_o...>>
{
    public:
    // Assert the single unit is in the same dimension
    static_assert(unit_o::exponent == unit_i::exponent, "Not same dimension");
    // Get the base unit of that dimension
    using base_unit_o = typename unit_o::u;
    using base_unit_i = typename unit_i::u;
    // Check that it is the same base unit
    constexpr static SI_unit u_o = base_unit_o::u;
    constexpr static SI_unit u_i = base_unit_i::u;
    static_assert(u_o == u_i, "Not same SI base unit");
    // Get the prefix of that base unit
    using prefix_o = typename base_unit_o::prefix;
    using prefix_i = typename base_unit_i::prefix;
    // And calculate the ratio for the scaling
    // TODO: Why are the last terms flipped?
    // XXX: Critical error, but working
    constexpr static NumericalType scale = power(ratio_calculator<prefix_o, prefix_i>::scale(
            ConversionRate<NumericalType, std::tuple<Units_o...>, std::tuple<Units_i...>>::scale), abs(unit_o::exponent));
};

template<typename NumericalType, typename... Units>
class Unit;

template<typename NumericalType, typename U>
class UnitTyper
{
};

template<typename NumericalType, typename... Args>
class UnitTyper<NumericalType, std::tuple<Args...>>
{
    public:
    using type = Unit<NumericalType, Args...>;
};

template<typename NumericalType, typename I, typename O>
class DivisionType
{
};

template<typename NumericalType>
class DivisionType<NumericalType, std::tuple<>, std::tuple<>>
{
    public:
    using combined = std::tuple<>;
};

template<typename NumericalType, typename... Units_i>
class DivisionType<NumericalType, std::tuple<Units_i...>, std::tuple<>>
{
    public:
    using combined = std::tuple<Units_i...>;
};

template<typename NumericalType, typename unit_o, typename... Units_o>
class DivisionType<NumericalType, std::tuple<>, std::tuple<unit_o, Units_o...>>
{
    public:
    using base_unit_o = typename unit_o::u;
    constexpr static std::intmax_t u_o_exp = unit_o::exponent;
   
    using combined = decltype(std::tuple_cat(std::tuple<Unit_Exponent<base_unit_o, (-u_o_exp)>>(), std::tuple<Units_o...>()));
};

template<typename NumericalType, typename unit_i, typename... Units_i, typename unit_o, typename... Units_o>
class DivisionType<NumericalType, std::tuple<unit_i, Units_i...>, std::tuple<unit_o, Units_o...>>
{
    public:
    // Get the base unit of that dimension
    using base_unit_o = typename unit_o::u;
    using base_unit_i = typename unit_i::u;

    constexpr static std::intmax_t u_o_exp = unit_o::exponent;
    constexpr static std::intmax_t u_i_exp = unit_i::exponent;

    // Check that it is the same base unit
    constexpr static SI_unit u_o = base_unit_o::u;
    constexpr static SI_unit u_i = base_unit_i::u;
   
    using combined = typename std::conditional<(u_o < u_i),
          // output u_o, process with u_i
          decltype(std::tuple_cat(std::tuple<Unit_Exponent<base_unit_o, (-u_o_exp)>>(), typename DivisionType<NumericalType, std::tuple<unit_i, Units_i...>, std::tuple<Units_o...>>::combined())),
          typename std::conditional<(u_o > u_i),
            // output u_i, process with u_o
            decltype(std::tuple_cat(std::tuple<unit_i>(), typename DivisionType<NumericalType, std::tuple<Units_i...>, std::tuple<unit_o, Units_o...>>::combined())),
            // Check if the exponent math yields a zero term
            typename std::conditional<((u_i_exp-u_o_exp) == 0),
                // if it does, eliminate the term completely
                typename DivisionType<NumericalType, std::tuple<Units_i...>, std::tuple<Units_o...>>::combined,
                // u_o is equal to u_i, do exponents
                decltype(std::tuple_cat(std::tuple<Unit_Exponent<base_unit_o, (u_i_exp-u_o_exp)>>(), typename DivisionType<NumericalType, std::tuple<Units_i...>, std::tuple<Units_o...>>::combined())) // TODO: Scaling on prefix
                    >::type
            >::type
        >::type;

    using type = typename UnitTyper<NumericalType, combined>::type;
};

template<typename NumericalType, typename I, typename O>
class MultiplicationType
{
};

template<typename NumericalType>
class MultiplicationType<NumericalType, std::tuple<>, std::tuple<>>
{
    public:
    using combined = std::tuple<>;
};

template<typename NumericalType, typename... Units_i>
class MultiplicationType<NumericalType, std::tuple<Units_i...>, std::tuple<>>
{
    public:
    using combined = std::tuple<Units_i...>;
};

template<typename NumericalType, typename... Units_o>
class MultiplicationType<NumericalType, std::tuple<>, std::tuple<Units_o...>>
{
    public:
    using combined = std::tuple<Units_o...>;
};

template<typename NumericalType, typename unit_i, typename... Units_i, typename unit_o, typename... Units_o>
class MultiplicationType<NumericalType, std::tuple<unit_i, Units_i...>, std::tuple<unit_o, Units_o...>>
{
    public:
    // Get the base unit of that dimension
    using base_unit_o = typename unit_o::u;
    using base_unit_i = typename unit_i::u;

    constexpr static std::intmax_t u_o_exp = unit_o::exponent;
    constexpr static std::intmax_t u_i_exp = unit_i::exponent;

    // Check that it is the same base unit
    constexpr static SI_unit u_o = base_unit_o::u;
    constexpr static SI_unit u_i = base_unit_i::u;
   
    using combined = typename std::conditional<(u_o < u_i),
          // output u_o, process with u_i
          decltype(std::tuple_cat(std::tuple<unit_o>(), typename MultiplicationType<NumericalType, std::tuple<unit_i, Units_i...>, std::tuple<Units_o...>>::combined())),
          typename std::conditional<(u_o > u_i),
            // output u_i, process with u_o
            decltype(std::tuple_cat(std::tuple<unit_i>(), typename MultiplicationType<NumericalType, std::tuple<Units_i...>, std::tuple<unit_o, Units_o...>>::combined())),
            // Check if the exponent math yields a zero term
            typename std::conditional<((u_i_exp+u_o_exp) == 0),
                // if it does, eliminate the term completely
                typename MultiplicationType<NumericalType, std::tuple<Units_i...>, std::tuple<Units_o...>>::combined,
                // u_o is equal to u_i, do exponents
                decltype(std::tuple_cat(std::tuple<Unit_Exponent<base_unit_o, (u_i_exp+u_o_exp)>>(), typename MultiplicationType<NumericalType, std::tuple<Units_i...>, std::tuple<Units_o...>>::combined())) // TODO: Scaling on prefix
                    >::type
            >::type
        >::type;

    using type = typename UnitTyper<NumericalType, combined>::type;
};

template<typename NumericalType, typename... Units>
class Unit
{
    public:
    // Check that this unit is in normal form, units only exist in normal form!
    // Normal form has elements in sorted order
    /*
    using NormalForm = typename TemplateSorter<std::tuple<Units...>>::sorted;
    using CurrentForm = typename std::tuple<Units...>;
    static_assert(std::is_same<NormalForm, CurrentForm>::value, "Unit is not in NormalForm: Base units are not in sorted order");
    */
    static_assert(Sorted<Units...>::value, "Unit is not in NormalForm: Base units are not in sorted order");
    // Normal form has no 0-exponent terms
    static_assert(ZeroExponentChecker<Units...>::value, "Unit is not in NormalForm: Contains a zero exponent base unit");

    // The numerical value of this unit
    NumericalType val;

    public:
    explicit constexpr Unit(NumericalType val = 0)
        : val(val)
    {
    }

    template<typename NumericalType_o, typename... Units_o>
    constexpr Unit(const Unit<NumericalType_o, Units_o...>& o)
    {
        val = ConversionRate<NumericalType, std::tuple<Units...>, std::tuple<Units_o...>>::scale * o.getVal();
    }

    template<typename NumericalType_o, typename... Units_o>
    constexpr auto operator/(Unit<NumericalType_o, Units_o...> o) const ->
        typename DivisionType<NumericalType, std::tuple<Units...>, std::tuple<Units_o...>>::type 
    {
        return typename DivisionType<NumericalType, std::tuple<Units...>, std::tuple<Units_o...>>::type(getVal() / o.getVal());
    }

    template<typename NumericalType_o, typename... Units_o>
    constexpr auto operator*(Unit<NumericalType_o, Units_o...> o) const ->
        typename MultiplicationType<NumericalType, std::tuple<Units...>, std::tuple<Units_o...>>::type
    {
        return typename MultiplicationType<NumericalType, std::tuple<Units...>, std::tuple<Units_o...>>::type(getVal() * o.getVal());
    }

    constexpr NumericalType getVal() const
    {
        return val;
    }
};


namespace units
{
    using namespace units_base;
    using namespace units_base::length;
    using namespace units_base::mass;
    using namespace units_base::time;

    // Units of Length
    namespace length
    {
        // SI / Metric
        using nanometer     = Unit<float, Unit_Exponent<nanometer_base,1>>;
        using micrometer    = Unit<float, Unit_Exponent<micrometer_base,1>>;
        using micron        = micrometer;
        using millimeter    = Unit<float, Unit_Exponent<millimeter_base,1>>;
        using centimeter    = Unit<float, Unit_Exponent<centimeter_base,1>>;

        // Base unit
        using meter         = Unit<float, Unit_Exponent<meter_base,1>>;
        using kilometer     = Unit<float, Unit_Exponent<kilometer_base,1>>;

        // Imperial
        using line          = Unit<float, Unit_Exponent<line_base,1>>;
        using thou          = Unit<float, Unit_Exponent<thou_base,1>>;
        using mil           = thou;
        using inch          = Unit<float, Unit_Exponent<inch_base,1>>;
        using foot          = Unit<float, Unit_Exponent<foot_base,1>>;
        using yard          = Unit<float, Unit_Exponent<yard_base,1>>;
        using mile          = Unit<float, Unit_Exponent<mile_base,1>>;
        using league        = Unit<float, Unit_Exponent<league_base,1>>;

        // Marine
        using fathom        = Unit<float, Unit_Exponent<fathom_base,1>>;
        using nautical_mile = Unit<float, Unit_Exponent<nautical_mile_base,1>>;

        // Astronomy
        using astronomical_unit = Unit<float, Unit_Exponent<astronomical_unit_base,1>>;
        using lightyear         = Unit<float, Unit_Exponent<lightyear_base,1>>;

        // Horse racing
        using furlong       = Unit<float, Unit_Exponent<furlong_base,1>>;
    }

    // Units of Mass
    namespace mass
    {
        using nanogram      = Unit<float, Unit_Exponent<nanogram_base,1>>;
        using microgram     = Unit<float, Unit_Exponent<microgram_base,1>>;
        using milligram     = Unit<float, Unit_Exponent<milligram_base,1>>;
        using centigram     = Unit<float, Unit_Exponent<centigram_base,1>>;
        using decigram      = Unit<float, Unit_Exponent<decigram_base,1>>;
        using gram          = Unit<float, Unit_Exponent<gram_base,1>>;
        using kilogram      = Unit<float, Unit_Exponent<kilogram_base,1>>;
        using megagram      = Unit<float, Unit_Exponent<megagram_base,1>>;
        using tonne         = megagram;

        using grain         = Unit<float, Unit_Exponent<grain_base,1>>;
        using dram          = Unit<float, Unit_Exponent<dram_base,1>>;
        using ounce         = Unit<float, Unit_Exponent<ounce_base,1>>;
        using pound         = Unit<float, Unit_Exponent<pound_base,1>>;
        using short_ton     = Unit<float, Unit_Exponent<short_ton_base,1>>;
        using long_ton      = Unit<float, Unit_Exponent<long_ton_base,1>>;
    }

    // Units of Time
    namespace time
    {
        // SI / Metric
        using picosecond    = Unit<float, Unit_Exponent<picosecond_base,1>>;
        using nanosecond    = Unit<float, Unit_Exponent<nanosecond_base,1>>;
        using microsecond   = Unit<float, Unit_Exponent<microsecond_base,1>>;
        using millisecond   = Unit<float, Unit_Exponent<millisecond_base,1>>;
        using centisecond   = Unit<float, Unit_Exponent<centisecond_base,1>>;

        // Base unit
        using second        = Unit<float, Unit_Exponent<second_base,1>>;

        // Astronomy
        using minute        = Unit<float, Unit_Exponent<minute_base,1>>;
        using hour          = Unit<float, Unit_Exponent<hour_base,1>>;

        // Julian calender
        using day           = Unit<float, Unit_Exponent<day_base,1>>;
        using week          = Unit<float, Unit_Exponent<week_base,1>>;
        using year          = Unit<float, Unit_Exponent<year_base,1>>;
        using decade        = Unit<float, Unit_Exponent<decade_base,1>>;
        using century       = Unit<float, Unit_Exponent<century_base,1>>;
        using millennium    = Unit<float, Unit_Exponent<millennium_base,1>>;
    }
}

using namespace units;
using namespace units::length;
using namespace units::mass;
using namespace units::time;

// Speed
using meters_per_second         = Unit<float, Unit_Exponent<meter_base,1>, Unit_Exponent<second_base,-1>>;
//using meters_per_second         = Unit<float, Unit_Exponent<second_base,-1>, Unit_Exponent<meter_base,1>>;
using kilometers_per_second     = Unit<float, Unit_Exponent<kilometer_base,1>, Unit_Exponent<second_base,-1>>;
using kilometers_per_hour       = Unit<float, Unit_Exponent<kilometer_base,1>, Unit_Exponent<hour_base,-1>>;
using miles_per_hour            = Unit<float, Unit_Exponent<mile_base,1>, Unit_Exponent<hour_base,-1>>;
using knot                      = Unit<float, Unit_Exponent<nautical_mile_base,1>, Unit_Exponent<hour_base,-1>>;
using feet_per_second           = Unit<float, Unit_Exponent<foot_base,1>, Unit_Exponent<second_base,-1>>;

// Area
using meters_squared            = Unit<float, Unit_Exponent<meter_base,2>>;
using kilometers_squared        = Unit<float, Unit_Exponent<kilometer_base,2>>;

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

minute operator ""_minute(long double d)
{
    return minute(d);
}

minute operator ""_minute(unsigned long long int d)
{
    return minute(d);
}

hour operator ""_hour(long double d)
{
    return hour(d);
}

hour operator ""_hour(unsigned long long int d)
{
    return hour(d);
}

meters_per_second operator "" _mps(long double d)
{
    return meters_per_second(d);
}

meters_per_second operator "" _mps(unsigned long long int d)
{
    return meters_per_second(d);
}

namespace units
{
    using namespace units_base;
    using namespace units_base::length;
    using namespace units_base::mass;
    using namespace units_base::time;

    using ampere = Unit<float, Unit_Exponent<Base_Unit<unit_ratio, SI_unit::AMPERE>,1>>;
    using volt = Unit<float,
	Unit_Exponent<meter_base, 2>, 
	Unit_Exponent<kilogram_base, 1>, 
	Unit_Exponent<second_base, -3>, 
	Unit_Exponent<Base_Unit<unit_ratio, SI_unit::AMPERE>, -1>
    >;
    using watt = Unit<float,
	Unit_Exponent<meter_base, 2>,
	Unit_Exponent<kilogram_base, 1>,
	Unit_Exponent<second_base, -3>
    >;
    using joule = Unit<float,
	Unit_Exponent<meter_base, 2>,
	Unit_Exponent<kilogram_base, 1>, 
	Unit_Exponent<second_base, -2>
    >;
}

using namespace units;

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
    std::cout << speed.getVal() << "m/s is " << km_speed.getVal() << "km/s" << std::endl;
    // 79.2
    kilometers_per_hour kph_speed = speed;
    std::cout << speed.getVal() << "m/s is " << kph_speed.getVal() << "km/h" << std::endl;

    miles_per_hour mph_speed = speed;
    std::cout << speed.getVal() << "m/s is " << mph_speed.getVal() << "mph" << std::endl;

    knot knot_speed = speed;
    std::cout << speed.getVal() << "m/s is " << knot_speed.getVal() << "knots" << std::endl;

    feet_per_second fps_speed = speed;
    std::cout << speed.getVal() << "m/s is " << fps_speed.getVal() << "ft/s" << std::endl;

    // Division unit
    meters_per_second calc_speed = distance / seconds;
    std::cout << "Going " << distance.getVal() << " meters in " << seconds.getVal() << " seconds means a speed of: " << calc_speed.getVal() << "m/s" << std::endl;
    
    meter distance2 = calc_speed * second(4);
    std::cout << "Going " << calc_speed.getVal() << "m/s for 4 seconds, means traveling: " << distance2.getVal() << " meters" << std::endl;
    
    // Multiplication unit
    meters_squared area = distance * meter(1000);
    std::cout << "A square " << distance.getVal() << "meters times 1000meters has an area of: " << area.getVal() << " m^2" << std::endl;
    kilometers_squared area_km = area;
    std::cout << "\t" << "which equals " << area_km.getVal() << " km^2" << std::endl;

    auto voltage = volt(10);
    auto current = ampere(1.44);
    auto power = voltage * current;
    //static_assert(std::is_same<decltype(power), watt>::value, "");

    // TODO: power * hour doesn't work
    std::cout << voltage.getVal() << " volts * " << current.getVal() << " ampere" << std::endl;
    std::cout << " = " << power.getVal() << " watts " << std::endl;
    std::cout << " running it for 1 minute spends " << ((joule) (power * (second) 1_minute)).getVal() << " joules" << std::endl;
    std::cout << " running it for 1 hour spends " << ((joule) (power * (second) 1_hour)).getVal() << " joules" << std::endl;

}
static_assert(std::is_same<decltype(meter() / second()), meters_per_second>::value, "");

