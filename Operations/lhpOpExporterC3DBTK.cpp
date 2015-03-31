/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterC3DBTK.cpp,v $
  Language:  C++
  Date:      $Date: 2009/05/19 14:29:53 $
  Version:   $Revision: 1.1.1.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2009
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpOpExporterC3DBTK.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mmuTimeSet.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVector.h"
#include "medVMEAnalog.h"
#include "mafVMEOutputScalarMatrix.h"
#include "mafTagArray.h"

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkMAFSmartPointer.h"
#include "vtkCellArray.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

//#include "C3D_Reader.h"

#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vnl\vnl_matrix.h>

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpExporterC3DBTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpExporterC3DBTK::lhpOpExporterC3DBTK(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = "";
  m_FileDir = "";
  m_Input   = NULL;
  m_GlobalPos = true;
  m_Subtree   = false;
}
//----------------------------------------------------------------------------
lhpOpExporterC3DBTK::~lhpOpExporterC3DBTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool lhpOpExporterC3DBTK::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  if(node == NULL)
    return false;
  /*if(!node->IsMAFType(mafVMELandmarkCloud))
    return false;*/
  return true;
//  return node && node->IsMAFType(mafVMELandmarkCloud);
}
enum C3D_EXPORTER_ID
{
  ID_ABS_POSITION = MINID,
  ID_SUBTREE
};
//----------------------------------------------------------------------------
void lhpOpExporterC3DBTK::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label("absolute matrix",true);
  m_Gui->Bool(ID_ABS_POSITION,"apply",&m_GlobalPos,0);
  if(mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input))
    m_Gui->Bool(ID_SUBTREE,"Subtree",&m_Subtree,0);
  else if(medVMEAnalog *analog = medVMEAnalog::SafeDownCast(m_Input))
    m_Gui->Bool(ID_SUBTREE,"Subtree",&m_Subtree,0);
  m_Gui->OkCancel();
  m_Gui->Divider();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpExporterC3DBTK::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_ABS_POSITION:
    case ID_SUBTREE:
      break;
    case wxOK:
      /*{
        wxString proposed = mafGetApplicationDirectory().c_str();
        proposed += "/Data/External/";
        proposed += m_Input->GetName();
        proposed += ".txt";
        wxString wildc = "ascii file (*.txt)|*.txt";
        wxString f = mafGetSaveFile(proposed,wildc).c_str(); 
        int result = OP_RUN_CANCEL;
        if(!f.IsEmpty())
        {
          m_File = f;
          ExportLandmark();
          result = OP_RUN_OK;
        }
      }*/
      
      {
        int result = OP_RUN_CANCEL;
        m_Gui->Enable(wxOK, false);
        m_Gui->Enable(wxCANCEL, false);

        assert(m_Input);
        wxString proposed = (mafGetApplicationDirectory() + "/Data/External/").c_str();

        if(true)//(m_Input->IsMAFType(mafVMELandmarkCloud))
        {
          proposed += m_Input->GetName();
          proposed += ".c3d";
          wxString wildc = "c3d file (*.c3d)|*.c3d";

          wxString f = mafGetSaveFile(proposed,wildc).GetCStr(); 

          if(f != "") 
          {
            m_File = f;
            ExportLandmark();
            result = OP_RUN_OK;
          }
        }
        else
        {
          /*wxMessageDialog dialog(mafGetFrame(), _("Do you want to create separate files?"),
            _("Options"), wxYES_NO|wxYES_DEFAULT);
          if(dialog.ShowModal() == wxID_NO)
          {

            proposed += m_Input->GetName();
            proposed += ".txt";
            wxString wildc = "ascii file (*.txt)|*.txt";
            wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

            if(f != "") 
            {
              m_File = f;
              ExportLandmark();
              result = OP_RUN_OK;
            }
          }
          else*/
          {
            wxString f = mafGetDirName(proposed).GetCStr();

            if(f != "") 
            {
              m_FileDir = f;
              ExportLandmark();
              result = OP_RUN_OK;
            }
          }
        }
      }
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
}



