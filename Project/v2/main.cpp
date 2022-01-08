#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
#define StartEnergy 20
#define StepsToReborn 200
#define GenLength 64
#define TreeX 13
#define TreeY 17
#define TreeZ 13

using namespace sf; using namespace std;

float ScreenSize[2] = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) }, TimesGone, WindowHaveFocusCD = 0, cd = 0;
GLuint vertexVBO = 0; GLuint colorVBO[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; GLuint indexEBO = 0; RenderWindow window;
int FPS[5], fpscounter = 0, step = 0, generation = 0; bool work = true, sunmode = false, stop = false, mode = false, timemode = false; 
const int qxx = 9, qzz = 9, showxx = 2, showzz = 2; int bests[showxx * showzz][2];
float TimeToStep = 50;

struct  Mappp
{
    int themap[TreeX][TreeY][TreeZ];
};

struct Tree
{
    Mappp themap;
    Mappp themapbuf;
    int Genom[GenLength][6];
    double energy = StartEnergy;
    bool dead = false;
    Tree()
    {
        for (int x = 0; x < TreeX; x++) for (int y = 0; y < TreeY; y++) for (int z = 0; z < TreeZ; z++) themap.themap[x][y][z] = -2;
        themap.themap[TreeX / 2][TreeY / 2][TreeZ / 2] = 0;
    }
    void GenGenom()
    {
        for (int x = 0; x < GenLength; x++) for (int y = 0; y < 6; y++) Genom[x][y] = rand() % (GenLength * 3);
    }
    void Born()
    {
        for (int x = 0; x < TreeX; x++) for (int y = 0; y < TreeY; y++) for (int z = 0; z < TreeZ; z++) themap.themap[x][y][z] = -2;
        themap.themap[TreeX / 2][TreeY / 2][TreeZ / 2] = 0; energy = StartEnergy; dead = false;
    }
    void GrewUp()
    {
        themapbuf = themap;
        if (energy >= 0)
        {
            float sunener = 0, earthener = 0;
            for (int x = 0; x < TreeX; x++) for (int y = 0; y < TreeY; y++) for (int z = 0; z < TreeZ; z++)
            {
                if (themap.themap[x][y][z] >= 0)
                {
                    if (x + 1 < TreeX) if (themap.themap[x + 1][y][z] == -2) if (Genom[themap.themap[x][y][z]][0] < GenLength)
                        themapbuf.themap[x + 1][y][z] = Genom[themap.themap[x][y][z]][0];
                    if (x - 1 >= 0) if (themap.themap[x - 1][y][z] == -2) if (Genom[themap.themap[x][y][z]][1] < GenLength)
                        themapbuf.themap[x - 1][y][z] = Genom[themap.themap[x][y][z]][1];
                    if (y + 1 < TreeY) if (themap.themap[x][y + 1][z] == -2) if (Genom[themap.themap[x][y][z]][2] < GenLength)
                        themapbuf.themap[x][y + 1][z] = Genom[themap.themap[x][y][z]][2];
                    if (y - 1 >= 0) if (themap.themap[x][y - 1][z] == -2) if (Genom[themap.themap[x][y][z]][3] < GenLength)
                        themapbuf.themap[x][y - 1][z] = Genom[themap.themap[x][y][z]][3];
                    if (z + 1 < TreeZ) if (themap.themap[x][y][z + 1] == -2) if (Genom[themap.themap[x][y][z]][4] < GenLength)
                        themapbuf.themap[x][y][z + 1] = Genom[themap.themap[x][y][z]][4];
                    if (z - 1 >= 0) if (themap.themap[x][y][z - 1] == -2) if (Genom[themap.themap[x][y][z]][5] < GenLength)
                        themapbuf.themap[x][y][z - 1] = Genom[themap.themap[x][y][z]][5];
                    themapbuf.themap[x][y][z] = -1;
                    themap.themap[x][y][z] = -1;
                }
                if (themap.themap[x][y][z] == -1)
                {
                    energy -= 4.5;
                    if (y >= TreeY / 2)
                    {
                        int need = 3, need1 = -1;
                        for (int yy = TreeY; yy > y; yy--)
                        {
                            if (themap.themap[x][yy][z] != -2) need--; else if (need < 3) need++;
                        }
                        if (need < 1) need = 0;
                        for (int xx = -1; xx <= 1; xx++) for (int yy = -1; yy <= 1; yy++) for (int zz = -1; zz <= 1; zz++)
                            if (x + xx >= 0 && x + xx < TreeX && y + yy >= TreeY / 2 && y + yy < TreeY && z + zz >= 0 && z + zz < TreeZ)
                                if (themap.themap[x + xx][y + yy][z + zz] != -2) need1++;
                        energy += pow(4.8 - need1 * 0.02, (1.1 + double(y - TreeY / 2) * 0.03125)) * (pow((double)need, 1.1) * 0.3 + 0.1);
                    }
                    else
                    {
                        int need = -1;
                        for (int xx = -2; xx <= 2; xx++) for (int yy = -2; yy <= 2; yy++) for (int zz = -2; zz <= 2; zz++)
                            if (x + xx >= 0 && x + xx < TreeX && y + yy >= 0 && y + yy < (TreeY / 2) + 1 && z + zz >= 0 && z + zz < TreeZ)
                                if (themap.themap[x + xx][y + yy][z + zz] != -2) need += abs(xx) * abs(yy) + abs(xx) * abs(zz) + abs(zz) * abs(yy) + 1;
                        energy += pow(7.0, (1.0 + double(TreeY / 2 - y) * 0.06)) / ((double)need * 0.012 + 1.0f);
                    }
                }
            }
            themap = themapbuf;
            if (energy < 0) this->Die();
        }
    }
    void Die()
    {
        dead = true; for (int x = 0; x < TreeX; x++) for (int y = 0; y < TreeY; y++) for (int z = 0; z < TreeZ; z++) themap.themap[x][y][z] = -2;
    }
    void Evolve()
    {
        for (int i = 0; i < 32; i++) if (rand() % 4 == 0) Genom[rand() % GenLength][rand() % 6] = rand() % (GenLength * 3);
    }
};

struct God
{
public:
    Vector3f pos = Vector3f(0, 0, 0); float horang = 0, verang = 0;
    God(){}
};

Tree trees[qxx][qzz];
God god;

void Control()
{
    if (window.hasFocus())
    {
        if (WindowHaveFocusCD < 200) { WindowHaveFocusCD += TimesGone; SetCursorPos(ScreenSize[0] / 2, ScreenSize[1] / 2); }
        else
        {
            Vector2i MousePos; MousePos = Mouse::getPosition(window); float modif = 0.0001f * TimesGone;
            if (Keyboard::isKeyPressed(Keyboard::Escape)) { window.close(); work = false; }
            if (Keyboard::isKeyPressed(Keyboard::O)) { stop = !stop; Sleep(750); }
            if (Keyboard::isKeyPressed(Keyboard::M)) { mode = !mode; Sleep(750); }
            if (Keyboard::isKeyPressed(Keyboard::U)) { sunmode = !sunmode; Sleep(750); }
            if (Keyboard::isKeyPressed(Keyboard::T)) { timemode = !timemode; Sleep(750); TimeToStep = 50; if (timemode) TimeToStep += 350; }
            if (Keyboard::isKeyPressed(Keyboard::R)) cout << "Fps: " << (FPS[0] + FPS[1] + FPS[2] + FPS[3] + FPS[4]) / 5 << endl;
            god.horang += ((ScreenSize[0] / 2) - MousePos.x) * TimesGone * 0.07f; god.verang += ((ScreenSize[1] / 2) - MousePos.y) * TimesGone * 0.07f;
            SetCursorPos(ScreenSize[0] / 2, ScreenSize[1] / 2); if (abs(god.verang) > 90) god.verang = 90 * god.verang / abs(god.verang);
            if (god.horang > 360) god.horang -= 360; if (god.horang < 0) god.horang += 360;
            if (Keyboard::isKeyPressed(Keyboard::LShift)) modif *= 2;
            modif *= 20;
            if (Keyboard::isKeyPressed(Keyboard::Space)) god.pos.y += modif * 1.4f; if (Keyboard::isKeyPressed(Keyboard::LAlt)) god.pos.y -= modif * 1.4f;
            if (Keyboard::isKeyPressed(Keyboard::W)) { god.pos.z += cos(god.horang * 0.0175f) * modif * 5; god.pos.x += sin(god.horang * 0.0175f) * modif * 5; }
            if (Keyboard::isKeyPressed(Keyboard::S)) { god.pos.z -= cos(god.horang * 0.0175f) * modif * 5; god.pos.x -= sin(god.horang * 0.0175f) * modif * 5; }
            if (Keyboard::isKeyPressed(Keyboard::A)) { god.pos.z -= sin(god.horang * 0.0175f) * modif * 5; god.pos.x += cos(god.horang * 0.0175f) * modif * 5; }
            if (Keyboard::isKeyPressed(Keyboard::D)) { god.pos.z += sin(god.horang * 0.0175f) * modif * 5; god.pos.x -= cos(god.horang * 0.0175f) * modif * 5; }
        }
    }
    else WindowHaveFocusCD = 0;
}
void DrawAll()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); glClear(GL_DEPTH_BUFFER_BIT);
    for (int sx = 0; sx < showxx; sx++) for (int sz = 0; sz < showzz; sz++)
    {
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        gluLookAt(god.pos.x, god.pos.y, god.pos.z,
            god.pos.x + sin(god.horang * 0.01745),
            god.pos.y + sin(god.verang * 0.01745),
            god.pos.z + cos(god.horang * 0.01745), 0, 1, 0);
        glTranslatef(sx * 26.0f, 0.25f, sz * 26.0f);
        glScalef(13.0f, 0.5f, 13.0f);
        glEnableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO[2 + mode * 5]); glColorPointer(4, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, NULL);
        glScalef(1.0f, 1.0f, 1.0f);
        int x = bests[sx * sz + sz][0], z = bests[sx * sz + sz][1];
        for (int xx = 0; xx < TreeX; xx++) for (int zz = 0; zz < TreeZ; zz++) for (int yy = -(TreeY / 2); yy <= (TreeY / 2); yy++)// * (god.pos.y > 0.5)
            if (trees[x][z].themap.themap[xx][yy + (TreeY / 2)][zz] != -2)
                if (!mode || !((trees[x][z].themap.themap[xx + 1][yy + (TreeY / 2)][zz] != -2 && xx < TreeX) && (trees[x][z].themap.themap[xx - 1][yy + (TreeY / 2)][zz] != -2 && xx > 0) &&
                    (trees[x][z].themap.themap[xx][yy + (TreeY / 2) + 1][zz] != -2 && yy < TreeY) && (trees[x][z].themap.themap[xx][yy + (TreeY / 2) - 1][zz] != -2 && yy > 0) &&
                    (trees[x][z].themap.themap[xx][yy + (TreeY / 2)][zz + 1] != -2 && zz < TreeZ) && (trees[x][z].themap.themap[xx][yy + (TreeY / 2)][zz - 1] != -2 && zz > 0)))
                {
                    int need = 0;
                    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); 
                    gluLookAt(god.pos.x, god.pos.y, god.pos.z,
                        god.pos.x + sin(god.horang * 0.01745),
                        god.pos.y + sin(god.verang * 0.01745),
                        god.pos.z + cos(god.horang * 0.01745), 0, 1, 0);
                    glTranslatef(sx * (TreeX * 2) + xx, yy, sz * (TreeZ * 2) + zz);
                    if (yy < 1)
                    {
                        if (sunmode)
                        {
                            int need1 = -1;
                            for (int xxx = -2; xxx <= 2; xxx++) for (int yyy = -2; yyy <= 2; yyy++) for (int zzz = -2; zzz <= 2; zzz++)
                                if (xx + xxx >= 0 && xx + xxx < TreeX && (yy + (TreeY / 2)) + yyy >= 0 
                                    && (yy + (TreeY / 2)) + yyy < (TreeY / 2) + 1 && zz + zzz >= 0 && zz + zzz < TreeZ)
                                    if (trees[x][z].themap.themap[xx + xxx][(yy + (TreeY / 2)) + yyy][zz + zzz] != -2)
                                        need1 += abs(xxx) * abs(yyy) + abs(xxx) * abs(zzz) + abs(zzz) * abs(yyy) + 1;
                            float ener = pow(7.0, (1.0 + double(-1 * yy) * 0.06)) / ((double)need1 * 0.012 + 1.0f);
                            glDisableClientState(GL_COLOR_ARRAY);
                            glColor4f(((ener - 1.25) / (16.2)) * 0.8 + 0.2, ((ener - 1.25) / (16.2)) * 0.8 + 0.2, 0.2, 0.5 + 0.5 * mode);
                        }
                        else
                        {
                            for (int yyy = yy + 1; yyy < 1; yyy++) if (trees[x][z].themap.themap[xx][yyy + (TreeY / 2)][zz] != -2) need++;
                            glScalef(1.0f, need + 1.0f, 1.0f);
                            if (trees[x][z].themap.themap[xx][yy + (TreeY / 2)][zz] == -1)
                            {
                                glBindBuffer(GL_ARRAY_BUFFER, colorVBO[4 + mode * 5]); glColorPointer(4, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
                            }
                            else
                            {
                                glBindBuffer(GL_ARRAY_BUFFER, colorVBO[1 + mode * 5]); glColorPointer(4, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
                            }
                        }
                    }
                    else
                    {
                        if (sunmode)
                        {
                            int need2 = 3, need1 = -1; float need3;
                            for (int yyy = TreeY; yyy > yy + (TreeY / 2); yyy--)
                            {
                                if (trees[x][z].themap.themap[xx][yyy][zz] != -2) need2--; else if (need2 < 3) need2++;
                            }
                            if (need2 < 1) need2 = 0;
                            for (int xxx = -1; xxx <= 1; xxx++) for (int yyy = -1; yyy <= 1; yyy++) for (int zzz = -1; zzz <= 1; zzz++)
                                if (xx + xxx >= 0 && xx + xxx < TreeX && (yy + (TreeY / 2)) + yyy >= TreeY / 2 
                                    && (yy + (TreeY / 2)) + yyy < TreeY && zz + zzz >= 0 && zz + zzz < TreeZ)
                                    if (trees[x][z].themap.themap[xx + xxx][(yy + (TreeY / 2)) + yyy][zz + zzz] != -2) need1++;
                            float ener = pow(4.8 - need1 * 0.02, (1.1 + double(yy) * 0.03125)) * (pow((double)need2, 1.1) * 0.3 + 0.1);
                            glDisableClientState(GL_COLOR_ARRAY);
                            glColor4f(0.2, ((ener - 0.5) / (12.0)) * 0.8 + 0.2, 0.2, 0.5 + 0.5 * mode);
                        }
                        else
                        {
                            for (int yyy = yy + 1; yyy <= (TreeY / 2); yyy++) if (trees[x][z].themap.themap[xx][yyy + (TreeY / 2)][zz] != -2) need++;
                            glScalef(1.0f, need + 1.0f, 1.0f);
                            if (trees[x][z].themap.themap[xx][yy + (TreeY / 2)][zz] == -1)
                            {
                                glBindBuffer(GL_ARRAY_BUFFER, colorVBO[3 + mode * 5]); glColorPointer(4, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
                            }
                            else
                            {
                                glBindBuffer(GL_ARRAY_BUFFER, colorVBO[mode * 5]); glColorPointer(4, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
                            }
                        }
                    }
                    glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, NULL);
                    yy += need;
                }
    }
}
void Init()
{
    glewExperimental = GL_TRUE; glewInit(); glEnable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glEnable(GL_CULL_FACE);
    srand(time(NULL)); glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glViewport(0, 0, ScreenSize[0], ScreenSize[1]); gluPerspective(90.0f, 1.5f, 0.1f, 20000.0f);
    glGenBuffers(1, &vertexVBO);
    float* Vertexes; Vertexes = new float[42];
    Vertexes[0] = 0;  Vertexes[1] = 0;  Vertexes[2] = 0;Vertexes[3] = 1;  Vertexes[4] = 0;  Vertexes[5] = 0;
    Vertexes[6] = 1;  Vertexes[7] = 0;  Vertexes[8] = 1;Vertexes[9] = 0;  Vertexes[10] = 0; Vertexes[11] = 1;
    Vertexes[12] = 0; Vertexes[13] = 1; Vertexes[14] = 0;Vertexes[15] = 1; Vertexes[16] = 1; Vertexes[17] = 0;
    Vertexes[18] = 1; Vertexes[19] = 1; Vertexes[20] = 1;Vertexes[21] = 0; Vertexes[22] = 1; Vertexes[23] = 1;
    Vertexes[24] = 0;  Vertexes[25] = 0.5; Vertexes[26] = 0.5;Vertexes[27] = 0.5; Vertexes[28] = 0.5; Vertexes[29] = 0;
    Vertexes[30] = 0.5; Vertexes[31] = 0; Vertexes[32] = 0.5;Vertexes[33] = 1;  Vertexes[34] = 0.5; Vertexes[35] = 0.5;
    Vertexes[36] = 0.5; Vertexes[37] = 0.5; Vertexes[38] = 1;Vertexes[39] = 0.5; Vertexes[40] = 1; Vertexes[41] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(float), Vertexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); delete[] Vertexes;
    glGenBuffers(1, &indexEBO);
    int* Indexes; Indexes = new int[72];
    Indexes[0] = 1; Indexes[1] = 0; Indexes[2] = 9; Indexes[3] = 5; Indexes[4] = 1; Indexes[5] = 9;
    Indexes[6] = 4; Indexes[7] = 5; Indexes[8] = 9;Indexes[9] = 0; Indexes[10] = 4; Indexes[11] = 9;
    Indexes[12] = 4; Indexes[13] = 0; Indexes[14] = 8;Indexes[15] = 7; Indexes[16] = 4; Indexes[17] = 8;
    Indexes[18] = 3; Indexes[19] = 7; Indexes[20] = 8;Indexes[21] = 0; Indexes[22] = 3; Indexes[23] = 8;
    Indexes[24] = 3; Indexes[25] = 0; Indexes[26] = 10;Indexes[27] = 2; Indexes[28] = 3; Indexes[29] = 10;
    Indexes[30] = 1; Indexes[31] = 2; Indexes[32] = 10;Indexes[33] = 0; Indexes[34] = 1; Indexes[35] = 10;
    Indexes[36] = 6; Indexes[37] = 7; Indexes[38] = 12;Indexes[39] = 7; Indexes[40] = 3; Indexes[41] = 12;
    Indexes[42] = 3; Indexes[43] = 2; Indexes[44] = 12;Indexes[45] = 2; Indexes[46] = 6; Indexes[47] = 12;
    Indexes[48] = 6; Indexes[49] = 2; Indexes[50] = 11;Indexes[51] = 2; Indexes[52] = 1; Indexes[53] = 11;
    Indexes[54] = 1; Indexes[55] = 5; Indexes[56] = 11;Indexes[57] = 5; Indexes[58] = 6; Indexes[59] = 11;
    Indexes[60] = 6; Indexes[61] = 5; Indexes[62] = 13;Indexes[63] = 5; Indexes[64] = 4; Indexes[65] = 13;
    Indexes[66] = 4; Indexes[67] = 7; Indexes[68] = 13;Indexes[69] = 7; Indexes[70] = 6; Indexes[71] = 13;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 72 * sizeof(int), Indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    delete[] Indexes;
    glGenBuffers(1, &colorVBO[0]);
    float* Colors; Colors = new float[56];
    Colors[0] = 0;  Colors[1] = 1;  Colors[2] = 0;  Colors[3] = 0.5;
    Colors[4] = 0;  Colors[5] = 1;  Colors[6] = 0;  Colors[7] = 0.5;
    Colors[8] = 0;  Colors[9] = 1;  Colors[10] = 0; Colors[11] = 0.5;
    Colors[12] = 0; Colors[13] = 1; Colors[14] = 0; Colors[15] = 0.5;
    Colors[16] = 0; Colors[17] = 1; Colors[18] = 0; Colors[19] = 0.5;
    Colors[20] = 0; Colors[21] = 1; Colors[22] = 0; Colors[23] = 0.5;
    Colors[24] = 0; Colors[25] = 1; Colors[26] = 0; Colors[27] = 0.5;
    Colors[28] = 0; Colors[29] = 1; Colors[30] = 0; Colors[31] = 0.5;
    Colors[32] = 0; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 0.5;
    Colors[36] = 0; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 0.5;
    Colors[40] = 0; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 0.5;
    Colors[44] = 0; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 0.5;
    Colors[48] = 0; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 0.5;
    Colors[52] = 0; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glGenBuffers(1, &colorVBO[1]);
    Colors[0] = 1;  Colors[1] = 1;  Colors[2] = 0;  Colors[3] = 0.5;
    Colors[4] = 1;  Colors[5] = 1;  Colors[6] = 0;  Colors[7] = 0.5;
    Colors[8] = 1;  Colors[9] = 1;  Colors[10] = 0; Colors[11] = 0.5;
    Colors[12] = 1; Colors[13] = 1; Colors[14] = 0; Colors[15] = 0.5;
    Colors[16] = 1; Colors[17] = 1; Colors[18] = 0; Colors[19] = 0.5;
    Colors[20] = 1; Colors[21] = 1; Colors[22] = 0; Colors[23] = 0.5;
    Colors[24] = 1; Colors[25] = 1; Colors[26] = 0; Colors[27] = 0.5;
    Colors[28] = 1; Colors[29] = 1; Colors[30] = 0; Colors[31] = 0.5;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 0.5;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 0.5;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 0.5;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 0.5;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 0.5;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[2]);
    Colors[0] = 0.5;  Colors[1] = 0.3;  Colors[2] = 0.1;  Colors[3] = 0.5;
    Colors[4] = 0.5;  Colors[5] = 0.3;  Colors[6] = 0.1;  Colors[7] = 0.5;
    Colors[8] = 0.5;  Colors[9] = 0.3;  Colors[10] = 0.1; Colors[11] = 0.5;
    Colors[12] = 0.5; Colors[13] = 0.3; Colors[14] = 0.1; Colors[15] = 0.5;
    Colors[16] = 0.5; Colors[17] = 0.3; Colors[18] = 0.1; Colors[19] = 0.5;
    Colors[20] = 0.5; Colors[21] = 0.3; Colors[22] = 0.1; Colors[23] = 0.5;
    Colors[24] = 0.5; Colors[25] = 0.3; Colors[26] = 0.1; Colors[27] = 0.5;
    Colors[28] = 0.5; Colors[29] = 0.3; Colors[30] = 0.1; Colors[31] = 0.5;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0.1; Colors[35] = 0.5;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0.1; Colors[39] = 0.5;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0.1; Colors[43] = 0.5;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0.1; Colors[47] = 0.5;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0.1; Colors[51] = 0.5;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0.1; Colors[55] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[3]);
    Colors[0] = 0;  Colors[1] = 0.8;  Colors[2] = 0;  Colors[3] = 0.5;
    Colors[4] = 0;  Colors[5] = 0.8;  Colors[6] = 0;  Colors[7] = 0.5;
    Colors[8] = 0;  Colors[9] = 0.8;  Colors[10] = 0; Colors[11] = 0.5;
    Colors[12] = 0; Colors[13] = 0.8; Colors[14] = 0; Colors[15] = 0.5;
    Colors[16] = 0; Colors[17] = 0.8; Colors[18] = 0; Colors[19] = 0.5;
    Colors[20] = 0; Colors[21] = 0.8; Colors[22] = 0; Colors[23] = 0.5;
    Colors[24] = 0; Colors[25] = 0.8; Colors[26] = 0; Colors[27] = 0.5;
    Colors[28] = 0; Colors[29] = 0.8; Colors[30] = 0; Colors[31] = 0.5;
    Colors[32] = 0; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 0.5;
    Colors[36] = 0; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 0.5;
    Colors[40] = 0; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 0.5;
    Colors[44] = 0; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 0.5;
    Colors[48] = 0; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 0.5;
    Colors[52] = 0; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[3]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[4]);
    Colors[0] = 0.8;  Colors[1] = 0.8;  Colors[2] = 0;  Colors[3] = 0.5;
    Colors[4] = 0.8;  Colors[5] = 0.8;  Colors[6] = 0;  Colors[7] = 0.5;
    Colors[8] = 0.8;  Colors[9] = 0.8;  Colors[10] = 0; Colors[11] = 0.5;
    Colors[12] = 0.8; Colors[13] = 0.8; Colors[14] = 0; Colors[15] = 0.5;
    Colors[16] = 0.8; Colors[17] = 0.8; Colors[18] = 0; Colors[19] = 0.5;
    Colors[20] = 0.8; Colors[21] = 0.8; Colors[22] = 0; Colors[23] = 0.5;
    Colors[24] = 0.8; Colors[25] = 0.8; Colors[26] = 0; Colors[27] = 0.5;
    Colors[28] = 0.8; Colors[29] = 0.8; Colors[30] = 0; Colors[31] = 0.5;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 0.5;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 0.5;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 0.5;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 0.5;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 0.5;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 0.5;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[4]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[5]);
    Colors[0] = 0;  Colors[1] = 1;  Colors[2] = 0;  Colors[3] = 1;
    Colors[4] = 0;  Colors[5] = 1;  Colors[6] = 0;  Colors[7] = 1;
    Colors[8] = 0;  Colors[9] = 1;  Colors[10] = 0; Colors[11] = 1;
    Colors[12] = 0; Colors[13] = 1; Colors[14] = 0; Colors[15] = 1;
    Colors[16] = 0; Colors[17] = 1; Colors[18] = 0; Colors[19] = 1;
    Colors[20] = 0; Colors[21] = 1; Colors[22] = 0; Colors[23] = 1;
    Colors[24] = 0; Colors[25] = 1; Colors[26] = 0; Colors[27] = 1;
    Colors[28] = 0; Colors[29] = 1; Colors[30] = 0; Colors[31] = 1;
    Colors[32] = 0; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 1;
    Colors[36] = 0; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 1;
    Colors[40] = 0; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 1;
    Colors[44] = 0; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 1;
    Colors[48] = 0; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 1;
    Colors[52] = 0; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[5]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[6]);
    Colors[0] = 1;  Colors[1] = 1;  Colors[2] = 0;  Colors[3] = 1;
    Colors[4] = 1;  Colors[5] = 1;  Colors[6] = 0;  Colors[7] = 1;
    Colors[8] = 1;  Colors[9] = 1;  Colors[10] = 0; Colors[11] = 1;
    Colors[12] = 1; Colors[13] = 1; Colors[14] = 0; Colors[15] = 1;
    Colors[16] = 1; Colors[17] = 1; Colors[18] = 0; Colors[19] = 1;
    Colors[20] = 1; Colors[21] = 1; Colors[22] = 0; Colors[23] = 1;
    Colors[24] = 1; Colors[25] = 1; Colors[26] = 0; Colors[27] = 1;
    Colors[28] = 1; Colors[29] = 1; Colors[30] = 0; Colors[31] = 1;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 1;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 1;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 1;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 1;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 1;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[6]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[7]);
    Colors[0] = 0.5;  Colors[1] = 0.3;  Colors[2] = 0.1;  Colors[3] = 1;
    Colors[4] = 0.5;  Colors[5] = 0.3;  Colors[6] = 0.1;  Colors[7] = 1;
    Colors[8] = 0.5;  Colors[9] = 0.3;  Colors[10] = 0.1; Colors[11] = 1;
    Colors[12] = 0.5; Colors[13] = 0.3; Colors[14] = 0.1; Colors[15] = 1;
    Colors[16] = 0.5; Colors[17] = 0.3; Colors[18] = 0.1; Colors[19] = 1;
    Colors[20] = 0.5; Colors[21] = 0.3; Colors[22] = 0.1; Colors[23] = 1;
    Colors[24] = 0.5; Colors[25] = 0.3; Colors[26] = 0.1; Colors[27] = 1;
    Colors[28] = 0.5; Colors[29] = 0.3; Colors[30] = 0.1; Colors[31] = 1;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0.1; Colors[35] = 1;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0.1; Colors[39] = 1;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0.1; Colors[43] = 1;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0.1; Colors[47] = 1;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0.1; Colors[51] = 1;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0.1; Colors[55] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[7]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[8]);
    Colors[0] = 0;  Colors[1] = 0.8;  Colors[2] = 0;  Colors[3] = 1;
    Colors[4] = 0;  Colors[5] = 0.8;  Colors[6] = 0;  Colors[7] = 1;
    Colors[8] = 0;  Colors[9] = 0.8;  Colors[10] = 0; Colors[11] = 1;
    Colors[12] = 0; Colors[13] = 0.8; Colors[14] = 0; Colors[15] = 1;
    Colors[16] = 0; Colors[17] = 0.8; Colors[18] = 0; Colors[19] = 1;
    Colors[20] = 0; Colors[21] = 0.8; Colors[22] = 0; Colors[23] = 1;
    Colors[24] = 0; Colors[25] = 0.8; Colors[26] = 0; Colors[27] = 1;
    Colors[28] = 0; Colors[29] = 0.8; Colors[30] = 0; Colors[31] = 1;
    Colors[32] = 0; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 1;
    Colors[36] = 0; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 1;
    Colors[40] = 0; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 1;
    Colors[44] = 0; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 1;
    Colors[48] = 0; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 1;
    Colors[52] = 0; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[8]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorVBO[9]);
    Colors[0] = 0.8;  Colors[1] = 0.8;  Colors[2] = 0;  Colors[3] = 1;
    Colors[4] = 0.8;  Colors[5] = 0.8;  Colors[6] = 0;  Colors[7] = 1;
    Colors[8] = 0.8;  Colors[9] = 0.8;  Colors[10] = 0; Colors[11] = 1;
    Colors[12] = 0.8; Colors[13] = 0.8; Colors[14] = 0; Colors[15] = 1;
    Colors[16] = 0.8; Colors[17] = 0.8; Colors[18] = 0; Colors[19] = 1;
    Colors[20] = 0.8; Colors[21] = 0.8; Colors[22] = 0; Colors[23] = 1;
    Colors[24] = 0.8; Colors[25] = 0.8; Colors[26] = 0; Colors[27] = 1;
    Colors[28] = 0.8; Colors[29] = 0.8; Colors[30] = 0; Colors[31] = 1;
    Colors[32] = 0.1; Colors[33] = 0.1; Colors[34] = 0; Colors[35] = 1;
    Colors[36] = 0.1; Colors[37] = 0.1; Colors[38] = 0; Colors[39] = 1;
    Colors[40] = 0.1; Colors[41] = 0.1; Colors[42] = 0; Colors[43] = 1;
    Colors[44] = 0.1; Colors[45] = 0.1; Colors[46] = 0; Colors[47] = 1;
    Colors[48] = 0.1; Colors[49] = 0.1; Colors[50] = 0; Colors[51] = 1;
    Colors[52] = 0.1; Colors[53] = 0.1; Colors[54] = 0; Colors[55] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO[9]);
    glBufferData(GL_ARRAY_BUFFER, 56 * sizeof(float), Colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] Colors;
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO); glVertexPointer(3, GL_FLOAT, 0, NULL); glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexEBO);
}
void WindowInit()
{
    sf::ContextSettings settings; settings.depthBits = 16; settings.stencilBits = 8; settings.antialiasingLevel = 8;
    window.create(VideoMode(ScreenSize[0], ScreenSize[1]), "Prog", Style::Fullscreen, settings);
    window.setVerticalSyncEnabled(true); window.setMouseCursorVisible(false);
}
void EventUpdate()
{
    Event event;
    while (window.pollEvent(event)) if (event.type == Event::Closed) { window.close(); work = false; }
}
int main()
{
    Image genim; genim.loadFromFile("num.png");
    genim.createMaskFromColor(Color(255, 255, 255));
    Texture gent; gent.loadFromImage(genim);
    Sprite gens; gens.setTexture(gent);
    WindowInit(); Init();
    for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++) trees[x][z].GenGenom();
    for (int i = 0; i < showxx * showzz; i++) { bests[i][0] = 0; bests[i][1] = i; }
    cout << "Generation: " << generation << endl; 
    Clock clock;
    while (work)
    {
        TimesGone = float(clock.getElapsedTime().asMicroseconds()) / 1000.0f; clock.restart();
        FPS[fpscounter] = int(1000 / TimesGone); fpscounter++; if (fpscounter == 5) fpscounter = 0;
        EventUpdate(); 
        if (!stop) cd += TimesGone;
        if (Keyboard::isKeyPressed(Keyboard::V) && Keyboard::isKeyPressed(Keyboard::H)) { window.setVisible(true); while (!window.hasFocus()); }
        if (window.hasFocus())
        {
            window.clear();
            DrawAll();
            gens.setTextureRect(IntRect(0, 0, 226, 71));
            gens.setPosition(ScreenSize[0] * 0.02, ScreenSize[1] * 0.1);
            window.pushGLStates();
            window.draw(gens);
            gens.setTextureRect(IntRect(279 + 37 * (generation % 10), 0, 37, 61));
            gens.setPosition(ScreenSize[0] * 0.316, ScreenSize[1] * 0.1);
            window.draw(gens);
            gens.setTextureRect(IntRect(279 + 37 * ((generation % 100 - generation % 10) / 10), 0, 37, 61));
            gens.setPosition(ScreenSize[0] * 0.279, ScreenSize[1] * 0.1);
            window.draw(gens);
            gens.setTextureRect(IntRect(279 + 37 * ((generation - generation % 100) / 100), 0, 37, 61));
            gens.setPosition(ScreenSize[0] * 0.242, ScreenSize[1] * 0.1);
            window.draw(gens);
            gens.setTextureRect(IntRect(279 + 37 * ((generation - generation % 1000) / 1000), 0, 37, 61));
            gens.setPosition(ScreenSize[0] * 0.205, ScreenSize[1] * 0.1);
            window.draw(gens);
            window.popGLStates();
            window.display();
            Control();
        } else window.setVisible(false);
        while (cd > TimeToStep)
        {
            step++; cd -= TimeToStep;
            for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++)
            {
                trees[x][z].GrewUp();
            }
        }
        if (step > StepsToReborn)
        {
            step = 0; generation++; cout << "Generation: " << generation << endl;
            int thebest[2], theworst[2];
            double maxenergy = -99999, minenergy;
            for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++)
                if (trees[x][z].energy > maxenergy) { maxenergy = trees[x][z].energy; thebest[0] = x; thebest[1] = z; }
            for (int u = 0; u < showxx * showzz; u++)
            {
                float maxenergy1 = -99999; bool da = true; int thebest1[2];
                for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++)
                    if (trees[x][z].energy > maxenergy1)
                    {
                        da = true;
                        for (int i = 0; i < u; i++) if (bests[i][0] == x && bests[i][1] == z) da = false;
                        if (da) { maxenergy1 = trees[x][z].energy; thebest1[0] = x; thebest1[1] = z; }
                    }
                bests[u][0] = thebest1[0]; bests[u][1] = thebest1[1];
            }
            for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++) if (trees[x][z].dead)
            {
                for (int xx = 0; xx < GenLength; xx++) for (int yy = 0; yy < 6; yy++)
                    trees[x][z].Genom[xx][yy] = trees[thebest[0]][thebest[1]].Genom[xx][yy];
                trees[x][z].energy = maxenergy;
            }
            for (int i = 0; i < 3; i++)
            {
                minenergy = maxenergy;
                for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++)
                    if (trees[x][z].energy < minenergy) { minenergy = trees[x][z].energy; theworst[0] = x; theworst[1] = z; }
                for (int x = 0; x < GenLength; x++) for (int y = 0; y < 6; y++)
                    trees[theworst[0]][theworst[1]].Genom[x][y] = trees[thebest[0]][thebest[1]].Genom[x][y];
                trees[theworst[0]][theworst[1]].energy = maxenergy;
            }
            for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++) trees[x][z].Evolve();
            for (int x = 0; x < qxx; x++) for (int z = 0; z < qzz; z++) trees[x][z].Born();
        }
    }
    glDeleteBuffers(1, &vertexVBO); glDeleteBuffers(1, &indexEBO);
    glDeleteBuffers(1, &colorVBO[0]);
    glDeleteBuffers(1, &colorVBO[1]);
    glDeleteBuffers(1, &colorVBO[2]);
    glDeleteBuffers(1, &colorVBO[3]);
    glDeleteBuffers(1, &colorVBO[4]);
    return 0;
}