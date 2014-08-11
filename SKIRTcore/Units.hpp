/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef UNITS_HPP
#define UNITS_HPP

#include <QHash>
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** Units is the abstract base class for providing a system of units for input/output purposes
    (internally in SKIRT, everything is in SI units). Units provides two types of services: static
    functions and instance functions. The static functions, fully implemented in this base class,
    allow converting to and from a list of predefined units (each identified by a unique text
    string), including the SI units and a bunch of alternates for the physical quantities used in
    SKIRT. There are also some static functions returning fundamental physical and astronomical
    constants. The instance functions offer conversion facilities for the physical quantities
    relevant to SKIRT, using a particular set of units as defined by the concrete subclass under
    consideration. In other words, the subclass selects a particular unit for each physical
    quantity. For each type of physical quantity, there is a function to return a text string
    identifying the corresponding unit, a function to convert a value from external units to internal
    SI units, and a function to convert a value from internal SI units units back to external units.

    Implementation notes: This base class offers all public functions and holds a dictionary
    with selected units per quantity to be initialized in the constructor of the subclass. This
    initialization must happen during construction (rather than in the setup() function) because the
    functions in the Units class are used while the simulation hierarchy is being built, i.e. well
    before the setup() function is invoked. Also, the current implementation assumes that a unit
    conversion can be defined by a single proportionality factor; supporting other schemes (such as
    temperatures in Celsius or fractional angles in minutes and seconds) may involve a rather
    substantial change. */
class Units : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a units system")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    Units();

    /** This function initializes privately cached copies of the conversion factors for the units
        selected by the concrete subclass of this instance. It MUST be called from the constructor
        of any subclass AFTER the protected member values have been initialized with the
        appropriate unit identification strings. */
    void initCache();

    //======================== Other Functions =======================

