#include "mafPics.h" 

#include <map>

struct mafPictureFactory_Pimpl
{
	std::map<mafString, wxImage> map;
	std::map<mafString, wxImage> vme_map; //here are kept the vme icons
};
#define mafADDPICINIT(X) Add( _R(#X) , X##_xpm )

mafPictureFactory::mafPictureFactory()
{
	m_PictureMaps = std::make_unique<mafPictureFactory_Pimpl>();
#include <pic/FRAME_ICON16x16.xpm>
	mafADDPICINIT(FRAME_ICON16x16);
#include <pic/FRAME_ICON32x32.xpm>
	mafADDPICINIT(FRAME_ICON32x32);
#include <pic/CLOSE_SASH.xpm>
	mafADDPICINIT(CLOSE_SASH);
#include <pic/ROLLOUT_CLOSE.xpm>
	mafADDPICINIT(ROLLOUT_CLOSE);
#include <pic/ROLLOUT_OPEN.xpm>
	mafADDPICINIT(ROLLOUT_OPEN);
#include <pic/FILE_NEW.xpm>    
	mafADDPICINIT(FILE_NEW);
#include <pic/FILE_OPEN.xpm>
	mafADDPICINIT(FILE_OPEN);
#include <pic/FILE_SAVE.xpm>
	mafADDPICINIT(FILE_SAVE);
#include <pic/FLYTO.xpm>
	mafADDPICINIT(FLYTO);
#include <pic/INFO.xpm>
	mafADDPICINIT(INFO);
#include <pic/PRINT.xpm>
	mafADDPICINIT(PRINT);
#include <pic/PRINT_PREVIEW.xpm>
	mafADDPICINIT(PRINT_PREVIEW);
#include <pic/SPLASH_SCREEN.xpm>
	mafADDPICINIT(SPLASH_SCREEN);
#include <pic/MDICHILD_ICON.xpm>
	mafADDPICINIT(MDICHILD_ICON);
#include <pic/OP_COPY.xpm>
	mafADDPICINIT(OP_COPY);
#include <pic/OP_DELETE.xpm>
	mafADDPICINIT(OP_DELETE);
#include <pic/OP_CUT.xpm>
	mafADDPICINIT(OP_CUT);
#include <pic/OP_PASTE.xpm>
	mafADDPICINIT(OP_PASTE);
#include <pic/OP_REDO.xpm>
	mafADDPICINIT(OP_REDO);
#include <pic/OP_UNDO.xpm>
	mafADDPICINIT(OP_UNDO);
#include <pic/PIC_BACK.xpm>
	mafADDPICINIT(PIC_BACK);
#include <pic/PIC_BOTTOM.xpm>
	mafADDPICINIT(PIC_BOTTOM);
#include <pic/PIC_FRONT.xpm>
	mafADDPICINIT(PIC_FRONT);
#include <pic/PIC_LEFT.xpm>
	mafADDPICINIT(PIC_LEFT);
#include <pic/PIC_RIGHT.xpm>
	mafADDPICINIT(PIC_RIGHT);
#include <pic/PIC_TOP.xpm>
	mafADDPICINIT(PIC_TOP);
#include <pic/TIME_BEGIN.xpm>
	mafADDPICINIT(TIME_BEGIN);
#include <pic/TIME_END.xpm>      
	mafADDPICINIT(TIME_END);
#include <pic/TIME_NEXT.xpm>
	mafADDPICINIT(TIME_NEXT);
#include <pic/TIME_PLAY.xpm>
	mafADDPICINIT(TIME_PLAY);
#include <pic/TIME_PREV.xpm>
	mafADDPICINIT(TIME_PREV);
#include <pic/TIME_STOP.xpm>
	mafADDPICINIT(TIME_STOP);
#include <pic/MOVIE_RECORD.xpm>
	mafADDPICINIT(MOVIE_RECORD);
#include <pic/ZOOM.xpm>
	mafADDPICINIT(ZOOM);
#include <pic/ZOOM_ALL.xpm>
	mafADDPICINIT(ZOOM_ALL);
#include <pic/ZOOM_SEL.xpm>
	mafADDPICINIT(ZOOM_SEL);

	// these are used by the ListCtrl
#include <pic/NODE_YELLOW.xpm>
	mafADDPICINIT(NODE_YELLOW);
#include <pic/NODE_RED.xpm>
	mafADDPICINIT(NODE_RED);
#include <pic/NODE_BLUE.xpm>
	mafADDPICINIT(NODE_BLUE);
#include <pic/NODE_GRAY.xpm>
	mafADDPICINIT(NODE_GRAY);

#include <pic/DISABLED.xpm>
	mafADDPICINIT(DISABLED);
#include <pic/RADIO_ON.xpm>
	mafADDPICINIT(RADIO_ON);
#include <pic/RADIO_OFF.xpm>
	mafADDPICINIT(RADIO_OFF);
#include <pic/CHECK_ON.xpm>
	mafADDPICINIT(CHECK_ON);
#include <pic/CHECK_OFF.xpm>
	mafADDPICINIT(CHECK_OFF);
}

mafPictureFactory::~mafPictureFactory() = default;

void mafPictureFactory::Add(const mafString& id, const char** xpm)
{
	m_PictureMaps->map[id] = wxImage(xpm);
}

wxBitmap mafPictureFactory::GetBmp(const mafString& id) const
{
	if (auto it = m_PictureMaps->map.find(id); it != m_PictureMaps->map.end())
	{
		return it->second;
	}
	mafLogMessage(_M(_R("mafPictureFactory: bmp with id = ") + id + _R(" not found")));
	return wxNullBitmap;
}

wxImage mafPictureFactory::GetImg(const mafString& id) const
{
	if (auto it = m_PictureMaps->map.find(id); it != m_PictureMaps->map.end())
	{
		return it->second;
	}
	mafLogMessage(_M(_R("mafPictureFactory: img with id = ") + id + _R(" not found")));
	return wxNullImage;
}

wxIcon mafPictureFactory::GetIcon(const mafString& id) const
{
	if (auto it = m_PictureMaps->map.find(id); it != m_PictureMaps->map.end())
	{
		wxIcon icon;
		icon.CopyFromBitmap(it->second);
		return icon;
	}
	mafLogMessage(_M(_R("mafPictureFactory: icon with id = ") + id + _R(" not found")));
	return wxNullIcon;
}

void mafPictureFactory::AddVmePic(const mafString& id, const char** xpm)
{
	m_PictureMaps->vme_map[id] = wxImage(xpm);
}

wxBitmap mafPictureFactory::GetVmePic(const mafString& id) const
{
	if (auto it = m_PictureMaps->vme_map.find(id); it != m_PictureMaps->vme_map.end())
	{
		return it->second;
	}
	mafLogMessage(_M(_R("mafPictureFactory: vme-pic with id = ") + id + _R(" not found")));
	return wxNullBitmap;

}

std::vector<mafString> mafPictureFactory::GetVmeNames() const
{
	std::vector<mafString> v;
	for (auto& entry : m_PictureMaps->vme_map)
	{
		v.push_back(entry.first);
	}
	return v;
}

mafPictureFactory* mafPictureFactory::GetPictureFactory()
{
	static mafPictureFactory pictureFactory;
	return &pictureFactory;
}
