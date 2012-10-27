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
    Foam::plcTrigger

Description
    Abstract base class for plcTriggers (switches)
    
SourceFiles
    plcTriggerI.H
    plcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/


#ifndef plcTrigger_H
#define plcTrigger_H

#include "word.H"
#include "typeInfo.H"
#include "runTimeSelectionTables.H"
#include "IOdictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declarations
class plcEmulator;

/*---------------------------------------------------------------------------*\
                           Class plcTrigger Declaration
\*---------------------------------------------------------------------------*/

class plcTrigger
{

protected:
    // Protected data

        //- Reference to the controller
        plcEmulator& control_;

        //- Name of trigger
        const word name_;

    // Protected data
    
        //- Last evaluated value
        bool value_;

private:

    // Private member functions

        //- Disallow default bitwise copy construct and assignment
        void operator=(const plcTrigger&);
        plcTrigger(const plcTrigger&);
        
public:

    TypeName("plcTrigger");

    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            plcTrigger,
            dictionary,
            (
                plcEmulator& control,
                const word& triggerName,
                const dictionary& inputDict
            ),
            (control, triggerName, inputDict)
        );

    // Constructors

        //- Construct reading from dictionary
        plcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Selectors

        //- Return a reference to a new plcTrigger
        //  *** Located in newPlcTrigger.C ***
        static autoPtr<plcTrigger> New
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor
    
        virtual ~plcTrigger()
        {}

    // Member Functions
    
        // Access
        
            //- Return the controller
            inline const plcEmulator& control() const;
            inline plcEmulator& control();

            //- Return the name
            inline const word& name() const;
            
            //- Return the value
            inline const bool& value() const;

        // Evaluation
        
            //- Evaluate
            virtual bool evaluate(const Time& runTime)
            {
                notImplemented("plcTrigger::evaluate");
                return false;
            }
            
        // Read and write
        
            //- Read the outputDict, used to get latest values for local data
            //  when resuming a simulation mid-run
            virtual void readOutputDict(const dictionary& outputSubDict)
            {
                // do nothing by default
            }
            
            //- Write local data to a dictionary
            virtual dictionary writeOutputDict() const
            {
                // return an empty dictionary by default
                return dictionary();
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "plcTriggerI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
