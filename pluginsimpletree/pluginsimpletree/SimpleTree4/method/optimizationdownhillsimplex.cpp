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

#include "optimizationdownhillsimplex.h"


MethodCoefficients OptimizationDownHillSimplex::get_end_coeff() const
{
    return _end;
}


MethodCoefficients OptimizationDownHillSimplex::convert_to_coefficients(const Eigen::MatrixXd &matrix, int i)
{
    MethodCoefficients coeff = _start;
    if(matrix.cols()==_ndim)
    {
        switch (_ndim) {
        case 5:
            coeff.sphere_radius_multiplier = matrix(i,0);
            coeff.epsilon_sphere = matrix(i,1);
            coeff.epsilon_cluster_branch = matrix(i,2);
            coeff.min_radius_sphere = matrix(i,3);
            coeff.ransac_circle_inlier_distance = matrix(i,4);
            break;
        case 6:
            coeff.sphere_radius_multiplier = matrix(i,0);
            coeff.epsilon_sphere = matrix(i,1);
            coeff.epsilon_cluster_branch = matrix(i,2);
            coeff.min_radius_sphere = matrix(i,3);
            coeff.ransac_circle_inlier_distance = matrix(i,4);
            coeff.epsilon_cluster_stem = matrix(i,5);

            break;
        default:
            emit emit_qstring("OptimizationDownHillSimplex wrong number of ndim for OptimizationDownHillSimplex::convert_to_coefficients(const Eigen::MatrixXd &matrix, int i)");
            break;
        }
    }
    else
    {
        emit emit_qstring("OptimizationDownHillSimplex wrong number of ndim for OptimizationDownHillSimplex::convert_to_coefficients(const Eigen::MatrixXd &matrix, int i)");
    }

    return coeff;


}

void OptimizationDownHillSimplex::generate_coeff_from_point(QVector<double> &coeff)
{
    if(coeff.size() == _ndim)
    {
        switch (_ndim) {
        case 5:

            _end.sphere_radius_multiplier = coeff.at(0);
            _end.epsilon_sphere = coeff.at(1);
            _end.epsilon_cluster_branch = coeff.at(2);
            _end.min_radius_sphere = coeff.at(3);
            _end.ransac_circle_inlier_distance = coeff.at(4);
            break;
        case 6:
            _end.sphere_radius_multiplier = coeff.at(0);
            _end.epsilon_sphere = coeff.at(1);
            _end.epsilon_cluster_branch = coeff.at(2);
            _end.min_radius_sphere = coeff.at(3);
            _end.ransac_circle_inlier_distance = coeff.at(4);
            _end.epsilon_cluster_stem = coeff.at(5);
            break;
        default:
            emit emit_qstring("OptimizationDownHillSimplex wrong number of ndim for OptimizationDownHillSimplex::generate_coeff_from_point(QVector<double> &coeff)");
            break;
        }
    }
    else
    {
        emit emit_qstring("OptimizationDownHillSimplex wrong number of ndim for OptimizationDownHillSimplex::generate_coeff_from_point(QVector<double> &coeff)");
    }


}

QVector<double> OptimizationDownHillSimplex::minimize(QVector<double> &point, QVector<double> &factors)
{
    Eigen::MatrixXd matrix_copy;
    matrix_copy.resize(_mpts,_ndim);
    for(int i=0;i < _mpts; i++)
    {
        for(int j = 0; j < _ndim; j++)
        {
            matrix_copy(i,j) = point[j];
        }
        if(i!=0)
        {
            matrix_copy(i,i-1) *= factors[i-1];
        }
    }
    return (minimize(matrix_copy));
}

QVector<double> OptimizationDownHillSimplex::minimize(MethodCoefficients coeff, const double del)
{
    QVector<double> factors;
    for(size_t i = 0; i < _ndim; i++)
    {
        factors.push_back(del);
    }
    QVector<double> point(_ndim);
    point[0] = coeff.sphere_radius_multiplier;
    point[1] = coeff.epsilon_sphere;
    point[2] = coeff.epsilon_cluster_branch;
    point[3] = coeff.min_radius_sphere;
    point[4] = coeff.ransac_circle_inlier_distance;
    if(_ndim == 6)
    {
        point[5] = coeff.epsilon_cluster_stem;
    }

    else if(_ndim == 5)
    {
    }
    else
    {
        emit  emit_qstring
                ("OptimizationDownHillSimplex wrong number of ndim for OptimizationDownHillSimplex::minimize(MethodCoefficients coeff, const double del)");
    }


    return minimize(point,factors);

}

void OptimizationDownHillSimplex::swap(double x1, double x2)
{
    double x3 = x1;
    x1 = x2;
    x2 = x3;
}