#ifdef gytytytytytytytyt
void oips()
{
  btk::Acquisition::Pointer source = btk::Acquisition::New();
  btk::Acquisition::Pointer target = btk::Acquisition::New();
  // Acquisition info
  //target->SetFirstFrame(lb);
  //target->SetPointFrequency(source->GetPointFrequency());
  //target->SetAnalogResolution(source->GetAnalogResolution());
  //target->SetPointUnits(source->GetPointUnits());
  // Event
  btk::EventCollection::Pointer targetEvents = target->GetEvents();
  for (QMap<int,Event*>::const_iterator it = this->m_Events.begin() ; it != this->m_Events.end() ; ++it)
  {
    Event* e = it.value();
    if ((e->frame >= lb) && (e->frame <= rb))
    {
      targetEvents->InsertItem(btk::Event::New(e->label.toStdString(), 
        e->time,
        e->context.toStdString(),
        btk::Event::Unknown,
        e->subject.toStdString(),
        e->description.toStdString(),
        e->iconId));
    }
  }
  // Metadata
  target->SetMetaData(source->GetMetaData()->Clone());
  // - POINT
  btk::MetaData::Pointer point;
  btk::MetaData::Iterator it = target->GetMetaData()->FindChild("POINT");
  if (it != target->GetMetaData()->End())
    point = *it;
  else
  {
    point = btk::MetaData::New("POINT");
    target->GetMetaData()->AppendChild(point);
  }
  QString strProp;
  QMap<int, QVariant>::const_iterator itProp;
  // - POINT:X_SCREEN
  strProp = "+X";
  if ((itProp = properties.find(xScreen)) != properties.end())
    strProp = itProp.value().toString();
  btk::MetaDataCreateChild(point, "X_SCREEN", strProp.toStdString());
  // - POINT:Y_SCREEN
  strProp = "+Z";
  if ((itProp = properties.find(yScreen)) != properties.end())
    strProp = itProp.value().toString();
  btk::MetaDataCreateChild(point, "Y_SCREEN", strProp.toStdString());
  // Video
  std::vector<std::string> movieFilename(this->m_Videos.size());
  std::vector<std::string> movieId(this->m_Videos.size());
  std::vector<float> movieDelay(this->m_Videos.size());
  bool videoCompatibleVicon = true;
  int inc = 0;
  QString fileBaseName = QFileInfo(filename).baseName();
  for (QMap<int,Video*>::const_iterator it = this->m_Videos.begin() ; it != this->m_Videos.end() ; ++it)
  {
    Video* v = it.value();
    movieFilename[inc] = v->filename.toStdString();
    movieDelay[inc] = v->delay / 1000.0f;
    if (videoCompatibleVicon)
    {
      if (v->filename.startsWith(fileBaseName + "." + v->label))
        movieId[inc] = v->label.toStdString();
      else 
        videoCompatibleVicon = false;
    }
    ++inc;
  }
  // - POINT:MOVIE_DELAY
  btk::MetaDataCreateChild(point, "MOVIE_DELAY", movieDelay);
  // Try first to be compatible with VICON using the metadata POINT:MOVIE_ID...
  if (videoCompatibleVicon)
  {
    btk::MetaDataCreateChild(point, "MOVIE_ID", movieId);
    point->RemoveChild("MOVIE_FILENAME");
  }
  // ... Or if it is not possible, then create the metadata POINT:MOVIE_FILENAME
  else
  {
    btk::MetaDataCreateChild(point, "MOVIE_FILENAME", movieFilename);
    point->RemoveChild("MOVIE_ID");
  }
  // Point
  int numFramePoint = rb - lb + 1;
  int numPoints = 0;
  btk::PointCollection::Pointer sourcePoints = source->GetPoints();
  btk::PointCollection::Pointer targetPoints = target->GetPoints();
  for (QMap<int,Point*>::const_iterator it = this->m_Points.begin() ; it != this->m_Points.end() ; ++it)
  {
    Point* p = it.value();
    btk::Point::Type type = btk::Point::Marker;
    if (p->type == Point::Angle)
      type = btk::Point::Angle;
    else if (p->type == Point::Force)
      type = btk::Point::Force;
    else if (p->type == Point::Moment)
      type = btk::Point::Moment;
    else if (p->type == Point::Power)
      type = btk::Point::Power;
    else if (p->type == Point::Scalar)
      type = btk::Point::Scalar;
    btk::Point::Pointer sourceP = sourcePoints->GetItem(p->btkidx);
    btk::Point::Pointer targetP = btk::Point::New(p->label.toStdString(), numFramePoint, type, p->description.toStdString());
    targetP->SetValues(sourceP->GetValues().block(lb-this->m_FirstFrame,0,numFramePoint,3));
    targetP->SetResiduals(sourceP->GetResiduals().block(lb-this->m_FirstFrame,0,numFramePoint,1));
    targetP->SetMasks(sourceP->GetMasks().block(lb-this->m_FirstFrame,0,numFramePoint,1));
    targetPoints->InsertItem(targetP);
    ++numPoints;
  }
  // Analog
  int numAnalogs = 0;
  int numFrameAnalog = numFramePoint * source->GetNumberAnalogSamplePerFrame();
  btk::AnalogCollection::Pointer sourceAnalogs = source->GetAnalogs();
  btk::AnalogCollection::Pointer targetAnalogs = target->GetAnalogs();
  for (QMap<int,Analog*>::const_iterator it = this->m_Analogs.begin() ; it != this->m_Analogs.end() ; ++it)
  {
    Analog* a = it.value();
    btk::Analog::Gain gain = btk::Analog::Unknown;
    if (a->gain == Analog::PlusMinus10)
      gain = btk::Analog::PlusMinus10;
    else if (a->gain == Analog::PlusMinus5)
      gain = btk::Analog::PlusMinus5;
    else if (a->gain == Analog::PlusMinus2Dot5)
      gain = btk::Analog::PlusMinus2Dot5;
    else if (a->gain == Analog::PlusMinus1Dot25)
      gain = btk::Analog::PlusMinus1Dot25;
    else if (a->gain == Analog::PlusMinus1)
      gain = btk::Analog::PlusMinus1;
    btk::Analog::Pointer sourceA = sourceAnalogs->GetItem(it.key());
    btk::Analog::Pointer targetA = btk::Analog::New(a->label.toStdString(), numFrameAnalog);
    targetA->SetUnit(a->unit.toStdString());
    targetA->SetGain(gain);
    targetA->SetScale(a->scale);
    targetA->SetOffset(a->offset);
    targetA->SetDescription(a->description.toStdString());
    targetA->SetValues(sourceA->GetValues().block((lb-this->m_FirstFrame)*source->GetNumberAnalogSamplePerFrame(),0,numFrameAnalog,1));
    targetAnalogs->InsertItem(targetA);
    ++numAnalogs;
  }
  // Final setup
  target->Resize(numPoints, numFramePoint, numAnalogs, source->GetNumberAnalogSamplePerFrame());
  // BTK writer
  btk::AcquisitionFileWriter::Pointer writer = btk::AcquisitionFileWriter::New();
  writer->SetFilename(filename.toStdString());
  writer->SetInput(target);
  try
  {
    writer->Update();
  }
  catch (...)
  {
    return false;
  }
  if (updateInfo)
    this->emitGeneratedInformations(writer->GetAcquisitionIO());
  return true;
}
#endif





