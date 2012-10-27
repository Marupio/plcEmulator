/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2008-2009 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#   include "createTime.H"
#   include "createMesh.H"
#   include "algorithm2CreateFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // Allow control to adjust the timestep if necessary
    control.initializeAndAdjustTime(runTime);

	for (runTime++; !runTime.end(); runTime++)
    {
        Info<< "---algorithm2--- t = " << runTime.timeName()
            << endl;

        T0 += ascentRate0 * runTime.deltaT();
        T1 += ascentRate1 * runTime.deltaT();

        // Random solverSignal "sumAbove1000"
        scalar theSum(T0.internalField()[0] + T1.internalField()[1]);
        Info << "Cell zero sum = " << theSum << endl;
        if (theSum == 1000)
        {
            Info << "Sending random solver signal" << endl;
            control.signal("sumAbove1000", true);
        }

        if (runTime.outputTime())
        {
            Info << "---algorithm2--- Step successful, writing output"
                << endl;
        }
        else
        {
            Info << "---algorithm2--- Step successful, no output "
                << "generated" << endl;
        }

        control.updateAndWrite(runTime);
    }
    Info << "---algorithm2--- End" << endl;

    // Reset control signal
    control.signal("sumAbove1000", false);