void OptimizationDownHillSimplex::get_psum(Eigen::MatrixXd &matrix, QVector<double> &psum)
{
    for(int j = 0; j < _ndim; j++)
    {
        double sum = 0;
        for(int i = 0; i < _mpts; i++)
        {
            sum += matrix(i,j);
        }
        psum[j] = sum;
    }
}

double OptimizationDownHillSimplex::amotry(Eigen::MatrixXd &p, QVector<double> &y, QVector<double> &psum, const int ihi, const double fac)
{
    QVector<double> ptry(_ndim);
    double fac1 = (1.0-fac)/_ndim;
    double fac2 = fac1-fac;
    for(int j = 0; j < _ndim; j++)
    {
        ptry[j]=psum[j]*fac1-p(ihi,j)*fac2;
    }
    double ytry = evaluate(ptry);
    if(ytry < y[ihi])
    {
        y[ihi] = ytry;
        for(int j = 0; j < _ndim; j++)
        {
            psum[j] += ptry[j]-p(ihi,j);
            p(ihi,j) = ptry[j];
        }
    }
    return ytry;
}


void OptimizationDownHillSimplex::sent_counter(int counter)
{
    emit emit_counter(counter);
}

void OptimizationDownHillSimplex::set_distance(int i, double d)
{
    y[i] = d;
}



void OptimizationDownHillSimplex::minimize()
{
    QVector<double> result = minimize(_start,1.2);
    generate_coeff_from_point(result);

}

QString OptimizationDownHillSimplex::get_qstring_from_eigen(Eigen::MatrixXd &matrix_copy)
{
    Eigen::IOFormat OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";<br>", "", "", "[", "]");
    std::string sep = "<br>----------------------------------------<br>";
    std::stringstream str_stream;
    str_stream <<  sep << matrix_copy.format(OctaveFmt) << sep;
    QString str = QString::fromStdString(str_stream.str());

    return str;
}

QString OptimizationDownHillSimplex::get_qstring_from_vector(QVector<double> &vec)
{
    QString qstr;
    for(int i = 0; i < (_mpts-1); i++)
    {

        qstr.append(QString::number(vec[i])).append("; ");
    }
    qstr.append(QString::number(vec[_mpts-1])).append(" <br>");
    return qstr;
}

QVector<double> OptimizationDownHillSimplex::minimize(Eigen::MatrixXd &matrix_copy)
{
    _copy = _start;

    int ihi, ilo,inhi;
    QVector<double> psum(_ndim);
    QVector<double> pmin(_ndim);
    QVector<double> pmin_copy(_ndim);
    QVector<double> x (_ndim);
    for(int j = 0; j < _ndim; j++)
    {
        pmin_copy[j] = matrix_copy(0,j);
    }
    _matrix=matrix_copy;
    y.resize(_mpts);
    for(int i = 0; i < _mpts; i++)
    {
        for(int j = 0; j < _ndim; j++)
        {
            x[j] = _matrix(i,j);
        }
        y[i] = evaluate(x);
        if(i == 0)
        {
            _distance_start = y[i];
            QString qstr = "Starting downhill Simplex with distance ";
            qstr.append(QString::number(_distance_start));
            qstr.append(".");
            emit emit_qstring(qstr);
        }
    }

    nfunc = 0;
    get_psum(_matrix,psum);
    QString qstr;
    for(int i = 0; i < _matrix.rows(); i++)
    {

        for(int j = 0; j < _matrix.cols(); j++)
        {
            qstr.append(QString::number(_matrix(i,j))).append(" ");
        }
        qstr.append(" = ").append(QString::number(y[i])).append("<br>");
    }
    emit emit_qstring(qstr);

    for(;;)
    {
        int counter = ((((double) nfunc)/(double)_NMAX)*100.0);
        emit emit_counter(counter);
        ilo=0;
        ihi = y[0]>y[1]?(inhi=1,0) :(inhi = 0,1);
        for(int i = 0; i < _mpts; i++)
        {
            if(y[i]<=y[ilo])
            {
                ilo = i;
            }
            if(y[i]>y[ihi])
            {
                inhi = ihi;
                ihi = i;
            } else if(y[i]>y[inhi] && i!=ihi) inhi =i;
        }
        double rtol = 2.0*std::abs(y[ihi]-y[ilo]) / (std::abs(y[ihi]) + std::abs(y[ilo]) + TINY);
        if(rtol < ftol)
        {

            swap(y[0],y[ilo]);
            for(int i = 0; i < _ndim; i++)
            {
                swap(_matrix(0,1),_matrix(ilo,i));
                pmin[i] = _matrix(0,i);
            }
            _distance_end = y[0];
            if(_distance_end ==0)
            {
                emit emit_qstring("Downhill Simplex resulted in a zero distance - Error. Initial parameters returned. <br>");
                return pmin_copy;

            } else
            {
                QString qstr = "Downhill Simplex converged after ";
                qstr.append(QString::number(nfunc));
                qstr.append(" evaluations. <br>");
                emit emit_qstring(qstr);
                qstr = "Ending with distance :  <br>";
                qstr.append(QString::number(_distance_end));
                emit emit_qstring(qstr);


                qstr = "Final Matrix has Values of: <br>";

                QString str = get_qstring_from_eigen(_matrix);
                qstr.append(str);

                emit emit_qstring(qstr);

                return pmin;
            }
        }
        if(nfunc >= _NMAX)
        {
            QString qstr = "Downhill Simplex reached end criterium of 200 operations. <br>";
            emit emit_qstring(qstr);
            qstr = "Ending with distance :  <br>";
            swap(y[0],y[ilo]);
            for(int i = 0; i < _ndim; i++)
            {
                swap(_matrix(0,1),_matrix(ilo,i));
                pmin[i] = _matrix(0,i);
            }
            _distance_end = y[0];
            qstr.append(QString::number(_distance_end));
            emit emit_qstring(qstr);
            qstr = "Final Matrix has Values of: <br>";
            QString str = get_qstring_from_eigen(_matrix);
            qstr.append(str);
            emit emit_qstring(qstr);

            return pmin;
        }
        nfunc+=2;








        double ytry = amotry(_matrix,y,psum,ihi,-1.0);

        if(ytry <= y[ilo])
        {
            ytry = amotry(_matrix,y,psum,ihi,2.0);
        }
        else if(ytry>=y[inhi])
        {
            double ysave = y[ihi];
            ytry = amotry(_matrix,y,psum,ihi,0.5);

            if(ytry>=ysave)
            {
                for(int i = 0; i < _mpts; i++)
                {
                    if(i!=ilo)
                    {
                        for(int j = 0; j < _ndim; j++)
                        {
                            _matrix(i,j) = psum[j] = 0.5*(_matrix(i,j)+_matrix(ilo,j));
                        }
                        y[i] = evaluate(psum);
                    }
                }
                nfunc += _ndim;
                get_psum(_matrix,psum);
            }
        }
        else --nfunc;
    }
}

