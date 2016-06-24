/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef WAVELENGTHGRID_HPP
#define WAVELENGTHGRID_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

class ProcessAssigner;

//////////////////////////////////////////////////////////////////////

/** WavelengthGrid is an abstract class that represents grids of wavelengths on which Monte Carlo
    simulations are defined. Each WavelengthGrid class object basically consists of a set of
    wavelength grid points \f$\lambda_\ell\f$ and the corresponding wavelength bin widths
    \f$\Delta_\ell\f$. A particular Monte Carlo simulation uses a single instance of this class. */
class WavelengthGrid : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a wavelength grid")

    Q_CLASSINFO("Property", "assigner")
    Q_CLASSINFO("Title", "the assignment scheme that assigns the wavelengths to the different parallel processes")
    Q_CLASSINFO("Default", "IdenticalAssigner")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Silent", "true")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    WavelengthGrid();

    /** This function must be implemented in a subclass to initialize the vector with wavelengths.
        Wavelengths must be sorted in ascending order and there must be at least one wavelength. In
        the abstract WavelengthGrid class, this function does nothing. */
    void setupSelfBefore();

    /** This function must be implemented in a subclass to initialize the vector with wavelength
        bin widths based on the vector with wavelengths, which has been initialized in
        setupSelfBefore(). In the abstract WavelengthGrid class, this function verifies that there
        is at least one wavelength, and caches the number of wavelengths in a data member. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

    /** This function sets the wavelength assigner for the Monte Carlo simulation. The process assigner is
        the object that assigns different wavelengths to different processes, to parallelize the photon
        shooting algorithm, and the data storage. The ProcessAssigner class is the abstract class that
        represents different types of assigners; different subclass implement the assignment in different ways.
        The default assigner that is used for the Monte Carlo simulation is an IdenticalAssigner, which assigns
        each process to all of the wavelengths to obtain the best load balancing. Another option would
        be to use a StaggeredAssigner, which would hand out the wavelengths to the different processes
        in a staggered way, enabling data parallelization for reduced memory usage.
        Using a SequentialAssigner for this purpose would
        not be recommended due to very poor load balancing. */
    Q_INVOKABLE void setAssigner(ProcessAssigner* value);

public:
    /** Returns the wavelength assigner for this Monte Carlo simulation. */
    Q_INVOKABLE ProcessAssigner* assigner() const;

    //======================== Other Functions =======================

public:
    /** This function returns true if the wavelength grid represents a sampled wavelength range (as
        required for panchromatic simulations), and false if it contains individual distinct
        wavelengths (as used by oligochromatic simulations). Subclasses must implement this
        function appropriately. */
    virtual bool issampledrange() const = 0;

    /** This function returns the number of wavelength grid points in the grid. */
    int Nlambda() const;

    /** This function returns the wavelength \f$\lambda_\ell\f$ corresponding to the index
        \f$\ell\f$. */
    double lambda(int ell) const;

    /** This function returns the width of the \f$\ell\f$'th wavelength bin. */
    double dlambda(int ell) const;

    /** This function returns the minimum border of the wavelength bin corresponding to the index
        \f$\ell\f$. */
    double lambdamin(int ell) const;

    /** This function returns the maximum border of the wavelength bin corresponding to the index
        \f$\ell\f$. */
    double lambdamax(int ell) const;

    /** This function returns the index \f$\ell\f$ of the grid point to which the wavelength
        \f$\lambda\f$ naturally belongs. It is the natural number \f$\ell\f$ that corresponds to
        the grid point \f$\lambda_\ell\f$ that lies nearest to \f$\lambda\f$ in logarithmic space.
        If \f$\lambda\f$ lies outside the wavelength interval, the value \f$\ell=-1\f$ is returned.
        */
    int nearest(double lambda) const;

    /** This function returns the entire table with the wavelength grid points. */
    const Array& lambdav() const;

    /** This function returns the entire table with the wavelength bin widths. */
    const Array& dlambdav() const;

    //======================== Data Members ========================

protected:
    // subclasses should fill this table in their setupSelfBefore() function
    Array _lambdav;

    // subclasses should fill this table in their setupSelfAfter() function
    Array _dlambdav;

    // this data member is initialized in the setupSelfAfter() function of this class
    // and thus can already be used in the setupSelfAfter() function of subclasses
    int _Nlambda;

    // determines which wavelengths are assigned to this process
    ProcessAssigner* _assigner;
};

//////////////////////////////////////////////////////////////////////

#endif // WAVELENGTHGRID_HPP
