/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: medVMEStentModelSource.cpp,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.7 $
Authors:   Hui Wei
==========================================================================
Copyright (c) 2013
University of Bedfordshire, UK
=========================================================================*/


#include "vtkMEDStentModelSource.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkMEDMatrixVectorMath.h"
#include "vtkMEDPolyDataNavigator.h"

#include <fstream>


/************************************************************************/
/* construct method
give some init parameters to create stent
*/
/************************************************************************/
vtkMEDStentModelSource::vtkMEDStentModelSource(void)
  : m_CenterLine(NULL), m_NumberOfPointsAllocatedToCenterLine(0)
{
  m_StentDiameter = 1.0;
  m_CrownLength = 2.2;
  m_LinkLength = 0.8;
  m_strutsNumber = 16; //how many points in one circle
  m_CrownNumber = 10;
  m_LinkNumber = 4;
  m_LinkConnection = peak2peak;
  m_StentConfiguration = OutOfPhase;
  m_LinkOrientation = None;
  m_LinkAlignment =3;
  m_LinkInterlace = 0;
  m_CenterLine = NULL;
  m_InitialStentLength = 0;
  m_SimplexMesh = SimplexMeshType::New(); // smart pointer - don't try to delete
}



/************************************************************************/
/* deconstruct method                                                   */
/************************************************************************/
vtkMEDStentModelSource::~vtkMEDStentModelSource(void)
{
  if (m_CenterLine != NULL)
    delete [] m_CenterLine;
}




/**
* setters
*/
/* set diameter */
void vtkMEDStentModelSource::setStentDiameter(double value){
  m_StentDiameter = value;
}

/* set crown length */
void vtkMEDStentModelSource::setCrownLength(double value){
  m_CrownLength = value;
}

/* set struct angle and calculate strut length */
void vtkMEDStentModelSource::setStrutAngle(double theta){
  m_StrutAngle = theta ;
  m_StrutLength = 0.99 * m_CrownLength / cos(m_StrutAngle/2.0) ;
}

/* set link length */
void vtkMEDStentModelSource::setLinkLength(double value){
  m_LinkLength = value;
}

/* set struts number */
void vtkMEDStentModelSource::setStrutsNumber(int value){
  m_strutsNumber = value;
}

/* set link connection {peak2valley, valley2peak, peak2peak, valley2valley}*/
void vtkMEDStentModelSource::setLinkConnection(LinkConnectionType value){
  m_LinkConnection = value;
}

/* set link alignment {-1,0,+1}*/
void vtkMEDStentModelSource::setLinkAlignment(unsigned int value){
  m_LinkAlignment = value;
}

/* set stent configuration {InPhase, OutOfPhase}*/
void vtkMEDStentModelSource::setStentConfiguration(StentConfigurationType value){
  m_StentConfiguration = value;
}

/* set link orientation {InPhase, OutOfPhase}*/
void vtkMEDStentModelSource::setLinkOrientation(LinkOrientationType value){
  m_LinkOrientation = value;
}

/* set crown number, default 10*/
void vtkMEDStentModelSource::setCrownNumber(int value){
  m_CrownNumber = value;
}

/* set link number*/
void vtkMEDStentModelSource::setLinkNumber(int value){
  m_LinkNumber = value;
}

void vtkMEDStentModelSource::setLinkInterlace(int value){
  m_LinkInterlace = value;
}



void vtkMEDStentModelSource::setStentType(int value){
  m_StentType = value;
}



void vtkMEDStentModelSource::setCenterLineFromPolyData(vtkPolyData *line){
  std::vector<std::vector<double> > stentCenterLine; // actually just an array of points x[n][3]
  m_CenterLineLength = 0;

  //------------to get start and end------------
  std::vector<double> vertex;
  vtkIdType numOfCells = line->GetNumberOfLines();
  vtkPoints *linePoints = line->GetPoints();
  if(numOfCells > 0){
    vtkPoints *linePoints = line->GetPoints();
    line->GetLines()->InitTraversal();

    vtkIdType numPts = 0;
#if VTK_MAJOR_VERSION > 8
    const vtkIdType* pts = NULL;
#else
    vtkIdType *pts = NULL;
#endif
    double aVertex[3];
    while(line->GetLines()->GetNextCell(numPts,pts)){
      for(vtkIdType i = 0; i < numPts; i++){
        linePoints->GetPoint(pts[i], aVertex);
        //-----for deformation---
        vertex.push_back(aVertex[0]);
        vertex.push_back(aVertex[1]);
        vertex.push_back(aVertex[2]);
        stentCenterLine.push_back(vertex);
        vertex.clear();
      }	
    }

    std::vector<std::vector<double> >::const_iterator centerLineStart = stentCenterLine.begin();
    std::vector<std::vector<double> >::const_iterator centerLineEnd = stentCenterLine.end();
    if(getStentType()==1 && getInphaseShort()==1){
      this->setCenterLineForInphaseShort(centerLineStart, centerLineEnd,0);//last param is useless
    }
    else if(getStentType()==2){
      this->setCenterLineForBardHelical(centerLineStart,centerLineEnd);
    }
    else{
      this->setCenterLine(centerLineStart, centerLineEnd,0);//last param is useless
    }
  }

  std::cout << "hello\n" ;
}



/************************************************************************/
/* set center line
calculate the centers for sampling circles.
these centres are located on the vessel centre line 
with intervals successively equal to crown length and length between crowns.
*/
/************************************************************************/ 
void vtkMEDStentModelSource::setCenterLine(std::vector<std::vector<double> >::const_iterator centerLineStart,
  std::vector<std::vector<double> >::const_iterator centerLineEnd,int pointNumber){

    /**
    * calculate the distance between crowns
    */
    double distanceBetweenCrown = calculateDistanceBetweenCrown();
    //int maxNumber = computeCrownNumberAfterSetCenterLine(); //can be compute after set centerline polydata
    //if(m_CrownNumber>maxNumber){
    //	m_CrownNumber = maxNumber;
    //}
    //---------------------

    /** To adapt different configuration of stent, 
    * the control mesh have two more circles of vertices in each side than the stent.*/
    m_NumCenterVertex = 2*(m_CrownNumber + 2);
    AllocateCentreLine(m_NumCenterVertex) ;
    int *centerIdx = new int [m_NumCenterVertex];

    std::vector<std::vector<double> >::const_iterator iterCur = centerLineStart;
    std::vector<std::vector<double> >::const_iterator iterNext = iterCur+1;
    double direc[3];
    direc[0] = (*iterCur)[0] - (*iterNext)[0];
    direc[1] = (*iterCur)[1] - (*iterNext)[1];
    direc[2] = (*iterCur)[2] - (*iterNext)[2];
    double magnitude = sqrt(direc[0]*direc[0]+direc[1]*direc[1]+direc[2]*direc[2]);
    /**first two centers, for the extra circle for simplex mesh, no stent strut will locate on it*/
    m_CenterLine[0][0] = (*iterCur)[0] + (m_CrownLength+distanceBetweenCrown)*direc[0]/magnitude;
    m_CenterLine[0][1] = (*iterCur)[1] + (m_CrownLength+distanceBetweenCrown)*direc[1]/magnitude;
    m_CenterLine[0][2] = (*iterCur)[2] + (m_CrownLength+distanceBetweenCrown)*direc[2]/magnitude;
    centerIdx[0] = -1;
    m_CenterLine[1][0] = (*iterCur)[0] + distanceBetweenCrown*direc[0]/magnitude;
    m_CenterLine[1][1] = (*iterCur)[1] + distanceBetweenCrown*direc[1]/magnitude;
    m_CenterLine[1][2] = (*iterCur)[2] + distanceBetweenCrown*direc[2]/magnitude;
    centerIdx[1] = -1;

    /**here is where stent model start,  where the stent is located */
    m_CenterLine[2][0] = (*iterCur)[0];
    m_CenterLine[2][1] = (*iterCur)[1];
    m_CenterLine[2][2] = (*iterCur)[2];	
    centerIdx[2] = 0;

    int idx = 1;
    int moveFlag = 0;
    double dis,left[3],right[3],distance;
    for(int i = 3; i<m_NumCenterVertex; i++){
      if(i%2==0) 
        distance = distanceBetweenCrown;
      else
        distance = m_CrownLength;
      while(true){

        if (iterNext != centerLineEnd)
        {
          right[0] = (*iterNext)[0];
          right[1] = (*iterNext)[1];
          right[2] = (*iterNext)[2];
          dis = sqrt((m_CenterLine[i-1][0]-right[0])*(m_CenterLine[i-1][0]-right[0])
            +(m_CenterLine[i-1][1]-right[1])*(m_CenterLine[i-1][1]-right[1])
            +(m_CenterLine[i-1][2]-right[2])*(m_CenterLine[i-1][2]-right[2]));
          if(dis > distance)
            break;
          else{

            iterCur++;
            iterNext++;
            idx++;
            moveFlag=1;
          }
        }else{
          break;
        }
      }
      if (moveFlag==1)
      {
        left[0] = (*iterCur)[0];
        left[1] = (*iterCur)[1];
        left[2] = (*iterCur)[2];
      }else{
        left[0] = m_CenterLine[i-1][0];
        left[1] = m_CenterLine[i-1][1];
        left[2] = m_CenterLine[i-1][2];
      }

      bool ok = calculateSamplingPoint2(m_CenterLine[i-1], m_CenterLine[i], distance, left, right);
      assert(ok) ;
      moveFlag=0;
      centerIdx[i] = idx;
    }

    centerLocationIndex.clear();
    // std::vector<int>().swap(centerLocationIndex);
    /**
    * the beginning and the last circle of vertices on the simplex mesh will only have m_strutsNumber size
    * not m_strutsNumber * 2, to fit the simplex mesh structure constraint.
    * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
    * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
    */
    int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);
    for(int i = 0; i < m_strutsNumber; i++){
      centerLocationIndex.push_back(centerIdx[0]);	
    }
    for(int i = m_strutsNumber; i < m_strutsNumber*2;i++){
      centerLocationIndex.push_back(centerIdx[m_NumCenterVertex-1]);
    }
    for(int i = 2*m_strutsNumber; i < pointListSize; i++){
      centerLocationIndex.push_back(centerIdx[i/(2*m_strutsNumber)]);
    }

    /** calculate initial stent length*/
    m_InitialStentLength = m_CrownNumber*m_CrownLength
      + (m_CrownNumber-1)*distanceBetweenCrown;

    delete [] centerIdx ;
}



//------------------------------------------------------------------------------
// Set the stent center line
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::setCenterLineForBardHelical(
  std::vector<std::vector<double> >::const_iterator centerLineStart,
  std::vector<std::vector<double> >::const_iterator centerLineEnd
  )
{
  /**
  * calculate the distance between crowns
  */
  //double distanceBetweenCrown = calculateDistanceBetweenCrown();
  //int maxNumber = computeCrownNumberAfterSetCenterLine(); //can be compute after set centerline polydata
  //if(m_CrownNumber>maxNumber){
  //	m_CrownNumber = maxNumber;
  //}
  //---------------------

  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.*/
  m_NumCenterVertex = 2*(m_CrownNumber + 2)+2;
  AllocateCentreLine(m_NumCenterVertex) ;
  int *centerIdx = new int [m_NumCenterVertex];

  double Dc = m_CrownLength;//0.5;//m_CrownLength;
  double Dgap = m_LinkLength; //the parameter we need to know 0.05
  double pitch = m_CrownLength*1.0;//1.3;//0.8; // the parameter we need to know
  double Dsprh = pitch -Dgap;
  double Ds = Dc+Dsprh;//for second crown

  double distanceBetweenCrown = Dgap;
  std::vector<std::vector<double> >::const_iterator iterCur = centerLineStart;
  std::vector<std::vector<double> >::const_iterator iterNext = iterCur+1;
  double direc[3];
  direc[0] = (*iterCur)[0] - (*iterNext)[0];
  direc[1] = (*iterCur)[1] - (*iterNext)[1];
  direc[2] = (*iterCur)[2] - (*iterNext)[2];
  double magnitude = sqrt(direc[0]*direc[0]+direc[1]*direc[1]+direc[2]*direc[2]);
  /**first two centers, for the extra circle for simplex mesh, no stent strut will locate on it*/
  m_CenterLine[0][0] = (*iterCur)[0] + (Dc+distanceBetweenCrown)*direc[0]/magnitude;
  m_CenterLine[0][1] = (*iterCur)[1] + (Dc+distanceBetweenCrown)*direc[1]/magnitude;
  m_CenterLine[0][2] = (*iterCur)[2] + (Dc+distanceBetweenCrown)*direc[2]/magnitude;
  centerIdx[0] = -1;
  m_CenterLine[1][0] = (*iterCur)[0] + distanceBetweenCrown*direc[0]/magnitude;
  m_CenterLine[1][1] = (*iterCur)[1] + distanceBetweenCrown*direc[1]/magnitude;
  m_CenterLine[1][2] = (*iterCur)[2] + distanceBetweenCrown*direc[2]/magnitude;
  centerIdx[1] = -1;

  /**here is where stent model start,  where the stent is located */
  m_CenterLine[2][0] = (*iterCur)[0];
  m_CenterLine[2][1] = (*iterCur)[1];
  m_CenterLine[2][2] = (*iterCur)[2];	
  centerIdx[2] = 0;

  int idx = 1;
  int moveFlag = 0;
  double dis,left[3],right[3],distance;

  for(int i = 3; i<m_NumCenterVertex; i++){
    if(i==3 || i==5 || i== m_NumCenterVertex-3 || i==m_NumCenterVertex-5){
      distance = Dc;
    }
    else if(i%2==1){//odd
      distance = Dsprh;
    }
    else{//odd
      distance = Dgap;
    }
    while(true){
      if (iterNext != centerLineEnd){
        right[0] = (*iterNext)[0];
        right[1] = (*iterNext)[1];
        right[2] = (*iterNext)[2];
        dis = sqrt((m_CenterLine[i-1][0]-right[0])*(m_CenterLine[i-1][0]-right[0])
          +(m_CenterLine[i-1][1]-right[1])*(m_CenterLine[i-1][1]-right[1])
          +(m_CenterLine[i-1][2]-right[2])*(m_CenterLine[i-1][2]-right[2]));
        if(dis > distance)
          break;
        else{
          iterCur++;
          iterNext++;
          idx++;
          moveFlag=1;
        }
      }
      else{
        break;
      }
    }

    if (moveFlag==1){
      left[0] = (*iterCur)[0];
      left[1] = (*iterCur)[1];
      left[2] = (*iterCur)[2];
    }
    else{
      left[0] = m_CenterLine[i-1][0];
      left[1] = m_CenterLine[i-1][1];
      left[2] = m_CenterLine[i-1][2];
    }

    bool ok = calculateSamplingPoint2(m_CenterLine[i-1], m_CenterLine[i], distance, left, right);
    assert(ok) ;
    moveFlag=0;
    centerIdx[i] = idx;
  }

  centerLocationIndex.clear();
  /**
  * the beginning and the last circle of vertices on the simplex mesh will only have m_strutsNumber size
  * not m_strutsNumber * 2, to fit the simplex mesh structure constraint.
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  */
  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1)+2;
  for(int i = 0; i < m_strutsNumber; i++){
    centerLocationIndex.push_back(centerIdx[0]);	
  }
  for(int i = m_strutsNumber; i < m_strutsNumber*2;i++){
    centerLocationIndex.push_back(centerIdx[m_NumCenterVertex-1]);
  }
  for(int i = 2*m_strutsNumber; i < pointListSize; i++){
    centerLocationIndex.push_back(centerIdx[i/(2*m_strutsNumber)]);
  }

  /** calculate initial stent length*/
  m_InitialStentLength = m_CrownNumber*m_CrownLength
    + (m_CrownNumber-1)*distanceBetweenCrown;

  delete [] centerIdx ;
}



