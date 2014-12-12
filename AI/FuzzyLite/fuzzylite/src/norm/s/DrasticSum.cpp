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

#include "fl/norm/s/DrasticSum.h"

namespace fl {

    std::string DrasticSum::className() const {
        return "DrasticSum";
    }

    scalar DrasticSum::compute(scalar a, scalar b) const {
        if (Op::isEq(Op::min(a, b), 0.0)) {
            return Op::max(a, b);
        }
        return 1.0;
    }

    DrasticSum* DrasticSum::clone() const {
        return new DrasticSum(*this);
    }

    SNorm* DrasticSum::constructor() {
        return new DrasticSum;
    }

}