public:
    // - - - - - physical constants - - - - -

    /** This function returns the speed of light \f$c\f$. */
    static double c();

    /** This function returns Planck's constant \f$h\f$. */
    static double h();

    /** This function returns the Boltzmann constant \f$k\f$. */
    static double k();

    /** This function returns the Avogadro constant \f$N_\text{A}\f$, i.e. the number of atoms or
        molecules in one mole of a given substance. */
    static double NA();

    /** This function returns the distance of one astronomical unit. */
    static double AU();

    /** This function returns the distance of one parsec. */
    static double pc();

    /** This function returns the mass of the proton. */
    static double massproton();

    /** This function returns the solar mass. */
    static double Msun();

    /** This function returns the bolometric luminosity of the Sun. */
    static double Lsun();

    /** This function returns the wavelength of the center of the optical V band. */
    static double lambdaV();

    /** This function returns the total (i.e. absorption and scattering) dust opacity
        \f$\kappa_{\text{V}}\f$ in the V-band for a Draine & Li (2007) dust mix with
        characteristics corresponding to the InterstellarDistMix class. */
    static double kappaV();

    // - - - - - global predefined units - - - - -

    /** This function converts a physical value from the specified units to internal SI units. The
        name of the physical quantity (such as length) must be specified in all lowercase and
        without any spaces, just like in the u/i/o function names of this class. The string
        specifying the units is case sensitive, and also should not contain any spaces. If the
        value argument is omitted, the result is the proportionality factor for the conversion. If
        the specified combination of physical quantity and unit is unknown, the function throws an
        exception. */
    static double in(QString qty, QString unit, double value=1.);

    /** This function converts a physical value from internal SI units to the specified units. The
        name of the physical quantity (such as length) must be specified in all lowercase and
        without any spaces, just like in the u/i/o function names of this class. The string
        specifying the units is case sensitive, and also should not contain any spaces. If the
        value argument is omitted, the result is the proportionality factor for the conversion. If
        the specified combination of physical quantity and unit is unknown, the function throws an
        exception. */
    static double out(QString qty, QString unit, double value=1.);

    /** This function returns a string list containing the names of all known units for the
        specified physical quantity, sorted alphabetically. The name of the physical quantity must
        be specified in all lowercase and without any spaces, just like in the u/i/o function names
        of this class. If the specified physical quantity is unknown, the returned list is empty. */
    static QStringList units(QString qty);

    // - - - - - specific unit conversions as selected by subclass - - - - -

    /** This function returns true if the argument string specifies a physical quantity known to
        this units system, and false otherwise. The name of the physical quantity must be specified
        in all lowercase and without any spaces, just like in the u/i/o function names of this class. */
    bool isQuantity(QString qty) const;

    /** This function returns a string containing the name of the unit selected by the subclass for
        the specified physical quantity. The name of the physical quantity must be specified in all
        lowercase and without any spaces, just like in the u/i/o function names of this class.
        The function throws a fatal error if the specified physical quantity is unknown. */
    QString unit(QString qty) const;

    /** This function converts a physical value from the units selected by the subclass to internal
        SI units. The name of the physical quantity (such as length) must be specified in all
        lowercase and without any spaces, just like in the u/i/o function names of this class. If the
        value argument is omitted, the result is the proportionality factor for the conversion. If
        the specified combination of physical quantity and unit is unknown, the function throws an
        exception. */
    double in(QString qty, double value=1.) const;

    /** This function converts a physical value from internal SI units to the units selected by the
        subclass. The name of the physical quantity (such as length) must be specified in all
        lowercase and without any spaces, just like in the u/i/o function names of this class. If the
        value argument is omitted, the result is the proportionality factor for the conversion. If
        the specified combination of physical quantity and unit is unknown, the function throws an
        exception. */
    double out(QString qty, double value=1.) const;

    /** This function returns a string containing the name of the unit of length adopted by the
        program for input/output (internally, only SI units are used). Apart from this unit of
        length, the program uses an independent unit of distance, because usually the distance to
        an astronomical object and the typical length scale within an astronomical object are quite
        different. */
    QString ulength() const;

    /** This function converts the length from the program's input/output units to the internally
        used SI units (m). */
    double ilength(double x) const;

    /** This function converts the length \f$x\f$ from the internally used SI units (m) to the
        program's input/output units. */
    double olength(double x) const;

    /** This function returns a string containing the name of the unit of distance adopted by the
        program for input/output (internally, only SI units are used). Apart from this unit of
        distance, the program uses an independent unit of length, because usually the distance to
        an astronomical object and the typical length scale within an astronomical object are quite
        different. */
    QString udistance() const;

    /** This function converts the distance \f$d\f$ from the program's input/output units to the
        internally used SI units (m). */
    double idistance(double d) const;

    /** This function converts the distance \f$d\f$ from the internally used SI units (m) to the
        program's input/output units. */
    double odistance(double d) const;

    /** This function returns a string containing the name of the unit of wavelength adopted by the
        program for input/output (internally, only SI units are used). */
    QString uwavelength() const;

    /** This function converts the wavelength \f$\lambda\f$ from the program's input/output units
        to the internally used SI units (m). */
    double iwavelength(double lambda) const;

    /** This function converts the wavelength \f$\lambda\f$ from the internally used SI units (m)
        to the program's input/output units. */
    double owavelength(double lambda) const;

    /** This function returns a string containing the name of the unit of dust grain size adopted
        by the program for input/output (internally, only SI units are used). */
    QString ugrainsize() const;

    /** This function converts the dust grain size \f$a\f$ from the program's input/output units to
        the internally used SI units (m). */
    double igrainsize(double a) const;

    /** This function converts the dust grain size \f$a\f$ from the internally used SI units (m) to
        the program's input/output units. */
    double ograinsize(double a) const;

    /** This function returns a string containing the name of the unit of cross section adopted by the
        program for input/output (internally, only SI units are used). */
    QString usection() const;

    /** This function converts the cross section \f$C\f$ from the program's input/output units to the
        internally used SI units (\f${\text{m}}^2\f$). */
    double isection(double C) const;

    /** This function converts the volume \f$C\f$ from the internally used SI units
        (\f${\text{m}}^2\f$) to the program's input/output units. */
    double osection(double C) const;

    /** This function returns a string containing the name of the unit of volume adopted by the
        program for input/output (internally, only SI units are used). */
    QString uvolume() const;

    /** This function converts the volume \f$V\f$ from the program's input/output units to the
        internally used SI units (\f${\text{m}}^3\f$). */
    double ivolume(double V) const;

    /** This function converts the volume \f$V\f$ from the internally used SI units
        (\f${\text{m}}^3\f$) to the program's input/output units. */
    double ovolume(double V) const;

    /** This function returns a string containing the name of the unit of velocity adopted by the
        program for input/output (internally, only SI units are used). */
    QString uvelocity() const;

    /** This function converts the velocity \f$v\f$ from the program's input/output units to the
        internally used SI units (\f${\text{m}}\, {\text{s}}^{-1}\f$). */
    double ivelocity(double v) const;

    /** This function converts the velocity \f$v\f$ from the internally used SI units
        (\f${\text{m}}\, {\text{s}}^{-1}\f$) to the program's input/output units. */
    double ovelocity(double v) const;

    /** This function returns a string containing the name of the unit of mass adopted by the
        program for input/output (internally, only SI units are used). */
    QString umass() const;

    /** This function converts the mass \f$M\f$ from the program's input/output units to the
        internally used SI units (kg). */
    double imass(double M) const;

    /** This function converts the mass \f$M\f$ from the internally used SI units (kg) to the
        program's input/output units. */
    double omass(double M) const;

    /** This function returns a string containing the name of the unit of bulk mass adopted by the
        program for input/output (internally, only SI units are used). */
    QString ubulkmass() const;

    /** This function converts the bulk mass \f$\mu\f$ from the program's input/output units to the
        internally used SI units (kg). */
    double ibulkmass(double mu) const;

    /** This function converts the bulk mass \f$\mu\f$ from the internally used SI units (kg) to
        the program's input/output units. */
    double obulkmass(double mu) const;

    /** This function returns a string containing the name of the unit of surface density adopted
        by the program for input/output (internally, only SI units are used). */
    QString umasssurfacedensity() const;

    /** This function converts the surface density \f$\Sigma\f$ from the program's input/output
        units to the internally used SI units (\f${\text{kg\,m}}^{-2}\f$). */
    double imasssurfacedensity(double Sigma) const;

    /** This function converts the surface density \f$\Sigma\f$ from the internally used SI units
        (\f${\text{kg}}\, {\text{m}}^{-2}\f$) to the program's input/output units. */
    double omasssurfacedensity(double Sigma) const;

    /** This function returns a string containing the name of the unit of mass density adopted by
        the program for input/output (internally, only SI units are used). */
    QString umassvolumedensity() const;

    /** This function converts the mass density \f$\rho\f$ from the program's input/output units to
        the internally used SI units (\f${\text{kg\,m}}^{-3}\f$). */
    double imassvolumedensity(double rho) const;

    /** This function converts the mass density \f$\rho\f$ from the internally used SI units
        (\f${\text{kg}}\, {\text{m}}^{-3}\f$) to the program's input/output units. */
    double omassvolumedensity(double rho) const;

    /** This function returns a string containing the name of the unit of opacity adopted by the
        program for input/output (internally, only SI units are used). */
    QString uopacity() const;

    /** This function converts the opacity \f$\kappa\f$ from the program's input/output units to
        the internally used SI units (\f${\text{m}}^2\, {\text{kg}}^{-1}\f$). */
    double iopacity(double kappa) const;

    /** This function converts the opacity \f$\kappa\f$ from the internally used SI units
        (\f${\text{m}}^{-2}\, {\text{kg}}^{-1}\f$) to the program's input/output units. */
    double oopacity(double kappa) const;

    /** This function returns a string containing the name of the unit of energy adopted by the
        program for input/output (internally, only SI units are used). */
    QString uenergy() const;

    /** This function converts the energy \f$E\f$ from the program's input/output units to the
        internally used SI units (J). */
    double ienergy(double E) const;

    /** This function converts the energy \f$E\f$ from the internally used SI units (J) to the
        program's input/output units. */
    double oenergy(double E) const;

    /** This function returns a string containing the name of the unit of bolometric luminosity
        adopted by the program for input/output (internally, only SI units are used). */
    QString ubolluminosity() const;

    /** This function converts the bolometric luminosity \f$L\f$ from the program's input/output
        units to the internally used SI units (W). */
    double ibolluminosity(double L) const;

    /** This function converts the bolometric luminosity \f$L\f$ from the internally used SI units
        (W) to the program's input/output units. */
    double obolluminosity(double L) const;

    /** This function returns a string containing the name of the unit of monochromatic luminosity
        adopted by the program for input/output (internally, only SI units are used). */
    QString umonluminosity() const;

    /** This function converts the monochromatic luminosity \f$L_\lambda\f$ from the program's
        input/output units to the internally used SI units (\f${\text{W}}\, {\text{m}}^{-1}\f$). */
    double imonluminosity(double Llambda) const;

    /** This function converts the monochromatic luminosity \f$L_\lambda\f$ from the internally
        used SI units (\f${\text{W}}\, {\text{m}}^{-1}\f$) to the program's input/output units. */
    double omonluminosity(double Llambda) const;

    /** This function returns a string containing the name of the unit of flux density \f$F_\nu\f$
        adopted by the program for input/output (internally, only SI units are used). */
    QString ufluxdensity() const;

    /** This function converts the flux density \f$F_\nu\f$ from the program's input/output units
        to the internally used SI units (\f${\text{W}}\, {\text{m}}^{-2}\, {\text{Hz}}^{-1}\f$). */
    double ifluxdensity(double Fnu) const;

    /** This function converts the flux density \f$F_nu\f$ from the internally used SI units
        (\f${\text{W}}\, {\text{m}}^{-2}\, {\text{Hz}}^{-1}\f$) to the program's input/output units. */
    double ofluxdensity(double Fnu) const;

    /** This function returns a string containing the name of the unit of surface brightness adopted
        by the program for input/output (internally, only SI units are used). */
    QString usurfacebrightness() const;

    /** This function converts the surface brightness \f$f_\nu\f$ from the program's input/output units
        to the internally used SI units (\f${\text{W}}\, {\text{m}}^{-2}\, {\text{Hz}}^{-1}\,
        {\text{sr}}^{-1}\f$). */
    double isurfacebrightness(double fnu) const;

    /** This function converts the surface brightness \f$f_\nu\f$ from the internally used SI units
        (\f${\text{W}}\, {\text{m}}^{-2}\, {\text{Hz}}^{-1}\, {\text{sr}}^{-1}\f$) to the program's
        input/output units. */
    double osurfacebrightness(double fnu) const;

    /** This function returns a string containing the name of the unit of temperature adopted by
        the program for input/output (internally, only SI units are used). */
    QString utemperature() const;

    /** This function converts the temperature \f$T\f$ from the program's input/output units to the
        internally used SI units (K). */
    double itemperature(double T) const;

    /** This function converts the temperature \f$T\f$ from the internally used SI units (K) to the
        program's input/output units. */
    double otemperature(double T) const;

    /** This function returns a string containing the name of the unit of angular size adopted by
        the program for input/output (internally, only SI units are used). */
    QString uangle() const;

    /** This function converts the angular size \f$\theta\f$ from the program's input/output units
        to the internally used SI units (rad). */
    double iangle(double theta) const;

    /** This function converts the angular size \f$\theta\f$ from the internally used SI units
        (rad) to the program's input/output units. */
    double oangle(double theta) const;

    /** This function returns a string containing the name of the unit of positioning angle adopted
        by the program for input/output (internally, only SI units are used). */
    QString uposangle() const;

    /** This function converts the positioning angle \f$\theta\f$ from the program's input/output
        units to the internally used SI units (rad). */
    double iposangle(double theta) const;

    /** This function converts the positioning angle \f$\theta\f$ from the internally used SI units
        (rad) to the program's input/output units. */
    double oposangle(double theta) const;

    /** This function returns a string containing the name of the unit of solid angle adopted by
        the program for input/output (internally, only SI units are used). */
    QString usolidangle() const;

    /** This function converts the solid angle \f$\Omega\f$ from the program's input/output units
        to the internally used SI units (sr). */
    double isolidangle(double Omega) const;

    /** This function converts the solid angle \f$\Omega\f$ from the internally used SI units (sr)
        to the program's input/output units. */
    double osolidangle(double Omega) const;

    /** This function returns a string containing the name of the unit of pressure adopted by
        the program for input/output (internally, only SI units are used). */
    QString upressure() const;

    /** This function converts the pressure \f$p\f$ from the program's input/output units
        to the internally used SI units (Pa). */
    double ipressure(double p) const;

    /** This function converts the pressure \f$p\f$ from the internally used SI units (Pa)
        to the program's input/output units. */
    double opressure(double p) const;

    //======================== Data Members ========================