//------------------------------------------------------------------------------
// Set the stent center line
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::setCenterLineForInphaseShort(
  std::vector<std::vector<double> >::const_iterator centerLineStart,
  std::vector<std::vector<double> >::const_iterator centerLineEnd,
  int pointNumber
  )
{
  // calculate the distance between crowns

  //       ____________________     
  //                  /|\      
  //       __________/_|_\_______top    
  //                /  |  \   
  //   ____________/___|___\_______bottom
  //              /   / \   \

  int overLayNumber;                                                       
  double topLength,bottomLength;                                          

  overLayNumber = floor(m_CrownLength/m_LinkLength);                          
  topLength =   m_CrownLength - m_LinkLength*overLayNumber;          
  bottomLength = m_LinkLength-topLength;                          


  // To adapt different configuration of stent, 
  // the control mesh have two more circles of vertices in each side than the stent.
  m_NumCenterVertex = 2*(overLayNumber + m_CrownNumber+2)+1;
  AllocateCentreLine(m_NumCenterVertex) ;
  int *centerIdx = new int [m_NumCenterVertex];

  std::vector<std::vector<double> >::const_iterator iterCur = centerLineStart;
  std::vector<std::vector<double> >::const_iterator iterNext = iterCur+1;
  double direc[3];
  direc[0] = (*iterNext)[0] - (*iterCur)[0];
  direc[1] = (*iterNext)[1] - (*iterCur)[1];
  direc[2] = (*iterNext)[2] - (*iterCur)[2];
  double magnitude = sqrt(direc[0]*direc[0]+direc[1]*direc[1]+direc[2]*direc[2]);
  for (int j = 0 ;  j < 3 ;  j++)
    direc[j] /= magnitude ;

  /**first two centers, for the extra circle for simplex mesh, no stent strut will locate on it*/
  m_CenterLine[0][0] = (*iterCur)[0] - (bottomLength+topLength)*direc[0];
  m_CenterLine[0][1] = (*iterCur)[1] - (bottomLength+topLength)*direc[1];
  m_CenterLine[0][2] = (*iterCur)[2] - (bottomLength+topLength)*direc[2];
  centerIdx[0] = -1;
  m_CenterLine[1][0] = (*iterCur)[0] - bottomLength*direc[0];
  m_CenterLine[1][1] = (*iterCur)[1] - bottomLength*direc[1];
  m_CenterLine[1][2] = (*iterCur)[2] - bottomLength*direc[2];
  centerIdx[1] = -1;

  // here is where stent model start,  where the stent is located
  m_CenterLine[2][0] = (*iterCur)[0];
  m_CenterLine[2][1] = (*iterCur)[1];
  m_CenterLine[2][2] = (*iterCur)[2];	
  centerIdx[2] = 0;


  int idx = 1;
  double dis,left[3],right[3],distance;
  for(int i = 3; i<m_NumCenterVertex; i++){
    if(i%2==0) 
      distance = bottomLength; //bottomLength;//distanceBetweenCrown;
    else
      distance = topLength; //topLength//crownlength

    while(true){
      if (iterNext != centerLineEnd){
        right[0] = (*iterNext)[0];
        right[1] = (*iterNext)[1];
        right[2] = (*iterNext)[2];
        dis = sqrt((m_CenterLine[i-1][0]-right[0])*(m_CenterLine[i-1][0]-right[0])
          +(m_CenterLine[i-1][1]-right[1])*(m_CenterLine[i-1][1]-right[1])
          +(m_CenterLine[i-1][2]-right[2])*(m_CenterLine[i-1][2]-right[2]));
        if(dis > distance)
          break;
        else{
          iterCur++;
          iterNext++;
          idx++;
        }
      }
      else
        break;
    }

    left[0] = (*iterCur)[0];
    left[1] = (*iterCur)[1];
    left[2] = (*iterCur)[2];
    bool ok = calculateSamplingPoint2(m_CenterLine[i-1], m_CenterLine[i], distance, left, right);
    assert(ok) ;
    centerIdx[i] = idx;
  }

  centerLocationIndex.clear();
  // std::vector<int>().swap(centerLocationIndex);

  /**
  * the beginning and the last circle of vertices on the simplex mesh will only have m_strutsNumber size
  * not m_strutsNumber * 2, to fit the simplex mesh structure constraint.
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  */
  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);
  for(int i = 0; i < m_strutsNumber; i++){
    centerLocationIndex.push_back(centerIdx[0]);	
  }
  for(int i = m_strutsNumber; i < m_strutsNumber*2;i++){
    centerLocationIndex.push_back(centerIdx[m_NumCenterVertex-1]);
  }
  for(int i = 2*m_strutsNumber; i < pointListSize; i++){
    int idxTmp=	centerIdx[i/(2*m_strutsNumber)];
    centerLocationIndex.push_back(idxTmp);
  }

  // calculate initial stent length
  m_InitialStentLength = (m_CrownNumber-1)*m_LinkLength + m_CrownLength;

  // measure length from end to end
  // n.b. there seems to be an extra center line point, hence n-4, not n-3.
  double dx = m_CenterLine[m_NumCenterVertex-4][0] - m_CenterLine[2][0] ;
  double dy = m_CenterLine[m_NumCenterVertex-4][1] - m_CenterLine[2][1] ;
  double dz = m_CenterLine[m_NumCenterVertex-4][2] - m_CenterLine[2][2] ;
  double actualLength = sqrt(dx*dx + dy*dy + dz*dz) ;

  delete [] centerIdx ;
}




/*-------------------------
* getters
*----------------------*/
/** get struct length */
double vtkMEDStentModelSource::getStrutLength(){
  m_StrutLength = 0.99 * m_CrownLength/cos(m_StrutAngle/2.0);
  return m_StrutLength;
}
/** get link length */
double vtkMEDStentModelSource::getLinkLength(){
  return m_LinkLength;
}
/** get radius */
double vtkMEDStentModelSource::getRadius(){
  return m_StentDiameter/2;
}
/** get Initial Stent Length */
double vtkMEDStentModelSource::getInitialStentLength(){
  return m_InitialStentLength;
}
/** get Crown Number */
int vtkMEDStentModelSource::getCrownNumber(){
  return m_CrownNumber;
}
/** get Struts Number */
int vtkMEDStentModelSource::getStrutsNumber(){
  return m_strutsNumber;
}

int vtkMEDStentModelSource::getLinkInterlace(){
  return m_LinkInterlace;
}
//if inphase and m_LinkLength<m_CrownLength , dynamic stent structure is created
int vtkMEDStentModelSource::getInphaseShort(){
  int rtn = 0;
  if(m_StentConfiguration==InPhase && m_LinkLength<m_CrownLength && (m_LinkOrientation == None || m_LinkOrientation ==-1))
    rtn = 1;
  return rtn;
}
int vtkMEDStentModelSource::getStentType(){
  return m_StentType;
}
/**
*-----------these method is created for inPhase m_LinkLength < m_CrownLength
* separate from createStent
*/



//------------------------------------------------------------------------------
// Beds original simple stent
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::createStent(){
  init();
  createStentSimplexMesh();
  createStruts();
  createLinks();
}



//------------------------------------------------------------------------------
// Abbott inphase, short links
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::createStentInphaseShort(){
  std::ofstream thing ;
  thing.open("thing_centerline.csv", thing.out) ;
  for (int i = 0 ;  i < m_NumCenterVertex ;  i++){
    thing << i << ", " ;
    thing << m_CenterLine[i][0] << ", " ;
    thing << m_CenterLine[i][1] << ", " ;
    thing << m_CenterLine[i][2] << "\n" ;
  }
  thing.close() ;

  initShort();
  createStentSimplexMesh();
  createStrutsShort();
  createLinksShort();
}



//------------------------------------------------------------------------------
// No use !
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::createStentBard(){
  initBard();
  createBardStentSimplexMesh();
  createBardStruts();
  createLinks();
}


//------------------------------------------------------------------------------
// Bard Helical stent
//------------------------------------------------------------------------------
void vtkMEDStentModelSource::createStentBardHelical(){
  initBardHelical();
  createBardHelicalStentSimplexMesh2();
  createBardStruts2();
  createBardLinks();
}



/************************************************************************/
/* helix formular:  x=r*cos(t),y=r*sin(t),z=b*t, where 2*pi*b is pitch
/* bard helical defined as follows:
/*                   *-*__*-/__/-/__/-/__/-/__/-*__*-*
/*  the distance between *-* is Dc : a crown length
/*  the distance between /-/ is Dh : H-l  (H is a pitch of helical ,l is link length of longitudinal direction)
/*  the distance between __  is Dp : l
/*  the distance between *-/ is Ds : Dc+Dh for second crown.
/*  crown number is 4+whole number of sprial  
/* centerline was create like that : (Dc)-(Dp)-(Dc+Dh)-(Dp)-(Dh)-(Dp)-(Dh)...(Dc+Dh)-(Dp)-(Dc)
/************************************************************************/
void vtkMEDStentModelSource::initBardHelical(){

  m_StrutLength = 0.99 * m_CrownLength/cos(m_StrutAngle/2.0);

  double Dc = m_CrownLength;// 0.5;//m_CrownLength;
  double Dgap = m_LinkLength*1.0;//0.1; //the parameter we need to know 0.05
  double pitch =m_CrownLength*1.0;// 0.8; // the parameter we need to know
  double Dsprh = pitch -Dgap;
  double Ds = Dc+Dsprh;//for second crown


  //calculate crownNumber
  if(m_Stent_Length>0.0){ //Dc*3 + n*(Dgap+Dsprh) + Dc*3= stent_length; n= (stent_length -6*Dc)/Dpitch
    m_CrownNumber = floor( (m_Stent_Length)/pitch); //floor( (m_Stent_Length-6*Dc)/pitch);
  }

  m_NumCrownSimplex = m_CrownNumber + 2;


  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.
  */
  m_NumCenterVertex = 2*(m_CrownNumber+2)+2; // the +2 is for end helical connect	
  if (m_CenterLine != NULL){
    AllocateCentreLine(m_NumCenterVertex) ;
    return;
  }
  else
    AllocateCentreLine(m_NumCenterVertex) ;

  /*---------------------------
  * if no m_CenterLine is set, then create a default straight m_CenterLine
  * the default: startPosition(0,0,0), direction(1,0,0)
  *------------------------------------*/
  double startPosition[3], direction[3];
  startPosition[0] = 0.0;
  startPosition[1] = 0.0;
  startPosition[2] = 0.0;
  direction[0] = 1.0;
  direction[1] = 0.0;
  direction[2] = 0.0;


  double position = -1*(Dc/2 + Dgap);

  for(int i=0;i<m_NumCenterVertex;i++){
    m_CenterLine[i][0] = startPosition[0] + position*direction[0];
    m_CenterLine[i][1] = startPosition[1] + position*direction[1];
    m_CenterLine[i][2] = startPosition[2] + position*direction[2];

    if(i==0 ||i==2 ||i==4 || i== m_NumCenterVertex-2||i==m_NumCenterVertex-4||i==m_NumCenterVertex-6){//first and last mesh ,first and last struts
      position +=Dc;
      /*if(i==0){
      position +=Dc/2;
      }else{
      position +=Dc;
      }*/

    }else if(i==1 || i==3|| i== m_NumCenterVertex-3|| i== m_NumCenterVertex-5){//first and last link mesh
      position +=Dgap;
    }
    else if(i%2==0){
      position += Dsprh;
    }else{
      position += Dgap;
    }
    if(i>0){
      //mafLogMessage("position+=: %f",m_CenterLine[i][0]-m_CenterLine[i-1][0] );
    }
    //mafLogMessage("m_CenterLine[ %i]: %f %f %f",i,m_CenterLine[i][0], m_CenterLine[i][1], m_CenterLine[i][2] );

  }

  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);
  for(int i = 0; i < pointListSize; i++){
    centerLocationIndex.push_back(-1);
  }
}



