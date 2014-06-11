#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# Execute this script with "git" as default directory
#

# generate the latex documentation in a temporary folder next to the git folder
/Applications/Doxygen.app/Contents/Resources/doxygen doc/pdf.doxygen

# adjust the make file to allow more iterations for fixing cross-references
mv ../latex/Makefile ../latex/Makefile_original
sed s/count=5/count=15/ < ../latex/Makefile_original > ../latex/Makefile

# generate the pdf file
make -C ../latex

# move the result
mkdir -p ../doc
mv -f ../latex/refman.pdf ../doc/SKIRT.pdf

# remove the intermediate files
rm -r ../latex
