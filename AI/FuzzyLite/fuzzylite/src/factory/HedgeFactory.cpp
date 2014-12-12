/*
 Author: Juan Rada-Vilela, Ph.D.
 Copyright (C) 2010-2014 FuzzyLite Limited
 All rights reserved

 This file is part of fuzzylite.

 fuzzylite is free software: you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 fuzzylite is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with fuzzylite.  If not, see <http://www.gnu.org/licenses/>.

 fuzzylite™ is a trademark of FuzzyLite Limited.

 */

#include "fl/factory/HedgeFactory.h"

#include "fl/hedge/Any.h"
#include "fl/hedge/Extremely.h"
#include "fl/hedge/Not.h"
#include "fl/hedge/Seldom.h"
#include "fl/hedge/Somewhat.h"
#include "fl/hedge/Very.h"


namespace fl {

    HedgeFactory::HedgeFactory() : ConstructionFactory<Hedge*>("Hedge") {
        registerConstructor("", NULL);
        registerConstructor(Any().name(), &(Any::constructor));
        registerConstructor(Extremely().name(), &(Extremely::constructor));
        registerConstructor(Not().name(), &(Not::constructor));
        registerConstructor(Seldom().name(), &(Seldom::constructor));
        registerConstructor(Somewhat().name(), &(Somewhat::constructor));
        registerConstructor(Very().name(), &(Very::constructor));
    }

    HedgeFactory::~HedgeFactory() {

    }

}
