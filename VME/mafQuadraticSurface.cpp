//
//  QuadraticSurface.cpp

//
#include "mafDefines.h" 
#include "mafQuadraticSurface.hpp"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafVMELandmark.h"
#include <limits>
#include "wx/busyinfo.h"
#include "mafMathtools.hpp"
#include "mafVMELandmarkCloud.h"
//#include "../DataElementInfoPanel.hpp"


mafQuadraticSurface::mafQuadraticSurface()
{
    y = VectorXd(6);
    res = VectorXd(6);
}

mafQuadraticSurface::~mafQuadraticSurface()
{
	;

}
int mafQuadraticSurface::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		if (
			parent->StoreDouble("Centerx", center[0]) == MAF_OK &&
			parent->StoreDouble("Centery", center[1]) == MAF_OK &&
			parent->StoreDouble("Centerz", center[2]) == MAF_OK 
			
			)
			return MAF_OK;
	}
	return MAF_ERROR;
}

int mafQuadraticSurface::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		
		if (
			node->RestoreDouble("Centerx", center[0]) == MAF_OK &&
			node->RestoreDouble("Centery", center[1]) == MAF_OK &&
			node->RestoreDouble("Centerz", center[2]) == MAF_OK 
			)
		{
		
			return MAF_OK;
		}
	}
	return MAF_ERROR;
}

// a mass rolling on a quadratic surface will move according to this eq
// y = [p;p.]
// Least action principles and their application to constrained and task-level problems in robotics and biomechanics
// April 2008Multibody System Dynamics 19(3):303-322
// DOI: 10.1007/s11044-007-9097-8


// TODO make this method accept Vector3d for p and pdot and return vector3d for pdotdot
// that would dave the convertion from and to vector6d


void mafQuadraticSurface:: geodesicFunction(const VectorXd &y,VectorXd& ydot) const
{
 //   VectorXd ydot(6);
    ydot(0) = y(3);
    ydot(1) = y(4);
    ydot(2) = y(5);
    
    Vector3d p;
    p(0) = y(0);
    p(1) = y(1);
    p(2) = y(2);
    
    Vector3d pdot;
    pdot(0) = y(3);
    pdot(1) = y(4);
    pdot(2) = y(5);
    
    double g = this->surf(p);
    RowVector3d G = this->grad(p);
    Matrix3d H = this->hess(p);
    
    double factor = (pdot.transpose() * H * pdot + beta * G * pdot).value() + alpha * g;
    Vector3d next = -G.transpose() * (G*G.transpose()).inverse() * factor;
  
    ydot(3) = next(0);
    ydot(4) = next(1);
    ydot(5) = next(2);
    
}






