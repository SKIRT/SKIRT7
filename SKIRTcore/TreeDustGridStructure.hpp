/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TREEDUSTGRIDSTRUCTURE_HPP
#define TREEDUSTGRIDSTRUCTURE_HPP

#include "Box.hpp"
#include "DustGridDensityInterface.hpp"
#include "DustMassInBoxInterface.hpp"
#include "GenDustGridStructure.hpp"
class DustDistribution;
class TreeNode;
class Parallel;

//////////////////////////////////////////////////////////////////////

/** TreeDustGridStructure is an abstract subclass of the GenDustGridStructure class, and
    represents three-dimensional dust grid structures with cuboidal cells organized in a tree.
    The tree's root node encloses the complete spatial domain, and nodes on subsequent levels
    recursively divide space into ever finer nodes. The depth of the tree can vary from place
    to place. The leaf cells (those that are not further subdivided) are the actual dust cells.
    The type of TreeNode used by the TreeDustGridStructure is decided in each subclass through
    a factory method. Depending on the type of TreeNode, the tree
    can become an octtree (8 children per node) or a kd-tree (2 children per node). Other
    node types could be implemented, as long as they are cuboids lined up with the axes. */
class TreeDustGridStructure : public GenDustGridStructure, public Box, public DustGridDensityInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a tree dust grid structure")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the outer radius of the root cell in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the outer radius of the root cell in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the outer radius of the root cell in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "minLevel")
    Q_CLASSINFO("Title", "the minimum level of grid refinement (typically 2 to 3)")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "50")
    Q_CLASSINFO("Default", "2")

    Q_CLASSINFO("Property", "maxLevel")
    Q_CLASSINFO("Title", "the maximum level of grid refinement (typically 6 to 10)")
    Q_CLASSINFO("MinValue", "2")
    Q_CLASSINFO("MaxValue", "50")
    Q_CLASSINFO("Default", "6")

    Q_CLASSINFO("Property", "searchMethod")
    Q_CLASSINFO("Title", "the search method used for traversing the tree grid")
    Q_CLASSINFO("TopDown", "top-down (start at root and recursively find appropriate child node)")
    Q_CLASSINFO("Neighbor", "neighbor (construct and use neighbor list for each node wall) ")
    Q_CLASSINFO("Bookkeeping", "bookkeeping (derive appropriate neighbor through node indices)")
    Q_CLASSINFO("Default", "Neighbor")

    Q_CLASSINFO("Property", "sampleCount")
    Q_CLASSINFO("Title", "the number of random density samples for determining cell subdivision")
    Q_CLASSINFO("MinValue", "10")
    Q_CLASSINFO("MaxValue", "1000")
    Q_CLASSINFO("Default", "100")

    Q_CLASSINFO("Property", "maxOpticalDepth")
    Q_CLASSINFO("Title", "the maximum mean optical depth for each dust cell")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "100")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "maxMassFraction")
    Q_CLASSINFO("Title", "the maximum fraction of dust mass contained in each dust cell")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1e-2")
    Q_CLASSINFO("Default", "1e-6")

    Q_CLASSINFO("Property", "maxDensDispFraction")
    Q_CLASSINFO("Title", "the maximum density dispersion in each dust cell, as fraction of the reference density")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    TreeDustGridStructure();

public:
    /** The destructor deletes all nodes from the tree vector created during setup. */
    ~TreeDustGridStructure();

protected:
    /** This function verifies that all attribute values have been appropriately set and actually
        constructs the tree. The first step is to create the root node (through the factory method
        createRoot() to be implemented in each subclass), and store it in the tree vector,
        which is just a list of pointers to nodes). The second phase is to recursively subdivide
        the root node and add the children at the end of the tree vector, until all nodes satisfy
        the criteria for no further subdivision. When this task is accomplished,
        the function creates a vector that contains the node IDs of all leaves. This is the actual
        dust cell vector (only the leaf nodes are the actual dust cells). The function also creates
        a vector with the cell numbers of all the nodes, i.e. the rank \f$m\f$ of the node in the
        ID vector if the node is a leaf, and the number -1 if the node is not a leaf (and hence not
        a dust cell). Finally, the function logs some details on the number of nodes and the number
        of cells, and if writeFlag() returns true, it writes the distribution of the grid cells to
        a file. */
    void setupSelfBefore();

