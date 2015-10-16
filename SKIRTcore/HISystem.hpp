/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef HISYSTEM_HPP
#define HISYSTEM_HPP

#include <mutex>
#include <vector>
#include "Array.hpp"
#include "Position.hpp"
#include "SimulationItem.hpp"

class DustGrid;
class HIDistribution;
class LyaWavelengthGrid;
class PhotonPackage;
class Units;

//////////////////////////////////////////////////////////////////////

/** HISystem is the class for representing complete neutral hydrogen systems. */
class HISystem : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a neutral hydrogen system")

    Q_CLASSINFO("Property", "hIDistribution")
    Q_CLASSINFO("Title", "the neutral hydrogen distribution")

    Q_CLASSINFO("Property", "grid")
    Q_CLASSINFO("Title", "the grid")
    Q_CLASSINFO("Default", "OctTreeDustGrid")

    Q_CLASSINFO("Property", "sampleCount")
    Q_CLASSINFO("Title", "the number of random density samples for determining cell mass")
    Q_CLASSINFO("MinValue", "10")
    Q_CLASSINFO("MaxValue", "1000")
    Q_CLASSINFO("Default", "100")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE HISystem();

private:
    /** This function verifies that all attribute values have been appropriately set. */
    void setupSelfBefore();

    /** This function performs setup for the neutral hydrogen system, which takes two phases. The first
        phase consists of the creation of the vectors that hold the properties of the
        neutral hydrogen gas cells. The second phase consists of calculating and storing the volume and
        the neutral hydrogen gas properties (density, gas temperature and bulk velocity) of all the cells. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the neutral hydrogen distribution. */
    Q_INVOKABLE void setHIDistribution(HIDistribution* value);

    /** Returns the neutral hydrogen distribution. */
    Q_INVOKABLE HIDistribution* hIDistribution() const;

    /** Sets the grid. */
    Q_INVOKABLE void setGrid(DustGrid* value);

    /** Returns the grid. */
    Q_INVOKABLE DustGrid* grid() const;

    /** Sets the number of random positions on which the density is sampled for each cell in the
        grid. The default value is 100 samples per cell. */
    Q_INVOKABLE void setSampleCount(int value);

    /** Returns the number of random positions on which the density is sampled for each cell in the
        grid. */
    Q_INVOKABLE int sampleCount() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the neutral hydrogen system, which depends on the (lack of)
        symmetry in the geometry of its distribution. A value of 1 means spherical symmetry, 2
        means axial symmetry and 3 means none of these symmetries. */
    int dimension() const;

    /** This function returns the number of cells. */
    int Ncells() const;

    /** This function returns the number of the cell that contains the position
        \f${\boldsymbol{r}}\f$. The function just passes the call to corresponding function of the
        grid. */
    int whichcell(Position bfr) const;

    /** This function returns a random location in the cell with cell number \f$m\f$. The
        function just passes the call to corresponding function of the grid. */
    Position randomPositionInCell(int m) const;

    /** This function returns the volume of the cell with cell number \f$m\f$. */
    double volume(int m) const;

    /** This function returns the mass density of the cell with cell number \f$m\f$.
        If \f$m=-1\f$, i.e. if the cell number corresponds to a
        non-existing cell outside the grid, the value zero is returned. */
    double density(int m) const;

    /** This function returns the gas temperature of the cell with cell number \f$m\f$.
        If \f$m=-1\f$, i.e. if the cell number corresponds to a
        non-existing cell outside the grid, the value zero is returned. */
    double gastemperature(int m) const;

    /** This function returns the bulk velocity of the cell with cell number \f$m\f$.
        If \f$m=-1\f$, i.e. if the cell number corresponds to a
        non-existing cell outside the grid, the value zero is returned. */
    Vec bulkvelocity(int m) const;

    /** This function calculates the optical depth
        \f$\tau_{\ell,{\text{path}}}({\boldsymbol{r}},{\boldsymbol{k}})\f$ at wavelength index
        \f$\ell\f$ along a path through the neutral hydrogen system starting at the position
        \f${\boldsymbol{r}}\f$ into the direction \f${\boldsymbol{k}}\f$, where \f$\ell\f$,
        \f${\boldsymbol{r}}\f$ and \f${\boldsymbol{k}}\f$ are obtained from the specified
        PhotonPackage object, and it stores the resulting details back into the photon package
        object. The hard work is done by calling the DustGrid::path() function which
        stores the geometrical information on the path through the grid into the photon
        package: the cell numbers \f$m\f$ of the cells that are crossed by the path, the pathlength
        \f$(\Delta s)_m\f$ covered in that particular cell and a total path length counter
        \f$s_m\f$ that gives the total path length covered between the starting point
        \f${\boldsymbol{r}}\f$ and the boundary of the cell. With this information given, the
        calculation of the optical depth is rather straightforward: it is calculated as \f[
        \tau_{\ell,{\text{path}}}({\boldsymbol{r}},{\boldsymbol{k}}) = \sum_m (\Delta s)_m \sum_h
        \kappa_{\ell,h}^{\text{ext}}\, \rho_m, \f] where \f$\kappa_{\ell,h}^{\text{abs}}\f$ is the
        extinction coefficient corresponding to the \f$h\f$'th dust component at wavelength index
        \f$\ell\f$ and \f$\rho_{m,h}\f$ the dust density in the cell with cell number \f$m\f$
        corresponding to the \f$h\f$'th dust component. The function also stores the details on the
        calculation of the optical depth in the photon package, specifically it stores the optical
        depth covered within the \f$m\f$'th dust cell, \f[ (\Delta\tau_\ell)_m = (\Delta s)_m
        \sum_h \kappa_{\ell,h}^{\text{ext}}\, \rho_m, \f] and the total optical depth
        \f$\tau_{\ell,m}\f$ covered between the starting point \f${\boldsymbol{r}}\f$ and the
        boundary of the cell. */
    void fillOpticalDepth(PhotonPackage* pp);

    /** This function returns the optical depth
        \f$\tau_{\ell,{\text{d}}}({\boldsymbol{r}},{\boldsymbol{k}})\f$ at wavelength index
        \f$\ell\f$ along a path through the dust system starting at the position
        \f${\boldsymbol{r}}\f$ into the direction \f${\boldsymbol{k}}\f$ for a distance \f$d\f$,
        where \f$\ell\f$, \f${\boldsymbol{r}}\f$ and \f${\boldsymbol{k}}\f$ are obtained from the
        specified PhotonPackage object. The function first determines the photon package's path
        through the dust grid, storing the geometric information about the path segments through
        each cell into the photon package, and then calculates the optical depth at the specified
        distance. The calculation proceeds as described for the fillOpticalDepth() function; the
        differences being that the path length is limited to the specified distance, and that this
        function does not store the optical depth information back into the PhotonPackage object.
        */
    double opticaldepth(PhotonPackage* pp, double distance);

    //======================== Data Members ========================

protected:
    // data members to be set before setup is invoked
    HIDistribution* _hid;
    DustGrid* _grid;
    int _Nrandom;

    // data members initialized during setup
    int _Ncells;
    Array _volumev;             // volume for each cell
    Array _rhov;                // density for each cell
    Array _Tv;                  // gas temperature for each cell
    std::vector<Vec> _vbulkv;   // bulk velocity for each cell
    LyaWavelengthGrid* _lambdagrid;
    Units* _units;
};

//////////////////////////////////////////////////////////////////////

#endif // HISYSTEM_HPP
