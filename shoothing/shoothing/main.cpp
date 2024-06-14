#define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLUT/glut.h>

// グローバル変数
static bool KeyUpON    = false;    // 矢印キーの状態フラグ
static bool KeyDownON  = false;    // 矢印キーの状態フラグ
static bool KeyLeftON  = false;    // 矢印キーの状態フラグ
static bool KeyRightON = false;    // 矢印キーの状態フラグ
static bool KeyRespawn = false;    // リスポーン用のフラグ

static int MouseX = 0;    // マウスX座標
static int MouseY = 0;    // マウスY座標
static int MouseLB_ON = 0; //マウスの左クリック
static float SpinX = 0;    // X軸周りの回転量
static float SpinY = 0;    // Y軸周りの回転量

static float MoveX = 0;    // X軸方向の移動量
static float MoveZ = 0;    // Y軸方向の移動量
static float Scale = 1.0; // 拡大量
static int apple = 0;     //アイテムの総数

// 自機の弾の情報を格納する構造体
#define MAX_SHOT  10 // 同時に撃てる弾の最大数
typedef struct _MyShot{
    int isAlive; // 自機の弾が生きてるか？
    float x;  // 弾のx座標
    float z;  // 弾のz座標
    float vx; // 弾のx軸方向速度
    float vz; // 弾のz軸方向速度
} MyShot;
// 自機の情報を格納する構造体
typedef struct _MyShip{
    int isAlive;   // 自機が生きてるか？
    float x;  // 自機のx座標
    float z;  // 自機のz座標
    MyShot myShot[MAX_SHOT];// 弾を管理する構造体配列
} MyShip;
MyShip myShip; // 自機の構造体を作成

// 敵の情報を格納する構造体
#define MAX_ENEMY  6 // 同時に出現する敵の最大数
typedef struct _Enemy{
    int isAlive;   // 敵が生きてるか？
    float x;  // 敵のx座標
    float z;  // 敵のz座標
    float vx; // 敵のx軸方向速度
    float vz; // 敵のz軸方向速度
} Enemy;
Enemy enemy[MAX_ENEMY]; // 敵の構造体配列を作成

#define MAX_ITEM  4 // 同時に出現するアイテムの最大数
typedef struct _Item{
    int isAlive;   // アイテムが生きてるか？
    float x;  // アイテムのx座標
    float z;  // アイテムのz座標
    float vx; // アイテムのx軸方向速度
    float vz; // アイテムのz軸方向速度
} Item;
Item item[MAX_ITEM]; // アイテムの構造体配列を作成

// マウス入力

void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        MouseLB_ON =1;
      //printf("(%3d,%3d)でスペースが押されました\n", x, y);
      for (int i=0;i<MAX_SHOT; i++){
        if(myShip.myShot[i].isAlive == 0){
            myShip.myShot[i].isAlive = 1;
            myShip.myShot[i].x = myShip.x;
            myShip.myShot[i].z = myShip.z;
            break;
        }
      }
    }else if (button==GLUT_LEFT_BUTTON && state == GLUT_UP){
        MouseLB_ON=0;
    }
}


// キーボード入力(矢印キーを押し続けている間は自機を移動)
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'w':
        //printf("(%3d,%3d)で[w]が押されました\n", x, y);
        KeyUpON = true;
        break;
    case 's':
        //printf("(%3d,%3d)で[s]が押されました\n", x, y);
        KeyDownON  = true;
        break;
    case 'a':
        //printf("(%3d,%3d)で[a]が押されました\n", x, y);
        KeyLeftON  = true;
        break;
    case 'd':
        //printf("(%3d,%3d)で[d]が押されました\n", x, y);
        KeyRightON = true;
        break;
    case 'r':
        //printf("(%3d,%3d)で[r]が押されました\n", x, y);
        KeyRespawn = true;
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
    case 'w':
        //printf("(%3d,%3d)で[w]が離されました\n", x, y);
        KeyUpON    = false;
        break;
    case 's':
        //printf("(%3d,%3d)で[s]が離されました\n", x, y);
        KeyDownON  = false;
        break;
    case 'a':
        //printf("(%3d,%3d)で[a]が離されました\n", x, y);
        KeyLeftON  = false;
        break;
    case 'd':
        //printf("(%3d,%3d)で[d]が離されました\n", x, y);
        KeyRightON = false;
        break;
    case 'r':
        //printf("(%3d,%3d)で[d]が離されました\n", x, y);
        KeyRespawn = false;
        break;
    }
}

