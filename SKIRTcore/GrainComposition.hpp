/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GRAINCOMPOSITION_HPP
#define GRAINCOMPOSITION_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"
#include "Table.hpp"

////////////////////////////////////////////////////////////////////

/** GrainComposition is an abstract class that represents the optical and calorimetric properties
    of a population of dust grains with a given chemical composition.

    The optical properties are provided for arbitrary grain sizes and at arbitrary wavelengths; in
    practice, they are defined on a two-dimensional grid of wavelengths \f$\lambda_k\f$ and grain
    sizes \f$a_i\f$. The optical properties include the absorption and scattering efficiencies
    \f$Q_{k,i}^{\text{abs}}\f$ and \f$Q_{k,i}^{\text{sca}}\f$ and the scattering phase function
    asymmetry parameter \f$g_{k,i}\f$.

    The calorimetric properties include the bulk mass density \f$\rho_\text{bulk}\f$ (a single
    value), and the specific enthalpy at arbitrary temperature. The specific enthalpy is the
    internal energy per unit mass, using an arbitrary zero point, obtained by integrating the
    specific heat capacity of the material over the temperature range. In practice, the specific
    enthalpy is defined on a grid of temperatures \f$T_t\f$ resulting in a set of values \f$h_t\f$.

    The GrainComposition class provides a public interface for retrieving the values of the optical
    and calorimetric properties, properly interpolated for arbitrary wavelength, grain size, and/or
    temperature values. These public functions use the underlying property values stored in private
    data members defined in this class. Subclasses are required to initialize these data members
    during setup, using the protected functions offered by this class. */
class GrainComposition : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grain composition")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    GrainComposition();

    /** This function verifies that the setupSelfBefore() function of the subclass has properly
        initialized the protected data members defined by this class. */
    void setupSelfAfter();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. The identifier is \em not allowed to contain white space. This
        function must be be implemented by each subclass. */
    virtual QString name() const = 0;

    /** This function returns the largest temperature for which this dust composition can provide
        meaningfull enthalpy data. */
    double uppertemperature() const;

    //======== Getters for Optical and Calorimetric Properties =======

public:
    /** This function returns the absorption efficiency \f$Q^\text{abs}(\lambda,a)\f$ of dust
        grains with size \f$a\f$ at wavelength \f$\lambda\f$. It uses log-log interpolation on the
        internally stored values. If either of the specified values lie outside of the internally
        defined grid, the value at the nearest border is used instead. */
    double Qabs(double lambda, double a) const;

    /** This function returns the scattering efficiency \f$Q^\text{sca}(\lambda,a)\f$ of dust
        grains with size \f$a\f$ at wavelength \f$\lambda\f$. It uses log-log interpolation on the
        internally stored values. If either of the specified values lie outside of the internally
        defined grid, the value at the nearest border is used instead. */
    double Qsca(double lambda, double a) const;

    /** This function returns the scattering asymmetry parameter \f$g(\lambda,a)\f$ of dust grains
        with size \f$a\f$ at wavelength \f$\lambda\f$. It uses log-linear interpolation on the
        internally stored values. If either of the specified values lie outside of the internally
        defined grid, the value at the nearest border is used instead. */
    double asymmpar(double lambda, double a) const;

    /** This function returns the specific enthalpy of dust grains at temperature \f$T\f$, i.e. the
        internal energy per unit mass, using an arbitrary zero point. It uses log-log
        interpolation on the internally stored values. If the specified temperature lies outside of
        the internally defined grid, the enthalpy value at the nearest border is used instead. */
    double specificenthalpy(double T) const;

    /** This function returns the bulk mass density \f$\rho_\text{bulk}\f$ of the dust grains. */
    double bulkdensity() const;

    /** This function returns true if this grain composition supports polarization (i.e. it
        provides meaningful Mueller matrix coefficients); false if not. */
    bool polarization() const;

    /** This function returns the four Mueller matrix coefficients \f$S_{11}(\lambda,a,\theta),
        S_{12}(\lambda,a,\theta), S_{33}(\lambda,a,\theta), S_{34}(\lambda,a,\theta)\f$ of dust
        grains with size \f$a\f$ at wavelength \f$\lambda\f$ and for a scattering angle
        \f$\theta\f$. It uses log-linear interpolation on the internally stored values. If either
        of the specified values lie outside of the internally defined grid, the value at the
        nearest border is used instead. */
    void Sxx(double lambda, double a, double theta, double& S11, double& S12, double& S33, double& S34) const;

    //========= Setup Functions for Use in Subclasses ========