void vtkMEDStentModelSource::createBardHelicalStentSimplexMesh2(){
  m_SimplexMesh->Initialize() ;
  m_SimplexMesh->GetGeometryData()->Initialize() ;
  m_SimplexMesh->SetLastCellId(0) ;

  /*used for visualize m_SimplexMesh*/
  Strut theMesh;
  //----------------------------------------------------
  //-----sample vertices of simplex mesh for stent--------
  //------------------------------------------------------
  int i,j;
  double normalCircle[3];//,bardNormalCircle[3];	

  double theta = vtkMath::Pi()/m_strutsNumber;  //Pi/m_strutsNumber;
  //double offTheta = vtkMath::Pi()/36; //plane offset
  double offCircleTheta = 0;//-vtkMath::Pi()/120;//angle between points from one circle to next
  double currentTheta;
  double pitch = m_CrownLength*1.0;//0.8;
  double b = pitch/(2*vtkMath::Pi());

  //the number of the beginning and last circle is m_strutsNumber，others m_strutsNumber*2
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int tmpIdx;

  /*
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  * other circles of vertices follows successively, n-th circle occupies from n*m_strutsNumber ~ (n+1)*m_strutsNumber-1
  * here,in this comment, n is calulated from 0
  * the +2 vertexes are for helical last ring to connect with
  */
  //-first ring and last ring share 0-strutnumber*2
  int m_SampleArraySize = sampleNumberPerCircle * (m_NumCenterVertex-1-2) +2;//sampleNumberPerCircle * (m_NumCenterVertex-1);//sampleNumberPerCircle * (m_NumCenterVertex-1-2) +2; //note +2 is for helical last ring ,-2 is for 2empty helical
  double (*m_SampleArray)[3] = new double[m_SampleArraySize][3];

  double UVector[3], VVector[3] ;

  //------------create vertices first------------	
  for(i=0;i<m_NumCenterVertex;i++){//loop one by one ring
    if(i%2==0){
      normalCircle[0] = m_CenterLine[i+1][0] - m_CenterLine[i][0];
      normalCircle[1] = m_CenterLine[i+1][1] - m_CenterLine[i][1];
      normalCircle[2] = m_CenterLine[i+1][2] - m_CenterLine[i][2];
    }
    getUVVector( normalCircle,UVector, VVector);
    vtkMath::Normalize(normalCircle);
    //getBardNormalCircle(m_CenterLine[i],m_CenterLine[i+1],bardNormalCircle,offTheta);
    //getUVVector( bardNormalCircle,UVector1, VVector1);

    //i=0
    if(i==0){//vertex num 0 (first crown of cL vertex) first mesh crown
      for(int n=0;n<m_strutsNumber;n++){//the beginning(0) circle 				
        m_SampleArray[n][0] = m_CenterLine[i][0] + cos(theta*n*2)*UVector[0] - sin(theta*n*2)*VVector[0];
        m_SampleArray[n][1] = m_CenterLine[i][1] + cos(theta*n*2)*UVector[1] - sin(theta*n*2)*VVector[1];
        m_SampleArray[n][2] = m_CenterLine[i][2] + cos(theta*n*2)*UVector[2] - sin(theta*n*2)*VVector[2];
      }
    }//i=1-4, 21-24
    else if( (i>=1 && i<=4) || (i>=m_NumCenterVertex-5 && i<=m_NumCenterVertex-2 )){//circle rings//tmp -7 should -5
      for(int n = 0;n<sampleNumberPerCircle;n++){
        currentTheta = -theta*n+offCircleTheta*i;
        tmpIdx = i*sampleNumberPerCircle+n;
        if(i>=m_NumCenterVertex-5 && i<=m_NumCenterVertex-2 ){
          tmpIdx =  (i-2)*sampleNumberPerCircle+n+2;
          //tmpIdx =  (i-2)*sampleNumberPerCircle+n;
        }
        m_SampleArray[tmpIdx][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[tmpIdx][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[tmpIdx][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
    }//5-18
    else if (i>=5 && i<=m_NumCenterVertex-8){//helical
      //17,18 idx from to 607 then add 608 609
      /*for(int n = 0;n<sampleNumberPerCircle;n++){
      currentTheta = -theta*n+offCircleTheta*i;
      m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*n)*pitch*normalCircle[0]/vtkMath::DoublePi()/2;
      m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*n)*pitch*normalCircle[1]/vtkMath::DoublePi()/2;
      m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*n)*pitch*normalCircle[2]/vtkMath::DoublePi()/2;

      }*/
      if(i==m_NumCenterVertex-9||i==m_NumCenterVertex-8){//last two helicals add one more points each
        for(int n = 0;n<sampleNumberPerCircle+1;n++){
          currentTheta = -theta*n+offCircleTheta*i;
          tmpIdx = i*sampleNumberPerCircle+n;
          if(i==m_NumCenterVertex-8){
            tmpIdx = i*sampleNumberPerCircle+n+1;
          }
          m_SampleArray[tmpIdx][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*n)*pitch*normalCircle[0]/vtkMath::Pi()/2;
          m_SampleArray[tmpIdx][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*n)*pitch*normalCircle[1]/vtkMath::Pi()/2;
          m_SampleArray[tmpIdx][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*n)*pitch*normalCircle[2]/vtkMath::Pi()/2;

        }
      }else{
        for(int n = 0;n<sampleNumberPerCircle;n++){
          currentTheta = -theta*n+offCircleTheta*i;
          m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*n)*pitch*normalCircle[0]/vtkMath::Pi()/2;
          m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*n)*pitch*normalCircle[1]/vtkMath::Pi()/2;
          m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*n)*pitch*normalCircle[2]/vtkMath::Pi()/2;

        }
      }
    }//25
    else if(i ==m_NumCenterVertex-1){

      for(int n=0;n<m_strutsNumber;n++){//the last circle
        currentTheta =  -theta*n*2+offCircleTheta*i;//-theta*n*2
        m_SampleArray[m_strutsNumber+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[m_strutsNumber+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[m_strutsNumber+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
    }

  } //end of for i
  for(int k=0;k<m_SampleArraySize;k++){
    //mafLogMessage("m_SampleArray[%i]=[ %f,%f,%f ]",k,m_SampleArray[k][0],m_SampleArray[k][1],m_SampleArray[k][2]);
  }
  /** Add our vertices to the simplex mesh.*/
  PointType point;
  for(i=0; i < m_SampleArraySize ; ++i)
  {
    point[0] = m_SampleArray[i][0];
    point[1] = m_SampleArray[i][1];
    point[2] = m_SampleArray[i][2];
    m_SimplexMesh->SetPoint(i, point);
    m_SimplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );
  }
  delete[] m_SampleArray;

  /** Specify the method used for allocating cells */
  m_SimplexMesh->SetCellsAllocationMethod( SimplexMeshType::CellsAllocatedDynamicallyCellByCell );

  /** AddEdge,AddNeighbor,and add the symmetric relationships */
  //-----------------------------------------------------------------------------------
  /* pay attention to the order of adding
  *  the three vertices should be commected anticlockwise
  *  to make sure the normal calculate on the surface of simplex mesh is pointing outwards
  *-------------------------*/
  /* for(int k=0;k<m_SampleArraySize-1;k++){// ---------||
  m_SimplexMesh->AddEdge(k,k+1);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =k ;  //for visualize mesh      
  theMesh.endVertex = k+1; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  }*/
  //------------Add Edge
  int tmpIdx1,tmpIdx2,tmpIdx3,tmpIdx4;
  //--------------------------- ------------ ------------ ------------ ------------    1. the beginning circle --idx 0 ring  -----------------------------
  //vertical
  for(j=0;j<m_strutsNumber-1;j++){// ---------||
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex = j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,0); 
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j ;  //for visualize mesh      
  theMesh.endVertex =0; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  //horizontal 
  for(j=0;j<m_strutsNumber;j++){//-------------====
    m_SimplexMesh->AddEdge(j,sampleNumberPerCircle+j*2);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex =sampleNumberPerCircle+j*2; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  // ----------------------------------------- ----------- ----------------------3.the last circle -- --------------------------------------
  //vertical 
  for(j=m_strutsNumber;j<2*m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j;  //for visualize mesh      
    theMesh.endVertex =j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,m_strutsNumber);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j;  //for visualize mesh      
  theMesh.endVertex =m_strutsNumber; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  // -- horizontal 
  i=m_NumCenterVertex-2-2;
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j*2+2,m_strutsNumber+j);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j*2+2;  //for visualize mesh      
    theMesh.endVertex =m_strutsNumber+j; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }

  /**-------------- ------------ ------------ ------------ ------------ ------------  2.the middle circles and helicals -- */
  ///1-4 rings from begin
  for(i=1;i<5;i++){  
    for(j=0;j<sampleNumberPerCircle-1;j++){

      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle); //j,0
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
    theMesh.endVertex =i*sampleNumberPerCircle; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
  }
  //last 4 rings of circle from end
  for(i=m_NumCenterVertex-5-2;i<m_NumCenterVertex-1-2;i++){  
    for(j=0;j<sampleNumberPerCircle-1;j++){

      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+2,i*sampleNumberPerCircle+j+1+2);//j,j+1
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j+2 ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle+j+1+2; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+2,i*sampleNumberPerCircle+2); //j,0
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j+2 ;  //for visualize mesh      
    theMesh.endVertex =i*sampleNumberPerCircle+2; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
  }
  //helical
  for(i=5;i<=m_NumCenterVertex-8;i++){
    //middle helicals:
    if(i<=m_NumCenterVertex-10){
      for(j=0;j<sampleNumberPerCircle-1;j++){
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);//j,j+1
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
      }

      //interface 
      if(i==m_NumCenterVertex-10){
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle+1); //j,0
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =(i+2)*sampleNumberPerCircle+1; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh

      }else{
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle); //j,0
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =(i+2)*sampleNumberPerCircle; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
      }
    }else if(i==m_NumCenterVertex-9){//17
      for(j=0;j<sampleNumberPerCircle;j++){
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);//j,j+1
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
      }
    }else if(i==m_NumCenterVertex-8 ){//18
      for(j=0;j<sampleNumberPerCircle;j++){
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j+1+1);//j,j+1
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j+1 ;  //for visualize mesh      
        theMesh.endVertex =i*sampleNumberPerCircle+j+1+1; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
      }
    }
    //----------------Add neighbors for a vertex------------

  }
  //----------horizontal-----------------------
  for(i=1;i<m_NumCenterVertex-2;i++){
    if(i%2==0){//even long link
      if(i<m_NumCenterVertex-8){
        for(j=0;j<sampleNumberPerCircle;j=j+2){
          m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =i*sampleNumberPerCircle+j;  //for visualize mesh      
          theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------
        }
      }
      else if(i==m_NumCenterVertex-8){//18

        for(j=0;j<sampleNumberPerCircle;j=j+2){
          if(j==0){//interface of 18
            //m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+1,(i+1)*sampleNumberPerCircle+j+1);
            tmpIdx3 = i*sampleNumberPerCircle+j;
            m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+1,tmpIdx3); //neighber points;first 18 and last 17
            //---------------for visualize mesh start---------------
            theMesh.startVertex =i*sampleNumberPerCircle+j+1;  //for visualize mesh      
            theMesh.endVertex =tmpIdx3; //for visualize mesh 
            m_MeshList.push_back(theMesh); //for visualize mesh
            //---------------for visualize mesh end---------------
          } else{
            m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j+1,(m_NumCenterVertex-5-2)*sampleNumberPerCircle+j+2);

            //---------------for visualize mesh start---------------
            theMesh.startVertex =i*sampleNumberPerCircle+j+1;  //for visualize mesh      
            theMesh.endVertex =(m_NumCenterVertex-5-2)*sampleNumberPerCircle+j+2; //for visualize mesh 
            m_MeshList.push_back(theMesh); //for visualize mesh 
            //---------------for visualize mesh end---------------
          }
        }
        //last point of 18 connect with first of 21
        tmpIdx4 = i*sampleNumberPerCircle+j+1;
        m_SimplexMesh->AddEdge(tmpIdx4,(m_NumCenterVertex-5-2)*sampleNumberPerCircle+2);
        //---------------for visualize mesh start---------------
        theMesh.startVertex =tmpIdx4;  //for visualize mesh      
        theMesh.endVertex =(m_NumCenterVertex-5-2)*sampleNumberPerCircle+2; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
        //connect end of 17 and 18

        m_SimplexMesh->AddEdge(tmpIdx3,tmpIdx4);
        //---------------for visualize mesh start---------------
        theMesh.startVertex =tmpIdx3;  //for visualize mesh      
        theMesh.endVertex =tmpIdx4; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
      }else if(i>=m_NumCenterVertex-4){//>20 last rings ,leave 2 rings
        for(j=0;j<sampleNumberPerCircle;j=j+2){
          m_SimplexMesh->AddEdge((i-2)*sampleNumberPerCircle+j+2,(i+1-2)*sampleNumberPerCircle+j+2);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =(i-2)*sampleNumberPerCircle+j+2;  //for visualize mesh      
          theMesh.endVertex =(i+1-2)*sampleNumberPerCircle+j+2; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------

        }
      }
    }else{//odd gap link 1,3,5..

      if(i<m_NumCenterVertex-9){//17	 
        if(i==5){
          tmpIdx1 = i*sampleNumberPerCircle;
          tmpIdx2 = (i+1)*sampleNumberPerCircle;
          m_SimplexMesh->AddEdge(tmpIdx1,tmpIdx2);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =tmpIdx1;  //for visualize mesh      
          theMesh.endVertex =tmpIdx2; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------
        }
        for(j=1;j<sampleNumberPerCircle;j=j+2){
          m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
          theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------
        }
      }else if(i==m_NumCenterVertex-9){//17
        for(j=1;j<sampleNumberPerCircle;j=j+2){
          m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j+1);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
          theMesh.endVertex =(i+1)*sampleNumberPerCircle+j+1; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------
        }
      }else if(i>= m_NumCenterVertex-5){//over 19
        for(j=1;j<sampleNumberPerCircle;j=j+2){
          m_SimplexMesh->AddEdge((i-2)*sampleNumberPerCircle+j+2,(i+1-2)*sampleNumberPerCircle+j+2);
          //---------------for visualize mesh start---------------
          theMesh.startVertex =(i-2)*sampleNumberPerCircle+j+2;  //for visualize mesh      
          theMesh.endVertex =(i+1-2)*sampleNumberPerCircle+j+2; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
          //---------------for visualize mesh end---------------
        }
      }

    }

  }//ADD EDGE FINISH

  //------------------------------**AddNeighbor-----------------
  //**  the beginning circle 
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(j,j-1);
    m_SimplexMesh->AddNeighbor(j,j+1);
    m_SimplexMesh->AddNeighbor(j,sampleNumberPerCircle+j*2);

  }
  m_SimplexMesh->AddNeighbor(0,m_strutsNumber-1);
  m_SimplexMesh->AddNeighbor(0,1);
  m_SimplexMesh->AddNeighbor(0,sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,m_strutsNumber-2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,0);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,sampleNumberPerCircle+(m_strutsNumber-1)*2);
  //-------------i=1
  i=1;  
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){//-----------ODD

    NeighborLRDown(i,sampleNumberPerCircle,j);

  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);

  for(j=2;j<sampleNumberPerCircle-1;j=j+2){//-----------EVEN
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,j/2);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle, 0);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);

  //-------------
  for(i=2;i<=m_NumCenterVertex-3;i++){
    if(i==2||i==4){
      for(j=2;j<sampleNumberPerCircle-1;j=j+2){       //even                                            //-1 -- -- +1 |

        NeighborLRDown(i,sampleNumberPerCircle,j);
      }
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);//left//interface 
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);//right
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);//down
      for(j=1;j<sampleNumberPerCircle-2;j=j+2){//odd //|+1__-1__

        NeighborUpRL(i,sampleNumberPerCircle,j);
      }
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    }//end of i==2
    else if(i==3){
      for(j=1;j<sampleNumberPerCircle-2;j=j+2){//-1 -- -- +1 |

        NeighborLRDown(i,sampleNumberPerCircle,j);
      } 
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//L
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);//R
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);//down
      for(j=2;j<sampleNumberPerCircle-1;j=j+2){//|+1__-1__
        NeighborUpRL(i,sampleNumberPerCircle,j);
      }
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);//up
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);//R
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);//L

    }else if(i>=5 && i<=m_NumCenterVertex-5-2-1){ //helical 5-18

      if(i%2==0){//6,8,10,12,14,16,18 even 
        for (j=1;j<sampleNumberPerCircle-2;j=j+2){//|+1__-1__ up
          if(i==m_NumCenterVertex-5-2-1){//18
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,(i-1)*sampleNumberPerCircle+j);//up
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j+1);//right
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j+1-1);//left
          }else{
            NeighborUpRL(i, sampleNumberPerCircle, j);
          }
        }
        //31
        if(i==m_NumCenterVertex-5-2-1){//18
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,tmpIdx3-1);//up
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,tmpIdx4);//Right with 18
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j);//left//interface 
          //node tmpIdx4
          m_SimplexMesh->AddNeighbor(tmpIdx4,i*sampleNumberPerCircle+j+1);//left 18_31
          m_SimplexMesh->AddNeighbor(tmpIdx4,tmpIdx3);//up
          m_SimplexMesh->AddNeighbor(tmpIdx4,(i+1)*sampleNumberPerCircle+2);//down

        }else if(i==m_NumCenterVertex-5-2-1-2){//16*
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);//up
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle+1);//Right with 18
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//left//interface 

        }else{
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);//up
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle);//Right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//left//interface 
        }

        for (j=2;j<sampleNumberPerCircle-1;j=j+2){//-1 -- -- +1 | down
          //NeighborLRDown(i, sampleNumberPerCircle, j);*
          if(i==m_NumCenterVertex-5-2-1){//18
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j-1+1);//left
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,i*sampleNumberPerCircle+j+1+1);//right
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j+1,(i+1)*sampleNumberPerCircle+j+1+1);//down
          }else{
            NeighborLRDown(i, sampleNumberPerCircle, j);
          }
        }
        //j=0
        if(i==6){//interface special link j=0
          m_SimplexMesh->AddNeighbor(tmpIdx2,tmpIdx2+1);
          m_SimplexMesh->AddNeighbor(tmpIdx2,(i+1)*sampleNumberPerCircle);
          m_SimplexMesh->AddNeighbor(tmpIdx2,tmpIdx1);

        }else if(i==m_NumCenterVertex-5-2-1){//18
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+1,(i-1)*sampleNumberPerCircle-1);//left//interface 
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+1,i*sampleNumberPerCircle+1+1);//right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+1,i*sampleNumberPerCircle);//down

        }else{//
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle-1);//left//interface 
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);//right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);//down
        }

      }else{//5,7 odd

        for (j=1;j<sampleNumberPerCircle-2;j=j+2){// odd j -1 -- -- +1 | down
          if(i==m_NumCenterVertex-5-2-2){//17
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//left
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);//right
            m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j+1);//down
          }
          NeighborLRDown(i, sampleNumberPerCircle, j);
        }
        //j=31
        if(i==m_NumCenterVertex-5-2-2){//17*
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//left//interface 
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,tmpIdx3);//right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,tmpIdx4-1);//down

          //node tmpIdx3
          m_SimplexMesh->AddNeighbor(tmpIdx3,tmpIdx3-1);//i*sampleNumberPerCircle+j);//left tmpIdx3-left
          m_SimplexMesh->AddNeighbor(tmpIdx3,(i+1)*sampleNumberPerCircle+1);//up tmpIdx3-18_0
          m_SimplexMesh->AddNeighbor(tmpIdx3,tmpIdx4);//down tmpIdx3,tmpIdx4

        }else{
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);//left//interface 
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle);//right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);//down
        }

        for (j=2;j<sampleNumberPerCircle-1;j=j+2){//|+1__-1__ up
          NeighborUpRL(i, sampleNumberPerCircle, j);
        }
        // j=0
        if(i==5){
          m_SimplexMesh->AddNeighbor(tmpIdx1,(i-1)*sampleNumberPerCircle);
          m_SimplexMesh->AddNeighbor(tmpIdx1,tmpIdx1+1);
          m_SimplexMesh->AddNeighbor(tmpIdx1,tmpIdx2);

        }//else if(i==m_NumCenterVertex-5-2-2){//17
        //}
        else{//7
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);//up
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);//Right
          m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle-1);//left//interface 
        }
      }
    }//end of helical
    else if(i==m_NumCenterVertex-5){
      for(j=1;j<sampleNumberPerCircle-2;j=j+2){//1,3,5..29odd
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2+1)*sampleNumberPerCircle+j+2);//down
      }
      //31
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+2);//right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2+1)*sampleNumberPerCircle+j+2);//down


      for(j=2;j<sampleNumberPerCircle-1;j=j+2){
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+1);//up
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//Right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left//interface 
      }
      //0

      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,tmpIdx4);//up
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2)*sampleNumberPerCircle+2+1);//Right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2+1)*sampleNumberPerCircle+2-1);//left//interface 


    } else if(i==m_NumCenterVertex-4){
      for(j=1;j<sampleNumberPerCircle-2;j=j+2){//1,3,5..29odd
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+2);//up
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//Right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left//interface 
      }
      //31
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+2);//up
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+2);//Right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left//interface 

      for(j=2;j<sampleNumberPerCircle-1;j=j+2){
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2+1)*sampleNumberPerCircle+j+2);//down
      }
      //0
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2+1)*sampleNumberPerCircle+2-1);//left
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2)*sampleNumberPerCircle+2+1);//right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2+1)*sampleNumberPerCircle+2);//down

    }else if(i==m_NumCenterVertex-3){
      for(j=1;j<sampleNumberPerCircle-2;j=j+2){//1,3,5..29odd
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2+1)*sampleNumberPerCircle+j+2);//down
      }
      //31
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+2);//right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2+1)*sampleNumberPerCircle+j+2);//down

      for(j=2;j<sampleNumberPerCircle-1;j=j+2){
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+2);//up
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//Right
        m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left//interface 
      }	 
      //0
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2-1)*sampleNumberPerCircle+2);//up
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2)*sampleNumberPerCircle+2+1);//Right
      m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2+1)*sampleNumberPerCircle+2-1);//left//interface 

    }

  }//end of for i m_NumCenterVertex-3

  i=m_NumCenterVertex-2;//second last
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+2);//up
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//right
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
  }
  //31
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2-1)*sampleNumberPerCircle+j+2);//up
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+2);//right
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2-1);//left
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,(i-2)*sampleNumberPerCircle+j+2+1);//right
    m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+j+2,m_strutsNumber+j/2);//down
  }
  //0
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2+1)*sampleNumberPerCircle+2-1);//left
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,(i-2)*sampleNumberPerCircle+2+1);//right
  m_SimplexMesh->AddNeighbor((i-2)*sampleNumberPerCircle+2,m_strutsNumber);//down
  //last
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,(i-2)*sampleNumberPerCircle+j*2+2);//up
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j+1);//right
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);//left
  }
  //31,0
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,(i-2)*sampleNumberPerCircle+j*2+2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,(i-2)*sampleNumberPerCircle+2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,m_strutsNumber+1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,sampleNumberPerCircle-1);
  //add neighbor finish
  //------------------------------------
  /** AddFace */
  OutputCellAutoPointer m_NewSimplexCellPointer;
  /** the biginning circle : cells */
  for(j=0;j<m_strutsNumber-1;j++){
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, j);
    m_NewSimplexCellPointer->SetPointId(1, j+1);
    m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle+j*2+2);
    m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
    m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, j);
  m_NewSimplexCellPointer->SetPointId(1, 0);
  m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  //--------test face----------
  long faceSize = m_SimplexMesh->GetNumberOfCells();
  //m_SimplexMesh->se
  //------------test neighbors
  typedef SimplexMeshType::NeighborListType NeighbosListType;
  typedef SimplexMeshType::IndexArray IndexArray;

  //NeighbosListType* neighbors = NULL;
  int pSize = m_SimplexMesh->GetPoints()->Size();
  for(unsigned int pointIndex = 0;pointIndex<pSize ;pointIndex++){
    IndexArray neighbors = m_SimplexMesh->GetNeighbors(pointIndex);

    for(int l=0;l<3;l++){
      /*			  
      theMesh.startVertex =pointIndex;  //for visualize mesh      
      theMesh.endVertex =neighbors[0]; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      theMesh.startVertex =pointIndex;  //for visualize mesh      
      theMesh.endVertex =neighbors[1]; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      theMesh.startVertex =pointIndex;  //for visualize mesh      
      theMesh.endVertex =neighbors[2]; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      */
    }

  }//end of for

}