void output_string(char *pstr){
    while (*pstr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *pstr);
        ++pstr;
    }
}

void reset_data(){
    // 自機の構造体データを初期化
    myShip.x = 0;    // 自機の初期位置(X座標)
    myShip.z = 0;    // 自機の初期位置(Z座標)
    myShip.isAlive = 1;    // 自機は生きている
    apple = 0;        //スコア初期化
    // 弾の構造体データを初期化(弾はZ軸方向に毎フレーム-1.0移動する)
    for(int i=0; i<MAX_SHOT; i++){
        myShip.myShot[i].isAlive = 0; // 弾は発射されていない
        myShip.myShot[i].x = 0;        // 弾の初期位置(X座標)
        myShip.myShot[i].z = 0;        // 弾の初期位置(X座標)
        myShip.myShot[i].vx = 0;    // 弾の基本速度(X座標)
        myShip.myShot[i].vz = -1.0; // 弾の基本速度(X座標)
        
    }
    // 敵の構造体データを初期化(敵はZ軸方向に毎フレーム0.1移動する)
    for(int i=0; i<MAX_ENEMY; i++){
        enemy[i].isAlive = 0; // 敵は出現していない
        enemy[i].x = 0;        // 敵の初期位置(X座標)
        enemy[i].z = -100;    // 敵の初期位置(Z座標)
        enemy[i].vx = 0;    // 敵の基本速度(X座標)
        enemy[i].vz = 0.1;    // 敵の基本速度(Z座標)
    }
    
    // アイテムの構造体データを初期化(敵はZ軸方向に毎フレーム0.1移動する)
    for(int i=0; i<MAX_ITEM; i++){
        item[i].isAlive = 0; // アイテムは出現していない
        item[i].x = 0;        // アイテムの初期位置(X座標)
        item[i].z = -100;    // アイテムの初期位置(Z座標)
        item[i].vx = 0;    // アイテムの基本速度(X座標)
        item[i].vz = 0.1;    // アイテムの基本速度(Z座標)
    }
}

