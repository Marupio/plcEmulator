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
    Foam::volScalarFieldLimitPlcTrigger

Description
    A plcTrigger that compares a volScalarField against a limit.
    
SourceFiles
    volScalarFieldLimitPlcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef volScalarFieldLimitPlcTrigger_H
#define volScalarFieldLimitPlcTrigger_H

#include "plcTrigger.H"
#include "objectRegistrySearcher.H"
#include "scalarField.H"
#include "volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                   Class volScalarFieldLimitPlcTrigger Declaration
\*---------------------------------------------------------------------------*/

class volScalarFieldLimitPlcTrigger
:
    public plcTrigger,
    public objectRegistrySearcher<volScalarField>
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

        //- Variable name
        const word variableName_;
        
        //- geoIndex
        //      0=internal field, greater than 0=(boundary patch+1)
        label geoIndex_;
        
        //- cellIndex (applies only to point method)
        label cellIndex_;

    // Private member functions

        //- Read input dictionary
        void readDict(const dictionary& inputDict);
        
public:

    //- Runtime type information
    TypeName("volScalarFieldLimit");


    // Constructors

        //- Construct from dictionary, given name
        volScalarFieldLimitPlcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor

        virtual ~volScalarFieldLimitPlcTrigger()
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