/************************************************************************/
/* create a simplex mesh for Bard stent       
/* helix:  x=r*cos(t),y=r*sin(t),z=b*t, where 2*pi*b is pitch
/* pitch set to 0.8 
/************************************************************************/
void vtkMEDStentModelSource::createBardHelicalStentSimplexMesh(){
  m_SimplexMesh->Initialize() ;
  m_SimplexMesh->GetGeometryData()->Initialize() ;
  m_SimplexMesh->SetLastCellId(0) ;

  /*used for visualize m_SimplexMesh*/
  Strut theMesh;

  //----------------------------------------------------
  //-----sample vertices of simplex mesh for stent--------
  //------------------------------------------------------
  int i,j;
  double normalCircle[3];//,bardNormalCircle[3];	

  double theta =2* vtkMath::Pi()/m_strutsNumber;  //Pi/m_strutsNumber;
  //double offTheta = vtkMath::Pi()/36; //plane offset
  double offCircleTheta = 0;//-vtkMath::Pi()/60;//angle between points from one circle to next
  double currentTheta;
  double pitch = 0.8;
  double b = pitch/(2*vtkMath::Pi());

  //the number of the beginning and last circle is m_strutsNumber，others m_strutsNumber*2
  int sampleNumberPerCircle = 2*m_strutsNumber;

  /*
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  * other circles of vertices follows successively, n-th circle occupies from n*m_strutsNumber ~ (n+1)*m_strutsNumber-1
  * here,in this comment, n is calulated from 0
  * the +2 vertexes are for helical last ring to connect with
  */
  int m_SampleArraySize = sampleNumberPerCircle * (m_NumCenterVertex-1) +2; //note +2 is for helical last ring
  double (*m_SampleArray)[3] = new double[m_SampleArraySize][3];

  double UVector[3], VVector[3] ;

  for(i=0;i<m_NumCenterVertex;i=i+2){

    normalCircle[0] = m_CenterLine[i+1][0] - m_CenterLine[i][0];
    normalCircle[1] = m_CenterLine[i+1][1] - m_CenterLine[i][1];
    normalCircle[2] = m_CenterLine[i+1][2] - m_CenterLine[i][2];


    getUVVector( normalCircle,UVector, VVector);
    vtkMath::Normalize(normalCircle);
    //getBardNormalCircle(m_CenterLine[i],m_CenterLine[i+1],bardNormalCircle,offTheta);
    //getUVVector( bardNormalCircle,UVector1, VVector1);



    //mafLogMessage("Normal: %f %f %f",normalCircle[0], normalCircle[1], normalCircle[2] );
    //mafLogMessage("UVector: %f %f %f",UVector[0], UVector[1], UVector[2] );
    //mafLogMessage("VVector: %f %f %f",VVector[0], VVector[1], VVector[2] );

    if(i==0){//vertex num 0 (first crown of cL vertex) first mesh crown
      for(int n=0;n<m_strutsNumber;n++){//the beginning(0) circle 
        m_SampleArray[n][0] = m_CenterLine[i][0] + cos(-theta*n*2)*UVector[0] + sin(-theta*n*2)*VVector[0];
        m_SampleArray[n][1] = m_CenterLine[i][1] + cos(-theta*n*2)*UVector[1] + sin(-theta*n*2)*VVector[1];
        m_SampleArray[n][2] = m_CenterLine[i][2] + cos(-theta*n*2)*UVector[2] + sin(-theta*n*2)*VVector[2];
      }//vertex num 1 (1)
      for(int n=0;n<sampleNumberPerCircle;n++){//the first circle
        m_SampleArray[sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(-theta*n)*UVector[0] + sin(-theta*n)*VVector[0];
        m_SampleArray[sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(-theta*n)*UVector[1] + sin(-theta*n)*VVector[1];
        m_SampleArray[sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(-theta*n)*UVector[2] + sin(-theta*n)*VVector[2];
      }
      continue;
    }
    if(i == m_NumCenterVertex-2){//last crown of Cl vetex last crown of mesh

      for(int n = 0;n<sampleNumberPerCircle;n++){//the second last circle
        currentTheta =  -theta*n+offCircleTheta*i; //currentTheta
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }

      for(int n=0;n<m_strutsNumber;n++){//the last circle
        currentTheta =  -theta*n*2+offCircleTheta*(i+1);//-theta*n*2
        m_SampleArray[m_strutsNumber+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[m_strutsNumber+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[m_strutsNumber+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
      continue;
    }
    //add two extra points on last helical to close the mesh
    if(i==m_NumCenterVertex-6){//helical start
      m_SampleArray[m_SampleArraySize-2][0] = m_CenterLine[m_NumCenterVertex-6][0] + UVector[0];
      m_SampleArray[m_SampleArraySize-2][1] = m_CenterLine[m_NumCenterVertex-6][1] + UVector[1];
      m_SampleArray[m_SampleArraySize-2][2] = m_CenterLine[m_NumCenterVertex-6][2] + UVector[2];


    }if(i==m_NumCenterVertex-8){//helical end
      m_SampleArray[m_SampleArraySize-1][0] = m_CenterLine[m_NumCenterVertex-7][0] + UVector[0];
      m_SampleArray[m_SampleArraySize-1][1] = m_CenterLine[m_NumCenterVertex-7][1] + UVector[1];
      m_SampleArray[m_SampleArraySize-1][2] = m_CenterLine[m_NumCenterVertex-7][2] + UVector[2];

    }



    //middle crowns:
    for(int n = 0;n<sampleNumberPerCircle;n++){//middle crowns where stent on
      currentTheta = -theta*n;
      if (i>1)//twink from centerline vertex 2,(0and1 will keep) ||-|/-//-// 
      {
        currentTheta += offCircleTheta*i;
      }
      //mafLogMessage("Normal: %f %f %f",normalCircle[0], normalCircle[1], normalCircle[2] );
      //--------start of each crown ----------
      if (i>=6 && i<=m_NumCenterVertex-8 )//helical start
      {
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*n)*pitch*normalCircle[0]/vtkMath::Pi()/2;
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*n)*pitch*normalCircle[1]/vtkMath::Pi()/2;
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*n)*pitch*normalCircle[2]/vtkMath::Pi()/2;

        /*if(i==m_NumCenterVertex-6 && n==sampleNumberPerCircle-1){////add last point for  -6
        //m_NumCenterVertex-6
        //m_SampleArray[m_SampleArraySize-1][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*(n+1))*pitch*normalCircle[0]/vtkMath::DoublePi()/2;
        //m_SampleArray[m_SampleArraySize-1][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*(n+1))*pitch*normalCircle[1]/vtkMath::DoublePi()/2;
        //m_SampleArray[m_SampleArraySize-1][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*(n+1))*pitch*normalCircle[2]/vtkMath::DoublePi()/2;
        m_SampleArray[m_SampleArraySize-1][0] = m_CenterLine[i+2][0] + UVector[0];
        m_SampleArray[m_SampleArraySize-1][1] = m_CenterLine[i+2][1] + UVector[1];
        m_SampleArray[m_SampleArraySize-1][2] = m_CenterLine[i+2][2] + UVector[2];

        }*/

        //mafLogMessage("STEP: %i %f %f %f %f",n,theta*n,  (theta*n)*pitch*normalCircle[0]/vtkMath::DoublePi(),  (theta*n)*pitch*normalCircle[1]/vtkMath::DoublePi(), (theta*n)*pitch*normalCircle[2]/vtkMath::DoublePi() );

      }else if(i!= m_NumCenterVertex-6){//circle start
        //crown start
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }

      //---------- end of each crown
      /*m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      */
      //crown end
      if(i>=4 && i<=m_NumCenterVertex-10){//helical end
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0]+ ( theta*n)*pitch*normalCircle[0]/vtkMath::Pi()/2;
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1]+ ( theta*n)*pitch*normalCircle[1]/vtkMath::Pi()/2;
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2]+ ( theta*n)*pitch*normalCircle[2]/vtkMath::Pi()/2;


      }else if(i!=m_NumCenterVertex-8 ){//circle end
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
    }//end of for n



  }//end of for(i=0;i<m_NumCenterVertex;i=i+2) 




  //--------------------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------------------
  /** Add our vertices to the simplex mesh.*/
  PointType point;
  for(i=0; i < m_SampleArraySize ; ++i)
  {
    point[0] = m_SampleArray[i][0];
    point[1] = m_SampleArray[i][1];
    point[2] = m_SampleArray[i][2];
    m_SimplexMesh->SetPoint(i, point);
    m_SimplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );
  }
  delete[] m_SampleArray;

  /** Specify the method used for allocating cells */
  m_SimplexMesh->SetCellsAllocationMethod( SimplexMeshType::CellsAllocatedDynamicallyCellByCell );

  /** AddEdge,AddNeighbor,and add the symmetric relationships */
  //-----------------------------------------------------------------------------------
  /* pay attention to the order of adding
  *  the three vertices should be commected anticlockwise
  *  to make sure the normal calculate on the surface of simplex mesh is pointing outwards
  *-------------------------*/

  //-----------------
  /**AddEdge */
  //-------------------------first ring----------------------------
  /**--------------------------- ------------ ------------ ------------ ------------    1. the beginning circle --  -----------------------------*/
  //vertical
  for(j=0;j<m_strutsNumber-1;j++){// ---------||
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex = j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,0); 
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j ;  //for visualize mesh      
  theMesh.endVertex =0; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  //horizontal 
  for(j=0;j<m_strutsNumber;j++){//-------------====
    m_SimplexMesh->AddEdge(j,sampleNumberPerCircle+j*2);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex =sampleNumberPerCircle+j*2; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  /**-------------- ------------ ------------ ------------ ------------ ------------  2.the middle circles and helicals -- */
  //2.1. vertical circle and helical ||
  for(i=1;i<m_NumCenterVertex-1;i++){  
    for(j=0;j<sampleNumberPerCircle-1;j++){

      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
    // m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);//change connect
    //---------------for visualize mesh start---------------
    /* theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
    theMesh.endVertex =i*sampleNumberPerCircle; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh  
    */
    //---------------for visualize mesh end---------------
    // ring interface
    if(i<5 || i>m_NumCenterVertex-6){//circles begin and end 
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle); //connect begin and end for circle
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
    }
    else{//helical interface
      if(i==m_NumCenterVertex-7){//extra second last helical
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,m_SampleArraySize-2);//connect end to second last vertex 
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =m_SampleArraySize-2; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
      }else if(i==m_NumCenterVertex-6){//last helical
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,m_SampleArraySize-1);//connect end to last vertex 
        theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
        theMesh.endVertex =m_SampleArraySize-1; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
      }

      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+2)*sampleNumberPerCircle);//connect end to begin of next ring 
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =(i+2)*sampleNumberPerCircle; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
    }


  }
  // 2.2 horizontal helical===
  //2.2.1 odd number ring
  for(i=1;i<m_NumCenterVertex-1;i=i+2){ 
    for(j=1;j<sampleNumberPerCircle;j=j+2){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
  }
  //2.2.2 even number ring
  for(i=2;i<m_NumCenterVertex-2;i=i+2){// 2.2 horizontal helical
    for(j=0;j<sampleNumberPerCircle;j=j+2){
      if(i==m_NumCenterVertex-8){//helical connect to circle
        if (j>0)
        {
          m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+3)*sampleNumberPerCircle+j);
          theMesh.startVertex =i*sampleNumberPerCircle+j;  //for visualize mesh      
          theMesh.endVertex =(i+3)*sampleNumberPerCircle+j; //for visualize mesh 
          m_MeshList.push_back(theMesh); //for visualize mesh 
        }

      }else{
        m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
        //---------------for visualize mesh start---------------
        theMesh.startVertex =i*sampleNumberPerCircle+j;  //for visualize mesh      
        theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
        m_MeshList.push_back(theMesh); //for visualize mesh 
        //---------------for visualize mesh end---------------
      }

    }
  }
  //connection of double helical begin |==
  m_SimplexMesh->AddEdge(5*sampleNumberPerCircle,6 *sampleNumberPerCircle); //connect begin and end for circle
  theMesh.startVertex =5*sampleNumberPerCircle ;  //for visualize mesh      
  theMesh.endVertex =6*sampleNumberPerCircle; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 


  /** ----------------------------------------- ----------- ----------------------3.the last circle -- ---------------------------------------*/
  //vertical 
  for(j=m_strutsNumber;j<2*m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j;  //for visualize mesh      
    theMesh.endVertex =j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,m_strutsNumber);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j;  //for visualize mesh      
  theMesh.endVertex =m_strutsNumber; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  // -- horizontal 
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j*2,m_strutsNumber+j);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j*2;  //for visualize mesh      
    theMesh.endVertex =m_strutsNumber+j; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }

  //-------------last edge for helical end------------
  m_SimplexMesh->AddEdge(m_SampleArraySize-1,m_SampleArraySize-2);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =m_SampleArraySize-2;  //for visualize mesh      
  theMesh.endVertex =m_SampleArraySize-1; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 

  //extra edge -|-
  m_SimplexMesh->AddEdge((m_NumCenterVertex-8)*sampleNumberPerCircle,m_SampleArraySize-1);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =(m_NumCenterVertex-8)*sampleNumberPerCircle ;  //for visualize mesh      
  theMesh.endVertex =m_SampleArraySize-1; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  m_SimplexMesh->AddEdge((m_NumCenterVertex-5)*sampleNumberPerCircle,m_SampleArraySize-2);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =(m_NumCenterVertex-5)*sampleNumberPerCircle ;  //for visualize mesh      
  theMesh.endVertex =m_SampleArraySize-2; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  m_SimplexMesh->AddEdge(m_SampleArraySize-1,(m_NumCenterVertex-9)*sampleNumberPerCircle+sampleNumberPerCircle-1);
  //---------------for visualize mesh end---------------
  theMesh.startVertex =(m_NumCenterVertex-9)*sampleNumberPerCircle+sampleNumberPerCircle-1 ;  //for visualize mesh      
  theMesh.endVertex =m_SampleArraySize-1; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 

  m_SimplexMesh->AddEdge(m_SampleArraySize-2,(m_NumCenterVertex-8)*sampleNumberPerCircle+sampleNumberPerCircle-1);
  //---------------for visualize mesh end---------------
  theMesh.startVertex =(m_NumCenterVertex-8)*sampleNumberPerCircle+sampleNumberPerCircle-1 ;  //for visualize mesh      
  theMesh.endVertex =m_SampleArraySize-2; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 

  //-------------------------------------------------------------------
  /**AddNeighbor
  *the beginning circle */
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(j,j-1);
    m_SimplexMesh->AddNeighbor(j,j+1);
    m_SimplexMesh->AddNeighbor(j,sampleNumberPerCircle+j*2);
  }
  m_SimplexMesh->AddNeighbor(0,m_strutsNumber-1);
  m_SimplexMesh->AddNeighbor(0,1);
  m_SimplexMesh->AddNeighbor(0,sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,m_strutsNumber-2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,0);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,sampleNumberPerCircle+(m_strutsNumber-1)*2);
  /** the first circle */
  i=1;  
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);

  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,j/2);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle, 0);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  /** the middle circles */
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);

  }
  for(i=3;i<m_NumCenterVertex-2;i=i+2){
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    } 
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);		
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  }

  /** the second last circle */
  i= m_NumCenterVertex-2;
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,m_strutsNumber+j/2);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,m_strutsNumber);
  //the last circle
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j+1);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  }
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,m_strutsNumber+1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,sampleNumberPerCircle-1);

  //------------------------------------
  /** AddFace */
  OutputCellAutoPointer m_NewSimplexCellPointer;
  /** the biginning circle : cells */
  for(j=0;j<m_strutsNumber-1;j++){
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, j);
    m_NewSimplexCellPointer->SetPointId(1, j+1);
    m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle+j*2+2);
    m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
    m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, j);
  m_NewSimplexCellPointer->SetPointId(1, 0);
  m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  /** the last circle : cells */
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber-1;j++){
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j*2+2);
    m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber+j+1);
    m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber);
  m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);

  /** the middle circles : cells */
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=0;j<sampleNumberPerCircle-2;j=j+2){
      m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
      m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
      m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
    }
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
    m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  }

  /** connect struts to struts */
  for(i=1;i<m_NumCenterVertex-1;i=i+2){
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
      m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
      m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
    }
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+1);
    m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+1);
    m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);	
  }



  //  m_SimplexMesh->BuildCellLinks();
}



