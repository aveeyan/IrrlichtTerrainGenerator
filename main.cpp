#include <noise/noise.h>
#include <noise/noiseutils.h>
#include <iostream>
#include <string>
#include <irrlicht/irrlicht.h>
#include <random>
#include <irrlicht/driverChoice.h>

using namespace noise;
using namespace std;
using namespace irr;

string BMPFILENAME;

class Terrain
{
    module::Perlin pModule;
    module::Billow baseflatT;
    module::RidgedMulti mountainT;
    module::Voronoi voroniT;
    module::Terrace terraceT;
    module::ScaleBias flatT;

    utils::NoiseMap pHeightMap;
    utils::NoiseMapBuilderPlane pHeightMapBuilder;
    utils::Image pImage;
    utils::RendererImage pRenderer;
    utils::WriterBMP pWriter;

    public:
        string filename;

        Terrain(){}

        Terrain(string filename="terrain.bmp")
        {
            this->filename=filename;
        }

        void terrainType(int terrainChoice = 0){
            switch (terrainChoice)
            {
                case 0:
                    pHeightMapBuilder.SetSourceModule(pModule);
                    break;
                case 1:
                    pHeightMapBuilder.SetSourceModule(baseflatT);
                    break;
                case 2:
                    pHeightMapBuilder.SetSourceModule(mountainT);
                    break;
                case 3:
                    pHeightMapBuilder.SetSourceModule(voroniT);
                    break;
                case 4:
                    pHeightMapBuilder.SetSourceModule(terraceT);
                    break;
                case 5:
                    pHeightMapBuilder.SetSourceModule(flatT);
                    break;
                default:
                    cout << "Invalid Input.\nExiting Program..." << endl;
                    exit(1);
            }
        }

        void setup(int BMPHeight=256, int BMPWidth=256)
        {
            pHeightMapBuilder.SetDestNoiseMap(pHeightMap);
            pHeightMapBuilder.SetDestSize(BMPHeight, BMPWidth);
        }

        void setSeed(int seed = 0)
        {
            pModule.SetSeed(seed);
        }

        void setBounds(float ux=6.0, float lx=10.0, float y=1.0, float z=5.0)
        {
            pHeightMapBuilder.SetBounds(ux, lx, y, z);
        }

        void build()
        {
            pHeightMapBuilder.Build();
            cout << "SUCCESS: Terrain Built (" << filename << ")" << endl;
        }

        void setBrightness(float contrast=1.0, float brightness=1.0)
        {
            pRenderer.EnableLight ();
            pRenderer.SetLightContrast (contrast);
            pRenderer.SetLightBrightness (brightness);
        }

        void setOctaveCount(int octCount = 6)
        {
            pModule.SetOctaveCount(octCount);
        }

        void setFrequency(float freqCount = 1.0)
        {
            pModule.SetFrequency(freqCount);
        }

        void setPersistence(float persCount = 0.5)
        {
            pModule.SetPersistence(persCount);
        }

        void render(bool clearGradient=true, bool addColor=true, bool setWater=true)
        {
            pRenderer.SetSourceNoiseMap(pHeightMap);
            pRenderer.SetDestImage(pImage);
            if (clearGradient)  {pRenderer.ClearGradient();}
            if (addColor)
            {
                pRenderer.AddGradientPoint ( 0.1250, utils::Color ( 32, 160,   0, 255)); // grass
                pRenderer.AddGradientPoint ( 0.3750, utils::Color (224, 224,   0, 255)); // dirt
                pRenderer.AddGradientPoint ( 0.7500, utils::Color (128, 128, 128, 255)); // rock
                pRenderer.AddGradientPoint ( 1.0000, utils::Color (255, 255, 255, 255)); // snow
                pRenderer.AddGradientPoint ( 0.0625, utils::Color (240, 240,  64, 255)); // sand
            }
            if (setWater)
            {
                pRenderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 128, 255)); // deeps
                pRenderer.AddGradientPoint (-0.2500, utils::Color (  0,   0, 255, 255)); // shallow
                pRenderer.AddGradientPoint ( 0.0000, utils::Color (  0, 128, 255, 255)); // shore
            }
            pRenderer.Render();
            cout << "SUCCESS: Terrain Rendered: " << filename << endl;
        }

        void exportBMP()
        {
            pWriter.SetSourceImage (pImage);
            pWriter.SetDestFilename (filename);
            pWriter.WriteDestFile ();
            cout << "SUCCESS: Terrain Exported: " << filename << endl;
        }
};

    void createTerrain()
    {
        string bmpName;
        int tHeight, tWidth, tSeed, tOctave, tChoice;
        float tBrightness, tContrast, XU, XL, Y, Z, tFrequency, tPersistent, tNoise;
        bool addWater, addColor, clearGradient;

        cout << "Welcome to Perlin Terrain Generator 0.01\n" << endl;
        cout << "ENTER THE FOLLOWING DETAILS: " << endl;

        cout << "Select Your Preferred Terrain Type" << endl;
        do {
            cout << "Enter Values from 1 to 6:" << endl;
            cout << "(1) Perlin Terrain (Basic Hills)" << endl;
            cout << "(2) Billow Terrain (Deep Valleys)" << endl;
            cout << "(3) Ridged Multifractal Terrain (Mountains)" << endl;
            cout << "(4) Voroni Terrain (Polygon Based)" << endl;
            cout << "(5) Terrace Terrain (Steeped)" << endl;
            cout << "(6) ScaleBias Terrain (Flat)" << endl;
            cout << ": ";
            cin >> tChoice;
        } while (tChoice < 0 || tChoice > 6);

        cout << "FileName: ";
        cin >> bmpName;
        Terrain t(bmpName);
        BMPFILENAME = bmpName;
        t.terrainType(tChoice - 1);

        cout << "Seed (default = 0): ";
        cin >> tSeed;
        t.setSeed(tSeed);

        cout << "Brightness (default = 1.0): ";
        cin >> tBrightness;
        cout << "Contrast (default = 1.0): ";
        cin >> tContrast;
        t.setBrightness(tContrast, tBrightness);

        cout << "Bounds (default <6.0, 10.0, 1.0, 5.0>)" << endl;
        cout << "UpperX: ";
        cin >> XU;
        cout << "LowerX: ";
        cin >> XL;
        cout << "Y: ";
        cin >> Y;
        cout << "Z: ";
        cin >> Z;
        t.setBounds(XU, XL, Y, Z);

        cout << "Width (default = 512): ";
        cin >> tWidth;
        cout << "Height (default = 512): ";
        cin >> tHeight;
        t.setup(tWidth, tHeight);

        cout << "Octave (default = 6): ";
        cin >> tOctave;
        t.setOctaveCount(tOctave);

        cout << "Frequency (default = 1.0): ";
        cin >> tFrequency;
        t.setFrequency(tFrequency);

        cout << "Persistence (default = 0.5): ";
        cin >> tPersistent;
        t.setPersistence(tPersistent);

        cout << "Clear Gradient (default = 1): ";
        cin >> clearGradient;

        cout << "Add Color (default = 1): ";
        cin >> addColor;

        cout << "Add Water (default = 1): ";
        cin >> addWater;

        t.build();
        t.render(clearGradient, addColor, addWater);
        t.exportBMP();

        }