protected:
    /** This function should be used by a subclass to read the complete grid with optical
        properties from a resource or input data file with the specified name. The file should have a simple
        text format as described here. Any initial lines that start with a # character are
        considered to be part of a header and are thus ignored. The first number on the first
        non-header line specifies the number of grain size grid points \f$N_a\f$; the first number
        on the second non-header line specifies the number of wavelength grid points
        \f$N_\lambda\f$. Subsequently there must be \f$N_a\f$ blocks containing \f$N_\lambda+1\f$
        lines each. The first number on the first line in each block provides the grain size value
        for this block (in micron); blocks must be in order of increasing grain size. Subsequent
        lines in the block provide information for the various wavelengths at this grain size. If
        the \em reverse flag is false, these lines are in order of increasing wavelength, otherwise
        they are in order of decreasing wavelength. All data lines in a block must have 4 to 7
        columns in the following order: \f$X_1\f$, \f$\lambda\f$, \f$X_2\f$, \f$Q^\text{abs}\f$,
        \f$Q^\text{sca}\f$, \f$X_3\f$, \f$g\f$, where the presence or absence of each of the dummy
        \f$X_i\f$ fields is indicated by the corresponding \em skip flag (true means the field
        is present, false means it is absent). The wavelength must be given in micron; the other
        three values are dimensionless. For all lines discussed above, any additional information
        at the end of the line is ignored. */
    void loadOpticalGrid(bool resource, QString name, bool reverse, bool skip1, bool skip2, bool skip3);

    /** This function should be used by a subclass to read the complete grid with optical
        properties from three resource data files with the specified name, respectively containing:
         - the wavelength grid \f$\lambda_k\f$, in increasing order;
         - the grain sizes \f$a_i\f$ plus the absorption and scattering efficiencies
           \f$Q_{k,i}^{\text{abs}}\f$ and \f$Q_{k,i}^{\text{sca}}\f$ on the two-dimensional grid;
         - the grain sizes \f$a_i\f$ plus the scattering phase function asymmetry parameter
           \f$g_{k,i}\f$ on the same grid; the grain sizes values must be identical in both files.

        The files should have a simple text format as described here. For all files, any initial
        lines that start with a # character are considered to be part of a header and are thus
        ignored. Also, for all lines discussed below, any additional information at the end of the
        line is ignored.

        In the wavelength grid file, the first number on the first non-header line specifies the
        number of wavelength grid points \f$N_\lambda\f$. The first number on each subsequent line
        specifies a wavelength grid point \f$\lambda_k\f$ in (micron).

        The efficiencies file contains three blocks, each optionally starting with a header
        consisting of lines that start with a # characters. In the first block, the first number on
        the first non-header line specifies the number of grain size grid points \f$N_a\f$. The
        second line has \f$N_a\f$ columns specifying the grain sizes \f$a_i\f$ (in micron), in
        increasing order. The second block contains \f$N_\lambda\f$ non-header lines. Each line
        specifies the \f$N_a\f$ absorption efficiencies \f$Q_{k,i}^{\text{abs}}\f$ corresponding to
        line (wavelength) \f$k\f$ and column (grain size) \f$i\f$. Similarly, the third block
        contains \f$N_\lambda\f$ non-header lines. Each line now specifies the \f$N_a\f$ scattering
        efficiencies \f$Q_{k,i}^{\text{sca}}\f$ corresponding to line (wavelength) \f$k\f$ and
        column (grain size) \f$i\f$.

        The scattering assymetry parameter file similarly contains two blocks. The first block
        describes the grain size grid and should be identical to the first block in the
        efficiencies file. The second block again contains \f$N_\lambda\f$ non-header lines. Each
        line now specifies the \f$N_a\f$ scattering assymetry parameters \f$g_{k,i}\f$
        corresponding to line (wavelength) \f$k\f$ and column (grain size) \f$i\f$. */
    void loadOpticalGrid(QString resourceLambda, QString resourceQ, QString resourceG);

    /** This function can be used by a subclass to read enthalpy values in function of temperature
        from a table specified in the resource or input data file with the specified name. The file should
        have a simple text format as described here. Any initial lines that start with a #
        character are considered to be part of a header and are thus ignored. The first number on
        the first non-header line specifies the number of temperature grid points \f$N_T\f$. The
        remaining lines tabulate the enthalpy for increasing temperature. Each line must have two
        columns specifying the temperature \f$T_t\f$ (in K) and the corresponding enthalpy
        \f$h_t\f$ (in erg/g). Any additional information at the end of a line is ignored. */
    void loadEnthalpyGrid(bool resource, QString name);

    /** This function can be used by a subclass to read a table, containing the logarithm of the
        heat capacity in function of the logarithm of temperature, from the resource data file with
        the specified name, and calculate an appropriate enthalpy grid through interpolation and
        integration of the values in the table.

        Since this function needs to convert the heat capacity (or enthalpy) per unit volume to the
        corresponding quantity per unit mass, the bulk mass density \f$\rho_\text{bulk}\f$ of the
        dust grains must have been set through setBulkDensity() \em before calling this function.

        The file should have a simple text format as described here. Any initial lines that start
        with a # character are considered to be part of a header and are thus ignored. The first
        two non-header lines are ignored as well. The third non-header line specifies the number of
        temperature grid points \f$N_T\f$. The remaining lines tabulate the heat capacity for
        increasing temperature. Each line must have two columns specifying the base 10 logarithm of
        the temperature \f$\log T_t\f$ (in K) and the base 10 logarithm of the corresponding heat
        capacity per unit volume \f$\log C_t\f$ (in erg/K/cm3). Any additional information at the
        end of a line is ignored.

        This function first interpolates the heat capacity data read from the resource file to a
        grid with an appropriate (typically much larger) number of grid points, using linear
        interpolation in log-log space. Then it performs the integration to calculate the enthalpy,
        using the following substitution: \f[ H(T) = \int_0^T C(T')\,{\text{d}}T' = \ln 10
        \int_{-\infty}^{\log T} 10^{\log C(\log T')} 10^{\log T'} \,{\text{d}}\log T' \f] There is
        no need to determine the correct zero point of the integral, since the specificenthalpy()
        function returns enhalpy values with an arbitrary zero point. Finally the function converts
        the enthalpy values to the appropriate units. */
    void loadLogHeatCapacityGrid(QString resourcename);

    /** This is the formal type of the call-back function used by calculateEnthalpyGrid(). The
        call-back function takes a temperature value as its argument and returns the corresponding
        specific enthalpy (energy per unit mass). */
    typedef double EnthalpyFunction(double T);

    /** This function builds an appropriate internal temperature grid \f$T_t\f$ and calculates the
        corresponding specific enthalpy values \f$h_t\f$ from the analytical function specified as
        an argument. It should be used by subclasses that offer an analytical representation of the
        enthalpy function. */
    void calculateEnthalpyGrid(EnthalpyFunction efun);

    /** This function sets the bulk mass density \f$\rho_\text{bulk}\f$ of the dust grains. */
    void setBulkDensity(double value);

    /** This function should be used by a subclass to read the complete grid with optical and
        polarization properties from a resource or input data file with the specified name. The
        file should have the text format as used by the STOKES code version 2.06. */
    void loadPolarizedOpticalGrid(bool resource, QString name);

    //================= Private Helper Functions ================