void timer(int t)             // 指定時間後に呼び出される関数（Timerコールバック関数）
{
    // 矢印キーを押し続けている間は自機を移動
    if(KeyUpON   == 1) myShip.z -= 0.25;
    if(KeyDownON == 1) myShip.z += 0.25;
    if(KeyLeftON == 1) myShip.x -= 0.25;
    if(KeyRightON== 1) myShip.x += 0.25;

    // 速度を足して弾を移動させる
    for (int i=0;i<MAX_SHOT; i++){
        if(myShip.myShot[i].isAlive == 1){
            myShip.myShot[i].z += myShip.myShot[i].vx; //x軸方向の速度を加算
            myShip.myShot[i].z += myShip.myShot[i].vz; //z軸方向の速度を加算
        }
        if(myShip.myShot[i].z < -100) //画面外(領域外)判定
            myShip.myShot[i].isAlive = 0; //画面外(領域外)に出たら弾を消す
    }

    // 速度を足して敵を移動させる
    int NofEnemys = 0; // 出現している敵の数を数える変数
    for(int i=0;i<MAX_ENEMY; i++){
        if(enemy[i].isAlive == 1){ // i番目の敵は生きているか？
             // 生きていたら敵の位置を乱数（ノイズ）で移動
            enemy[i].x += enemy[i].vx + (0.5 * ((float)rand()/RAND_MAX - 0.5)); //x軸方向の速度(と乱数)を加算
            enemy[i].z += enemy[i].vz + (2 * ((float)rand()/RAND_MAX)); //z軸方向の速度(と乱数)を加算
            NofEnemys++; // 出現している敵の数を数える
        }
        if(enemy[i].z > 10 || enemy[i].x < -10 || enemy[i].x > 10) //画面外(領域外)判定
            enemy[i].isAlive = 0; //画面外(領域外)に出たら敵を消す
    }
    
    // 速度を足してアイテムを移動させる
    int NofItems = 0; // 出現しているアイテムの数を数える変数
    for(int i=0;i<MAX_ITEM; i++){
        if(item[i].isAlive == 1){ // i番目の敵は生きているか？
             // 生きていたらアイテムの位置を乱数（ノイズ）で移動
            item[i].x += item[i].vx +(0.5 * ((float)rand()/RAND_MAX - 0.5)); //x軸方向の速度(と乱数)を加算
            item[i].z += item[i].vz + (2 * ((float)rand()/RAND_MAX)); //z軸方向の速度(と乱数)を加算
            NofItems++; // 出現しているアイテムの数を数える
        }
        if(item[i].z > 10 || item[i].x < -10 || item[i].x > 10) //画面外(領域外)判定
            item[i].isAlive = 0; //画面外(領域外)に出たらアイテムを消す
    }

    // 弾と敵の当たり判定
    for(int i=0;i<MAX_ENEMY;i++){
        for(int j=0;j<MAX_SHOT;j++){
            // 弾と敵の距離が一定値以内の場合に当たりと判定
            if(((enemy[i].x - myShip.myShot[j].x)*(enemy[i].x - myShip.myShot[j].x)+
                (enemy[i].z - myShip.myShot[j].z)*(enemy[i].z - myShip.myShot[j].z)) < 2.0 ){
                enemy[i].isAlive = 0;           //敵を消す
                myShip.myShot[j].isAlive = 0;   //弾を消す(ここで弾を消さないと貫通弾になる)
            }
        }
    }

    // 敵の出現
    // 現存する敵がMAX_ENEMYよりも少なかったら1つ新たに出現させる
    if (NofEnemys<MAX_ENEMY){
        for(int i=0;i<MAX_ENEMY; i++){
            // isAliveが0の構造体を見つけてisAliveを1にする
            // 出現位置はランダムに変化させる
            if(enemy[i].isAlive == 0){
                enemy[i].x = ((float)rand()/RAND_MAX - 0.5) * 10;
                enemy[i].z = -100 + ((float)rand()/RAND_MAX - 0.5) * 100;
                enemy[i].isAlive = 1;
                break;
            }
        }
    }

    // アイテムの出現
    // 現存するアイテムがMAX_ITEMよりも少なかったら1つ新たに出現させる
    if (NofItems<MAX_ITEM){
        for(int i=0;i<MAX_ITEM; i++){
            // isAliveが0の構造体を見つけてisAliveを1にする
            // 出現位置はランダムに変化させる
            if(item[i].isAlive == 0){
                item[i].x = ((float)rand()/RAND_MAX - 0.5) * 10;
                item[i].z = -100 + ((float)rand()/RAND_MAX - 0.5) * 100;
                item[i].isAlive = 1;
                break;
            }
        }
    }
    
    // 敵と自機の当たり判定
    for(int i=0;i<MAX_ENEMY; i++){
        // 敵と自機の距離が一定値以内の場合に当たりと判定
        if(((enemy[i].x - myShip.x)*(enemy[i].x - myShip.x)
            +(enemy[i].z - myShip.z)*(enemy[i].z - myShip.z)) < 2.0 ){
            myShip.isAlive = 0; // 自機を消す
        }
    }
    
    // アイテムと自機の当たり判定
    for(int i=0;i<MAX_ITEM; i++){
        // 敵と自機の距離が一定値以内の場合に当たりと判定
        if(((item[i].x - myShip.x)*(item[i].x - myShip.x)
            +(item[i].z - myShip.z)*(item[i].z - myShip.z)) < 2.0 ){
            apple += 1;
            item[i].isAlive = 0;
        }
    }

    glutPostRedisplay();          // 画面の再描画
    glutTimerFunc(20, timer, 20); // 次回のタイマー関数の呼び出しをセット
}