void vtkMEDStentModelSource::getMiddlePoint(double *start, double *end,double *middle){
  middle[0]=(start[0]+end[0])/2;
  middle[1]=(start[1]+end[1])/2;
  middle[2]=(start[2]+end[2])/2;
}



void vtkMEDStentModelSource::getOffsetVectorWithScale(
  const double *start, const double *end, const double scale, const double *org, double offsetVec[3]) const
{
  offsetVec[0]=org[0]+(end[0]-start[0])*scale;
  offsetVec[1]=org[1]+(end[1]-start[1])*scale;
  offsetVec[2]=org[2]+(end[2]-start[2])*scale;
}



//tindices: 4 index
//void caculateShortStrutsLines(int* tindices, double middlePoints[][3]) {
void vtkMEDStentModelSource::getShortStrutsLines(int* tindices,double middlePoints[][3]){// double (&middlePoints)[4][3]) {
  double leftMiddle[3],rightMiddle[3],start[3],end[3] ;
  start[0]=m_SampleArray[tindices[0]][0];
  start[1]=m_SampleArray[tindices[0]][1];
  start[2]=m_SampleArray[tindices[0]][2];
  end[0]=m_SampleArray[tindices[1]][0];
  end[1]=m_SampleArray[tindices[1]][1];
  end[2]=m_SampleArray[tindices[1]][2];
  getMiddlePoint(start,end,leftMiddle);
  start[0]=m_SampleArray[tindices[2]][0];
  start[1]=m_SampleArray[tindices[2]][1];
  start[2]=m_SampleArray[tindices[2]][2];
  end[0]=m_SampleArray[tindices[3]][0];
  end[1]=m_SampleArray[tindices[3]][1];
  end[2]=m_SampleArray[tindices[3]][2];
  getMiddlePoint(start,end,rightMiddle);
  //-----------
  getOffsetVectorWithScale(leftMiddle, rightMiddle, 0.05, leftMiddle, middlePoints[0]);
  getOffsetVectorWithScale(leftMiddle, rightMiddle, -0.05, leftMiddle, middlePoints[1]);
  getOffsetVectorWithScale(rightMiddle, leftMiddle, 0.05, rightMiddle, middlePoints[2]);
  getOffsetVectorWithScale(rightMiddle, leftMiddle, -0.05, rightMiddle, middlePoints[3]);


}



/************************************************************************/
/* create struts for inphase m_LinkLength < m_CrownLength                                                                     */
/************************************************************************/
void vtkMEDStentModelSource::createStrutsShort(){
  Strut theStrut;
  int i,j,cn;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int m_SampleArraySize = sampleNumberPerCircle * (m_NumCenterVertex-1); 

  int overLayNumber = floor(m_CrownLength/m_LinkLength);
  double topLength=   m_CrownLength - m_LinkLength*overLayNumber;
  double bottomLength = m_LinkLength-topLength;   
  int startIdx,endIdx,startIdx2;
  m_StrutsList.clear();
  /*
  PointType point;
  for(i=0; i < m_SampleArraySize ; ++i)
  {
  point[0] = m_SampleArray[i][0];
  point[1] = m_SampleArray[i][1];
  point[2] = m_SampleArray[i][2];
  m_SimplexMesh->SetPoint(i, point);
  m_SimplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );
  }
  delete[] m_SampleArray;

  */

  /** the even circles, counted from zero */	
  int counter = 0;
  PointType tmpPoint;
  for(j=2,cn=0;cn<m_CrownNumber;j=j+2,cn++){
    for(i=0;i<sampleNumberPerCircle-2;i=i+2){                //"for ever strut \/"
      startIdx = sampleNumberPerCircle*j+i;
      endIdx = sampleNumberPerCircle*(j+overLayNumber*2+1)+i+1;
      startIdx2 = sampleNumberPerCircle*j+i+2;

      theStrut.startVertex = startIdx;       //   "/"  
      theStrut.endVertex = endIdx;
      m_StrutsList.push_back(theStrut);	

      theStrut.startVertex = startIdx2;       //   "/"  
      theStrut.endVertex = endIdx;
      m_StrutsList.push_back(theStrut);	

      /*getMiddlePoint(m_SampleArray[startIdx],m_SampleArray[endIdx],leftMiddle);
      getMiddlePoint(m_SampleArray[startIdx2],m_SampleArray[endIdx],rightMiddle);

      leftPoint = getOffsetVectorWithScale(leftMiddle,rightMiddle,0.1,leftMiddle);
      rightPoint = getOffsetVectorWithScale(leftMiddle,rightMiddle,-0.1,leftMiddle);

      tmpPoint[0] = leftPoint[0];
      tmpPoint[1] = leftPoint[1];
      tmpPoint[2] = leftPoint[2];

      idx1=m_SampleArraySize+counter;

      m_SimplexMesh->SetPoint(idx1,tmpPoint);
      m_SimplexMesh->SetGeometryData(m_SampleArraySize+counter, new SimplexMeshGeometryType );
      counter++;

      tmpPoint[0] = rightPoint[0];
      tmpPoint[1] = rightPoint[1];
      tmpPoint[2] = rightPoint[2];

      idx2=m_SampleArraySize+counter;
      m_SimplexMesh->SetPoint(idx2,tmpPoint);
      m_SimplexMesh->SetGeometryData(m_SampleArraySize+counter, new SimplexMeshGeometryType );
      counter++;

      //-----3 lines----------
      theStrut.startVertex = startIdx;         //"\"
      theStrut.endVertex = idx1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = idx1;         //"\"
      theStrut.endVertex = idx2;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = idx2;         //"\"
      theStrut.endVertex = endIdx;
      m_StrutsList.push_back(theStrut);
      */

      //theStrut.startVertex = startIdx;       //   "/"  
      //theStrut.endVertex = endIdx;

      //m_StrutsList.push_back(theStrut);		
    }
    theStrut.startVertex = sampleNumberPerCircle*j+i;              //last strut of the circle
    theStrut.endVertex = sampleNumberPerCircle*(j+overLayNumber*2+1)+i+1;          //"\"
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*j;               //coneect last strut with first strut
    theStrut.endVertex = sampleNumberPerCircle*(j+overLayNumber*2+1)+i+1;         //   "/"
    m_StrutsList.push_back(theStrut);
  }
}



void vtkMEDStentModelSource::createLinksShort(){
  Strut theLink;
  int i,j,cn;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int interval = (m_strutsNumber / m_LinkNumber)*4;
  int indexFirstLink=0;
  int indexCurrent;
  int overLayNumber = floor(m_CrownLength/m_LinkLength);
  m_LinksList.clear();



  /* other stents

  indexFirstLink = 1;
  for(j=1;j<m_NumCrownSimplex-2;j=j+1+this->getLinkInterlace()){ //links are not connected line by line but 1-2 3-4 instead of 1-2-3-4
  for(i=0;i<m_LinkNumber;i++){
  indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
  theLink.startVertex = sampleNumberPerCircle*(j*2+1) + indexCurrent;
  theLink.endVertex = sampleNumberPerCircle*(j*2+3) + indexCurrent;
  m_LinksList.push_back(theLink);
  }
  indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
  }
  */


  if(m_LinkConnection == peak2valley && (m_LinkOrientation == None||m_LinkOrientation == -1)){ //p2v 

    indexFirstLink = 1;
    for(j=2,cn=0;cn<m_CrownNumber-1;cn++,j=j+2){//+this->getLinkInterlace()*2){ //links are not connected line by line but 1-2 3-4 instead of 1-2-3-4 //this->getLinkInterlace()
      if(j/2 % 2==0){//even
        indexFirstLink = 1;
      }else{//odd
        indexFirstLink = 3;
      }

      for(i=0;i<m_LinkNumber;i++){
        indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
        theLink.startVertex = sampleNumberPerCircle*(j+overLayNumber*2+1) + indexCurrent;
        theLink.endVertex = sampleNumberPerCircle*(j+overLayNumber*2+3) + indexCurrent;
        m_LinksList.push_back(theLink);
      }

    }

    /* indexFirstLink = 1;
    for(j=2,cn=0;cn<m_CrownNumber-1;cn++,j=j+2){//+this->getLinkInterlace()*2){ //links are not connected line by line but 1-2 3-4 instead of 1-2-3-4 //this->getLinkInterlace()
    for(i=0;i<m_LinkNumber;i++){
    indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
    theLink.startVertex = sampleNumberPerCircle*(j+overLayNumber*2+1) + indexCurrent;
    theLink.endVertex = sampleNumberPerCircle*(j+overLayNumber*2+3) + indexCurrent;
    m_LinksList.push_back(theLink);
    }
    indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
    }*/

  }else if(m_LinkConnection == valley2peak && (m_LinkOrientation == None||m_LinkOrientation == -1)){//v2p

    indexFirstLink = 0;
    for(j=1;j<m_NumCrownSimplex-2;j++){
      for(i=0;i<m_LinkNumber;i++){
        indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
        theLink.startVertex = sampleNumberPerCircle*(j*2) + indexCurrent;
        theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent;
        m_LinksList.push_back(theLink);
      }
      indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
    }

  }


}



void vtkMEDStentModelSource::initShort(){
  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.
  */

  int overLayNumber = floor(m_CrownLength/m_LinkLength);
  double topLength=   m_CrownLength - m_LinkLength*overLayNumber;
  double bottomLength = m_LinkLength-topLength;                   
  //calculate crownNumber
  if(m_Stent_Length>0.0){ //top *number+bottom= stent_length
    m_CrownNumber = floor((m_Stent_Length-bottomLength)/m_LinkLength);
  }
  m_NumCrownSimplex = floor(m_CrownLength/m_LinkLength)+ m_CrownNumber + 2;

  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.
  */

  //        ______           
  //          /\    top length  
  //      ___/__\___
  //        /    \
  //       /      \ bottom length
  //     _/________\_______

  m_NumCenterVertex = 2* m_NumCrownSimplex;
  if (m_CenterLine != NULL){
    AllocateCentreLine(m_NumCenterVertex) ;
    return; 
  }
  else
    AllocateCentreLine(m_NumCenterVertex) ;


  //--------------------------                                                        
  // if no m_CenterLine is set, then create a default straight m_CenterLine          
  //the default: startPosition(0,0,0), direction(1,0,0)
  //------------------------------------
  double startPosition[3], direction[3];
  startPosition[0] = 0.0;
  startPosition[1] = 0.0;
  startPosition[2] = 0.0;
  direction[0] = 1.0;
  direction[1] = 0.0;
  direction[2] = 0.0;

  //double distanceBetweenCrown = calculateDistanceBetweenCrown();
  m_InitialStentLength = m_CrownNumber*m_LinkLength+m_CrownLength;

  double position = 0;                               //              

  for(int i=0;i<m_NumCenterVertex;i++){
    m_CenterLine[i][0] = startPosition[0] + position*direction[0];
    m_CenterLine[i][1] = startPosition[1] + position*direction[1];
    m_CenterLine[i][2] = startPosition[2] + position*direction[2];
    if(i%2==0) 
      position += topLength;//bottomLength;
    else
      position += bottomLength;//topLength;
  }

  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);//points on the circle, first and last circle only take half space
  for(int i = 0; i < pointListSize; i++){
    centerLocationIndex.push_back(-1);
  }
}





//no use
void vtkMEDStentModelSource::initBard(){
  m_NumCrownSimplex = m_CrownNumber + 2;
  m_StrutLength = 0.99 * m_CrownLength/cos(m_StrutAngle/2.0);

  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.
  */
  m_NumCenterVertex = 2*(m_CrownNumber + 2);	
  if (m_CenterLine != NULL){
    AllocateCentreLine(m_NumCenterVertex) ;
    return; 
  }
  else
    AllocateCentreLine(m_NumCenterVertex) ;


  /*---------------------------
  * if no m_CenterLine is set, then create a default straight m_CenterLine
  * the default: startPosition(0,0,0), direction(1,0,0)
  *------------------------------------*/
  double startPosition[3], direction[3];
  startPosition[0] = 0.0;
  startPosition[1] = 0.0;
  startPosition[2] = 0.0;
  direction[0] = 1.0;
  direction[1] = 0.0;
  direction[2] = 0.0;

  double position = -1*(m_CrownLength + m_LinkLength);
  for(int i=0;i<m_NumCenterVertex;i++){
    m_CenterLine[i][0] = startPosition[0] + position*direction[0];
    m_CenterLine[i][1] = startPosition[1] + position*direction[1];
    m_CenterLine[i][2] = startPosition[2] + position*direction[2];
    if(i%2==0) {
      position += m_CrownLength;
    }	
    else
      position += m_LinkLength;
  }



  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);
  for(int i = 0; i < pointListSize; i++){
    centerLocationIndex.push_back(-1);
  }
}