// computes the geodesic curve on the surface between p and q.
// p0 and q0 are the two muscle attachment points
// p and q are the points on the ellipsoid where the geodesic curve should start and stop
vector< Vector3d > mafQuadraticSurface::computeGeodesicPath(mafVMELandmark* mp0, mafVMELandmark* mq0, mafVMELandmark* mp, mafVMELandmark* mq, double length,int rate, double *error) //const
{

	mafTimeStamp currTs = GetTimeStamp();
	double point[3];
	double xyzr[3];
	//mp0->GetPoint(point, currTs);
	mp0->GetOutput()->GetAbsPose(point, xyzr, currTs);
	//added
	//mafMatrix tm;
	//mafTransform *m_TmpTransform1;
	//mafNEW(m_TmpTransform1);
	//added//
	//((mafVMELandmarkCloud *)mp0->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);
	//m_TmpTransform1->SetMatrix(tm);
	//m_TmpTransform1->TransformPoint(point,point);
	//
	Vector3d p0;
	p0 << point[0], point[01], point[02];

	

	//mq0->GetPoint(point, currTs);
	mq0->GetOutput()->GetAbsPose(point, xyzr, currTs);
	//added
	//((mafVMELandmarkCloud *)mq0->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);
	//m_TmpTransform1->SetMatrix(tm);
	//m_TmpTransform1->TransformPoint(point, point);
	//
	Vector3d q0;
	q0<< point[0], point[01], point[02];
	//mp->GetPoint(point, currTs);
	//added
	mp->GetOutput()->GetAbsPose(point, xyzr, currTs);
	//
	Vector3d p;
	p<<point[0], point[01], point[02];
	//string s1 = "position P inside the geodesic function " + std::to_string(point[0]) + " " + std::to_string(point[1]) + " " + std::to_string(point[2]);
	//wxBusyInfo wait50(s1.c_str());
	//Sleep(1500);

	//mq->GetPoint(point, currTs);
	//added
	mq->GetOutput()->GetAbsPose(point, xyzr, currTs);
	//
	Vector3d q;
	q<<point[0], point[01], point[02];
	//string s2 = "position Q inside the geodesic function " + std::to_string(point[0]) + " " + std::to_string(point[1]) + " " + std::to_string(point[2]);
	//wxBusyInfo wait501(s2.c_str());
	//Sleep(1500);

    //Vector3d p0 = mp0.getCenter(); //Vector3d(mp0.x, mp0.y, mp0.z);
    //Vector3d q0= mq0.getCenter(); //Vector3d(mq0.x, mq0.y, mq0.z);
    //Vector3d p = mp.getCenter(); //Vector3d(mp.x, mp.y, mp.z);
    //Vector3d q = mq.getCenter(); //Vector3d(mq.x, mq.y, mq.z);
    
    // initial velocity direction vector in p and q -> line from p0 and q0 to p and q
    Vector3d up = (p - p0) / (p - p0).norm();
    Vector3d uq = (q - q0) / (q - q0).norm();
    
    state_type xp = { p(0) , p(1) , p(2), up(0) ,up(1),up(2)}; // initial conditions
    state_type xq = { q(0) , q(1) , q(2), uq(0) ,uq(1),uq(2)}; // initial conditions

    vector< Vector3d > mstates;
	//vector< Vector3d > mstates20;
	vector< Vector3d > mstates2;
	//vector< Vector3d > mstates50;
    // TODO the two lambda's might be declared outside
    auto stateEquation = [this] (const state_type x , state_type &dxdt , double t) { geodesicCurveTrajectory(x,dxdt,t); } ;
    auto stateCollector = [&] (const state_type &x , double t) {mstates.push_back(Vector3d(x[0],x[1],x[2])); };
	//auto stateCollector2 = [&](const state_type &x, double t) {mstates2.push_back(Vector3d(x[0], x[1], x[2])); };
	double dt = 1/(1.0*rate);
	//wxBusyInfo wait150(std::to_string(dt).c_str());
	//Sleep(1500);
	integrate(stateEquation, xp, 0.0, length, dt, stateCollector);//80->180
	//integrate(stateEquation, xq, 0.0, length, 0.001, stateCollector2);//80->180//stateCollector2
	//for (int i = mstates20.size() - 1; i > 0; i--)
	//{
	//	mstates50.push_back(mstates20.at(i));
	//}

	
	/*integrate_const(runge_kutta4<state_type>(), stateEquation, xp, 0.0, 80.0, 0.001, stateCollector);
	//integrate_const(runge_kutta4<state_type>(), stateEquation, xq, 0.0, 80.0, 0.001, stateCollector);
	//typedef runge_kutta_cash_karp54< state_type > error_stepper_rkck54;
	//double err_abs = 1.0e-10;
	//double err_rel = 1.0e-6;
	//integrate_adaptive(make_controlled(err_abs, err_rel, error_stepper_rkck54()),stateEquation, xp, 0.0, 80.0, 0.001, stateCollector);
	//integrate_adaptive(make_controlled(err_abs, err_rel, error_stepper_rkck54()), stateEquation, xq, 0.0, 80.0, 0.001, stateCollector);
	*/
	

    // TODO we might need to store both sides of the path separately
	//string s1 = "states size " + std::to_string(mstates.size());
	//wxBusyInfo wait50(s1.c_str());
	//Sleep(1500);
	

	/*string s2 = "states2 size " + std::to_string(mstates2.size());
	wxBusyInfo wait5(s2.c_str());
	Sleep(1500);*/
	double epsilon = 0;
	double distToQ = 10000000;
	int i = 0;
	if (mstates.size() > 1)
	{
		while (((mstates.at(i) - q).norm() < distToQ + epsilon) && (i < mstates.size() - 1))
		if ((std::abs(mstates.at(i)[0])< 1000000) && (std::abs(mstates.at(i)[1])< 1000000) && (std::abs(mstates.at(i)[2])< 1000000))
		{
			
				mstates2.push_back(mstates.at(i));
				distToQ = (mstates.at(i) - q).norm();
				i++;
		
		}
		else
		{
			string s20 = "mstates out of bounds " + std::to_string(mstates.at(i)[0]) + " " + std::to_string(mstates.at(i)[1]) + " " + std::to_string(mstates.at(i)[2]);
			wxBusyInfo wait5(s20.c_str());
			Sleep(250);
		}
	}
	(*error) = distToQ;
	//string s20 = "size to Q " + std::to_string(i)+" error toQ " +std::to_string(distToQ);
	//wxBusyInfo wait5(s20.c_str());
	//Sleep(250); 
	/*i = mstates50.size()-1;
	if (mstates50.size() > 1)
	{		
		while (((mstates50.at(i) - p).norm() > 1) && (i >=1))
		{
			mstates2.push_back(mstates50.at(i));
			i--;
		}
	}*/


    return mstates2;


}

