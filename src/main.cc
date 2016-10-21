#include <GLEngine/Common.h>
#include <GLEngine/Renderer.h>
#include <GLEngine/Scene.h>
#include <GLEngine/Engine.h>
#include <GLEngine/FileReader/OBJFileReader.h>
#include <GLEngine/ResourceManager.h>

using namespace GLEngine;
using namespace std;

TAG_DEF("Main")

std::vector<std::string> getObjFiles();

int main(int argc, char *argv[]) {
    LOG("Starting"); (void) argc; (void) argv;

    Engine glEngine("../data");

    vector<MeshPtr> meshes;
    vector<string> objFiles = getObjFiles();

    const int maxNum = 200;

    int q = 0;
    for(string& i : objFiles) {
        if((q++) >= maxNum)
            break;
        MeshPtr j(glEngine.getResourceManager().get<Mesh>(i));
        if(!j) {
            ERRP("Cannot load %s", i.c_str());
            continue;
        }
        meshes.push_back(j);
    }

    MeshPtr pav(glEngine.getResourceManager().get<Mesh>("pav.obj"));
    if(!pav)
        return 0;

    Scene sc(&glEngine);

    for(size_t i=0;i<meshes.size();i++) {
        ObjectPtr pika = std::make_shared<Object>(meshes[i]);
        float px = (float)((int)i % 5) * 20 - (80/2);
        float py = (float)((int)i / 5) * 15 - 50;
        pika->setPosition(glm::vec3(px, 0, py));
        pika->setOrientation(glm::angleAxis(i / 6.28f, glm::vec3(0,1,0)));
        sc.addObject(pika);
    }
    ObjectPtr pPav = std::make_shared<Object>(pav);
    sc.addObject(pPav);

    glEngine.setScene(&sc);

    Renderer *renderer = glEngine.getRenderer();
    FirstPersonCamera c(75.0f, 4.0f/3.0f);

    renderer->registerInputHandler(&c);
    float cnt = 0.0f;

    for(int i=0;i<8;i++)
        renderer->setPointLight(i, glm::vec3(0,0,0), glm::vec3(0,0.3,0),
                                   glm::vec3(1,1,1), glm::vec3(1,1,1),
                                   glm::vec3(1,1,1));
    //c.setCameraPos(glm::vec3(80 * sin(-cnt/2.0f),20,-80 * cos(-cnt/2.0f)));
    while(renderer->isRunning()) {
        //c->setCameraPos(glm::vec3(40,20,-40));
        //sc.setLightRot(glm::vec3(sin(cnt),cos(cnt),0));
        renderer->setAmbientLight(
            glm::vec3(0,-1,0), glm::vec3(1,1,1), 
            glm::vec3(1,1,1),  glm::vec3(1,1,1));
        //sc.setLightRot(glm::normalize(glm::vec3(0,-1,0)));
        //sc.setLightPos(glm::vec3(120 + sin(cnt*50), 50, 120 * cos(cnt*50)));
        //sc.setLightPos(glm::vec3(0, 0, 20));
        //
        /*
        static float col[][3] = {
            {0.2,0.1,1},
            {0.1,0.1,1},
            {0.1,1,0.1},
            {0.1,1,1},
            {1,0.1,0.1},
            {1,0.1,1},
            {1,1,0},
            {1,1,0},
        };

        for(int i=0;i<8;i++) {
            float dist = 30;
            float phase = 1;
            if(i < 4) {
                dist = 60;
                phase = 1.5;
            }

        glm::vec4 lpos = glm::vec4(sin(cnt*phase + i/4.0*6.28) * dist,5, 
                          cos(cnt*phase + i/4.0*6.28) * dist,1);  // position
        lpos = c.getViewMatrix() * lpos;
        renderer->setPointLight(i, glm::vec3(lpos), 
                glm::vec3(0,0.08,0.0001),  // attenuation
                glm::vec3(col[i][0],col[i][1],col[i][2]) * 0.1f, 
                glm::vec3(col[i][0],col[i][1],col[i][2]) * 0.5f, 
                glm::vec3(col[i][0],col[i][1],col[i][2]) * 0.3f);
        }
        */
        //mCurScene->setLightPos(glm::normalize(glm::vec3(0,1,0)));
        c.update();

        renderer->prepareFrame();
        sc.render(&c);
        renderer->endFrame();

        cnt += 0.01f;
    }

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