void reshape(int w, int h)    // ウインドウサイズ変更時に呼び出される関数（Reshapeコールバック関数）
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (double)w/h, 1.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 10.0, 18.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// 自機と弾の描画
void DrawMyShip(void)
{
    glPushMatrix();
        glTranslatef(myShip.x, 0.0, myShip.z); //X軸方向にMoveX，Y軸方向にMoveYだけ移動
        glRotatef(180.0, 0.0, 1.0, 0.0);  //Y軸周りに90度回転
        //glutSolidTeapot(1.0);
    glPushMatrix();
    glTranslatef(1.25f, -0.35f, 0.0f);
    glRotatef(340.0, 0.0, 0.0, 1.0);
    glScalef(1.1f,0.5f, 0.5f);
    glColor3f(1.0f,0.8f,1.0f);
    glutSolidSphere(0.75, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-1.25f, -0.35f, 0.0f);
    glRotatef(20.0, 0.0, 0.0, 1.0);
    glScalef(1.1f,0.5f, 0.5f);
    glColor3f(1.0f,0.8f,1.0f);
    glutSolidSphere(0.75, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.0f, -1.5f, 0.3f);
    glRotatef(30.0, 0.0, 1.0, 0.0);
    glScalef(0.5f, 0.3f, 1.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glutSolidSphere(0.75, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-1.0f, -1.5f, 0.3f);
    glRotatef(330.0, 0.0, 1.0, 0.0);
    glScalef(0.5f, 0.3f, 1.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glutSolidSphere(0.75, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0, 0.8, 1.0);
    glutSolidSphere(1.5,16,16);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.4f, 0.4f, 1.5f);
    glScalef(0.8f, 1.5f, 0.2f);
    glColor3f(0.0f,0.0f,0.0f);
    glutSolidSphere(0.3, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.35f, 0.5f, 1.6f);
    glScalef(0.8f, 2.0f, 0.2f);
    glColor3f(1.0f,1.0f,1.0f);
    glutSolidSphere(0.1, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.4f, 0.4f, 1.5f);
    glScalef(0.8f, 1.5f, 0.2f);
    glColor3f(0.0f,0.0f,0.0f);
    glutSolidSphere(0.3, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.35f, 0.5f, 1.6f);
    glScalef(0.8f, 2.0f, 0.2f);
    glColor3f(1.0f,1.0f,1.0f);
    glutSolidSphere(0.1, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 1.5f);
    glScalef(0.5f, 1.0f, 0.2f);
    glColor3f(1.0f,0.0f,0.0f);
    glutSolidSphere(0.25, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.75f, -0.15f, 1.5f);
    glScalef(1.1f,0.5f, 0.2f);
    glColor3f(1.0f,0.5f,1.0f);
    glutSolidSphere(0.15, 36.0, 18.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.75f, -0.15f, 1.5f);
    glScalef(1.1f,0.5f, 0.2f);
    glColor3f(1.0f,0.5f,1.0f);
    glutSolidSphere(0.15, 36.0, 18.0);
    glPopMatrix();
    glPopMatrix();
    
    //自機の弾の描画
    for(int i=0;i<MAX_SHOT;i++){
        if(myShip.myShot[i].isAlive){
            glPushMatrix();
                glTranslatef(myShip.myShot[i].x, 0.0, myShip.myShot[i].z); //現在の弾の位置
            glColor3f(0.8f,1.0f,1.0f);
                glutSolidSphere(0.3,10,10);            //弾は球体
            glPopMatrix();
        }
    }
}

// 敵の描画
void DrawEnemy(void)
{
    //物体の描画
    for(int i=0;i<MAX_ENEMY;i++){
        if(enemy[i].isAlive == 1){
            glPushMatrix();
                glTranslatef(enemy[i].x, 0.0, enemy[i].z); //現在の敵の位置
            glPushMatrix();
            glTranslatef(1.0f, 0.0f, 0.0f);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.7f, 0.7f, 0.0f);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glRotatef(315.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-0.7f, 0.7f, 0.0f);
            glRotatef(270.0, 0.0, 1.0, 0.0);
            glRotatef(315.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-1.0f, 0.0f, 0.0f);
            glRotatef(270.0, 0.0, 1.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.0f, 1.0f, 0.0f);
            glRotatef(270.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-0.7f, -0.7f, 0.0f);
            glRotatef(270.0, 0.0, 1.0, 0.0);
            glRotatef(45.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.0f, -1.0f, 0.0f);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.7f, -0.7f, 0.0f);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glRotatef(45.0, 1.0, 0.0, 0.0);
            glColor3f(0.8f,0.8f,0.8f);
            glutSolidCone(0.25, 1.0, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(0.2, 0.2, 0.2);
            glutSolidSphere(1.0,16,16);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.4f, 0.4f, 1.0f);
            glScalef(0.8f, 1.5f, 0.2f);
            glColor3f(1.0f,1.0f,1.0f);
            glutSolidSphere(0.2, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.375f, 0.3f, 1.1f);
            glScalef(0.8f, 2.0f, 0.2f);
            glColor3f(0.0f,0.0f,0.0f);
            glutSolidSphere(0.08, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-0.4f, 0.4f, 1.0f);
            glScalef(0.8f, 1.5f, 0.2f);
            glColor3f(1.0f,1.0f,1.0f);
            glutSolidSphere(0.2, 36.0, 18.0);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-0.375f, 0.3f, 1.1f);
            glScalef(0.8f, 2.0f, 0.2f);
            glColor3f(0.0f,0.0f,0.0f);
            glutSolidSphere(0.08, 36.0, 18.0);
            glPopMatrix();
            glPopMatrix();
        }
    }
}

