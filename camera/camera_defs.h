
#ifndef _CAMERA_DEFS_H
#define _CAMERA_DEFS_H

// enum camera hardware piece types
enum
{
	CAMERA_HARDWARE_MAIN_BODY = 0,
	CAMERA_HARDWARE_LENS,
	CAMERA_HARDWARE_REMOTE,
	CAMERA_HARDWARE_FILTER,
	CAMERA_HARDWARE_COOLER
};

// defaults :: camera types
static int g_nCameraTypes = 6;
static DefHarwareDesignType g_vectCameraTypes[10] = 
{ 
	{ wxT("All"), "All", 0, 1 },
	{ wxT("Digital"), "Digital", 1, 0 },
	{ wxT("Digital SLR"), "DigitalSLR", 2, 0 },
	{ wxT("Video"), "Video", 3, 0 },
	{ wxT("Astro"), "Astro", 4, 0 },
	{ wxT("Film"), "Film", 5, 0 }
};

// 8 :: defaults :: camera lens types - http://www.digicamhelp.com/accessories/dslr-accessories/lens-types/
static int g_nCameraLensTypes = 10;
static DefHarwareDesignType g_vectCameraLensTypes[15] = 
{
	{ wxT("All"), "All", 0, 1 },
	// 35mm to 85mm 
	{ wxT("Standard"), "Standard", 1, 0 },
	// 28mm and lower
	{ wxT("Wide Angle"),	"WideAngle", 2, 0 },
	{ wxT("Ultra Wide"),	"UltraWide", 3, 0 },
	// 100mm to 300mm
	{ wxT("Telephoto"), "Telephoto", 4, 0 },	
	// 300mm and above
	{ wxT("Super Telephoto"), "SuperTelephoto", 5, 0 },	
	{ wxT("Macro"), "Macro", 6, 0 },
	// an ultra-wide angle lens that captures a view as much as 180 degrees
	{ wxT("Fisheye"), "Fisheye", 7, 0 },		
	{ wxT("Tilt-Shift"), "TiltShift", 8, 0 },
	{ wxT("Pancake"), "Pancake", 9, 0 }
};

// resolution
typedef struct
{
	int width;
	int height;

} DefCameraRes;

// structure type: camera lens
typedef struct 
{
	wxString name;

	// brand - made by
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	// camera lens type : standard, wide, zoom, telefoto etc 
	unsigned char type;

	double focal_length;
	double aperture;
	double view_angle;
	double closest_focus_distance;

} DefCameraLens;

// structure type: camera brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefCameraLens> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefCameraLensBrand;

// structure type: camera - DefCamera
typedef struct 
{
	wxString name;

	// manufacturer - made by - linke to the company/country where this was made
	wxString manufacturer;
	// brand - the original brand under which this was made
	wxString brand;

	// camera type : video(web), digital, astro ?
	unsigned char type;
	
	// resolutions (max=0)
	std::vector <DefCameraRes> resolution;
	//  low res - vector - also max ... max first ?
	// Image ratio w:h  
	double image_ratio;
	//  ISO rating - vector ?   
	std::vector <int> ISO_rating;

	// Effective pixels 
	unsigned long effective_pixels;
	// Sensor photo detectors
	unsigned long sensor_photo_detectors;

	// sensor info
	wxString sensor_manufacturer;
	unsigned char sensor_type;
	double sensor_width;
	double sensor_height;
	double sensor_psize;
	//	  
	double sensor_pixel_density;
	
	// attach here lens
	DefCameraLens lens;

	// Weight (gramms) (inc. batteries)  .. maybe add lens here as well ?
	double weight;
	// Dimensions      144 x 107 x 81 mm 

} DefCamera;

// structure type: camera brand
typedef struct 
{
	// brand - the original brand under which this was made
	wxString brand_name;
	wxString brand_ini_file;

	std::vector<DefCamera> vectData;
//	int nData;
//	int nDataAllocated;

	// types
	unsigned char vectTypes[50];
	unsigned char nTypes;

} DefCameraBrand;

#endif