//------------------------
/** init basic values
* if there is not a center line ,then create a default straight line
*/
void vtkMEDStentModelSource::init(){

  double distanceBetweenCrown = calculateDistanceBetweenCrown();

  if(m_Stent_Length>0.0){ //Dc*3 + n*(Dgap+Dsprh) + Dc*3= stent_length; n= (stent_length -6*Dc)/Dpitch
    m_CrownNumber = floor( (m_Stent_Length-m_CrownLength)/(m_CrownLength+distanceBetweenCrown))+1;
  }	


  m_NumCrownSimplex = m_CrownNumber + 2;
  m_StrutLength = 0.99 * m_CrownLength/cos(m_StrutAngle/2.0);

  /** To adapt different configuration of stent, 
  * the control mesh have two more circles of vertices in each side than the stent.
  */
  m_NumCenterVertex = 2*(m_CrownNumber + 2);	
  if (m_CenterLine != NULL){
    AllocateCentreLine(m_NumCenterVertex) ;
    return; 
  }
  else
    AllocateCentreLine(m_NumCenterVertex) ;


  /*---------------------------
  * if no m_CenterLine is set, then create a default straight m_CenterLine
  * the default: startPosition(0,0,0), direction(1,0,0)
  *------------------------------------*/
  double startPosition[3], direction[3];
  startPosition[0] = 0.0;
  startPosition[1] = 0.0;
  startPosition[2] = 0.0;
  direction[0] = 1.0;
  direction[1] = 0.0;
  direction[2] = 0.0;


  m_InitialStentLength = m_CrownNumber*m_CrownLength
    + (m_CrownNumber-1)*distanceBetweenCrown;

  double position = -1*(m_CrownLength + distanceBetweenCrown);

  for(int i=0;i<m_NumCenterVertex;i++){
    m_CenterLine[i][0] = startPosition[0] + position*direction[0];
    m_CenterLine[i][1] = startPosition[1] + position*direction[1];
    m_CenterLine[i][2] = startPosition[2] + position*direction[2];
    if(i%2==0) 
      position += m_CrownLength;
    else
      position += distanceBetweenCrown;
  }

  int pointListSize = 2*m_strutsNumber*(m_NumCenterVertex-1);
  for(int i = 0; i < pointListSize; i++){
    centerLocationIndex.push_back(-1);
  }
}



/************************************************************************/
/* create a simplex mesh for Bard stent                                                                     */
/************************************************************************/
void vtkMEDStentModelSource::createBardStentSimplexMesh(){
  m_SimplexMesh->Initialize() ;
  m_SimplexMesh->GetGeometryData()->Initialize() ;
  m_SimplexMesh->SetLastCellId(0) ;

  /*used for visualize m_SimplexMesh*/
  Strut theMesh;

  //----------------------------------------------------
  //-----sample vertices of simplex mesh for stent--------
  //------------------------------------------------------
  int i,j;
  double normalCircle[3],bardNormalCircle[3];	
  double theta = vtkMath::Pi()/m_strutsNumber;  //Pi/m_strutsNumber;
  double offTheta = vtkMath::Pi()/36; //plane offset
  double offCircleTheta = -vtkMath::Pi()/60;//
  double currentTheta;

  //the number of the beginning and last circle is m_strutsNumber，others m_strutsNumber*2
  int sampleNumberPerCircle = 2*m_strutsNumber;

  /*
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  * other circles of vertices follows successively, n-th circle occupies from n*m_strutsNumber ~ (n+1)*m_strutsNumber-1
  * here,in this comment, n is calulated from 0
  */
  int m_SampleArraySize = sampleNumberPerCircle * (m_NumCenterVertex-1); 
  double (*m_SampleArray)[3] = new double[m_SampleArraySize][3];

  for(i=0;i<m_NumCenterVertex;i=i+2){
    normalCircle[0] = m_CenterLine[i+1][0] - m_CenterLine[i][0];
    normalCircle[1] = m_CenterLine[i+1][1] - m_CenterLine[i][1];
    normalCircle[2] = m_CenterLine[i+1][2] - m_CenterLine[i][2];

    double UVector[3], VVector[3],UVector1[3],VVector1[3];
    getUVVector( normalCircle,UVector, VVector);

    getBardNormalCircle(m_CenterLine[i],m_CenterLine[i+1],bardNormalCircle,offTheta);
    getUVVector( bardNormalCircle,UVector1, VVector1);



    //mafLogMessage("Normal: %f %f %f",normalCircle[0], normalCircle[1], normalCircle[2] );
    //mafLogMessage("UVector: %f %f %f",UVector[0], UVector[1], UVector[2] );
    //mafLogMessage("VVector: %f %f %f",VVector[0], VVector[1], VVector[2] );

    if(i==0){//first crown of cL vetex
      for(int n=0;n<m_strutsNumber;n++){//the beginning(0) circle 
        m_SampleArray[n][0] = m_CenterLine[i][0] + cos(theta*n*2)*UVector[0] + sin(theta*n*2)*VVector[0];
        m_SampleArray[n][1] = m_CenterLine[i][1] + cos(theta*n*2)*UVector[1] + sin(theta*n*2)*VVector[1];
        m_SampleArray[n][2] = m_CenterLine[i][2] + cos(theta*n*2)*UVector[2] + sin(theta*n*2)*VVector[2];
      }
      for(int n=0;n<sampleNumberPerCircle;n++){//the first circle
        m_SampleArray[sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
        m_SampleArray[sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
        m_SampleArray[sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
      }
      continue;
    }
    if(i == m_NumCenterVertex-2){//last crown of Cl vetex

      for(int n = 0;n<sampleNumberPerCircle;n++){//the second last circle
        currentTheta =  theta*n+offCircleTheta*i;
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }

      for(int n=0;n<m_strutsNumber;n++){//the last circle
        currentTheta =  theta*n*2+offCircleTheta*(i+1);
        m_SampleArray[m_strutsNumber+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[m_strutsNumber+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[m_strutsNumber+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
      continue;
    }

    for(int n = 0;n<sampleNumberPerCircle;n++){//middle crowns
      currentTheta = theta*n;
      if (i>1)//twink from centerline vertex 2,(0and1 will keep) ||-|/-//-// 
      {
        currentTheta += offCircleTheta*i;
      }
      //--------start----------
      if (i>=6 && i<=m_NumCenterVertex-5)//lean
      {
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector1[0] + sin(currentTheta)*VVector1[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector1[1] + sin(currentTheta)*VVector1[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector1[2] + sin(currentTheta)*VVector1[2];

      }else{
        //crown start
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }

      //----------crown end
      /*m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      */
      //crown end
      if(i>=4 && i<=m_NumCenterVertex-8){//lean
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector1[0] + sin(currentTheta)*VVector1[0];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector1[1] + sin(currentTheta)*VVector1[1];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector1[2] + sin(currentTheta)*VVector1[2];

      }else{
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(currentTheta)*UVector[0] + sin(currentTheta)*VVector[0];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(currentTheta)*UVector[1] + sin(currentTheta)*VVector[1];
        m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(currentTheta)*UVector[2] + sin(currentTheta)*VVector[2];
      }
    }
  }//end of for(i=0;i<m_NumCenterVertex;i=i+2) 

  //--------------------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------------------
  /** Add our vertices to the simplex mesh.*/
  PointType point;
  for(i=0; i < m_SampleArraySize ; ++i)
  {
    point[0] = m_SampleArray[i][0];
    point[1] = m_SampleArray[i][1];
    point[2] = m_SampleArray[i][2];
    m_SimplexMesh->SetPoint(i, point);
    m_SimplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );
  }
  delete[] m_SampleArray;

  /** Specify the method used for allocating cells */
  m_SimplexMesh->SetCellsAllocationMethod( SimplexMeshType::CellsAllocatedDynamicallyCellByCell );

  /** AddEdge,AddNeighbor,and add the symmetric relationships */
  //-----------------------------------------------------------------------------------
  /* pay attention to the order of adding
  *  the three vertices should be commected anticlockwise
  *  to make sure the normal calculate on the surface of simplex mesh is pointing outwards
  *-------------------------*/

  //-----------------
  /**AddEdge */
  /** the beginning circle -- vertical */
  for(j=0;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex = j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,0);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j ;  //for visualize mesh      
  theMesh.endVertex =0; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  /** the beginning circle -- horizontal */
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(j,sampleNumberPerCircle+j*2);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex =sampleNumberPerCircle+j*2; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  /** the middle circles -- vertical */
  for(i=1;i<m_NumCenterVertex-1;i++){
    for(j=0;j<sampleNumberPerCircle-1;j++){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
    theMesh.endVertex =i*sampleNumberPerCircle; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  /** the middle circles -- horizontal */
  for(i=1;i<m_NumCenterVertex-1;i=i+2){
    for(j=1;j<sampleNumberPerCircle;j=j+2){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
  }
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=0;j<sampleNumberPerCircle;j=j+2){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j;  //for visualize mesh      
      theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
  }
  /** the last circle -- vertical */
  for(j=m_strutsNumber;j<2*m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j;  //for visualize mesh      
    theMesh.endVertex =j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,m_strutsNumber);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j;  //for visualize mesh      
  theMesh.endVertex =m_strutsNumber; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  /** the last circle -- horizontal */
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j*2,m_strutsNumber+j);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j*2;  //for visualize mesh      
    theMesh.endVertex =m_strutsNumber+j; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }

  //-------------------------------------------------------------------
  /**AddNeighbor
  *the beginning circle */
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(j,j-1);
    m_SimplexMesh->AddNeighbor(j,j+1);
    m_SimplexMesh->AddNeighbor(j,sampleNumberPerCircle+j*2);
  }
  m_SimplexMesh->AddNeighbor(0,m_strutsNumber-1);
  m_SimplexMesh->AddNeighbor(0,1);
  m_SimplexMesh->AddNeighbor(0,sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,m_strutsNumber-2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,0);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,sampleNumberPerCircle+(m_strutsNumber-1)*2);
  /** the first circle */
  i=1;  
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);

  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,j/2);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle, 0);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  /** the middle circles */
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);

  }
  for(i=3;i<m_NumCenterVertex-2;i=i+2){
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    } 
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);		
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  }

  /** the second last circle */
  i= m_NumCenterVertex-2;
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,m_strutsNumber+j/2);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,m_strutsNumber);
  //the last circle
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j+1);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  }
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,m_strutsNumber+1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,sampleNumberPerCircle-1);

  //------------------------------------
  /** AddFace */
  OutputCellAutoPointer m_NewSimplexCellPointer;
  /** the biginning circle : cells */
  for(j=0;j<m_strutsNumber-1;j++){
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, j);
    m_NewSimplexCellPointer->SetPointId(1, j+1);
    m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle+j*2+2);
    m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
    m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, j);
  m_NewSimplexCellPointer->SetPointId(1, 0);
  m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  /** the last circle : cells */
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber-1;j++){
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j*2+2);
    m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber+j+1);
    m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber);
  m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);

  /** the middle circles : cells */
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=0;j<sampleNumberPerCircle-2;j=j+2){
      m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
      m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
      m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
    }
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
    m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  }

  /** connect struts to struts */
  for(i=1;i<m_NumCenterVertex-1;i=i+2){
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
      m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
      m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
      m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
      m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
    }
    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
    m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
    m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+1);
    m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+1);
    m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle);
    m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddFace(m_NewSimplexCellPointer);	
  }
  //  m_SimplexMesh->BuildCellLinks();
}



