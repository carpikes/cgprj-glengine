#include <GLEngine/Common.h>
#include <GLEngine/Pipeline.h>
#include <GLEngine/Scene.h>
#include <GLEngine/Engine.h>
#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/ResourceManager.h>
#include <GLEngine/Skybox.h>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

using namespace GLEngine;
using namespace std;

TAG_DEF("Main")

std::vector<std::string> getObjFiles();

int main(int argc, char *argv[]) {
    LOG("Starting"); (void) argc; (void) argv;

    vector<MeshPtr> meshes;
    vector<string> objFiles = getObjFiles();

    const int maxNum = 100;

    int q = 0;
    for(string& i : objFiles) {
        if((q++) >= maxNum)
            break;
        MeshPtr j(sEngine.getResourceManager().getMesh(i));
        if(!j) {
            ERRP("Cannot load %s", i.c_str());
            continue;
        }
        meshes.push_back(j);
    }

    MeshPtr pav(sEngine.getResourceManager().getMesh("pav.obj"));
    if(!pav)
        return 0;

    ScenePtr scene = std::make_shared<Scene>();

    for(size_t i=0;i<meshes.size();i++) {
        ObjectPtr poke = std::make_shared<Object>(meshes[i]);
        float px = (float)((int)i % 10) * 30 - 100;
        float py = (float)((int)i / 10) * 30 - 100;
        { // Normalize Mesh
            Box b = meshes[i]->getBoundingBox();
            float dx = (b.vmax[0] - b.vmin[0]);
            float dy = (b.vmax[1] - b.vmin[1]);
            float dz = (b.vmax[2] - b.vmin[2]);
            float dmax = (dx > dy) ? dx : dy;
            dmax = (dmax > dz) ? dmax : dz;
            float sx = 1.0f;
            if(dmax < 5.0f)
                sx = 10.0f / dmax;
            if(dmax > 30.0f)
                sx = 10.0f / dmax;
            poke->setScaling(glm::vec3(sx, sx, sx));
            LOGP("Mesh %u = %f", i, dmax);
        }
        poke->setPosition(glm::vec3(px, 0, py));
        poke->setOrientation(glm::angleAxis(i / 6.28f, glm::vec3(0,1,0)));
        poke->setVideoTag(1 + (int)i);
        scene->addObject(poke);
    }

    ObjectPtr pPav = std::make_shared<Object>(pav);
    pPav->setVideoTag(0);
    scene->addObject(pPav);

    Device *device = sEngine.getDevice();
    DeferredPipeline pipeline(*device);
    
    FirstPersonCamera c(75.0f, 16.0f/9.0f);

    device->registerInputHandler(&c);
    float cnt = 0.0f;

    static float col[][3] = {
        {0.1,0.1,1}, {0.1,1,0.1}, {1,0.1,0.1}, {0.1,0.1,1},
        {0.1,1,0.1}, {1,0.1,0.1}, {0.1,0.1,1}, {0.1,1,0.1},
        {0.1,0.1,1}, {0.1,1,0.1}, {1,0.1,0.1}, {0.1,0.1,1},
        {0.1,1,0.1}, {1,0.1,0.1}, {0.1,0.1,1}, {0.1,1,0.1},
    };

    for(int i=0;i<16;i++) {
        PointLightPtr p = std::make_shared<PointLight>();
        p->setPosition(glm::vec3(0,10,0));
        //p->setAttenuation(glm::vec3(0,0.0, 0.01));
        p->setAttenuation(glm::vec3(0,0,0.0001f));
        p->setAmbientColor(glm::vec3(col[i][0],col[i][1],col[i][2]) * 0.1f);
        p->setDiffuseColor(glm::vec3(col[i][0],col[i][1],col[i][2]) * 0.9f);
        /*
        p->setConeDirection(glm::vec3(0,-1,0));
        p->setCutoffAngleDeg(30.0f);
        p->setExponent(10.0f);
        */
        p->enable();
        scene->addLight(p);
    }

    AmbientLightPtr ambient = std::make_shared<AmbientLight>();
    ambient->setDiffuseColor(glm::vec3(0.1f));
    ambient->setAmbientColor(glm::vec3(0.0f));
    ambient->setDirection(glm::vec3(0,1.0f,0));
    ambient->enable();
    scene->setAmbientLight(ambient);

    HemiLightPtr hemi = std::make_shared<HemiLight>();
    hemi->setPosition(glm::vec3(0,5000,0));
    hemi->setUpColor(glm::vec3(0.0f, 0.0f, 0.0f));
    hemi->setDownColor(glm::vec3(0.1f, 0.1f, 0.1f));
    hemi->enable();
    scene->setHemiLight(hemi);

    std::vector<std::string> files = {
        "../data/box3/ae_ft.tga.png",
        "../data/box3/ae_bk.tga.png",
        "../data/box3/ae_dn.tga.png",
        "../data/box3/ae_up.tga.png",
        "../data/box3/ae_rt.tga.png",
        "../data/box3/ae_lf.tga.png",
    };
    SkyboxPtr skybox = std::make_shared<Skybox>(files);

    scene->setSkybox(skybox);

    scene->loadInDevice(*device);

    auto t1 = Clock::now();
    vector<PointLightPtr>& plights = scene->getLights();
    float mLastExposure = pipeline.getExposure();
    while(device->isRunning()) {

        float avgCol = pipeline.getAvgLight();
        float avgLight = 1.0f - exp(-avgCol * mLastExposure);
        float dist = pow(avgLight - 0.4,4);
        if(avgLight > 0.4f) mLastExposure -= 0.9f * dist;
        if(avgLight < 0.4f) mLastExposure += 0.9f * dist;
        if(mLastExposure < 0.001f) mLastExposure = 0.001f;
        if(mLastExposure > 5.0f) mLastExposure = 5.0f;
        LOGP("AVG Light: %f -> LastExp: %f", avgLight, mLastExposure);

        pipeline.setExposure(mLastExposure);

        c.update();

        for(size_t i=0;i<plights.size();i++) {
            PointLightPtr p = scene->getLights()[i];

            float phase = i / 8.0f * 6.28f + i;
            float x = 0 * cnt * ((i>>3) * 0.1f + 1.0f) + phase;

            glm::vec3 lpos = glm::vec3(
                    sin(x) * (2+i) * 10.0f,
                    3, 
                    cos(x) * (2+i) * 10.0f
            );

            p->setPosition(lpos);
            /*
            SpotLight& s = dynamic_cast<SpotLight&>(*p);
            s.setConeDirection(lpos);
            s.setExponent(20.0f + 10 * pow(cos(cnt * M_PI),6));
            */
        }

        device->beginFrame();
        pipeline.renderFrame(scene, c);
        device->endFrame();

        auto t2 = Clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        t1 = t2;

        const float bpm = 123.0f;
        cnt += ((float)dt / 1000.0f) / (60000.0f / bpm);
        usleep(10000);
    }

    scene->removeFromDevice(*device);
    meshes.clear();
    scene = nullptr;
    return 0;
}