private:
    /** This private helper function determines the internal grid indices \f$k\f$ and \f$i\f$
        corresponding to given values of wavelength \f$\lambda\f$ and grain size \f$a\f$. If the
        specified wavelength or grain size value lies outside of the internal grid, it is set to
        the nearest border value. */
    void indices(double& lambda, double& a, int& k, int& i) const;

    //======================== Data Members ========================

private:
    // data members to be initialized by calling our service functions in setupSelfBefore() of subclass
    int _Nlambda;           // index k
    int _Na;                // index i
    Array _lambdav;         // indexed on k
    Array _av;              // indexed on i
    Table<2> _Qabsvv;       // indexed on k and i
    Table<2> _Qscavv;       // indexed on k and i
    Table<2> _asymmparvv;   // indexed on k and i
    int _NT;                // index t
    Array _Tv;              // indexed on t
    Array _hv;              // indexed on t
    double _rhobulk;

    // optional data members related to polarization properties; to be initialized in setupSelfBefore() of subclasses
    // that support polarization by calling the appropriate service function:
    // - if _Ntheta==0 (the default), there is no support for polarization and these tables remain empty
    // - if _Ntheta>0, these tables contain the Mueller matrix coefficients i.f.o. scattering angle theta
    int _Ntheta;            // index d
    Table<3> _S11vvv;       // indexed on k, i and d
    Table<3> _S12vvv;       // indexed on k, i and d
    Table<3> _S33vvv;       // indexed on k, i and d
    Table<3> _S34vvv;       // indexed on k, i and d
};

////////////////////////////////////////////////////////////////////

#endif // GRAINCOMPOSITION_HPP