bool lhpOpExporterC3DBTK::ExportClouds(btk::Acquisition::Pointer target, std::vector<mafVMELandmarkCloud*>& clouds, std::vector<medVMEAnalog*>& analogs)
{
  std::vector<mafTimeStamp> timeStamps;
  int numberLandmark = 0;
  for(int i = 0; i < clouds.size(); i++)
  {
    mafVMELandmarkCloud *cloud = clouds[i];

    numberLandmark += cloud->GetNumberOfLandmarks();

    std::vector<mafTimeStamp> lmcTimeStamps;
    if(m_GlobalPos)
      cloud->GetAbsTimeStamps(lmcTimeStamps);
    else
      cloud->GetLocalTimeStamps(lmcTimeStamps);

    mmuTimeSet::Merge(timeStamps, lmcTimeStamps, timeStamps);
  }

  int numberAnalog = 0;
  for(int i = 0; i < analogs.size(); i++)
  {
    medVMEAnalog *analog = analogs[i];

    const vnl_matrix<double>& matr = analog->GetScalarOutput()->GetScalarData();
    std::vector<mafTimeStamp> aTimeStamps;
    if(matr.rows() > 0)
    {
      numberAnalog += matr.rows() - 1;
      for(int i = 0; i < matr.columns(); i++)
      {
        aTimeStamps.push_back(matr(0, i));
      }
    }
    mmuTimeSet::Merge(timeStamps, aTimeStamps, timeStamps);
  }

  if(numberLandmark == 0 && numberAnalog == 0)
    return false;

  if(timeStamps.empty())
    return false;

  float freq = 100.0f;
  if(timeStamps.size() >= 2)
  {
    freq = timeStamps.size() - 1;
    freq /= (*(timeStamps.rbegin()) - *(timeStamps.begin()));
  }

  target->Init(numberLandmark, timeStamps.size(), numberAnalog);
  target->SetPointFrequency(freq);
  int firstFrame = (int)((*(timeStamps.begin())) * freq);
  target->SetFirstFrame(firstFrame);
  target->SetPointUnit();

  /*btk::PointCollection::Pointer targetPoints = btk::PointCollection::New();//target->GetPoints();
  
  targetP->SetValues(sourceP->GetValues().block(lb-this->m_FirstFrame,0,numFramePoint,3));
  targetP->SetResiduals(sourceP->GetResiduals().block(lb-this->m_FirstFrame,0,numFramePoint,1));
  targetP->SetMasks(sourceP->GetMasks().block(lb-this->m_FirstFrame,0,numFramePoint,1));
  targetPoints->InsertItem(targetP);
  ++numPoints;*/
  int pointIndex = 0;
  for(int i = 0; i < clouds.size(); i++)
  {
    mafVMELandmarkCloud *cloud = clouds[i];
    bool initState = cloud->IsOpen();
    if(!initState)
      cloud->Open();

    int lmcNumberLandmarks = cloud->GetNumberOfLandmarks();
    for(int j=0; j < lmcNumberLandmarks; j++)
    {
      btk::Point::Pointer targetP = target->GetPoint(pointIndex);
      pointIndex++;
      targetP->SetLabel(cloud->GetLandmarkName(j).GetCStr());

      for (int index = 0; index < timeStamps.size(); index++)
      {
        double t = timeStamps[index];

        //if(!cloud->GetLandmarkVisibility(j, timeStamps[index]))
        //  continue;

        mafMatrix cloudAbs;
        double invec[4];
        double outvec[4];
        cloud->GetOutput()->GetAbsMatrix(cloudAbs, timeStamps[index]);
        cloud->GetLandmark(j, invec, timeStamps[index]);
        invec[3] = 1.0;
        if(m_GlobalPos)
        {
          cloudAbs.MultiplyPoint(invec, outvec);
          for(unsigned indx = 0; indx < 3; indx++)
            invec[indx] = outvec[indx];
        }
        for(int idx = 0; idx < 3; idx++)
          targetP->GetValues()(index, idx) = invec[idx];
        if(cloud->GetLandmarkVisibility(j, index))
          targetP->GetResiduals()[index] = 1;
        else
          targetP->GetResiduals()[index] = -1;
      }
      //targetP->SetValues(sourceP->GetValues().block(lb-this->m_FirstFrame,0,numFramePoint,3));
      //targetP->SetResiduals(sourceP->GetResiduals().block(lb-this->m_FirstFrame,0,numFramePoint,1));
      //targetP->SetMasks(sourceP->GetMasks().block(lb-this->m_FirstFrame,0,numFramePoint,1));
      //targetPoints->InsertItem(targetP);

    }

    // and now, close the cloud
    if(!initState)
      cloud->Close();
  }

  int analogIndex = 0;
  for(int i = 0; i < analogs.size(); i++)
  {
    medVMEAnalog *analog = analogs[i];
    mafTagItem   *namesTag = analog->GetTagArray()->GetTag("SIGNALS_NAME");

    const vnl_matrix<double>& matr = analog->GetScalarOutput()->GetScalarData();
    std::vector<mafTimeStamp> aTimeStamps;
    int aNumberCh = 0;
    if(matr.rows() > 0)
    {
      aNumberCh = matr.rows() - 1;
    }

    int analogIndexDep = analogIndex;
    for(int j = 0; j < aNumberCh; j++)
    {
      btk::Analog::Pointer targetA = target->GetAnalog(analogIndex);
      analogIndex++;
      targetA->SetLabel(namesTag->GetValue(j).GetCStr());
    }
    analogIndex = analogIndexDep;

    for (int index = 0; index < timeStamps.size(); index++)
    {
      double t = timeStamps[index];
      analogIndex = analogIndexDep;

      int ti = 0;
      for(ti = 0; ti < matr.columns() - 1; ti++)
      {
        if(t < matr.get(0, ti + 1))
          break;
      }
      for(int j = 0; j < aNumberCh; j++)
      {
        btk::Analog::Pointer targetA = target->GetAnalog(analogIndex);
        analogIndex++;
        targetA->GetValues()(index) = matr.get(j + 1, ti);
      }
    }
  }

  return true;
}

