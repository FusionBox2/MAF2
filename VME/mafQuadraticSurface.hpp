//
//  QuadraticSurface.hpp


#ifndef __mafQuadraticSurface_hpp
#define __mafQuadraticSurface_hpp

#include <iostream>
#include <array>
#include <vector>
#include "mafMathtools.hpp"

#include "mafDataElement.hpp"
#include "mafLandmark.hpp"
#include "mafVMELandmark.h"
#include "../../Eigen/Eigen/Dense"

#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef boost::array< double , 6 > state_type;
//typedef std::array< double, 6 > state_type;


using Eigen::Matrix;
using Eigen::Dynamic;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Matrix4d;
using Eigen::Matrix3d;
using Eigen::Matrix2d;
using Eigen::VectorXd;
using Eigen::Vector3d;
using Eigen::VectorXcd;
using Eigen::RowVector3d;


// TODO should be abstract

class mafQuadraticSurface : public mafVME//public mafDataElement
{
    public:
    
    mafQuadraticSurface();
	virtual ~mafQuadraticSurface();
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);
	static bool VMEAccept(mafNode *node) { return(node != NULL ); };
    std::vector< Vector3d > computeGeodesicPath(mafVMELandmark* p0, mafVMELandmark* q0, mafVMELandmark* p, mafVMELandmark* q, double length, int rate, double *error) ; //const;
    
    void wrapfunF(const Vector3d &p,const Vector3d &q,const Vector3d &p0,const Vector3d &q0,VectorXd* F); //const;
    void wrapfunJ(const Vector3d &p,const Vector3d &q,const Vector3d &p0,const Vector3d &q0,MatrixXd* J); //const;
	void SetMatrix(const mafMatrix &mat){};
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};

	Vector3d center;
	mafString name;

	virtual double getSzX()const { return 0.0; };
	virtual double getSzY()const { return 0.0; };
	virtual double getSzZ()const { return 0.0; };
	virtual double* GetCenter()const { return NULL; };
	virtual double surf(const Vector3d &point) const { return 0.0; };
	
	private:
    void geodesicCurveTrajectory( const state_type &x , state_type &dxdt , double t ) ;//const;
    
    virtual RowVector3d grad(const Vector3d &point) const {return RowVector3d();};
    virtual Matrix3d hess(const Vector3d &point) const {return Matrix3d();};
    
    void geodesicFunction(const VectorXd &y, VectorXd& result) const;
   
    void calcError(const MatrixXd &y,const Vector3d &q,const Vector3d &qdot,int* minIndex, double* errorH, double* errorTheta); //const;
    void geoderrIntegration(const Vector3d& pp,const Vector3d& vp,const Vector3d& qq,const Vector3d& vq,int* minIndex, double* errorH, double* errorTheta); //const;
	
    // temp variables
    VectorXd y;
    VectorXd res;
    
    const double alpha = 1;
    const double beta = 2;
	
};


#endif /* QuadraticSurface_hpp */