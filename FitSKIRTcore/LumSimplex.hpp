/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LUMSIMPLEX_HPP
#define LUMSIMPLEX_HPP

#include "Image.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The LumSimplex class contains all the information used to optimize the disk luminosity and bulge-to-disk ratio.
    The optimize function requires the reference image, the two separate simulations, containers for the best fitting
    values and \f$\chi^2\f$ value. The optimization is done by the simplex algorithm described in
    http://www.scholarpedia.org/article/Nelder-Mead_algorithm. */

class LumSimplex : public SimulationItem
{
    Q_OBJECT

    Q_CLASSINFO("Title", "Simplex optimization boundaries")

    Q_CLASSINFO("Property", "minDlum")
    Q_CLASSINFO("Title", "the minimum Disk luminosity in solar units")
    Q_CLASSINFO("Default", "1e8")

    Q_CLASSINFO("Property", "maxDlum")
    Q_CLASSINFO("Title", "the maximum Disk luminosity in solar units")
    Q_CLASSINFO("Default", "1e10")

    Q_CLASSINFO("Property", "minblum")
    Q_CLASSINFO("Title", "the minimum Bulge-to-Disk ratio")
    Q_CLASSINFO("Default", "0.1")

    Q_CLASSINFO("Property", "maxblum")
    Q_CLASSINFO("Title", "the maximum Bulge-to-Disk ratio")
    Q_CLASSINFO("Default", "1.5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LumSimplex();

protected:
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the minimal Disk luminosity. */
    Q_INVOKABLE void setMinDlum(double value);

    /** Returns the minimal Disk luminosity.*/
    Q_INVOKABLE double minDlum() const;

    /** Sets the maximal Disk luminosity. */
    Q_INVOKABLE void setMaxDlum(double value);

    /** Returns the maximal Disk luminosity.*/
    Q_INVOKABLE double maxDlum() const;

    /** Sets the minimal Bulge-to-Disk ratio. */
    Q_INVOKABLE void setMinblum(double value);

    /** Returns the minimal Bulge-to-Disk ratio.*/
    Q_INVOKABLE double minblum() const;

    /** Sets the maximal Bulge-to-Disk ratio. */
    Q_INVOKABLE void setMaxblum(double value);

    /** Returns the maximal Bulge-to-Disk ratio.*/
    Q_INVOKABLE double maxblum() const;

    //======================== Other Functions =======================
public:
    /** This function sets the _ref, _disk, _bulge and returns best fitting disk luminosity and bulge-to-disk ratio.
        The disk and bulge simulations are adapted so they contain the same mask as the reference image.
        Together with the adapted simulations and best fitting parameters, the lowest \f$\chi^2\f$ value is returned.*/
    void optimize(const Image& refframe, Image& diskframe, Image& bulgeframe,
                  double& disklum, double& blumratio, double& chi2);

private:
    /** This function determines if the x- or y-value is present in the simplex. */
    bool inSimplex(double simplex[3][3], double value, int x_y ) const;

    /** This function determines the \f$\chi^2\f$ value for certain luminosity values. */
    double function(Image& disk, Image& bulge, double x, double y);

    /** This function determines if the simplex needs to be contracted or shrunk. */
    void contract(Image& disk, Image& bulge,
                  double simplex[3][3], double center[], double refl[], double beta, double delta);

    /** This function determines if the simplex needs to be reflected or expanded. */
    void expand(Image& disk, Image& bulge,
                double simplex[3][3], double center[], double refl[], int counter, double Gamma);

    /** This function determines the initial simplex and sorts it. */
    void initialize(Image& disk, Image& bulge, double simplex[3][3]);

    /** This function checks if the simplex goes out of bound and corrects if necessary.
        The counter is used to make sure the corrections are not applied twice so the
        simplex collapses to a line. */
    void nearEdgeCorrections(double simplex[3][3], double Dpoint[], int counter) const;

    /** This function places the two values in the simplex in the correct place.
        The simplex is sorted from lowest \f$\chi^2\f$ value to highest. */
    void place(Image& disk, Image& bulge, double simplex[3][3], double x, double y);

    /** This function determines and sets the center and reflected point. */
    void setCenterReflected(Image& disk, Image& bulge,
                            double simplex[3][3], double center[], double reflected[], int counter, double alpha);

    /** This function determines if the simplex needs to be shrunk. */
    void shrink(Image& disk, Image& bulge, double simplex[3][3], double delta);

    //======================== Data Members ========================

private:
    double _minDlum;
    double _maxDlum;
    double _minblum;
    double _maxblum;
    const Image* _ref;
};

////////////////////////////////////////////////////////////////////

#endif // LUMSIMPLEX_HPP
