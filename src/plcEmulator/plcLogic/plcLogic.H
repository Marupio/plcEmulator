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
    Foam::plcLogic

Description
    A container for user-defined logic tests for the plcEmulator.
    
SourceFiles
    plcLogicI.H
    plcLogic.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/


#ifndef plcLogic_H
#define plcLogic_H

#include "plcTrigger.H"
#include "UPtrList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class plcLogic Declaration
\*---------------------------------------------------------------------------*/

class plcLogic
{

    // Private data

        //- Name of logic test
        const word name_;

        //- Name of associated solver domain
        const word solverDomainName_;

        //- Index of associated solver domain
        label solverDomainIndex_;

        //- Required true triggers
        UPtrList<const plcTrigger> triggered_;
        
        //- Required false triggers
        UPtrList<const plcTrigger> notTriggered_;

    // Private member functions

        //- Disallow default bitwise copy construct and assignment
        void operator=(const plcLogic&);
        plcLogic(const plcLogic&);
        
        //- Read dictionary for construction
        void readDict
        (
            const dictionary& inputDict,
            const PtrList<plcTrigger>& triggers
        );


public:

    TypeName("plcLogic");

    // Constructors

        //- Construct reading from dictionary
        plcLogic
        (
            const word& name,
            const dictionary& inputDict,
            const PtrList<plcTrigger>& triggers,
            const wordList& solverDomains
        );

    // Destructor
    
        virtual ~plcLogic()
        {}

    // Member Functions

        //- Return the name of this logic
        inline const word& name() const;
    
        //- Return the solver domain
        inline const label& solverDomain() const;

        //- Perform the logic test - true means all conditions are met
        bool test() const;
        
        //- Report its current state to the console
        void report() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "plcLogicI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