double OptimizationDownHillSimplex::evaluate(QVector<double> x)
{
    MethodCoefficients coeff = _start;
    if(x.size()==_ndim)
    {
        switch (_ndim) {
        case 5:
            coeff.sphere_radius_multiplier = x.at(0);
            coeff.epsilon_sphere = x.at(1);
            coeff.epsilon_cluster_branch = x.at(2);
            coeff.min_radius_sphere = x.at(3);
            coeff.ransac_circle_inlier_distance = x.at(4);
            break;
        case 6:
            coeff.sphere_radius_multiplier = x.at(0);
            coeff.epsilon_sphere = x.at(1);
            coeff.epsilon_cluster_branch = x.at(2);
            coeff.min_radius_sphere = x.at(3);
            coeff.ransac_circle_inlier_distance = x.at(4);
            coeff.epsilon_cluster_stem = x.at(5);

            break;
        default:
            emit emit_qstring("Wrong number of dimensions in <br> OptimizationDownHillSimplex::evaluate(QVector<double> x) ");
            break;
        }
    }
    else
    {
        emit emit_qstring("Wrong number of dimensions in <br> OptimizationDownHillSimplex::evaluate(QVector<double> x) ");
    }



    SphereFollowing2 spherefollowing(coeff,_cloud, _subdivide_stem_and_branch_points);
    spherefollowing.sphere_following();
    double dist  = 50;

    if(spherefollowing.get_cylinders().size()>3)
    {
 //       int size_coeff = spherefollowing.get_cylinders().size();
  //      size_coeff = std::max(size_coeff,50);
        QVector<pcl::ModelCoefficients> coeff = spherefollowing.get_cylinders();
        BuildTree builder(coeff);
        QSharedPointer<Tree> tree (new Tree(builder.getRoot_segment(), _start.id));

        RemoveFalseCylinders remove(tree);
        ImproveByMedian improve_by_median(tree);
        ImproveByMerge improve_merge(tree);
        ReorderTree reorder(tree);


       // QVector<QSharedPointer> cylinders = tree->get_all_cylinders();

        coeff = tree->get_pcl_coefficients();
        ComputeDistanceCylindersCloud cd (coeff,_cloud);

//        BuildTree builder(spherefollowing.get_cylinders());
//        QSharedPointer<Tree> tree (new Tree(builder.getRoot_segment()));
//        RemoveFalseCylinders remove(tree);
//        //QVector<pcl::ModelCoefficients> coeff_cylinder = spherefollowing.get_cylinders();
//        ComputeDistanceCylindersCloud cd (tree,_cloud);


        dist = cd.get_mean_sqrd_dist();
      //  dist = (double) 1.0/cd.get_inliers_per_area();
//        if(length != 0 && height != 0)
//        {
//            dist = dist * std::pow(std::max<float>((length/height),1),3);
//        } else
//        {
//            dist = std::numeric_limits<float>::max();
//            qDebug() << "length or height in optimization zero";
//        }
    //    dist = dist * size_coeff;

    }

    return dist;

}