void lhpOpExporterC3DBTK::ExportingTraverse(mafNode *node, std::vector<mafVMELandmarkCloud*>& clouds, std::vector<medVMEAnalog*>& analogs)
{
  if(node == NULL)
    return;
  if(mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(node))
  {
    clouds.push_back(lmc);
  }
  if(medVMEAnalog *an = medVMEAnalog::SafeDownCast(node))
  {
    analogs.push_back(an);
  }
  int numberChildren = node->GetNumberOfChildren();
  for (int i= 0; i< numberChildren; i++)
  {
    //mafNode *child = node->GetChild(i);
    ExportingTraverse(node->GetChild(i), clouds, analogs);
  }
}
//----------------------------------------------------------------------------
void lhpOpExporterC3DBTK::ExportLandmark()
  //----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, exporting...");
  }
  //file creation
  const char    *fileName = (m_File);

  std::vector<mafVMELandmarkCloud*> clouds;
  std::vector<medVMEAnalog*>        analogs;
  if(m_Input->IsMAFType(mafVMELandmarkCloud) && !m_Subtree)
  {
    clouds.push_back(mafVMELandmarkCloud::SafeDownCast(m_Input));
  }
  else if(m_Input->IsMAFType(medVMEAnalog) && !m_Subtree)
  {
    analogs.push_back(medVMEAnalog::SafeDownCast(m_Input));
  }
  else
  {
    ExportingTraverse(m_Input, clouds, analogs);
  }
  if(!clouds.empty() || !analogs.empty())
  {
    btk::Acquisition::Pointer target = btk::Acquisition::New();
    if(ExportClouds(target, clouds, analogs))
    {
      btk::AcquisitionFileWriter::Pointer writer = btk::AcquisitionFileWriter::New();
      writer->SetFilename(fileName);
      writer->SetInput(target);
      writer->Update();
    }
  }
  if(!m_TestMode)
  {
    delete wait;
  }
}

//----------------------------------------------------------------------------
mafOp* lhpOpExporterC3DBTK::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpExporterC3DBTK *cp = new lhpOpExporterC3DBTK(GetLabel());
  cp->m_Canundo      = m_Canundo;
  cp->m_OpType       = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next         = NULL;
  cp->m_File         = m_File;
  cp->m_Input        = m_Input;
  cp->m_FileDir      = m_FileDir;
  cp->m_GlobalPos    = m_GlobalPos;
  cp->m_Subtree      = m_Subtree;
  return cp;
}

