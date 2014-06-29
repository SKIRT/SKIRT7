/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTSYSTEM_HPP
#define DUSTSYSTEM_HPP

#include <vector>
#include <QMutex>
#include "Array.hpp"
#include "Position.hpp"
#include "SimulationItem.hpp"

class DustCell;
class DustDistribution;
class DustGridDensityInterface;
class DustGridStructure;
class DustMix;
class DustSystemPath;

//////////////////////////////////////////////////////////////////////

/** DustSystem is an abstract class for representing complete dust systems, including both a dust
    distribution (i.e. a complete description of the spatial distribution and optical properties of
    the dust) and a grid structure on which this distribution is discretized. There are specialized
    subclasses for use with oligochromatic and panchromatic simulations respectively. A DustSystem
    object contains a vector of dust cells, each of which contain all useful information on the
    dust within that particular piece of the configuration space. The type of dust cell is selected
    by the subclass through the factory method createDustCell(). Furthermore, a DustSystem object
    contains pointers a DustDistribution object and to a DustGridStructure object). A subclass
    may of course maintain additional information depending on its needs. */
class DustSystem : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust system")

    Q_CLASSINFO("Property", "dustDistribution")
    Q_CLASSINFO("Title", "the dust distribution")
    Q_CLASSINFO("Default", "CompDustDistribution")

    Q_CLASSINFO("Property", "dustGridStructure")
    Q_CLASSINFO("Title", "the dust grid structure")
    Q_CLASSINFO("Default", "OctTreeDustGridStructure")

    Q_CLASSINFO("Property", "sampleCount")
    Q_CLASSINFO("Title", "the number of random density samples for determining cell mass")
    Q_CLASSINFO("MinValue", "10")
    Q_CLASSINFO("MaxValue", "1000")
    Q_CLASSINFO("Default", "100")

    Q_CLASSINFO("Property", "writeConvergence")
    Q_CLASSINFO("Title", "output a data file with convergence checks on the dust system")
    Q_CLASSINFO("Default", "yes")

    Q_CLASSINFO("Property", "writeDensity")
    Q_CLASSINFO("Title", "output FITS files displaying the dust density distribution")
    Q_CLASSINFO("Default", "yes")

    Q_CLASSINFO("Property", "writeDepthMap")
    Q_CLASSINFO("Title", "output FITS file with a V-band optical depth map seen from the center")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "writeQuality")
    Q_CLASSINFO("Title", "calculate and output quality metrics for the dust grid")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "writeCellProperties")
    Q_CLASSINFO("Title", "output a data file with relevant properties for all dust cells")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "writeCellsCrossed")
    Q_CLASSINFO("Title", "output statistics on the number of cells crossed per path")
    Q_CLASSINFO("Default", "no")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    DustSystem();

public:
    /** The destructor deletes the dust cells that were created during the setup phase. */
    ~DustSystem();

protected:
    /** This function verifies that all attribute values have been appropriately set. */
    void setupSelfBefore();

    /** This function performs setup for the dust system, which takes several phases. The first
        phase consists of the creation of the dust cells. The type of dust cell (OligoDustCell or
        PanDustCell) is selected by the subclass through the factory method createDustCell(). The
        second phase consists of calculating and storing the volume and the dust density (for every
        dust component) of all the cells. To calculate the volume of a given dust cell, we just
        call the corresponding function of the dust grid structure. To calculate and set the
        density corresponding to given dust component in a given cell, a number of random positions
        are generated within the cell (see sampleCount()). The density in the cell is calculated as
        the mean of the density values (found using a call to the corresponding function of the
        dust distribution) in these points. The calculation of both volume and density is
        parallellized. In the third phase, if the corresponding write flag is turned on, the
        function invokes writeconvergence() to calculate and output a convergence check on the dust
        system. In the fourth and final phase, if the corresponding write flag is turned on, the
        function invokes writedensity() to calculate and output theoretical and grid-discretized
        density maps in the coordinate planes. */
    void setupSelfAfter();

    /** This pure virtual factory function must be overridden in each subclass to create and return
        a pointer to a new dust cell of the appropriate type, i.e. OligoDustCell or PanDustCell. */
    virtual DustCell* createDustCell() = 0;