void DrawItem(void)
{
    //物体の描画
    for(int i=0;i<MAX_ITEM;i++){
        if(item[i].isAlive == 1){
            glPushMatrix();
                glTranslatef(item[i].x, 0.0, item[i].z); //現在のアイテムの位置
            glPushMatrix();
            glColor3f(0.8, 0.5, 0.5);
            glutSolidSphere(1.0,16,16);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.0f, 0.8f, 0.0f);
            glScalef(2.0f, 0.5f, 1.0f);
            glColor3f(0.0f,0.5f,0.0f);
            glutSolidSphere(0.4, 36.0, 18.0);
            glPopMatrix();
            
            glLineWidth(7.0f);
            glBegin(GL_LINES);
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.3f,-0.3f,1.05f);
            glVertex3f(0.3f,0.3f,1.05f);
            glVertex3f(-0.3f,-0.3f,1.05f);
            glVertex3f(-0.3f,0.3f,1.05f);
            glVertex3f(0.3f,0.3f,1.05f);
            glVertex3f(0.0f,0.0f,1.05f);
            glVertex3f(-0.3f,0.3f,1.05f);
            glVertex3f(0.0f,0.0f,1.05f);
            glEnd();
            glPopMatrix();
        }
    }
}
void display(void)// 描画時に呼び出される関数（Displayコールバック関数）
{
    glClearColor(0.0, 0.0, 0.0, 1.0); // 画面クリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 画面バッファクリア
    //glEnable(GL_DEPTH_TEST); // 隠面消去を有効

    // 自機が死んでいたらGame Overを表示する
    if (myShip.isAlive==0){
        // 文字列の描画
        glPushMatrix();
            GLfloat matString[] = { 0.8, 0.0, 0.2, 1.0 }; //環境光と拡散光の反射率をまとめて設定
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,  matString);
            glRasterPos3f(-1.0f, 0.0f, 0.0f);
            output_string("Game Over");
        
            glRasterPos3f(-4.0f, -8.0f, 0.0f);
            output_string("Press \"R\" key to respawn.");
        glPopMatrix();
        if(KeyRespawn== 1) {
            reset_data();
        }
        
    }else{
        // 自機が生きていたら自機と敵を描画する
        DrawMyShip(); // 自機の描画
        DrawEnemy();  // 敵の描画
        DrawItem();  //アイテムの描画
        
        glPushAttrib(GL_ENABLE_BIT);
        glColor3f(1.0f,0.0f,0.0f);
        
        char str[128];
        sprintf(str,"%d",apple);
        
        glRasterPos3f(-1.0f,-8.0f,0);
        output_string("Score");
        
        glRasterPos3f(0.0f,-9.0f,0);
        output_string(str);
        
        glPopAttrib();
    }
    glutSwapBuffers(); // 描画実行
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);          // GLUT初期化
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);   // ウィンドウサイズの指定
    glutCreateWindow("window");     // 表示ウィンドウ作成
    glutReshapeFunc(reshape);       // Reshapeコールバック関数の指定
    glutDisplayFunc(display);       // Displayコールバック関数の指定

    glutMouseFunc(mouse);     // 通常キーコールバック関数の指定(押したとき)
    glutKeyboardFunc(keyboard);    // 特殊キーコールバック関数の指定(押したとき)
    glutKeyboardUpFunc(keyboardUp);// 特殊キーコールバック関数の指定(離したとき)

    glutTimerFunc(20, timer, 20);   // 定期的に呼び出される関数の指定

    reset_data();

    glutMainLoop(); // メインループへ

    return 0;
}
