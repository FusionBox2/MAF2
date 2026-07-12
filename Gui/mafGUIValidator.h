#pragma once

#include <wx/validate.h>
#include "wx/listctrl.h"
#include "mafDecl.h"
#include "Base/mfString.h"
#include "mafEventSender.h"

class mafGUIFloatSlider;

enum VALIDATOR_MODES
{
	VAL_WRONG = 0,
	VAL_LABEL,
	VAL_STRING,  //TEXT
	VAL_MAF_STRING,  //TEXT
	VAL_INTEGER, //TEXT 
	VAL_FLOAT,   //TEXT
	VAL_DOUBLE,  //TEXT
	VAL_SLIDER,
	VAL_SLIDER_2,
	VAL_FLOAT_SLIDER,
	VAL_FLOAT_SLIDER_2,
	VAL_CHECKBOX,
	VAL_RADIOBOX,
	VAL_COMBOBOX,
	VAL_LISTBOX,
	VAL_LISTCTRL,
	VAL_BUTTON,
	VAL_FILEOPEN,
	VAL_FILESAVE,
	VAL_DIROPEN,
	VAL_COLOR
};

class MAF_EXPORT mafGUIValidator : public wxValidator, public mafEventSender
{
public:
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxStaticText* win, wxString* var);   //String
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxStaticText* win, mafString* var);  //String
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, wxString* var);   //String
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, mafString* var);  //String
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, int* var, int		min = -2147483647 - 1, int		max = 2147483647);     //Integer
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, float* var, float	min = -1.0e+38F, float	max = 1.0e+38F, int dec_digits = 2);//Float
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, double* var, double min = -1.0e+299, double max = 1.0e+299, int dec_digits = 2);//Double
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxSlider* win, int* var, wxTextCtrl* lab);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, int* var, wxSlider* lab, int min = -2147483647 - 1, int max = 2147483647);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, mafGUIFloatSlider* win, double* var, wxTextCtrl* lab);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxTextCtrl* win, double* var, mafGUIFloatSlider* lab, double min = -1.0e+299, double	max = 1.0e+299);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxCheckBox* win, int* var);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxRadioBox* win, int* var);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxComboBox* win, int* var);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxListBox* win);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxListCtrl* win);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxButton* win);
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxButton* win, mafString* var, wxTextCtrl* lab, bool openfile, const mafString wildcard); // FileOpen/Save
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxButton* win, mafString* var, wxTextCtrl* lab); // DirOpen
	mafGUIValidator(mafBaseEventHandler* listener, int mid, wxButton* win, wxColour* var, wxTextCtrl* lab);

	mafGUIValidator(const mafGUIValidator& val) { Copy(val); }
	~mafGUIValidator() override {}
	wxObject* Clone() const override { return new mafGUIValidator(*this); }
	bool Copy(const mafGUIValidator& val);

	virtual bool IsValid();
	bool Validate(wxWindow* parent) override;
	bool TransferToWindow() override;
	bool TransferFromWindow() override;
	bool TransferFromWindow(bool& changed);

	//	float RoundValue(float f_in);
	//	double RoundValue(double d_in);

	void Init(mafBaseEventHandler* listener, int mid, wxControl* win);
	void OnChar(wxKeyEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void OnScrollEvent(wxScrollEvent& event);
	void OnCommandEvent(wxCommandEvent& event);
	void OnButton(wxCommandEvent& event);

	/** Fill widget_data with the active widget value */
	void GetWidgetData(WidgetDataType& widget_data);

	/** Fill m_WidgetData member variable with widget_data information and validate it */
	void SetWidgetData(WidgetDataType& widget_data);

	DECLARE_EVENT_TABLE()

protected:
	enum VALIDATOR_MODES m_Mode = VAL_WRONG;
	// associated control (one of these)
	wxStaticText* m_StaticText = nullptr;
	wxTextCtrl* m_TextCtrl = nullptr;
	wxSlider* m_Slider = nullptr;
	mafGUIFloatSlider* m_FloatSlider = nullptr;
	wxCheckBox* m_CheckBox = nullptr;
	wxRadioBox* m_RadioBox = nullptr;
	wxComboBox* m_ComboBox = nullptr;
	wxButton* m_Button = nullptr;
	wxListBox* m_ListBox = nullptr;
	wxListCtrl* m_ListCtrl = nullptr;
	// associated variable (one of these)
	float* m_FloatVar = nullptr;
	double* m_DoubleVar = nullptr;
	int* m_IntVar = nullptr;
	wxString* m_StringVar = nullptr;
	mafString* m_MafStringVar = nullptr;
	wxColour* m_ColorVar = nullptr;
	// associated valid range (int or float)
	float m_FloatMax = -1;
	float m_FloatMin = -1;
	double m_DoubleMin = -1;
	double m_DoubleMax = -1;
	int m_IntMax = -1;
	int m_IntMin = -1;

	WidgetDataType m_WidgetData; ///< Used in Collaborative Mode and contains the active widget informations

	int m_ModuleId;
	int m_DecimalDigits = -1; ///< Number of decimal digits to consider and to show into the widget.
	mafString m_Wildcard;
};