protected:
    // dictionary to be initialized explicitly in the constructor of a derived class
    // key: physical quantity   value: unit selected by subclass for this qty
    QHash<QString,QString> _unitForQty;

private:
    // values to be initialized by calling initCache() in the constructor of a derived class
    QString _ulength;
    QString _udistance;
    QString _uwavelength;
    QString _ugrainsize;
    QString _usection;
    QString _uvolume;
    QString _uvelocity;
    QString _umass;
    QString _ubulkmass;
    QString _umasssurfacedensity;
    QString _umassvolumedensity;
    QString _uopacity;
    QString _uenergy;
    QString _ubolluminosity;
    QString _umonluminosity;
    QString _ufluxdensity;
    QString _usurfacebrightness;
    QString _utemperature;
    QString _uangle;
    QString _uposangle;
    QString _usolidangle;
    QString _upressure;

    double _clength;
    double _cdistance;
    double _cwavelength;
    double _cgrainsize;
    double _csection;
    double _cvolume;
    double _cvelocity;
    double _cmass;
    double _cbulkmass;
    double _cmasssurfacedensity;
    double _cmassvolumedensity;
    double _copacity;
    double _cenergy;
    double _cbolluminosity;
    double _cmonluminosity;
    double _cfluxdensity;
    double _csurfacebrightness;
    double _ctemperature;
    double _cangle;
    double _cposangle;
    double _csolidangle;
    double _cpressure;
};

//////////////////////////////////////////////////////////////////////

#endif // UNITS_HPP