void vector2state(const VectorXd &inputstate, state_type& target) // Matrix<double, 3, 1>
{
    for(int i=0;i<6;i++)
        target[i] = inputstate(i);
}

// differential equation "geodesicFunction" wrapped into boost array for
// solving it with ode
void mafQuadraticSurface::geodesicCurveTrajectory( const state_type &x , state_type &dxdt , double t ) //const
{


    
    //VectorXd res(6);
   // VectorXd y(6);
    
    for(int i=0;i<6;i++)
        y(i) = x[i];

    geodesicFunction(y,res);
    vector2state(res,dxdt);
}



/*
%CALCERROR
% calculates the error between the curve y and the point q.
%
% returns err = [height angle]', where
%  height = b . (q - y*)
%  angle = b . qdot
%  b = ydot* x n
%  n = surface normal at y*
%  y* = closest point to q on curve y
%
% Ian Stavness -- 26/Feb/2012
% Reference: stavness2012-a-general-approach-to-muscle-wrapping.pdf
% Stavness, I., Sherman, M., Delp, S.L.: A general approach to muscle wrapping over multiple
% surfaces, In: Proceedings of the American Society of Biomechanics (2012)

function [err, minidx] = calcError(y,q,qdot,geom)
n = length(y);
ps = y(:,1:3);
dist = sum((ps-repmat(q,1,n)').^2,2);
            
[~, minidx] = min(dist);
            
if (minidx == 1) % min is first point
    i0 = minidx;
    i1 = minidx+1;
elseif (minidx == n) % min is last point
    i0 = minidx-1;
    i1 = minidx;
else % line segments adjacent to min point for shortest distance
 
    pleft = ps(minidx-1,:)';
    pmin = ps(minidx,:)';
    pright = ps(minidx+1,:)';
    dist_L = dist2line(pleft,pmin,q);
    dist_R = dist2line(pmin,pright,q);
    if ( dist_L < dist_R )
        i0 = minidx-1;
        i1 = minidx;
    else
        i0 = minidx;
        i1 = minidx+1;
    end
end
            
            p0 = ps(i0,:)';
            p1 = ps(i1,:)';
            pdot0 = y(i0,4:6)';
            pdot1 = y(i1,4:6)';
            
            [~, pp, s] = dist2line(p0,p1,q);
            
            up = (s<=0)*pdot0 + (s>0&&s<1)*(pdot0 + (pdot1 - pdot0)*s) + (s>=1)*pdot1;
            
            b = cross(qdot,geom.grad(q(1),q(2),q(3)));
            theta = dot(up,b);
            h = dot(pp-q,b);
            
            err = [h; theta];
            
            if minidx > .9*n
            minidx = 1;
            end
            
            end
*/
// trajectory in y -> 1 row per point, every row has x,y,z,x',y',z' -> trajectory is N*6
void mafQuadraticSurface::calcError(const MatrixXd &y,const Vector3d &q,const Vector3d &qdot,int* minIndex, double* errorH, double* errorTheta) //const
{
    int N = y.rows();
    
    // check for every point in the trajectory whether it is closest to q
    int closestIndex = -1;
    double closestDistance = std::numeric_limits<double>::max();
    
    for(int i=0;i<N;i++)
    {
        Vector3d yvector = y.block<1,3>(i,0);
        double d_yq = (yvector - q).squaredNorm();
        if (d_yq < closestDistance)
        {
            closestDistance = d_yq;
            closestIndex = i;
        }
    }
    int i0, i1;
    if (closestIndex == 0)
    {
        i0 = closestIndex, i1 = closestIndex + 1;
    }
    else if(closestIndex == N - 1)
    {
        i0 = closestIndex - 1; i1 = closestIndex;
    }
    else
    {
       // Vector3d pleft = y.row(closestIndex - 1);
       // Vector3d pmin = y.row(closestIndex);
        //Vector3d pright = y.row(closestIndex + 1);
        
        Vector3d pleft = y.block(closestIndex - 1,0,1,3).transpose();
        Vector3d pmin = y.block(closestIndex,0,1,3).transpose();
        Vector3d pright = y.block(closestIndex + 1,0,1,3).transpose(); // TODO we can get rid of the transposes
        double distLeft = dist2line(pleft, pmin,q);
        double distRight = dist2line(pmin,pright,q);
        if(distLeft < distRight)
        {
            i0 = closestIndex - 1; i1 = closestIndex;
        }
        else
        {
            i0 = closestIndex; i1 = closestIndex + 1;
        }
    }
    Vector3d p0 = y.block<1,3>(i0,0);
    Vector3d p1 = y.block<1,3>(i1,0);
    Vector3d pdot0 = y.block<1,3>(i0,3);
    Vector3d pdot1 = y.block<1,3>(i1,3);

    double dist;
    double s;
    Vector3d closestPoint;
    dist2line(p0,p1,q,&dist,&closestPoint,&s);
    Vector3d up = (s<=0) * pdot0 + (s>0&&s<1) * (pdot0 + (pdot1 - pdot0) * s) + (s>=1) * pdot1;
    
    Vector3d b = qdot.cross(grad(q));
    double theta = up.dot(b);
    double h = (closestPoint - q).dot(b);
    
    
    
    if(closestIndex > 0.9 * N)
        closestIndex = 1; // TODO: WHY IS THIS ???
    *errorH = h;
    *errorTheta = theta;
    *minIndex = closestIndex;
}

