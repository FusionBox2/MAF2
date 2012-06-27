/*=========================================================================

 Program: MAF2
 Module: mafGUIFloatSlider
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIFloatSlider_H__
#define __mafGUIFloatSlider_H__

//#ifdef __WIN32__
//  #define FLOAT_SLIDER_PARENT wxSlider95
//#else
  #define FLOAT_SLIDER_PARENT wxSlider
//#endif	
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafGUIFloatSlider
/**  
a Slider that can be used to set double values,
mafGUIFloatSlider send usual wxWindows Events;
Use mafGUIFloatSlider in combination with a mafGUIValidator
to receive notification as mafEvents.
*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafGUIFloatSlider: public FLOAT_SLIDER_PARENT
{
    DECLARE_DYNAMIC_CLASS(mafGUIFloatSlider)
public:
	
  mafGUIFloatSlider( wxWindow *parent=NULL, 
									wxWindowID id=-1, 
									const double &value=0, 
									const double &minvalue=0, 
									const double &maxvalue=1,
									const wxPoint &position=wxDefaultPosition, 
									const wxSize& size=wxDefaultSize, 
									long style=wxSL_HORIZONTAL );
  ~mafGUIFloatSlider();
  
  /** Set the slider value. */
	void SetValue(double value);
  
  /** Get the slider value. */
  double GetValue();
  
  /** Set the slider range and value. */
	void SetRange(double min,double max,double value);
  
  /** Set the slider range. */
	void SetRange(double min,double max);
  
  /** Set the number of slider's steps. */
	void SetNumberOfSteps(int num);
  
  /** Get the number of slider's steps. */
	int GetNumberOfSteps() {return this->m_Ticks;};
  
  /** Get the slider's step value. */
  double GetStep() {return this->m_Step;};
  
  /** Set the slider's max value. */
	void SetMax(double max);
  
  /** Get the slider's max value. */
  double GetMax() {return this->m_Max;};
  
  /** Set the slider's min value. */
	void SetMin(double min);
  
  /** Get the slider's min value. */
  double GetMin() {return this->m_Min;};

protected:
	double	Round(double val);
  double	Pow10(double exponent);
	void DiscretizeRangeRange(double min,double max,double n, double *ra,double *rb,double *rs);

	int m_DesiredTicks;
	int m_Ticks;
	
	double m_Value;
	double m_Step;
	double m_Max;
	double m_Min;
};
#endif
