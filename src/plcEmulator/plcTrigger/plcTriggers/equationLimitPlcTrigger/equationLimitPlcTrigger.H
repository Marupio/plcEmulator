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
    Foam::equationLimitPlcTrigger

Description
    A plcTrigger that tests the result of an equation against a limit.
    
SourceFiles
    equationLimitPlcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef equationLimitPlcTrigger_H
#define equationLimitPlcTrigger_H

#include "plcTrigger.H"
#include "objectRegistrySearcher.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class fvMesh;
class IOEquationReader;

/*---------------------------------------------------------------------------*\
                   Class equationLimitPlcTrigger Declaration
\*---------------------------------------------------------------------------*/

class equationLimitPlcTrigger
:
    public plcTrigger,
    public objectRegistrySearcher<IOEquationReader>,
    public objectRegistrySearcher<fvMesh>
{

public:

    // Public data

        //- Different evaluation methods
        enum evalMethodEnum
        {
            emunknown,
            empoint,
            eminternalFieldAverage,
            eminternalFieldMinimum,
            eminternalFieldMaximum,
            empatchFieldAverage,
            empatchFieldMinimum,
            empatchFieldMaximum
        };

private:

    // Private data

        //- Evaluation method
        evalMethodEnum evalMethod_;
        
        //- Limit
        scalar limit_;
        
        //- Trigger value when over limit
        bool overLimitValue_;

        //- equation name
        const word eqnName_;
        
        //- equationReader name
        const word eqnReaderName_;
        
        //- geoIndex
        //      0=internal field, greater than 0=(boundary patch+1)
        label geoIndex_;
        
        //- cellIndex (applies only to point method)
        label cellIndex_;

        //- mesh name - used for objectRegistry lookup for volField operations
        const word meshName_;

    // Private member functions

        //- Read input dictionary
        void readDict(const dictionary& inputDict);

public:

    //- Runtime type information
    TypeName("equationLimit");


    // Constructors

        //- Construct from dictionary, given name
        equationLimitPlcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor

        virtual ~equationLimitPlcTrigger()
        {}


    // Member functions

        // Evaluation

            //- Evaluate
            virtual bool evaluate(const Time& runTime);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