/************************************************************************/
/* create a simplex mesh for deformation and vertices of stent                                                                     */
/************************************************************************/
void vtkMEDStentModelSource::createStentSimplexMesh(){
  m_SimplexMesh->Initialize() ;
  m_SimplexMesh->GetGeometryData()->Initialize() ;
  m_SimplexMesh->SetLastCellId(0) ;

  /*used for visualize m_SimplexMesh*/
  Strut theMesh;

  //----------------------------------------------------
  //-----sample vertices of simplex mesh for stent--------
  //------------------------------------------------------
  int i,j;
  double normalCircle[3];	
  double theta = vtkMath::Pi()/m_strutsNumber;  //Pi/m_strutsNumber;

  //the number of the beginning and last circle is m_strutsNumber，others m_strutsNumber*2
  int sampleNumberPerCircle = 2*m_strutsNumber;

  /*
  * notice here, we store the beginning circle of vertices on the simplex mesh from 0 ~ (m_strutsNumber-1)
  * and store the last circle of vertices on the simplex mesh from m_strutsNumber ~ (m_strutsNumber*2-1)
  * other circles of vertices follows successively, n-th circle occupies from n*m_strutsNumber ~ (n+1)*m_strutsNumber-1
  * here,in this comment, n is calculated from 0
  */
  int m_SampleArraySize = sampleNumberPerCircle * (m_NumCenterVertex-1); 
  m_SampleArray = new double[m_SampleArraySize][3];

  for(i=0;i<m_NumCenterVertex;i=i+2){
    normalCircle[0] = m_CenterLine[i+1][0] - m_CenterLine[i][0];
    normalCircle[1] = m_CenterLine[i+1][1] - m_CenterLine[i][1];
    normalCircle[2] = m_CenterLine[i+1][2] - m_CenterLine[i][2];
    //vtkMath::Normalize(normalCircle);
    double UVector[3], VVector[3],magnitude;
    /*
    * calculate the outer product of normalCircle with (0,1,0) to get a vector perpendicular to normalCircle
    * here (0,1,0) is a random vector chosed, any vertor which is not paralle works
    * might have little bug here, possibly the normalCircle is paralle to (0,1,0), 
    * in this extreme case, need choose another vector to replace (0,1,0)
    * BETTER ADD A SIMPLE TEST HERE TO AVOID THIS!!!
    */
    UVector[0] = -1.0*normalCircle[2];//0;
    UVector[1] = 0;//n2;
    UVector[2] = normalCircle[0];//-n1
    if((fabs(UVector[0])+ fabs(UVector[1]) + fabs(UVector[2])) < 0.05){
      UVector[0] = normalCircle[1];
      UVector[1] = -normalCircle[0];
      UVector[2] = 0;
      //mafLogMessage("set to zero");
    } 
    magnitude = sqrt(UVector[0]*UVector[0]+UVector[1]*UVector[1] + UVector[2]*UVector[2]);
    UVector[0] *= (m_StentDiameter/2.0/magnitude);
    UVector[1] *= (m_StentDiameter/2.0/magnitude);
    UVector[2] *= (m_StentDiameter/2.0/magnitude);
    //outer product of normalCircle & UVector
    VVector[0] = normalCircle[1]*UVector[2]-normalCircle[2]*UVector[1];
    VVector[1] = normalCircle[2]*UVector[0]-normalCircle[0]*UVector[2];
    VVector[2] = normalCircle[0]*UVector[1]-normalCircle[1]*UVector[0];
    //vtkMath::Cross(normalCircle,UVector,VVector);
    magnitude =sqrt(VVector[0]*VVector[0] + VVector[1]*VVector[1] + VVector[2]*VVector[2]);
    VVector[0] *= (m_StentDiameter/2.0/magnitude);
    VVector[1] *= (m_StentDiameter/2.0/magnitude);
    VVector[2] *= (m_StentDiameter/2.0/magnitude);

    //mafLogMessage("Normal: %f %f %f",normalCircle[0], normalCircle[1], normalCircle[2] );
    //mafLogMessage("UVector: %f %f %f",UVector[0], UVector[1], UVector[2] );
    //mafLogMessage("VVector: %f %f %f",VVector[0], VVector[1], VVector[2] );

    if(i==0){
      for(int n=0;n<m_strutsNumber;n++){//the beginning(0) circle 
        m_SampleArray[n][0] = m_CenterLine[i][0] + cos(theta*n*2)*UVector[0] + sin(theta*n*2)*VVector[0];
        m_SampleArray[n][1] = m_CenterLine[i][1] + cos(theta*n*2)*UVector[1] + sin(theta*n*2)*VVector[1];
        m_SampleArray[n][2] = m_CenterLine[i][2] + cos(theta*n*2)*UVector[2] + sin(theta*n*2)*VVector[2];
      }
      for(int n=0;n<sampleNumberPerCircle;n++){//the first circle
        m_SampleArray[sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
        m_SampleArray[sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
        m_SampleArray[sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
      }
      continue;
    }
    if(i == m_NumCenterVertex-2){
      for(int n = 0;n<sampleNumberPerCircle;n++){//the second last circle
        m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
        m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
        m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
      }
      for(int n=0;n<m_strutsNumber;n++){//the last circle
        m_SampleArray[m_strutsNumber+n][0] = m_CenterLine[i+1][0] + cos(theta*n*2)*UVector[0] + sin(theta*n*2)*VVector[0];
        m_SampleArray[m_strutsNumber+n][1] = m_CenterLine[i+1][1] + cos(theta*n*2)*UVector[1] + sin(theta*n*2)*VVector[1];
        m_SampleArray[m_strutsNumber+n][2] = m_CenterLine[i+1][2] + cos(theta*n*2)*UVector[2] + sin(theta*n*2)*VVector[2];
      }
      continue;
    }

    for(int n = 0;n<sampleNumberPerCircle;n++){
      m_SampleArray[i*sampleNumberPerCircle+n][0] = m_CenterLine[i][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
      m_SampleArray[i*sampleNumberPerCircle+n][1] = m_CenterLine[i][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
      m_SampleArray[i*sampleNumberPerCircle+n][2] = m_CenterLine[i][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][0] = m_CenterLine[i+1][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][1] = m_CenterLine[i+1][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
      m_SampleArray[(i+1)*sampleNumberPerCircle+n][2] = m_CenterLine[i+1][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];

    }
  }

  //--------------------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------------------
  /** Add our vertices to the simplex mesh.*/
  PointType point;
  for(i=0; i < m_SampleArraySize ; ++i)
  {
    point[0] = m_SampleArray[i][0];
    point[1] = m_SampleArray[i][1];
    point[2] = m_SampleArray[i][2];
    m_SimplexMesh->SetPoint(i, point);
    m_SimplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );
  }
  //delete[] m_SampleArray;

  /** Specify the method used for allocating cells */
  m_SimplexMesh->SetCellsAllocationMethod( SimplexMeshType::CellsAllocatedDynamicallyCellByCell );

  /** AddEdge,AddNeighbor,and add the symmetric relationships */
  //-----------------------------------------------------------------------------------
  /* pay attention to the order of adding
  *  the three vertices should be commected anticlockwise
  *  to make sure the normal calculate on the surface of simplex mesh is pointing outwards
  *-------------------------*/

  //-----------------
  /**AddEdge */
  /** the beginning circle -- vertical */
  for(j=0;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex = j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,0);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j ;  //for visualize mesh      
  theMesh.endVertex =0; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  /** the beginning circle -- horizontal */
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(j,sampleNumberPerCircle+j*2);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j ;  //for visualize mesh      
    theMesh.endVertex =sampleNumberPerCircle+j*2; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  /** the middle circles -- vertical */
  for(i=1;i<m_NumCenterVertex-1;i++){
    for(j=0;j<sampleNumberPerCircle-1;j++){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =i*sampleNumberPerCircle+j+1; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
    theMesh.endVertex =i*sampleNumberPerCircle; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  /** the middle circles -- horizontal */
  for(i=1;i<m_NumCenterVertex-1;i=i+2){
    for(j=1;j<sampleNumberPerCircle;j=j+2){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j ;  //for visualize mesh      
      theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
  }
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=0;j<sampleNumberPerCircle;j=j+2){
      m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
      //---------------for visualize mesh start---------------
      theMesh.startVertex =i*sampleNumberPerCircle+j;  //for visualize mesh      
      theMesh.endVertex =(i+1)*sampleNumberPerCircle+j; //for visualize mesh 
      m_MeshList.push_back(theMesh); //for visualize mesh 
      //---------------for visualize mesh end---------------
    }
  }
  /** the last circle -- vertical */
  for(j=m_strutsNumber;j<2*m_strutsNumber-1;j++){
    m_SimplexMesh->AddEdge(j,j+1);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =j;  //for visualize mesh      
    theMesh.endVertex =j+1; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }
  m_SimplexMesh->AddEdge(j,m_strutsNumber);
  //---------------for visualize mesh start---------------
  theMesh.startVertex =j;  //for visualize mesh      
  theMesh.endVertex =m_strutsNumber; //for visualize mesh 
  m_MeshList.push_back(theMesh); //for visualize mesh 
  //---------------for visualize mesh end---------------
  /** the last circle -- horizontal */
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber;j++){
    m_SimplexMesh->AddEdge(i*sampleNumberPerCircle+j*2,m_strutsNumber+j);
    //---------------for visualize mesh start---------------
    theMesh.startVertex =i*sampleNumberPerCircle+j*2;  //for visualize mesh      
    theMesh.endVertex =m_strutsNumber+j; //for visualize mesh 
    m_MeshList.push_back(theMesh); //for visualize mesh 
    //---------------for visualize mesh end---------------
  }

  //-------------------------------------------------------------------
  /**AddNeighbor
  *the beginning circle */
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(j,j-1);
    m_SimplexMesh->AddNeighbor(j,j+1);
    m_SimplexMesh->AddNeighbor(j,sampleNumberPerCircle+j*2);
  }
  m_SimplexMesh->AddNeighbor(0,m_strutsNumber-1);
  m_SimplexMesh->AddNeighbor(0,1);
  m_SimplexMesh->AddNeighbor(0,sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,m_strutsNumber-2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,0);
  m_SimplexMesh->AddNeighbor(m_strutsNumber-1,sampleNumberPerCircle+(m_strutsNumber-1)*2);
  /** the first circle */
  i=1;  
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);

  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,j/2);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle, 0);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  /** the middle circles */
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);

  }
  for(i=3;i<m_NumCenterVertex-2;i=i+2){
    for(j=1;j<sampleNumberPerCircle-2;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    } 
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
      m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);		
    }
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  }

  /** the second last circle */
  i= m_NumCenterVertex-2;
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  for(j=2;j<sampleNumberPerCircle-1;j=j+2){
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
    m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,m_strutsNumber+j/2);
  }
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle,m_strutsNumber);
  //the last circle
  for(j=1;j<m_strutsNumber-1;j++){
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j+1);
    m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  }
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,i*sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber);
  m_SimplexMesh->AddNeighbor(m_strutsNumber+j,m_strutsNumber+j-1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,i*sampleNumberPerCircle);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,m_strutsNumber+1);
  m_SimplexMesh->AddNeighbor(m_strutsNumber,sampleNumberPerCircle-1);

  //------------------------------------
  /** AddFace */
  /*  OutputCellAutoPointer m_NewSimplexCellPointer;
  // the biginning circle : cells
  for(j=0;j<m_strutsNumber-1;j++){
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, j);
  m_NewSimplexCellPointer->SetPointId(1, j+1);
  m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle+j*2+2);
  m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, j);
  m_NewSimplexCellPointer->SetPointId(1, 0);
  m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  // the last circle : cells 
  i=m_NumCenterVertex-2;
  for(j=0;j<m_strutsNumber-1;j++){
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j*2+2);
  m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber+j+1);
  m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, m_strutsNumber);
  m_NewSimplexCellPointer->SetPointId(4, m_strutsNumber+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);

  // the middle circles : cells 
  for(i=2;i<m_NumCenterVertex-2;i=i+2){
  for(j=0;j<sampleNumberPerCircle-2;j=j+2){
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
  m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
  m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);
  }

  // connect struts to struts 
  for(i=1;i<m_NumCenterVertex-1;i=i+2){
  for(j=1;j<sampleNumberPerCircle-2;j=j+2){
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
  m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
  m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
  m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
  m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
  m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
  m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+1);
  m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+1);
  m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle);
  m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddFace(m_NewSimplexCellPointer);	
  }*/
  //  m_SimplexMesh->BuildCellLinks();
}



void vtkMEDStentModelSource::createBardStruts2(){

  Strut theStrut;
  int i,j,k;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int smallStruts = 8;
  int bigStruts = (m_strutsNumber-smallStruts)/2;
  int endHelicalIdx;

  /** the even circles, counted from zero */	
  //the first ring of struts
  i=2;
  for(j=0;j<sampleNumberPerCircle-2;j=j+2){                //"for ever strut \/"
    theStrut.startVertex = sampleNumberPerCircle*i+j;         //"\"
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*i+j+2;       //   "/"  
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;
    m_StrutsList.push_back(theStrut);		
  }
  theStrut.startVertex = sampleNumberPerCircle*i+j;              //last strut of the circle
  theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;          //"\"
  m_StrutsList.push_back(theStrut);
  theStrut.startVertex = sampleNumberPerCircle*i;               //coneect last strut with first strut
  theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;         //   "/"
  m_StrutsList.push_back(theStrut);
  //----------second ring 8small + 4 big
  i=4;
  for(j=1;j<smallStruts*2+1;j=j+2){
    theStrut.startVertex = sampleNumberPerCircle*i+j;
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*i+j+2;       //   "/"  
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+1;
    m_StrutsList.push_back(theStrut);	
  }//j=17
  for(k=1;k<bigStruts;k++,j=j+4){//big struts
    theStrut.startVertex = sampleNumberPerCircle*i+j;
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*i+j+4;       //   "/"  
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;
    m_StrutsList.push_back(theStrut);	

  }//j=29
  theStrut.startVertex = sampleNumberPerCircle*i+j;
  theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;
  m_StrutsList.push_back(theStrut);
  theStrut.startVertex = sampleNumberPerCircle*i+1;       //   "/"  
  theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;
  m_StrutsList.push_back(theStrut);
  //-----------helical struts
  for(i=6;i<m_NumCenterVertex-8;i=i+2){
    int strutsN = sampleNumberPerCircle-2;
    /*if (i==m_NumCenterVertex-10)
    {
    strutsN = sampleNumberPerCircle-3;
    }*/
    for(j=0;j<strutsN;j=j+2){
      theStrut.startVertex = i*sampleNumberPerCircle+j;
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = i*sampleNumberPerCircle+j+2;       //   "/"  
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
    }
    if(i<m_NumCenterVertex-10){//interface
      theStrut.startVertex = i*sampleNumberPerCircle+j;
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = (i+2)*sampleNumberPerCircle;       //   "/"  
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
    }else if(i==m_NumCenterVertex-10){//10 crown i=16, last strut in the helical
      theStrut.startVertex = i*sampleNumberPerCircle+j;
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
      endHelicalIdx = (i+1)*sampleNumberPerCircle+j+3;
      int abc = (i+2)*sampleNumberPerCircle+1;
      theStrut.startVertex = endHelicalIdx;      //   "/\"  
      theStrut.endVertex = (i+1)*sampleNumberPerCircle+j+1;
      m_StrutsList.push_back(theStrut);
    }
  }
  //--------------big struts-------
  i=m_NumCenterVertex-8;
  for(j=1,k=1;k<=5;k++,j=j+4){//5big10small=18
    theStrut.startVertex = sampleNumberPerCircle*i+j;
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*(i+1)+j+2;
    theStrut.endVertex = sampleNumberPerCircle*i+j+4;
    m_StrutsList.push_back(theStrut);	

  }
  //last big
  /*theStrut.startVertex = sampleNumberPerCircle*(i-1)-2;//18
  theStrut.endVertex = sampleNumberPerCircle*(i+1)+2;//   /17_0-2
  m_StrutsList.push_back(theStrut);
  theStrut.startVertex = sampleNumberPerCircle*i+2+1; //   \  18_2
  theStrut.endVertex =  sampleNumberPerCircle*(i+1)+2;
  m_StrutsList.push_back(theStrut);	 */
  //-small struts in this line
  for(j=bigStruts*4+1;j<sampleNumberPerCircle;j=j+2){
    theStrut.startVertex = sampleNumberPerCircle*i+j;//18_14
    theStrut.endVertex = sampleNumberPerCircle*(i+1)+j+2;//   /
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*i+j+2;//18_16  \  
    theStrut.endVertex =  sampleNumberPerCircle*(i+1)+j+2;
    m_StrutsList.push_back(theStrut);	

  }
  //-------
  i=m_NumCenterVertex-4;
  for(j=0;j<sampleNumberPerCircle-2;j=j+2){                //"for ever strut \/"
    theStrut.startVertex = sampleNumberPerCircle*(i-2)+2+j;         //"/"
    theStrut.endVertex = sampleNumberPerCircle*(i-2+1)+2+j+1;
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*(i-2)+2+j+2;       //   "\"  
    theStrut.endVertex = sampleNumberPerCircle*(i-2+1)+2+j+1;
    m_StrutsList.push_back(theStrut);		
  }
  theStrut.startVertex = sampleNumberPerCircle*(i-2)+2+j;              //last strut of the circle
  theStrut.endVertex = sampleNumberPerCircle*(i-2+1)+2+j+1;          //"\"
  m_StrutsList.push_back(theStrut);
  theStrut.startVertex = sampleNumberPerCircle*(i-2)+2;               //coneect last strut with first strut
  theStrut.endVertex = sampleNumberPerCircle*(i-2+1)+2+j+1;         //   "/"
  m_StrutsList.push_back(theStrut);

}



void vtkMEDStentModelSource::createBardStruts(){
  Strut theStrut;
  int i,j;
  int sampleNumberPerCircle = 2*m_strutsNumber;

  m_StrutsList.clear();

  /** the even circles, counted from zero */	
  for(j=2;j<m_NumCenterVertex-2;j=j+4){
    for(i=0;i<sampleNumberPerCircle-2;i=i+2){                //"for ever strut \/"
      theStrut.startVertex = sampleNumberPerCircle*j+i;         //"\"
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j+i+2;       //   "/"  
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);		
    }
    theStrut.startVertex = sampleNumberPerCircle*j+i;              //last strut of the circle
    theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;          //"\"
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*j;               //coneect last strut with first strut
    theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;         //   "/"
    m_StrutsList.push_back(theStrut);
  }

  /** the odd circles, counted from zero */
  if(m_StentConfiguration == OutOfPhase){
    for(j=4;j<m_NumCenterVertex-2;j=j+4){
      for(i=1;i<sampleNumberPerCircle-2;i=i+2){
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
        m_StrutsList.push_back(theStrut);
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);		
      }
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1);
      m_StrutsList.push_back(theStrut);
    }
  } else if (m_StentConfiguration == InPhase){
    for(j=4;j<m_NumCenterVertex-2;j=j+4){
      for(i=0;i<sampleNumberPerCircle-2;i=i+2){
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);
        theStrut.startVertex = sampleNumberPerCircle*j+i+2;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);		
      }
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);	
    }
  }
}



void vtkMEDStentModelSource::createStruts(){
  Strut theStrut;
  int i,j;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  m_StrutsList.clear();

  /** the even circles, counted from zero */	
  for(j=2;j<m_NumCenterVertex-2;j=j+4){
    for(i=0;i<sampleNumberPerCircle-2;i=i+2){                //"for ever strut \/"
      theStrut.startVertex = sampleNumberPerCircle*j+i;         //"\"
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j+i+2;       //   "/"  
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);		
    }
    theStrut.startVertex = sampleNumberPerCircle*j+i;              //last strut of the circle
    theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;          //"\"
    m_StrutsList.push_back(theStrut);
    theStrut.startVertex = sampleNumberPerCircle*j;               //coneect last strut with first strut
    theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;         //   "/"
    m_StrutsList.push_back(theStrut);
  }

  /** the odd circles, counted from zero */
  if(m_StentConfiguration == OutOfPhase){
    for(j=4;j<m_NumCenterVertex-2;j=j+4){
      for(i=1;i<sampleNumberPerCircle-2;i=i+2){
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
        m_StrutsList.push_back(theStrut);
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);		
      }
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1);
      m_StrutsList.push_back(theStrut);
    }
  } else if (m_StentConfiguration == InPhase){
    for(j=4;j<m_NumCenterVertex-2;j=j+4){
      for(i=0;i<sampleNumberPerCircle-2;i=i+2){
        theStrut.startVertex = sampleNumberPerCircle*j+i;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);
        theStrut.startVertex = sampleNumberPerCircle*j+i+2;
        theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
        m_StrutsList.push_back(theStrut);		
      }
      theStrut.startVertex = sampleNumberPerCircle*j+i;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);
      theStrut.startVertex = sampleNumberPerCircle*j;
      theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
      m_StrutsList.push_back(theStrut);	
    }
  }
}