/*
%GEODERR
% computes the error in the geodesic wrapping path condition for the
% general case of a smooth surface. The geodesic curve from point p is
% computed using numerical integration.
%
% Note: error is calculated using calcError.m
%
% Ian Stavness -- 14/Feb/2012

function err = geoderr_integration(pp,vp,qq,vq,geom)

global geodesic geodesic_plot
global ppts
global tf
global geodlen

global geod_rel_tol
global geod_abs_tol
global geod_pos_stab
global geod_vel_stab

ppts = [];
min_dist = 1e10; % initialize to large distance

% ensure pdot is unit tangent vector at p
p = pp;
pdot = vp/norm(vp);

% ensure qdot is unit tangent vector at q
q = qq;
qdot = vq/norm(vq);

options = odeset('RelTol',geod_rel_tol, 'AbsTol', geod_abs_tol);

[t,y]=ode15s(@geodfun,[0,tf],[p pdot],options,q,qdot,geom,geod_vel_stab,geod_pos_stab);

[err,minidx] = calcError(y,q,qdot,geom);

err=err(:); % To make err a column

geodesic = y;
geodesic_plot = y;
geodlen = t(minidx);

if minidx == length(y),
geodlen = t(1);
end
*/



void mafQuadraticSurface::geoderrIntegration(const Vector3d &pp,const Vector3d &vp,const Vector3d &qq,const Vector3d &vq,int* minIndex, double* errorH, double* errorTheta) //const
{
    Vector3d pdot = vp / vp.norm();
    Vector3d qdot = vq / vq.norm();
    
    state_type p_pdot = { pp(0) , pp(1) , pp(2), pdot(0) ,pdot(1),pdot(2)}; // initial conditions
    
    vector< VectorXd > mstates;
    // TODO lambdas outside the method?
    auto stateEquation = [this] (const state_type x , state_type &dxdt , double t) { geodesicCurveTrajectory(x,dxdt,t); } ;
    auto stateCollector = [&] (const state_type &x , double t)
    {
        VectorXd r(6);
        r << x[0],x[1],x[2],x[3],x[4],x[5];
        mstates.push_back(r);
        
    };
    
    integrate(stateEquation,p_pdot , 0.0 , M_PI/2 , 0.00001 ,stateCollector);
    
    // convert stdvector mstates of triplets into Matrix Nx6
    const int n_cols = 6;
    const int n_rows = mstates.size();
    MatrixXd Y(n_rows,n_cols);
    for(int i=0;i<n_rows;i++)
    {
        Y(i,0) = mstates[i][0];
        Y(i,1) = mstates[i][1];
        Y(i,2) = mstates[i][2];
        Y(i,3) = mstates[i][3];
        Y(i,4) = mstates[i][4];
        Y(i,5) = mstates[i][5];
    }
    
    calcError(Y, qq,qdot, minIndex, errorH, errorTheta);
    
   // err=err(:); % To make err a column
    
    // TODO: WHAT IS THE RELEVANCE OF THIS PIECE OF CODE AT THE END?
    /*
    geodesic = y;
    geodesic_plot = y;
    geodlen = t(minidx);
    
    if minidx == length(y),
        geodlen = t(1);
     */
}

