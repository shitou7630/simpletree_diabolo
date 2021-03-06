/****************************************************************************

 Copyright (C) 2016-2017 INRA (Institut National de la Recherche Agronomique, France) and IGN (Institut National de l'information Géographique et forestière, France)
 All rights reserved.

 Contact : jan.hackenberg@posteo.de

 Developers : Jan Hackenberg

 This file is part of Simpletree plugin Version 4 for Computree.

 Simpletree plugin is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Simpletree plugin is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Simpletree plugin.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#ifndef NLSCYLINDERFIT_H
#define NLSCYLINDERFIT_H

#include <pcl/ModelCoefficients.h>
#include <SimpleTree4/model/pointsimpletree.h>
#include <SimpleTree4/model/cylinder.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/QR>

class NLSCylinderFit
{
    PointCloudS::Ptr _cloud;
    pcl::ModelCoefficients _updated_coeff;

    Eigen::MatrixXd generate_Jacobian(pcl::ModelCoefficients coeff);

    Eigen::MatrixXd
    generate_value_matrix(pcl::ModelCoefficients coeff);

public:
    NLSCylinderFit(pcl::ModelCoefficients initial_coeff, PointCloudS::Ptr cloud);
};

#endif // NLSCYLINDERFIT_H
