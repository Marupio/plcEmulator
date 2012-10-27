/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::objectRegistrySearcher

Description
    Adds 'objectRegistry' keyword capabilities to plcTriggers.  Looks up the
    Type in given objectRegistry paths, and returns a pointer when found.

SourceFiles
    objectRegistrySearcher.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef objectRegistrySearcher_H
#define objectRegistrySearcher_H

#include "objectRegistry.H"
//#include "wordList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


/*---------------------------------------------------------------------------*\
                           Class objectRegistrySearcher Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class objectRegistrySearcher
{
    // Private data
    
        //- objectRegistry hierarchies to search through for variableName,
        //  relative to runTime
        fileNameList objectRegistryPaths_;

        //- The objectRegistryPaths broken down into objectRegistry lookup
        //  names
        PtrList<wordList> decomposedPaths_;

    // Private member functions

        //- Decompose objectRegistry paths into words
        void decomposePaths();

public:

    // Constructors

        //- Construct from components
        objectRegistrySearcher
        (
            const fileNameList objectRegistryPaths
        );

    // Destructor

        virtual ~objectRegistrySearcher();

    // Member functions

        //- Access object registry paths
        inline const fileNameList& objectRegistryPaths() const
        {
            return objectRegistryPaths_;
        }

        //- Lookup the object
        const Type * lookupObject
        (
            const word& lookupName,
            const Time& runTime
        );
    
        //- List the names of available Types in each registry path
        void showNames(const Time& runTime);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "objectRegistrySearcher.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