/*
 %WRAPFUN f
 %   function representing the three wrapping conditions a
 %   single wrapping surface:
 %   (1) wrapping points are on surface
 %   (2) joining paths are tangent to the surface
 %   (3) wrapping path is a geodesic curves
 %
 % arguments:
 % p, q -- 3D coordinates of the wrapping points
 % p0, q0 -- 3D coordinates of joining points on the adjacent wrapping surfaces
 % geom -- geometry of the wrapping surface of interest
 % computeJacP -- flag to compute jacobian explicitly
 %
 % return:
 % F -- function value (6x1)
 % J -- function jacobian (6x6)
 %
 %
 % Ian Stavness -- 9/Feb/2012
 
 function [F J] = wrapfun(p,q,p0,q0,geom,computeJacP)
 
 
 global geod_jac_eps;
 
 
 vp = p-p0;
 lp = norm(vp);
 up = vp/lp;
 
 vq = q-q0;
 lq = norm(vq);
 uq = (vq)/lq;
 
 
 np = geom.grad(p(1),p(2),p(3));
 nq = geom.grad(q(1),q(2),q(3));
 
 err = geom.geoderr(p,up,q,uq);
 
 
 F = [  geom.surf(p(1),p(2),p(3));
 geom.surf(q(1),q(2),q(3));
 np*up;
 nq*uq;
 err];
 
 if (computeJacP == 1)
 ngeoderr = size(err,1);
 J = zeros(4+ngeoderr,6);
 J(1,1:3) = np;
 J(2,4:6) = nq;
 Hp = geom.hess(p(1),p(2),p(3));
 Hq = geom.hess(q(1),q(2),q(3));
 
 dudp = (eye(3)-up*up')/lp;
 dudq = (eye(3)-uq*uq')/lq;
 
 J(3,1:3) = up'*Hp + np*dudp;
 J(4,4:6) = uq'*Hq + nq*dudq;
 
 pp=p;
 h0=geod_jac_eps^(1/2);
 for i=1:3
 h1=h0*max(pp(i), 0.1);
 pp(i)=pp(i)+h1;
 upp = (pp-p0)/norm(pp-p0);
 derr = geom.geoderr(pp,upp,q,uq);
 J(5:5+ngeoderr-1,i)=(derr-err)/h1;
 pp(i)=p(i);
 end
 
 qq=q;
 for i=1:3
 h1=h0*max(qq(i), 0.1);
 qq(i)=qq(i)+h1;
 uqq = (qq-q0)/norm(qq-q0);
 derr = geom.geoderr(p,up,qq,uqq);
 J(5:5+ngeoderr-1,3+i)=(derr-err)/h1;
 qq(i)=q(i);
 end
 else
 J = [];
 end
 
 */