const std::string objFiles[] = {
"poke/001 - Bulbasaur/Bulbasaur.obj",
"poke/003 - Venusaur/Venusaur.obj",
"poke/004 - Charmander/Charmander.obj",
"poke/006 - Charizard/Charizard.obj",
"poke/006 - Charizard/P2_CharizardWP.obj",
"poke/007 - Squirtle/Squirtle.obj",
"poke/009 - Blastoise/Blastoise.obj",
"poke/010 - Caterpie/Caterpie.obj",
"poke/011 - Metapod/Transel.obj",
"poke/012 - Butterfree/Butterfree.obj",
"poke/013 - Weedle/Weedle.obj",
"poke/014 - Kakuna/Kakuna.obj",
"poke/017 - Pidgeotto/Pidgeotto.obj",
"poke/021 - Spearow/Spearow.obj",
"poke/025 - Pikachu/P2_Pikachu.obj",
"poke/025 - Pikachu/Pikachu.obj",
"poke/025 - Pikachu/XY_PikachuF.obj",
"poke/025 - Pikachu/XY_PikachuM.obj",
"poke/026 - Raichu/Raichu.obj",
"poke/026 - Raichu/XY_RaichuF.obj",
"poke/026 - Raichu/XY_RaichuM.obj",
"poke/029 - NidoranF/XY_NidoranF.obj",
"poke/030 - Nidorina/XY_Nidorina.obj",
"poke/031 - Nidoqueen/XY_Nidoqueen.obj",
"poke/032 - NidoranM/XY_NidoranM.obj",
"poke/033 - Nidorino/XY_Nidorino.obj",
"poke/034 - Nidoking/XY_Nidoking.obj",
"poke/037 - Vulpix/Vulpix.obj",
"poke/038 - Ninetales/Ninetales.obj",
"poke/041 - Zubat/Zubat.obj",
"poke/042 - Golbat/Golbat.obj",
"poke/043 - Oddish/Oddish.obj",
"poke/050 - Diglett/Diglett.obj",
"poke/051 - Dugtrio/Dugtrio.obj",
"poke/052 - Meowth/Meowth.obj",
"poke/054 - Psyduck/Psyduck.obj",
"poke/055 - Golduck/Golduck.obj",
"poke/056 - Mankey/Mankey.obj",
"poke/057 - Primeape/Primeape.obj",
"poke/059 - Arcanine/Arcanine.obj",
"poke/059 - Arcanine/WindiePosed.obj",
"poke/063 - Abra/Abra.obj",
"poke/065 - Alakazam/XY_AlakazamF.obj",
"poke/065 - Alakazam/XY_AlakazamMega.obj",
"poke/065 - Alakazam/XY_AlakazamM.obj",
"poke/066 - Machop/XY_Machop.obj",
"poke/067 - Machoke/XY_Machoke.obj",
"poke/068 - Machamp/Machamp.obj",
"poke/068 - Machamp/XY_Machamp.obj",
"poke/070 - Weepinbell/XY_Weepinbell.obj",
"poke/074 - Geodude/Geodude.obj",
"poke/076 - Golem/Golem.obj",
"poke/077 - Ponyta/Ponyta.obj",
"poke/079 - Slowpoke/Slowpoke.obj",
"poke/080 - Slowbro/XY_Slowbro.obj",
"poke/081 - Magnemite/Magnemite.obj",
"poke/083 - Farfetch'd/Farfetch'd.obj",
"poke/092 - Gastly/Gastly.obj",
"poke/093 - Haunter/Haunter.obj",
"poke/094 - Gengar/Gengar.obj",
"poke/098 - Krabby/Krabby.obj",
"poke/100 - Voltorb/Voltorb.obj",
"poke/101 - Electrode/Electrode.obj",
"poke/104 - Cubone/DolKarakara.obj",
"poke/104 - Cubone/XY_Cubone.obj",
"poke/105 - Marowak/Marowak.obj",
"poke/105 - Marowak/XY_Marowak.obj",
"poke/106 - Hitmonlee/Hitmonlee.obj",
"poke/107 - Hitmonchan/Hitmonchan.obj",
"poke/115 - Kangaskhan/XY_KangaskhanMega.obj",
"poke/115 - Kangaskhan/XY_Kangaskhan.obj",
"poke/122 - Mr. Mime/Mr. Mime.obj",
"poke/123 - Scyther/Scyther.obj",
"poke/125 - Electabuzz/Electabuzz.obj",
"poke/127 - Pinsir/XY_PinsirMega.obj",
"poke/127 - Pinsir/XY_Pinsir.obj",
"poke/129 - Magikarp/Magikarp.obj",
"poke/130 - Gyarados/Gyarados.obj",
"poke/131 - Lapras/Lapras.obj",
"poke/133 - Eevee/Eevee.obj",
"poke/133 - Eevee/Eievui Posed.obj",
"poke/133 - Eevee/XY_Eevee.obj",
"poke/134 - Vaporeon/Vaporeon.obj",
"poke/135 - Jolteon/Jolteon.obj",
"poke/136 - Flareon/Flareon.obj",
"poke/137 - Porygon/XY_Porygon.obj",
"poke/138 - Omanyte/XY_Omanyte.obj",
"poke/142 - Aerodactyl/Aerodactyl.obj",
"poke/142 - Aerodactyl/XY_AerodactylMega.obj",
"poke/142 - Aerodactyl/XY_Aerodactyl.obj",
"poke/143 - Snorlax/Snorlax.obj",
"poke/149 - Dragonite/Dragonite.obj",
"poke/151 - Mew/Mew.obj",
"poke/151 - Mew/XY_Mew.obj"
};

std::vector<std::string> getObjFiles() {
    vector<std::string> v;
    for(size_t i=0; i < (sizeof(objFiles) / sizeof(objFiles[0])); i++)
        v.push_back(objFiles[i]);
    return v;
}
