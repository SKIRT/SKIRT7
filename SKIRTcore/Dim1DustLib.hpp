/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DIM1DUSTLIB_HPP
#define DIM1DUSTLIB_HPP

#include "DustLib.hpp"

//////////////////////////////////////////////////////////////////////

/** The Dim1DustLib class calculates a relevant subset of the dust emission spectra for the
    simulation, and maps each dust cell to one of these library entries. This avoids performing the
    calculation explicitly for every dust cell in the simulation, in return for lack of accuracy.
    The library contains a one-dimensional set of dust emission spectra corresponding to different
    strengths of the interstellar radiation field, parameterized by the quantity \f[ U = \frac{
    \int_0^\infty J_\lambda\, {\text{d}}\lambda }{ \int_0^\infty J_\lambda^{\text{MW}}\,
    {\text{d}}\lambda }, \f] where \f$J_\lambda^{\text{MW}}\f$ is the interstellar radiation field
    in the Milky Way. The library is built dynamically from binning the \f$N_{\text{cells}}\f$
    values of \f$U\f$ as calculated from the dust system, onto a one-dimensional grid with
    \f$N_{\text{lib}}\f$ bins, and calculating the corresponding dust emissivity for each of these
    \f$N_{\text{lib}}\f$ library entries. */
class Dim1DustLib : public DustLib
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust library with a one-dimensional grid of emissivity entries")

    Q_CLASSINFO("Property", "entries")
    Q_CLASSINFO("Title", "the number of library entries")
    Q_CLASSINFO("MinValue", "10")
    Q_CLASSINFO("MaxValue", "10000000")
    Q_CLASSINFO("Default", "500")

    //============= Construction - Setup - Destruction =============

public:
    /** Default constructor. */
    Q_INVOKABLE Dim1DustLib();

protected:
    /** This function verifies the number of library entries. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of library entries. */
    Q_INVOKABLE void setEntries(int value);

    /** Returns the number of library entries. */
    Q_INVOKABLE int entries() const;

    //======================== Other Functions =======================

protected:
    /** This function returns a vector \em nv with length \f$N_{\text{cells}}\f$ that maps each
        cell \f$m\f$ to the corresponding library entry \f$n_m\f$. In this class the function loops
        over all dust cells and calculates the strength \f$U_m\f$ of the radiation field for each
        dust cell \f$m\f$. Based on these values, a logarithmic grid in \f$U\f$ embracing these
        values is established, \f[ U_{(n)} = U_{\text{min}} \left( \frac{ U_{\text{max}} }{
        U_{\text{min}} } \right)^{n/N_U} \qquad n=0,\ldots,N_U, \f] where \f$U_{\text{min}}\f$ and
        \f$U_{\text{max}}\f$ represent the smallest and largest values of the ISRF strength found
        among all dust cells. Then the function determines for each cell \f$m\f$ the corresponding
        library entry \f$n\f$. */
    std::vector<int> mapping() const;

    //======================== Data Members ========================

private:
    // discoverable properties
    int _NU;        // number of library entries
};

////////////////////////////////////////////////////////////////////

#endif // DIM1DUSTLIB_HPP