private:
    /** This function serves as the parallelization body for calculating the volume of each cell. */
    void setVolumeBody(int m);

    /** This function serves as the parallelization body for setting the density value of each cell
        through the DustGridDensityInterface interface, if available. */
    void setGridDensityBody(int m);

    /** This function serves as the parallelization body for setting the density value of each cell
        by taking random density sample. */
    void setSampleDensityBody(int m);

    /** This function writes out a simple text file, named <tt>prefix_ds_convergence.dat</tt>,
        providing a convergence check on the dust system. The function calculates the total dust
        mass, the face-on surface density and the edge-on surface density by directly integrating
        over the dust grid, and compares these values with the expected "theoretical" values found
        by just calling the corresponding functions of the dust distribution. The results are
        written to the file and can be studied to see whether the chosen dust grid structure is
        adequate for the chosen dust distribution. */
    void writeconvergence() const;

    /** This function writes out FITS files with the theoretical dust density and the
        grid-discretized dust density in the coordinate planes. Each of these maps contains 1024 x
        1024 pixels, and covers as a field of view the total extension of the grid. The number of
        data files written depends on the dimension of the dust system's geometry: for spherical
        symmetry only the intersection with the xy plane is written, for axial symmetry the
        intersections with the xy and xz planes are written, and for general geometries all three
        intersections are written. The difference between the theoretical dust density maps (named
        <tt>prefix_ds_trhoXX.fits</tt>) and the grid-discretized dust density maps (named
        <tt>prefix_ds_grhoXX.fits</tt>) is the following: the theoretical dust density is the total
        dust density of the dust distribution, i.e.\ the actual dust density that would correspond
        to an infinitely fine dust grid. The grid-discretized dust density maps on the other hand
        give the value of the dust as read from the finite-resolution dust grid structure. A
        comparison of both sets of maps can reveal whether the chosen dust grid structure is
        suitable (in the ideal case, there would be no difference between both sets of maps). */
    void writedensity() const;

    /** This function writes out a FITS file named <tt>prefix_ds_tau.fits</tt> with an all-sky
        V-band optical depth map as seen from the coordinate origin. The map has 1600 x 800 pixels
        and uses the Mollweide projection to project the complete sky onto a proportional 2:1 ellipse.
        The values outside of the ellipse are set to zero. The direction \f$\bf{k}=(\theta,\phi)\f$
        corresponding to the pixel in the map with horizontal and vertical indices \f$(i,j)\f$ can
        be found through the inverse Mollweide projection, which in this case can be written as
        follows:
        \f[ x=(i+\frac{1}{2})/N_\mathrm{pixels,x} \f]
        \f[ y=(j+\frac{1}{2})/N_\mathrm{pixels,y} \f]
        \f[ \alpha=\arcsin(2y-1) \f]
        \f[ \theta=\arccos(\frac{2\alpha+\sin 2\alpha}{\pi}) \f]
        \f[ \phi=\frac{\pi(2x-1)}{\cos\alpha}) \f]
    */
    void writedepthmap() const;

    /** This function writes out a simple text file, named <tt>prefix_ds_quality.dat</tt>,
        providing some basic quality metrics for the dust grid. The first metric consists of the
        mean value and the standard deviation for the absolute difference \f$|\rho_g-\rho_t|\f$
        between the theoretical and grid density in a large number of randomly chosen points,
        uniformly distributed over the dust grid volume. The second metric consists of the mean
        value and the standard deviation for the difference \f$|\tau_g-\tau_t|\f$ between the
        theoretical and grid optical depth, calculated for a large number of line segments with
        random end points uniformly distributed over the dust grid volume. */
    void writequality() const;

    /** This function writes out a text data file, named <tt>prefix_ds_cellprops.dat</tt>, which
        contains a line for each cell in the dust grid. Each line contains four columns
        representing the following dust cell properties: volume, density, mass fraction and optical
        depth. The first two values are listed in the corresponding output units; the last two are
        dimensionless quantities. */
    void writecellproperties() const;

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the dust distribution. */
    Q_INVOKABLE void setDustDistribution(DustDistribution* value);

    /** Returns the dust distribution. */
    Q_INVOKABLE DustDistribution* dustDistribution() const;

    /** Sets the dust grid structure. */
    Q_INVOKABLE void setDustGridStructure(DustGridStructure* value);

    /** Returns the dust grid structure. See also grid(). */
    Q_INVOKABLE DustGridStructure* dustGridStructure() const;

    /** Sets the number of random positions on which the density is sampled for each cell in the
        dust grid. The default value is 100 samples per cell. */
    Q_INVOKABLE void setSampleCount(int value);

    /** Returns the number of random positions on which the density is sampled for each cell in the
        dust grid. */
    Q_INVOKABLE int sampleCount() const;

    /** Sets the flag that indicates whether or not to output a data file with convergence checks
        on the dust system. The default value is true. */
    Q_INVOKABLE void setWriteConvergence(bool value);

    /** Returns the flag that indicates whether or not to output a data file with convergence
        checks on the dust system. */
    Q_INVOKABLE bool writeConvergence() const;

    /** Sets the flag that indicates whether or not to output FITS files displaying the dust
        density distribution for further analysis. The default value is true. */
    Q_INVOKABLE void setWriteDensity(bool value);

    /** Returns the flag that indicates whether or not to output FITS files displaying the dust
        density distribution for further analysis. */
    Q_INVOKABLE bool writeDensity() const;

    /** Sets the flag that indicates whether or not to output a FITS file with an all-sky V-band
        optical depth map as seen from the center. The default value is false. */
    Q_INVOKABLE void setWriteDepthMap(bool value);

    /** Returns the flag that indicates whether or not to output a FITS file with an all-sky V-band
        optical depth map as seen from the center. */
    Q_INVOKABLE bool writeDepthMap() const;

    /** Sets the flag that indicates whether or not to calculate and output quality metrics for the
        dust grid. The default value is false. */
    Q_INVOKABLE void setWriteQuality(bool value);

    /** Returns the flag that indicates whether or not to calculate and output quality metrics for
        the dust grid. */
    Q_INVOKABLE bool writeQuality() const;

    /** Sets the flag that indicates whether or not to output a data file with relevant properties
        for all dust cells, including volume, density, mass fraction and optical depth. The default
        value is false. */
    Q_INVOKABLE void setWriteCellProperties(bool value);

    /** Returns the flag that indicates whether or not to output a data file with relevant
        properties for all dust cells. */
    Q_INVOKABLE bool writeCellProperties() const;

    /** Sets the flag that indicates whether or not to output a text file with statistics on the
        number of dust grid cells crossed per path calculated through the grid. The default value
        is false. */
    Q_INVOKABLE void setWriteCellsCrossed(bool value);

    /** Returns the flag that indicates whether or not to output a text file with statistics on the
        number of dust grid cells crossed per path calculated through the grid. */
    Q_INVOKABLE bool writeCellsCrossed() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the dust system, which depends on the (lack of)
        symmetry in the geometry of its distribution. A value of 1 means spherical symmetry, 2
        means axial symmetry and 3 means none of these symmetries. */
    int dimension() const;

    /** This function returns the number of dust cells. */
    int Ncells() const;

    /** This function returns the number of dust components. */
    int Ncomp() const;

    /** This function returns a pointer to the dust mixture corresponding to the \f$h\f$'th dust
        component. */
    DustMix* mix(int h) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\boldsymbol{r}}\f$. The function just passes the call to corresponding function of the
        dust grid structure. */
    int whichcell(Position bfr) const;

    /** This function returns a random location in the dust cell with cell number \f$m\f$. The
        function just passes the call to corresponding function of the dust grid structure. */
    Position randomPositionInCell(int m) const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. The function
        just passes the call to the \f$m\f$'th dust cell in the internal dust cells array. */
    double volume(int m) const;

    /** This function returns the dust density corresponding to dust component \f$h\f$ of the dust
        cell with cell number \f$m\f$. If \f$m=-1\f$, i.e. if the cell number corresponds to a
        non-existing cell outside the grid, the value zero is returned. Otherwise, the function
        just passes the call to the \f$m\f$'th dust cell in the internal dust cells array. */
    double density(int m, int h) const;

    /** This function returns the total dust density of the dust cell with cell number \f$m\f$. If
        \f$m=-1\f$, i.e. if the cell number corresponds to a non-existing cell outside the grid,
        the value zero is returned. Otherwise, the function just passes the call to the \f$m\f$'th
        dust cell in the internal dust cells array. */
    double density(int m) const;

    /** This function returns the absorbed luminosity \f$L_{\ell,m}\f$ at wavelength index
        \f$\ell\f$ in the dust cell with cell number \f$m\f$. The function just passes the call to
        the \f$m\f$'th dust cell in the internal dust cells array. */
    double Labs(int m, int ell) const;

    /** This function returns the absorbed stellar luminosity \f$L^*_{\ell,m}\f$ at wavelength index
        \f$\ell\f$ in the dust cell with cell number \f$m\f$. The function just passes the call to
        the \f$m\f$'th dust cell in the internal dust cells array. */
    double Labsstellar(int m, int ell) const;

    /** This function returns the absorbed dust luminosity \f$L^{\text{d}}_{\ell,m}\f$ at wavelength index
        \f$\ell\f$ in the dust cell with cell number \f$m\f$. The function just passes the call to
        the \f$m\f$'th dust cell in the internal dust cells array. */
    double Labsdust(int m, int ell) const;

    /** This function returns the total (bolometric) absorbed luminosity in the dust cell with cell
        number \f$m\f$. It is calculated by summing the absorbed luminosity at all the wavelength indices. */
    double Labstot(int m) const;

    /** This function returns the total (bolometric) absorbed stellar luminosity in the dust cell with cell
        number \f$m\f$. It is calculated by summing the absorbed stellar luminosity at all the wavelength
        indices.  */
    double Labsstellartot(int m) const;

    /** This function returns the total (bolometric) absorbed dust luminosity in the dust cell with cell
        number \f$m\f$. It is calculated by summing the absorbed dust luminosity at all the wavelength
        indices. */
    double Labsdusttot(int m) const;

    /** This function returns the total (bolometric) absorbed stellar luminosity in the entire dust system.
        It is calculated by summing the absorbed luminosity of all the cells. */
    double Labstot() const;

    /** This function returns the total (bolometric) absorbed dust luminosity in the entire dust system.
        It is calculated by summing the absorbed stellar luminosity of all the cells. */
    double Labsstellartot() const;

    /** This function returns the total (bolometric) absorbed luminosity in the entire dust system.
        It is calculated by summing the absorbed dust luminosity of all the cells. */
    double Labsdusttot() const;

    /** This function resets the absorbed dust luminosity to zero in all cells of the dust system.
        It just passes the call to all the cells. */
    void rebootLabsdust() const;

    /** The function simulates the absorption of a monochromatic luminosity package in the dust cell
        with cell number \f$m\f$, i.e. it adds a fraction \f$\Delta L\f$ to the already absorbed
        luminosity at wavelength index \f$\ell\f$. The function just passes the call to the
        \f$m\f$'th dust cell in the internal dust cells array. */
    void absorb(int m, int ell, double DeltaL, bool ynstellar);

    /** This function returns a vector with the mean radiation field \f$J_{\ell,m}\f$ at all
        wavelength indices in the dust cell with cell number \f$m\f$. It is calculated as \f[
        J_{\ell,m} = \frac{ L_{\ell,m}^{\text{abs}} }{ 4\pi\, V_m\, \sum_h
        \kappa_{\ell,h}^{\text{abs}}\, \rho_{m,h} } \f] with \f$L_{\ell,m}^{\text{abs}}\f$ the
        absorbed luminosity, \f$\kappa_{\ell,h}^{\text{abs}}\f$ the absorption coefficient
        corresponding to the \f$h\f$'th dust component, \f$\rho_{m,h}\f$ the dust density
        corresponding to the \f$h\f$'th dust component, and \f$V_m\f$ the volume of the cell. */
    Array meanintensityv(int m) const;

    /** This function returns the optical depth
        \f$\tau_{\ell,{\text{path}}}({\boldsymbol{r}},{\boldsymbol{k}})\f$ at wavelength index
        \f$\ell\f$ along a path through the dust system starting at the position
        \f${\boldsymbol{r}}\f$ into the direction \f${\boldsymbol{k}}\f$. The hard work is done
        through a call to the construction of a DustGridPath class object, which contains all the
        geometrical information on the path through the dust grid: it contains vectors which
        contain the cell numbers \f$m\f$ of the cells that are crossed by the path, the pathlength
        \f$(\Delta s)_m\f$ covered in that particular cell and a total path length counter
        \f$s_m\f$ that gives the total path length covered between the starting point
        \f${\boldsymbol{r}}\f$ and the boundary of the cell. With this information given, the
        calculation of the optical depth is rather straightforward: it is calculated as \f[
        \tau_{\ell,{\text{path}}}({\boldsymbol{r}},{\boldsymbol{k}}) = \sum_m (\Delta s)_m \sum_h
        \kappa_{\ell,h}^{\text{ext}}\, \rho_m, \f] where \f$\kappa_{\ell,h}^{\text{abs}}\f$ is the
        extinction coefficient corresponding to the \f$h\f$'th dust component at wavelength index
        \f$\ell\f$ and \f$\rho_{m,h}\f$ the dust density in the cell with cell number \f$m\f$
        corresponding to the \f$h\f$'th dust component.

        The function also also stores the details on the calculation of the optical depth in the
        DustSystemPath object given in the function call. Apart from the quantities \f$m\f$,
        \f$(\Delta s)_m\f$ and \f$s_m\f$ provided by the DustGridPath, also the optical depth
        covered within the \f$m\f$'th dust cell, \f[ (\Delta\tau_\ell)_m = (\Delta s)_m \sum_h
        \kappa_{\ell,h}^{\text{ext}}\, \rho_m, \f] and the total optical depth \f$\tau_{\ell,m}\f$
        covered between the starting point \f${\boldsymbol{r}}\f$ and the boundary of the cell, are
        stored in the DustSystemPath object. */
    double opticaldepth(int ell, Position bfr, Direction bfk, DustSystemPath* ppd);

    /** This function returns the optical depth
        \f$\tau_{\ell,{\text{path}}}({\boldsymbol{r}},{\boldsymbol{k}})\f$ at wavelength index
        \f$\ell\f$ along a path through the dust system starting at the position
        \f${\boldsymbol{r}}\f$ into the direction \f${\boldsymbol{k}}\f$ for a distance \f$d\f$.
        The calculation proceeds as described for the other function with the same name; the
        differences being that the path length is limited to the specified distance, and that this
        function does not fill a DustSystemPath object. */
    double opticaldepth(int ell, Position bfr, Direction bfk, double distance);

    /** If the writeCellsCrossed attribute is true, this function writes out a data file (named
        <tt>prefix_ds_crossed.dat</tt>) with statistics on the number of dust grid cells crossed
        per path calculated through the grid. The first column on each line specifies a particular
        number of cells crossed; the second column indicates the number of paths that crossed this
        precise number of cells. In effect this provides a histogram for the distribution of the
        path length (measured in the number of cells crossed).

        This virtual function can be overridden in a subclass to write out additional results of
        the simulation stored in the dust system. In that case, the overriding function must also
        call the implementation in this base class. */
    virtual void write() const;

    //======================== Data Members ========================

protected:
    // data members to be set before setup is invoked
    DustDistribution* _dd;
    DustGridStructure* _grid;
    DustGridDensityInterface* _gdi;
    int _Nrandom;
    bool _writeConvergence;
    bool _writeDensity;
    bool _writeDepthMap;
    bool _writeQuality;
    bool _writeCellProperties;
    bool _writeCellsCrossed;

    // data members initialized during setup
    std::vector<DustCell*> _dustcellv;
    int _Ncomp;
    int _Ncells;
    std::vector<qint64> _crossed;
    QMutex _crossedMutex;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTSYSTEM_HPP
