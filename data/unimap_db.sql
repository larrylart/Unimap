
SELECT * FROM sqlite_master WHERE type='table';
.exit

-- Image Group Table
--  
--- DROP TABLE image_groups;
CREATE TABLE image_groups 
(
	group_id INTEGER PRIMARY KEY AUTOINCREMENT, 
	group_name VARCHAR(50) NOT NULL, 
	group_path VARCHAR(255) NOT NULL, 
	images_no INTEGER DEFAULT 0
	
	Registered INTEGER DEFAULT 0,
	StackMethod INTEGER DEFAULT 0,
	BiasImage VARCHAR(255) NOT NULL DEFAULT '',
	DarkImage VARCHAR(255) NOT NULL DEFAULT '',
	FlatImage VARCHAR(255) NOT NULL DEFAULT '',
	StackSigma DOUBLE DEFAULT 0,
	StackIterations INTEGER DEFAULT 0,
	StackBlurMethod INTEGER DEFAULT 0,
	DynamicObjHistCmpMethod INTEGER DEFAULT 0,
	DynamicObjHistNorm DOUBLE DEFAULT 0,
	DynamicObjMatchMaxFwhm DOUBLE DEFAULT 0,
	DynamicObjMinDistMatch DOUBLE DEFAULT 0,
	ObjDynamicsMagTol DOUBLE DEFAULT 0,
	ObjDynamicsKronFactorTol DOUBLE DEFAULT 0,
	ObjDynamicsFwhmTol DOUBLE DEFAULT 0,
	PathDevAngleTolerance DOUBLE DEFAULT 0
); 

-- Astro Image Hints Table
--  
--- DROP TABLE astro_image_hints;
CREATE TABLE astro_image_hints 
(
	hint_id INTEGER PRIMARY KEY AUTOINCREMENT, 
	ImgUseGroup INTEGER DEFAULT 0,
	RelOrigRa DOUBLE DEFAULT 0,
	RelOrigDec DOUBLE DEFAULT 0,
	RelFieldWidth DOUBLE DEFAULT 0,
	RelFieldHeight DOUBLE DEFAULT 0,
	OrigType INTEGER DEFAULT 0,
	ObjOrigType INTEGER DEFAULT 0,
	ObjName INTEGER DEFAULT 0,
	ObjId VARCHAR(50) DEFAULT '',
	OrigUnits INTEGER DEFAULT 0,
	FieldType INTEGER DEFAULT 0,
	HardwareSetup INTEGER DEFAULT 0,
	OrigUnits INTEGER DEFAULT 0,
	SensorWidth DOUBLE DEFAULT 0,
	SensorHeight DOUBLE DEFAULT 0,
	FocalLength DOUBLE DEFAULT 0,
	FieldFormat INTEGER DEFAULT 0,
	
	group_name VARCHAR(50) NOT NULL, 
	group_path VARCHAR(255) NOT NULL, 
	images_no INTEGER DEFAULT 0
); 


-- Group Images Table
--  
--- DROP TABLE group_images;
CREATE TABLE group_images
(
	image_id INTEGER PRIMARY KEY AUTOINCREMENT,
	image_uid VARCHAR(64) NOT NULL,
	image_name VARCHAR(50) NOT NULL, 
	image_path VARCHAR(255) NOT NULL, 
	
); 

-- Image Properties Table
--  
--- DROP TABLE image_props;
CREATE TABLE image_props
(
	image_id INTEGER PRIMARY KEY,
	image_name VARCHAR(50) NOT NULL, 
	image_path VARCHAR(255) NOT NULL, 
	
); 

-- Image Detected Table
--  
--- DROP TABLE image_detected;
CREATE TABLE image_detected
(
	image_id INTEGER PRIMARY KEY,
	image_name VARCHAR(50) NOT NULL, 
	image_path VARCHAR(255) NOT NULL, 
	
); 