MethodCoefficients OptimizationDownHillSimplex::get_coefficients_from_matrix(Eigen::MatrixXd &matrix, int i)
{
    MethodCoefficients coeff = _start;
    if(matrix.cols()==_ndim)
    {
        switch (_ndim) {
        case 5:
            coeff.sphere_radius_multiplier = matrix(i,0);
            coeff.epsilon_sphere = matrix(i,1);
            coeff.epsilon_cluster_branch = matrix(i,2);
            coeff.min_radius_sphere = matrix(i,3);
            coeff.ransac_circle_inlier_distance = matrix(i,4);
            break;
        case 6:

            coeff.sphere_radius_multiplier = matrix(i,0);
            coeff.epsilon_sphere = matrix(i,1);
            coeff.epsilon_cluster_branch = matrix(i,2);
            coeff.min_radius_sphere = matrix(i,3);
            coeff.ransac_circle_inlier_distance = matrix(i,4);
            coeff.epsilon_cluster_stem = matrix(i,5);
            break;
        default:
            emit emit_qstring("Wrong number of dimensions in <br> OptimizationDownHillSimplex::get_coefficients_from_matrix(Eigen::MatrixXd &matrix, int i) ");
            break;
        }
    }
    else {
        emit emit_qstring("Wrong number of dimensions in <br> OptimizationDownHillSimplex::get_coefficients_from_matrix(Eigen::MatrixXd &matrix, int i) ");
    }

    return coeff;
}

void OptimizationDownHillSimplex::sent_qstring(QString qstring)
{
    emit emit_qstring(qstring);
}

void OptimizationDownHillSimplex::set_ndim(const int dim)
{
    _ndim = dim;
    _mpts = _ndim+1;
    _matrix.resize(_mpts,_ndim);
}

//void OptimizationDownHillSimplex::evaluate_multithreaded(Eigen::MatrixXd &matrix)
//{
//    if(_is_multithreaded)
//    {
//        for(int i = 0; i < _mpts; i++)
//        {
//            MethodCoefficients coeff = get_coefficients_from_matrix(matrix,i);
//            WorkerDownhillSimplex* worker (new WorkerDownhillSimplex(coeff,_cloud, sharedFromThis(),_subdivide_stem_and_branch_points,i));
//            start(worker);
//        }
//        waitForDone();
//    }
//    else
//    {
//        for(int i = 0; i < _mpts; i++)
//        {
//            MethodCoefficients coeff = get_coefficients_from_matrix(matrix,i);
//            SphereFollowing2 spherefollowing(coeff,_cloud, _subdivide_stem_and_branch_points);
//            spherefollowing.sphere_following();

//            float dist  = 1;
//            if(spherefollowing.get_cylinders().size()>3)
//            {
//                BuildTree builder(spherefollowing.get_cylinders());
//                QSharedPointer<Tree> tree (new Tree(builder.getRoot_segment()));
//                RemoveFalseCylinders remove(tree);
//                //QVector<pcl::ModelCoefficients> coeff_cylinder = spherefollowing.get_cylinders();
//                ComputeDistanceCylindersCloud cd (tree,_cloud);
//                dist = cd.get_mean_sqrd_dist();
//            }

//            set_distance(i,dist);
//        }
//    }
//}

OptimizationDownHillSimplex::OptimizationDownHillSimplex(const double ftoll,  MethodCoefficients coeff, PointCloudS::Ptr cloud,
                                                         bool subdivide_stem_and_branch_points, bool is_multithreaded):
    ftol(ftoll), _subdivide_stem_and_branch_points(subdivide_stem_and_branch_points), _cloud(cloud), _start(coeff), _end(coeff),_is_multithreaded(is_multithreaded)
{
//    if(_is_multithreaded)
//    {
//        int numberThreads = QThread::idealThreadCount()-1;
//        this->setMaxThreadCount(std::max<int>(numberThreads,1));
//        QString qstr = "Number of Threads is ";
//        qstr.append(QString::number(numberThreads));
//        qstr.append(".<br>");
//        emit emit_qstring(qstr);
//    }
//    else
//    {
//        this->setMaxThreadCount(1);
//    }
}

