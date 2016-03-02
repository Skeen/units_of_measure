# Units of Measure

[//]: # (Finish off this documentation, prettify, ect)
[//]: # (https://www.sharelatex.com/project/558a66004f418282269e9d65/output/output.pdf?cache_bust=1456940073701&compileGroup=standard&build=15338654260)

A small library that implements 'Units of Measure' (dimensional analysis)
via compiler-generated argument-dependent types. Usability is provided via 
type-deduction and user-defined literals (c++11).

The library has been produced merely as a proof of concept, and is based
upon Andrew John Kennedy's thesis; ['Programming Languages and Dimensions'](http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=F8D1190123FAE5726E013BC312EEED6C?doi=10.1.1.93.807&rep=rep1&type=pdf)
(1995), which provides an implementation of dimensional analysis by language
extension of ML. Kennedy has since had his implementation introduced  in F#.

* A similar (production quality) library for C++ is; [Boost.Units](http://www.boost.org/doc/libs/1_60_0/doc/html/boost_units.html)
* A similar (production quality) library for haskell is; [dimensional](https://hackage.haskell.org/package/dimensional)

> *”Unit-of-measure are to science, what types are to programming.”*
> - Andrew John Kennedy in ’Types for units-of-measure’

## Motivation

### Overloading and understanding

The basic motivation for the library is avoiding units related programming bugs.

Inspect the following line of code:
```c++
sleep(100);
```
Answering the simple question of; how long the above code sleeps for,
is dependent on the context. Possible answers include;

* 100 seconds; Unix application developers
* 100 milliseconds; Windows application developers
* 100 nanoseconds; Digital signal processors
* 100 jiffies; Kernel module developers
* 100 clock cycles / loop cycles; Embedded developers
* 100 times standard timing delay / latency; Electronics and firmware developers

Replace the above line of code with:
```c++
sleep(100_seconds);
// or
sleep(100_msec);
```
And answering the above question, is now trivial, as the intent of the
programmer is directly propergated by the code.

Additionally, the need for `sleep` vs `usleep` is now gone, as a single
identifier for the function can be used. Simply by overloading the declaration
of `sleep` to include multiple variants. If a 'seconds' overload does not
exists, but a 'milliseconds' one does, type / unit conversion kicks in.

### Type / unit conversions

Another source of programming errors in the domain of dimensional analysis, is
missing unit conversions; i.e. providing a variable `speed` which holds a
metric value, to a function or method expecting an imperial value (or even just
another metric one).

This error was popularized by loss of the 'NASA Mars Climate Orbiter' in 1999,
a bug with an estimated cost of 125 million USD. [See more here](http://sunnyday.mit.edu/accidents/MCO_report.pdf)
The bug was due to missing conversion from imperial (*lbf* pound-force) to
metric units (*newton*), both units of force, but different systems.

Another popularization is the 'NASA SDI (Strategic Defense Initiative)' missile
defense system, which failed during an attempted testing procedure. The test
failed as the testing aircraft flew upside, after receiving input from mission
control directing the aircraft to point it's sensor to 10023 feet above
sea-level, unfortunately the onboard guidance system expected nautical miles
above sea level.

In the library such issues are handled by automatic type/unit conversions.

Inspect the following lines of code:
```c++
meter distance = 100_meter;

std::cout << distance.getVal() << " meters is;" << std::endl;
// 100 meters is;
std::cout << ((furlong) distance).getVal() << std::endl;
// 0.497512
std::cout << ((fathom) distance).getVal() << std::endl;
// 54.6807
std::cout << ((league) distance).getVal() << std::endl;
(( 0.0207124
std::cout << ((line) distance).getVal() << std::endl;
// 47244.1
```

When unit conversion cannot take place, because the units are incommensurable,
an error is thrown, inspect the following line of code, and error message:
```c++
second sec = 100_meter; // on line 765
```
Error:
```bash
main.cpp:388:5: error: static_assert failed;
    "Not same SI base unit"
    static_assert( u_o == u_i, "Not same SI base unit");
...
...
...
main.cpp:765:18: note: in instantiation of function template specialization
    '...' requested here
    second sec = 100_meter;
```

### Complex unit conversions

Conversion between simple units is easily handled; the strength and flexibility
of this library, comes from being able to automatically generate new unit types
on the fly.

That is, types does not have to be defined, before being used; assuming the
compiler has the necessary knowledge to deduced the output type. Inspect the 
following lines of code;
```c++
auto voltage = volt(10);
auto current = ampere(1.44);
auto power = voltage * current;
```
Here 'volt' and 'ampere' are defined types, multiplying these together yields 
the type 'watt', which multiplied by seconds yields 'joules';
```c++
std::cout << voltage.getVal() << " volts " << "*" <<
             current.getVal() << " ampere" << std::endl;
std::cout << " = " << power.getVal() << " watts " << std::endl;
std::cout << "Running it for 1 minute spends " << 
    ((joule) (power * 1_minute)).getVal() << " joules" << std::endl;
```
Output;
```
10 volts * 1.44 ampere
 = 14.4 watts 
Running it for 1 minute spends 864 joules
```

## Implementation

The implementation utilizes the [Turing Completeness of C++ template
meta-programming][CppTuringComplete]. The abstractions provided exists only on 
compile time, assuming an optimizing compiler, no tests has been performed.

The implementation does not handle offset unit conversion (say Celcius to
Fahrenheit), but can somewhat easily be extended to do so. Neither does it 
implement dynamic dimensional analysis, but this can be implemented as well,
by adding run-time conversion checks on casting operators to and from static
unit types.

There is currently no output mechanism in place, i.e. to pretty print units,
or to even avoid calling 'getVal()' on units, implementing this is trivial.

The implementation is flexible in regards to adding more base-units, say angles
or abstract units, but is currently only fitted with SI base units, derived SI
units are supported by complex unit conversions.

The current implementation requires some c++1z features, but the library can be
fully implemented in C++11, assuming one accepts the required cumbersome syntax

[CppTuringComplete]: http://netvor.sk/~umage/docs/3rdparty/C++%20Templates%20are%20Turing%20Complete%20(Todd%20L.%20Veldhuizen,%202003).pdf