void mafQuadraticSurface::wrapfunF(const Vector3d &p, const Vector3d &q, const Vector3d &p0, const Vector3d &q0, VectorXd* F) //const
{
    Vector3d vp = p-p0;
    Vector3d up = vp/vp.norm();
    
    Vector3d vq = q-q0;
    Vector3d uq = vq/vq.norm();
    
    RowVector3d np = grad(p);
    RowVector3d nq = grad(q);
    
    int minindexUnused;
    double errorH;
    double errorTheta;
    
    geoderrIntegration(p,up,q,uq,&minindexUnused, &errorH, &errorTheta);
    
    *F << surf(p), surf(q), np*up, nq*uq, errorH, errorTheta;
}


void mafQuadraticSurface::wrapfunJ(const Vector3d &p, const Vector3d &q, const Vector3d &p0, const Vector3d &q0,MatrixXd* J) //const
{
    Vector3d vp = p-p0;
    double lp = vp.norm();
    Vector3d up = vp/vp.norm();
    
    Vector3d vq = q-q0;
    double lq = vq.norm();
    Vector3d uq = vq/lq;
    
    RowVector3d np = grad(p);
    RowVector3d nq = grad(q);
    
    int minindexUnused;
    double errorH;
    double errorTheta;
    
    geoderrIntegration(p,up,q,uq,&minindexUnused, &errorH, &errorTheta);
    

        (*J)(0,0) = np(0); (*J)(0,1) = np(1); (*J)(0,2) = np(2);
        (*J)(1,3) = nq(0); (*J)(1,4) = nq(1); (*J)(1,5) = nq(2);
        
        Vector3d dnpup = up.transpose() * hess(p) + np * (Matrix3d::Identity() - up * up.transpose()) / lp;
        Vector3d dnquq = uq.transpose() * hess(q) + nq * (Matrix3d::Identity() - uq * uq.transpose()) / lq;
        (*J)(2,0) = dnpup(0); (*J)(2,1) = dnpup(1); (*J)(2,2) = dnpup(2);
        (*J)(3,3) = dnquq(0); (*J)(3,4) = dnquq(1); (*J)(3,5) = dnquq(2);
        
        double geod_jac_eps = 1.e-6;
        Vector3d pp = p;
        double h0 = sqrt(geod_jac_eps);
        
        double h1; int dminIndex; double derrorH; double derrorTheta;
        
        for(int i = 0;i<3;i++)
        {
            h1 = h0 * max(pp(i), 0.1);
            pp(i) = pp(i) + h1;
            Vector3d upp = (pp-p0)/(pp-p0).norm();
            geoderrIntegration(pp,upp,q,uq,&dminIndex,&derrorH,&derrorTheta);
            (*J)(4,i) = (derrorH - errorH) /h1;
            (*J)(5,i) = (derrorTheta - errorTheta)/h1;
            pp(i) = p(i);
        }
        
        Vector3d qq = q;
        for(int i = 0;i<3;i++)
        {
            h1 = h0 * max(qq(i), 0.1);
            qq(i) = qq(i) + h1;
            Vector3d uqq = (qq-q0)/(qq-q0).norm();
            geoderrIntegration(p,up,qq,uqq,&dminIndex,&derrorH,&derrorTheta);
            (*J)(4,i + 3) = (derrorH - errorH) / h1;
            (*J)(5,i + 3) = (derrorTheta - errorTheta) / h1;
            qq(i)=q(i);
        }
    
}