private:
    /** This function, only to be called during the construction phase, investigates whether a node
        should be further subdivided and also takes care of the actual subdivision. There are
        several criteria for subdivision. The simplest criterion is the level of subdivision of the
        node: if it is less then a minimum level, the node is always subdivided, if it higher then
        a maximum level, there is no subdivision (these levels are input parameters). In the
        general case, whether or not we subdivide depends on the following criterion: if the ratio
        of the dust mass in the cell and the total dust mass, corresponding to the dust density
        distribution \f$\rho({\bf{r}})\f$, is larger than a preset threshold (an input parameter as
        well), the node is subdivided. The dust mass in the cell is calculated by generating
        \f$N_{\text{random}}\f$ random positions \f${\bf{r}}_n\f$ in the node, so that the total
        mass using the density in these points is given by \f[ M \approx \frac{\Delta x\, \Delta
        y\, \Delta z}{N_{\text{random}}} \sum_{n=0}^{N_{\text{random}}} \rho({\bf{r}}_n), \f] If
        the mass criterion is not satisfied, the node is not subdivided. If the conditions for
        subdivision are met, the first task is to calculate the division point. There are two
        options: geocentric division and barycentric division (an input flag again). In the former
        case, the division point of the cell is just the geometric centre, i.e. \f[ {\bf{r}}_c =
        (x_c,y_c,z_c) = \left( \frac{x_{\text{min}}+x_{\text{max}}}{2},
        \frac{y_{\text{min}}+y_{\text{max}}}{2}, \frac{z_{\text{min}}+z_{\text{max}}}{2} \right).
        \f] In the latter case the division point is the centre of mass, which we estimate using
        the \f$N_{\text{random}}\f$ points generated before, \f[ {\bf{r}}_c = \frac{ \sum_n
        \rho({\bf{r}}_n)\, {\bf{r}}_n}{ \sum_n \rho({\bf{r}}_n) }. \f] The last task is to actually
        create the eight child nodes of the node and add them to the tree. */
    void subdivide(TreeNode* node);

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the maximum extent of the grid structure in the X direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the maximum extent of the grid structure in the X direction. See also xmax(). */
    Q_INVOKABLE double extentX() const;

    /** Sets the maximum extent of the grid structure in the Y direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the maximum extent of the grid structure in the Y direction. See also ymax(). */
    Q_INVOKABLE double extentY() const;

    /** Sets the maximum extent of the grid structure in the Z direction. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the maximum extent of the grid structure in the Z direction. See also zmax(). */
    Q_INVOKABLE double extentZ() const;

    /** Sets the minimum level for the tree. */
    Q_INVOKABLE void setMinLevel(int value);

    /** Returns the minimum level for the tree. */
    Q_INVOKABLE int minLevel() const;

    /** Sets the maximum level for the tree. */
    Q_INVOKABLE void setMaxLevel(int value);

    /** Returns the maximum level for the tree. */
    Q_INVOKABLE int maxLevel() const;

    /** The enumeration type indicating the search method to be used for finding the subsequent
        node while traversing the tree grid. The TopDown method (the default) always starts at
        the root node and recursively finds the child node containing the new position. The
        Neighbor method constructs a neighbor list for each node (at each of the six walls) during
        setup, and then uses this list to locate the neighboring node containing the new position.
        The Bookkeeping method relies on the order in which the occtree nodes are created and
        stored to derive the appropriate neighbor solely through the respective node indices. */
    Q_ENUMS(SearchMethod)
    enum SearchMethod { TopDown, Neighbor, Bookkeeping };

    /** Sets the enumeration value indicating the search method to be used for finding the
        subsequent node while traversing the tree grid. */
    Q_INVOKABLE void setSearchMethod(SearchMethod value);

    /** Returns the enumeration value indicating the search method to be used for finding the
        subsequent node while traversing the tree grid. */
    Q_INVOKABLE SearchMethod searchMethod() const;

    /** Sets the number of random positions on which the density in a cell is sampled to decide
        whether or not the cell will be subdivided. The default value is 100 samples per cell for
        each decision. */
    Q_INVOKABLE void setSampleCount(int value);

    /** Returns the number of random positions on which the density is sampled for each decision to
        subdivide. */
    Q_INVOKABLE int sampleCount() const;

    /** Sets the maximum mean optical depth for each dust cell. A value of zero means that this
        criterion is not used. */
    Q_INVOKABLE void setMaxOpticalDepth(double value);

    /** Returns the maximum mean optical depth for each dust cell. */
    Q_INVOKABLE double maxOpticalDepth() const;

    /** Sets the maximum fraction of the total mass contained in each dust cell. A value of zero
        means that this criterion is not used. */
    Q_INVOKABLE void setMaxMassFraction(double value);

    /** Returns the maximum fraction of the total mass contained in each dust cell. */
    Q_INVOKABLE double maxMassFraction() const;

    /** Sets the maximum density dispersion in each dust cell, as fraction of the reference
        density. A value of zero means that this criterion is not used. */
    Q_INVOKABLE void setMaxDensDispFraction(double value);

    /** Returns the maximum density dispersion in each dust cell, as fraction of the reference
        density. */
    Q_INVOKABLE double maxDensDispFraction() const;

    //======================== Other Functions =======================

