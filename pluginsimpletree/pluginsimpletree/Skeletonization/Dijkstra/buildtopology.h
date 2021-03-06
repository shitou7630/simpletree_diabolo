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

#ifndef BUILDTOPOLOGY_H
#define BUILDTOPOLOGY_H

#include "SimpleTree4/model/pointsimpletree.h"
#include "dijkstra_coefficients.h"
#include <QVector>
#include <QDebug>
#include "SimpleTree4/method/point_cloud_operations/bincloud.h"
#include "SimpleTree4/math/simplemath.h"


class BuildTopology
{
    DijkstraCoefficients _coeff;

    PointCloudS::Ptr _cloud_in;

    QVector<pcl::ModelCoefficients> _skeleton_coeff;

    void initialize();

    QVector<PointCloudS::Ptr> _bin_clusters;

    void compute();

public:

    BuildTopology(PointCloudS::Ptr cloud_in, DijkstraCoefficients coeff);

    QVector<pcl::ModelCoefficients> get_skeleton_coeff() const;
};

#endif // BUILDTOPOLOGY_H