int main (int argc, char** argv)
{
    createTerrain();
    core::stringw bmppath = core::stringw(BMPFILENAME.c_str());

    bool fpsMode;
    cout << "\nEnter 1 to enable First Person View: ";
    cin >> fpsMode;

    // Irrlicht Engine Setup

    int screenWidth = 800;
    int screenHeight = 600;

	// Selecting the Driver (ask user)
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

    // Initializing Parameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType=driverType;
	params.WindowSize=core::dimension2d<u32>(screenWidth, screenHeight);

    IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(screenWidth, screenHeight), 16, false, false, false);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, 1.6f, -1, 0, 0, false, 20.f);
	camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);

	// add terrain scene node
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		bmppath,
		0,					// parent node
		-1,					// node id
		core::vector3df(0.f, 0.f, 0.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(40.f, 4.4f, 40.f),	// scale
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		5,					// maxLOD
		scene::ETPS_17,				// patchSize
		5					// smoothFactor
		);

	terrain->setMaterialFlag(video::EMF_LIGHTING, false);

	terrain->setMaterialTexture(0, driver->getTexture(bmppath));
	terrain->setMaterialType(video::EMT_DETAIL_MAP);

	terrain->scaleTexture(1.0f, 20.0f);

	// create triangle selector for the terrain
	scene::ITriangleSelector* selector
		= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);

	// create collision response animator and attach it to the camera
    if (fpsMode){
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
		selector, camera, core::vector3df(60,100,60),
		core::vector3df(0, -70.0f, 0),
		core::vector3df(0, 60 ,0));
	selector->drop();
	camera->addAnimator(anim);
	anim->drop();
	}

    // Create Terrain using Bitmap
	scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(video::EVT_2TCOORDS, video::EIT_16BIT);
	terrain->getMeshBufferForLOD(*buffer, 0);
	video::S3DVertex2TCoords* data = (video::S3DVertex2TCoords*)buffer->getVertexBuffer().getData();
	buffer->drop(); // When done drop the buffer again.

	// create skybox and skydome
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	smgr->addSkyBoxSceneNode(
		driver->getTexture("./skybox/papertop.jpg"),
		driver->getTexture("./skybox/paper.jpg"),
		driver->getTexture("./skybox/paper.jpg"),
		driver->getTexture("./skybox/paper.jpg"),
		driver->getTexture("./skybox/paper.jpg"),
		driver->getTexture("./skybox/paper.jpg"));
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

    // Getting Player Position to track players
    core::vector3df playerPos = core::vector3df(1000, 2000, 1000);
    camera->setPosition(playerPos);

    // Disable mouse cursor and enable when Esc key is pressed
	device->getCursorControl()->setVisible(false);

    int lastFPS = -1;
	while(device->run()){

        if ((camera->getPosition().Y) < -7500)
        {
            camera->setPosition(playerPos);
        }


        if (device->isWindowActive())
        {
		driver->beginScene(true, true, 0 );

		smgr->drawAll();
		env->drawAll();

		driver->endScene();

		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"IrrlichtPTP - Infdev [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			str += " Y: ";
			str += terrain->getHeight(camera->getAbsolutePosition().X,
					camera->getAbsolutePosition().Z);
            str += " X: ";
            str += camera->getAbsolutePosition().X;
            str += " Z: ";
            str += camera->getAbsolutePosition().Z;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}
}
	device->drop();
}