public:
    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. */
    double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. For a tree
        dust grid structure, it determines the node ID corresponding to the cell number \f$m\f$,
        and then simply calculates the volume of the corresponding cuboidal cell using \f$V =
        \Delta x\, \Delta y\, \Delta z\f$. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. For a tree dust grid structure, the search algorithm starts at the root
        node and selects the child node that contains the position. This procedure is repeated
        until the node is childless, i.e. until it is a leaf node that corresponds to an actual
        dust cell. */
    int whichcell(Position bfr) const;

    /** This function returns the central location of the dust cell with cell number \f$m\f$. For
        a tree dust grid structure, it determines the node ID corresponding to the cell number
        \f$m\f$, and then calculates the central position in that cell through \f[ \begin{split} x
        &= x_{\text{min}} + \frac12\, \Delta x \\ y &= y_{\text{min}} + \frac12\, \Delta y \\ z &=
        z_{\text{min}} + \frac12\, \Delta z \end{split} \f] */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For a
        tree dust grid structure, it determines the node ID corresponding to the cell number
        \f$m\f$, and then calculates a random position in that cell through \f[ \begin{split} x &=
        x_{\text{min}} + {\cal{X}}_1\, \Delta x \\ y &= y_{\text{min}} + {\cal{X}}_2\, \Delta y \\
        z &= z_{\text{min}} + {\cal{X}}_3\, \Delta z \end{split} \f] with \f${\cal{X}}_1\f$,
        \f${\cal{X}}_2\f$ and \f${\cal{X}}_3\f$ three uniform deviates. */
    Position randomPositionInCell(int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object.

        For a tree dust grid structure, the function uses a rather straighforward algorithm. It
        determines the dust cell that contains the starting position, and calculates the first wall
        of the cell that will be crossed. The pathlength \f$\Delta s\f$ is determined and the
        current position is moved to a new position along this path, a tiny fraction further than
        \f$\Delta s\f$, \f[ \begin{split} x_{\text{new}} &= x_{\text{current}} + (\Delta s +
        \epsilon)\,k_x \\ y_{\text{new}} &= y_{\text{current}} + (\Delta s + \epsilon)\,k_y \\
        z_{\text{new}} &= z_{\text{current}} + (\Delta s + \epsilon)\,k_z \end{split} \f] where \f[
        \epsilon = 10^{-12} \sqrt{x_{\text{max}}^2 + y_{\text{max}}^2 + z_{\text{max}}^2} \f] By
        adding this small extra bit, we ensure that the new position is now within the next cell,
        and we can repeat this exercise. This loop is terminated when the next position is outside
        the dust grid. To determine the cell numbers in this algorithm, the function uses the
        method configured with setSearchMethod(). */
    void path(DustGridPath* path) const;

    /** This function is used by the interface() template function in the SimulationItem class. It
        returns a list of simulation items that should be considered in the search for an item that
        implements the requested interface. The implementation in this class returns the default
        list (i.e. the receiving object) except in the following case. If the requested interface
        is DustGridDensityInterface (which is implemented by this class) and the dust distribution
        for this simulation does \em not offer the DustMassInBoxInterface interface, the returned
        list is empty (because it is then impossible to provide DustGridDensityInterface). */
    QList<SimulationItem*> interfaceCandidates(const std::type_info& interfaceTypeInfo);

    /** This function implements the DustGridDensityInterface interface. It returns the density for
        the dust component \em h in the dust grid cell with index \em m. The implementation relies
        on the DustMassInBoxInterface interface in the dust distribution for this simulation. */
    double density(int h, int m) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane
        to the specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the xz plane
        to the specified DustGridPlotFile object. */
    void write_xz(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the yz plane
        to the specified DustGridPlotFile object. */
    void write_yz(DustGridPlotFile* outfile) const;

    /** This function writes 3D information for the cells up to a certain level in the dust grid
        structure to the specified DustGridPlotFile object. The output is restricted to limit the
        number of cells being written (to keep the line density in the output plot within reason).
        */
    void write_xyz(DustGridPlotFile* outfile) const;

private:
    /** This function returns a pointer to the root node of the tree. */
    TreeNode* root() const;

    /** This function returns a pointer to the node corresponding to cell number \f$m\f$. It just
        reads the node ID number of the \f$m\f$'th leave cell and returns the corresponding pointer
        of the tree vector. */
    TreeNode* getnode(int m) const;

    /** This function returns the cell number \f$m\f$ of a node in the tree. It just reads the ID
        of the node and determines the corresponding cell number from the internal cell number
        vector. */
    int cellnumber(const TreeNode* node) const;

protected:
    /** This pure virtual function, to be implemented in each subclass, creates a root node
        of the appropriate type, using a node identifier of zero and the specified spatial extent,
        and returns a pointer to it. The caller takes ownership of the newly created object. */
    virtual TreeNode* createRoot(const Box& extent) = 0;

    //======================== Data Members ========================

private:
    // discoverable attributes (in addition to extent which is stored in inherited Box)
    int _minlevel;
    int _maxlevel;
    SearchMethod _search;
    int _Nrandom;
    bool _ynbarycentric;
    double _maxOpticalDepth;
    double _maxMassFraction;
    double _maxDensDispFraction;

    // data members initialized during setup
    Parallel* _parallel;
    DustDistribution* _dd;
    DustMassInBoxInterface* _dmib;
    double _totalmass;
    double _eps;
    int _Nnodes;
    std::vector<TreeNode*> _tree;
    std::vector<int> _cellnumberv;
    std::vector<int> _idv;
    int _highestWriteLevel;

protected:
    bool _useDmibForSubdivide;
};

//////////////////////////////////////////////////////////////////////

#endif // TREEDUSTGRIDSTRUCTURE_HPP