void vtkMEDStentModelSource::createBardLinks(){

  Strut theLink;
  int i,j,idx;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int cross=0;
  //helical :7-15
  int startPosition = 1;

  int count = startPosition;
  int step=10;
  int start = 7*sampleNumberPerCircle;
  int end = (m_NumCenterVertex-9)*sampleNumberPerCircle-2;//(m_NumCenterVertex-11)*sampleNumberPerCircle-2;
  int currentPosition = start+startPosition;
  for(currentPosition=start+startPosition ;currentPosition<end; ){
    if (count==0)///0-31
    {
      theLink.startVertex = currentPosition;
      theLink.endVertex =   currentPosition-1;
      m_LinksList.push_back(theLink);

    }else{
      theLink.startVertex = currentPosition;
      theLink.endVertex =   currentPosition+sampleNumberPerCircle-1;
      m_LinksList.push_back(theLink);
    }
    count=count+10;
    if (count>sampleNumberPerCircle)
    {
      count = count%sampleNumberPerCircle;
      step = 10 + sampleNumberPerCircle;
    }else{
      step= 10;
    }
    currentPosition = theLink.startVertex+step;	

  }
  //first ring 
  idx=1;
  i=3;
  for(j=1;j<=4;j++){
    theLink.startVertex = sampleNumberPerCircle * i + idx;
    theLink.endVertex = sampleNumberPerCircle *(i+1) + idx;
    m_LinksList.push_back(theLink);
    idx=(idx+8)%sampleNumberPerCircle;

  }
  //second ring 
  idx=2;
  i=5;
  for(j=1;j<=4;j++){
    if(j>2){
      theLink.startVertex = sampleNumberPerCircle * i + idx+1;
      theLink.endVertex = sampleNumberPerCircle *(i+1) + idx;
      m_LinksList.push_back(theLink);
    }else{
      theLink.startVertex = sampleNumberPerCircle * i + idx;
      theLink.endVertex = sampleNumberPerCircle *(i+1) + idx;
      m_LinksList.push_back(theLink);
    }
    idx=(idx+8)%sampleNumberPerCircle;

  }
  /*
  //second last ring
  idx=3;
  i=m_NumCenterVertex-9;

  for(j=1;j<=4;j++){
  if(j>2){
  theLink.startVertex = sampleNumberPerCircle * i + idx;
  theLink.endVertex = sampleNumberPerCircle *(i+1) + idx+1;
  m_LinksList.push_back(theLink);
  }else{
  theLink.startVertex = sampleNumberPerCircle * i + idx;
  theLink.endVertex = sampleNumberPerCircle *(i+1) + idx;
  m_LinksList.push_back(theLink);
  }

  idx=(idx+8)%sampleNumberPerCircle;

  }
  //last ring
  i=m_NumCenterVertex-5;
  idx=0;
  for(j=1;j<=4;j++){
  theLink.startVertex = sampleNumberPerCircle * (i-2) +2 + idx;
  theLink.endVertex = sampleNumberPerCircle *(i-2+1) +2+ idx;
  m_LinksList.push_back(theLink);
  idx=(idx+8)%sampleNumberPerCircle;

  }
  */
}



/**
*different link types: 12 kinds + different alignment
*	6 kinds for out of phase:p2p+N; v2v+N; p2v+1;p2v-1;v2p+1;v2p-1
* 6 kinds for in phase:p2v+N; v2P+N; p2p+1;p2p-1;v2v+1;v2v-1
*/
void vtkMEDStentModelSource::createLinks(){
  Strut theLink;
  int i,j;
  int sampleNumberPerCircle = 2*m_strutsNumber;
  int interval = (m_strutsNumber / m_LinkNumber)*2;
  int indexFirstLink=0;
  int indexCurrent;
  m_LinksList.clear();
  /**--------OUT OF PHASE---------------*/
  if(m_StentConfiguration == OutOfPhase){ 

    if(m_LinkConnection == peak2peak && m_LinkOrientation == None){ /** out of phase + p2p + N */

      indexFirstLink = 1;
      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i=i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle *(j*2+2) + indexCurrent;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2-1))%sampleNumberPerCircle;
      }

    }else if(m_LinkConnection == valley2valley && m_LinkOrientation == None){/** out of phase + v2v + N */

      indexFirstLink = 0;

      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle * (j*2) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle *(j*2+3) + indexCurrent;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2-1))%sampleNumberPerCircle;
      }


    }else if(m_LinkConnection == peak2valley && m_LinkOrientation != None){ /** out of phase, p2v, +/-1 */

      int offset;
      if(m_LinkOrientation == PositiveOne) offset = 1;
      if(m_LinkOrientation == NegativeOne) offset = -1;

      indexFirstLink = 1;

      int indexCurrent2;
      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle *(j*2+3) + indexCurrent2;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2-1))%sampleNumberPerCircle;
      }

    }else if(m_LinkConnection == valley2peak && m_LinkOrientation != None){
      int offset;
      if(m_LinkOrientation == PositiveOne) offset = 1;
      if(m_LinkOrientation == NegativeOne) offset = -1;

      indexFirstLink = 0;
      int indexCurrent2;
      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle * (j*2) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent2;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2-1))%sampleNumberPerCircle;
      }

    }else{
      std::cout << "please check link settings"<<std::endl;
    }

  }else{ /** -------------IN PHASE----------------- */
    if(m_LinkConnection == peak2valley && m_LinkOrientation == None){ /** p2v */

      indexFirstLink = 1;
      for(j=1;j<m_NumCrownSimplex-2;j=j+1+this->getLinkInterlace()){ //links are not connected line by line but 1-2 3-4 instead of 1-2-3-4
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle*(j*2+1) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle*(j*2+3) + indexCurrent;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
      }

    }else if(m_LinkConnection == valley2peak && m_LinkOrientation == None){/** v2p */

      indexFirstLink = 0;
      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle*(j*2) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
      }

    }else if(m_LinkConnection == peak2peak && m_LinkOrientation != None){/** p2p */
      int offset;
      if(m_LinkOrientation == PositiveOne) offset = -1; // +1 change it
      if(m_LinkOrientation == NegativeOne) offset = +1; // -1 change it

      indexFirstLink = 1;
      int indexCurrent2;
      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent2;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
      }

    }else if(m_LinkConnection == valley2valley && m_LinkOrientation != None){/** v2v */
      int offset;
      if(m_LinkOrientation == PositiveOne) offset = 1;
      if(m_LinkOrientation == NegativeOne) offset = -1;

      indexFirstLink = 0;
      int indexCurrent2;

      for(j=1;j<m_NumCrownSimplex-2;j++){
        for(i=0;i<m_LinkNumber;i++){
          indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
          indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
          theLink.startVertex = sampleNumberPerCircle*(j*2) + indexCurrent;
          theLink.endVertex = sampleNumberPerCircle*(j*2+3) + indexCurrent2;
          m_LinksList.push_back(theLink);
        }
        indexFirstLink = (indexFirstLink + (m_LinkAlignment*2))%sampleNumberPerCircle;
      }

    }else{
      std::cout << "please check link settings"<<std::endl;
    }	
  }
}



//-----------------------------------------------------------------------
// Find the sampling point along the line segment 
//-----------------------------------------------------------------------
bool vtkMEDStentModelSource::calculateSamplingPoint2(double *preSamplePoint, double* samplePoint, 
  double distance, double* left, double* right)
{
  // Solve for alpha, where the line segment is x = (1-alpha)*xL + alpha*xR
  // and the distance from the presample pt is r^2 = |x-x0|^2
  // This is the intersection of a line and a sphere.
  vtkMEDMatrixVectorMath *matMath = vtkMEDMatrixVectorMath::New() ;
  matMath->SetHomogeneous(false) ;
  double p[3], q[3] ;
  matMath->SubtractVectors(left,preSamplePoint,p) ; // p = xL-x0
  matMath->SubtractVectors(right,left,q) ; // q = xR-x0

  // This gives x-x0 = p + alpha*q
  // r^2 = |x-x0|^2 = alpha^2*|q|^2 + 2*alpha*(p.q) + |p|^2
  double a = matMath->DotProduct(q,q) ; // |q|^2
  double b = 2.0*matMath->DotProduct(p,q) ; // 2(p.q)
  double c = matMath->DotProduct(p,p) - distance*distance ; // |p|^2 - r^2
  double discr = b*b - 4.0*a*c ;
  if (discr < 0.0){
    // No solution - the sphere does not reach the line
    matMath->Delete() ;
    return false ;
  }
  double alpha1 = (-b - sqrt(discr)) / (2.0*a) ;
  double alpha2 = (-b + sqrt(discr)) / (2.0*a) ;

  // Choose which solution to use.
  // Solution must be in line segment (0 <= alpha <= 1)
  // Solution must also move forward, so greater than alpha0, corresponding to x0.
  double alpha, w[3] ;
  matMath->SubtractVectors(right,left,w) ; // w = xR-xL
  double alpha0 = -matMath->DotProduct(p,w) / matMath->DotProduct(w,w) ; // alpha0 = -p*w / |w|^2

  bool alpha1Ok = ((alpha1 >= 0.0) && (alpha1 <= 1.0) && (alpha1 > alpha0)) ; 
  bool alpha2Ok = ((alpha2 >= 0.0) && (alpha2 <= 1.0) && (alpha2 > alpha0)) ; 
  if (alpha1 && !alpha2)
    alpha = alpha1 ;
  else if (!alpha1 && alpha2)
    alpha = alpha2 ;
  else if (alpha1 && alpha2){
    // Both solutions valid - should never actually happen
    alpha = std::max(alpha1, alpha2) ;
  }
  else{
    // neither solution is ok
    matMath->Delete() ;
    return false ;
  }

  // calculate sample point
  matMath->MultiplyVectorByScalar(1.0-alpha, left, samplePoint) ; // x = (1-alpha)*xL
  matMath->AddMultipleOfVector(samplePoint, alpha, right, samplePoint) ; // x += alpha*xR

  matMath->Delete() ;
  return true ;
}


/*
// to calculate how many crowns could be set on a centerline
int vtkMEDStentModelSource::computeCrownNumberAfterSetCenterLine(){
int rtn = 0;

double lineLength = this->m_CenterLineLength;//to do compute lineLength
double gapLength;
gapLength = calculateDistanceBetweenCrown();
//lineLength = lineLength - m_CrownLength ;

rtn = floor( lineLength / (m_CrownLength + gapLength))-2;
return rtn ;
}
*/


/** calculate the distance between crowns */
double vtkMEDStentModelSource::calculateDistanceBetweenCrown(){

  double distanceBetweenCrown = m_LinkLength;
  double horizontalLength = 0.0;
  double m_StrutAngle = 2.0*atan(m_StentDiameter*(vtkMath::Pi())/m_strutsNumber/2.0/m_CrownLength);
  m_StrutLength = 0.99 * m_CrownLength / cos(m_StrutAngle/2.0) ;

  if(m_LinkOrientation == None){
    if(m_LinkConnection == peak2peak)
      distanceBetweenCrown = m_LinkLength;
    else if (m_LinkConnection == valley2valley)
      distanceBetweenCrown = m_LinkLength-2*m_CrownLength;
    else
      distanceBetweenCrown = m_LinkLength-m_CrownLength;
  }else{ /** link orientation == PositiveOne or NegativeOne */
    distanceBetweenCrown = m_LinkLength;
    /*horizontalLength = sqrt(m_LinkLength*m_LinkLength
    - (m_CrownLength*tan(m_StrutAngle/2))*(m_CrownLength*tan(m_StrutAngle/2)));
    if(m_LinkConnection == peak2peak){			
    distanceBetweenCrown = horizontalLength;
    }
    else if (m_LinkConnection == valley2valley){
    distanceBetweenCrown = horizontalLength-2*m_CrownLength;
    }
    else {
    distanceBetweenCrown = horizontalLength-m_CrownLength;
    }*/
  }
  return distanceBetweenCrown;
}



//---------------bard------------
//move vector(V) to oringinal get(V1) ,get the projection vector (V2)in z=0 plane,
//V2[0]=Vx[0]=V1[0],V2[1]=V1[1]=Vx[1],V2[2]=0
//then find a point(Vx) get the correct angle with V1
//move back Vx to get result
void vtkMEDStentModelSource::getBardNormalCircle( double *startPoint,  double *endPoint, double *sampleVector,double theta)
  //void vtkMEDStentModelSource::getBardNormalCircle( double theta)
{
  double U[3],V[3],W[3];
  U[0]= endPoint[0]-startPoint[0];
  U[1]= endPoint[1]-startPoint[1];
  U[2]= endPoint[2]-startPoint[2];

  vtkMEDMatrixVectorMath *theMath;
  theMath = vtkMEDMatrixVectorMath::New() ;
  theMath->SetHomogeneous(false);
  theMath->CalculateNormalsToU(U,V,W);
  vtkMath::Normalize(W);
  double uMag = theMath->MagnitudeOfVector(U);
  double s = tan(theta)*uMag;
  theMath->AddMultipleOfVector(U,s,W,sampleVector);
  theMath->Delete() ;


  /*double V1[3],V1n[3],V2[3],Vx[3];
  V1[0]= endPoint[0]-startPoint[0];
  V1[1]= endPoint[1]-startPoint[1];
  V1[2]= endPoint[2]-startPoint[2];

  V1n[0]=V1[0];
  V1n[1]=V1[1];
  V1n[2]=V1[2];
  vtkMath::Normalize(V1n);
  //V1dotVx= cos(theta)
  Vx[0]=V1n[0];
  Vx[1]=V1n[1];

  //V1[0]*Vx[0]+V1[1]*Vx[1]+V1[2]*Vx[2]=cos(theta)
  double cosValue = cos(theta);
  double tmp = (V1n[0]*V1n[0]+V1n[1]*V1n[1]);
  Vx[2]= (cosValue-tmp)/V1n[2];
  vtkMath::Normalize(Vx);
  //move back
  samplePoint[0]=Vx[0]+startPoint[0];
  samplePoint[1]=Vx[1]+startPoint[1];
  samplePoint[2]=Vx[2]+startPoint[2];
  //----------test---------
  double testCos = vtkMath::Dot(V1n,Vx);

  //vtkMath::Normalize(samplePoint);
  */

}



//get U,V vector from normal
void vtkMEDStentModelSource::getUVVector(  double * normalCircle, double * UVector, double * VVector )
{
  double magnitude;
  /*
  * calculate the outer product of normalCircle with (0,1,0) to get a vector perpendicular to normalCircle
  * here (0,1,0) is a random vector chosed, any vertor which is not paralle works
  * might have little bug here, possibly the normalCircle is paralle to (0,1,0), 
  * in this extreme case, need choose another vector to replace (0,1,0)
  * BETTER ADD A SIMPLE TEST HERE TO AVOID THIS!!!
  */
  UVector[0] = -1.0*normalCircle[2];//0;
  UVector[1] = 0;//n2;
  UVector[2] = normalCircle[0];//-n1
  if((fabs(UVector[0])+ fabs(UVector[1]) + fabs(UVector[2])) < 0.05){
    UVector[0] = normalCircle[1];
    UVector[1] = -normalCircle[0];
    UVector[2] = 0;
    //mafLogMessage("set to zero");
  } 
  magnitude = sqrt(UVector[0]*UVector[0]+UVector[1]*UVector[1] + UVector[2]*UVector[2]);
  UVector[0] *= (m_StentDiameter/2.0/magnitude);
  UVector[1] *= (m_StentDiameter/2.0/magnitude);
  UVector[2] *= (m_StentDiameter/2.0/magnitude);
  //outer product of normalCircle & UVector
  VVector[0] = normalCircle[1]*UVector[2]-normalCircle[2]*UVector[1];
  VVector[1] = normalCircle[2]*UVector[0]-normalCircle[0]*UVector[2];
  VVector[2] = normalCircle[0]*UVector[1]-normalCircle[1]*UVector[0];
  //vtkMath::Cross(normalCircle,UVector,VVector);
  magnitude =sqrt(VVector[0]*VVector[0] + VVector[1]*VVector[1] + VVector[2]*VVector[2]);
  VVector[0] *= (m_StentDiameter/2.0/magnitude);
  VVector[1] *= (m_StentDiameter/2.0/magnitude);
  VVector[2] *= (m_StentDiameter/2.0/magnitude);
}



void vtkMEDStentModelSource::NeighborUpRL( int i, int sampleNumberPerCircle, int j )
{
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
}



void vtkMEDStentModelSource::NeighborLRDown( int i, int sampleNumberPerCircle, int j )
{
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
  m_SimplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
}



//----------------------------------------------------------------------------
// Allocate or reallocate centerline
//----------------------------------------------------------------------------
void vtkMEDStentModelSource::AllocateCentreLine(int n)
{
  if (m_CenterLine != NULL){
    if (n > m_NumberOfPointsAllocatedToCenterLine){
      // delete existing allocation
      delete [] m_CenterLine ;
      m_CenterLine = NULL ;
      m_NumberOfPointsAllocatedToCenterLine = 0 ;
    }
    else{
      // already enough allocated - do nothing
      return ;
    }
  }

  // allocate
  if (n > 0){
    m_CenterLine = new double[n][3];
    m_NumberOfPointsAllocatedToCenterLine = n ;
  }
}


