#include "DxLib.h"
#include <math.h>

struct Top
{
    VECTOR pos;
    VECTOR vel;

    float radius;
    float mass;

    float spin;  // 回転速度
    float angle; // 描画用の角度
};

// ベクトル長（DXライブラリのVSizeでも代用可能ですが、明示的に作成）
float Length(VECTOR v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// コマ衝突
void ResolveCollision(Top& a, Top& b)
{
    VECTOR diff = VSub(b.pos, a.pos);
    float dist = Length(diff);
    float hitDist = a.radius + b.radius;

    // 衝突していない、または重なりすぎている場合は無視
    if (dist >= hitDist || dist <= 0.0001f)
        return;

    // --- 1. 衝突応答 (物理演算) ---
    VECTOR normal = VDiv(diff, dist); // VNormの代わり（distが判明しているので効率的）
    VECTOR rv = VSub(b.vel, a.vel);   // 相対速度
    float velAlongNormal = VDot(rv, normal);

    // すでに離れようとしている場合は処理しない
    if (velAlongNormal > 0)
        return;

    // 反発係数 (1.0で完全弾性衝突)
    float e = 0.8f;

    // 衝撃量 j の計算
    float j = -(1.0f + e) * velAlongNormal;
    j /= (1.0f / a.mass + 1.0f / b.mass);
    
    // 衝撃ベクトルを適用
    VECTOR impulse = VScale(normal, j);

    // スピン量に応じて少しだけ反発力を強化（ゲーム的な味付け）
    float spinFactor = (a.spin + b.spin) * 0.001f;
    impulse = VScale(impulse, 1.0f + spinFactor);

    a.vel = VSub(a.vel, VScale(impulse, 1.0f / a.mass));
    b.vel = VAdd(b.vel, VScale(impulse, 1.0f / b.mass));

    // --- 2. めり込み補正 (Positional Correction) ---
    // これがないと、コマ同士が重なったまま震える現象が起きます
    float percent = 0.5f; // 補正の強さ
    float slop = 0.01f;   // 許容誤差
    VECTOR correction = VScale(normal, (fmaxf(hitDist - dist - slop, 0.0f) / (1.0f / a.mass + 1.0f / b.mass)) * percent);

    a.pos = VSub(a.pos, VScale(correction, 1.0f / a.mass));
    b.pos = VAdd(b.pos, VScale(correction, 1.0f / b.mass));

    // --- 3. スピンダメージ ---
    // 衝撃が強いほどスピンが削られる
    float damage = fabsf(j) * 0.1f;
    a.spin -= damage;
    b.spin -= damage;

    if (a.spin < 0) a.spin = 0;
    if (b.spin < 0) b.spin = 0;
}

// 壁反射
void WallCollision(Top& t)
{
    float limit = 40.0f;
    float bounce = 0.5f; // 壁での跳ね返り係数

    if (t.pos.x < -limit) {
        t.pos.x = -limit;
        t.vel.x *= -bounce;
        t.spin *= 0.95f; // 壁に当たるとスピンも減る
    }
    if (t.pos.x > limit) {
        t.pos.x = limit;
        t.vel.x *= -bounce;
        t.spin *= 0.95f;
    }
    if (t.pos.z < -limit) {
        t.pos.z = -limit;
        t.vel.z *= -bounce;
        t.spin *= 0.95f;
    }
    if (t.pos.z > limit) {
        t.pos.z = limit;
        t.vel.z *= -bounce;
        t.spin *= 0.95f;
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    if (DxLib_Init() == -1) return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    // カメラ設定
    SetCameraPositionAndTarget_UpVecY(VGet(0, 80, -100), VGet(0, 0, 0));
    SetCameraNearFar(1.0f, 500.0f);

    Top top1 = { VGet(-20, 0, 0), VGet(0, 0, 0), 5.0f, 1.0f, 150.0f, 0.0f, GetColor(255, 50, 50) };
    Top top2 = { VGet(20, 0, 0), VGet(0, 0, 0), 5.0f, 1.0f, 150.0f, 0.0f, GetColor(50, 50, 255) };

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();

        // 入力 (スピンが残っている時だけ移動可能にする)
        float accel = 0.15f;
        if (top1.spin > 0) {
            if (CheckHitKey(KEY_INPUT_W)) top1.vel.z += accel;
            if (CheckHitKey(KEY_INPUT_S)) top1.vel.z -= accel;
            if (CheckHitKey(KEY_INPUT_A)) top1.vel.x -= accel;
            if (CheckHitKey(KEY_INPUT_D)) top1.vel.x += accel;
        }
        if (top2.spin > 0) {
            if (CheckHitKey(KEY_INPUT_UP))    top2.vel.z += accel;
            if (CheckHitKey(KEY_INPUT_DOWN))  top2.vel.z -= accel;
            if (CheckHitKey(KEY_INPUT_LEFT))  top2.vel.x -= accel;
            if (CheckHitKey(KEY_INPUT_RIGHT)) top2.vel.x += accel;
        }

        // 摩擦（空気抵抗・地面摩擦）
        top1.vel = VScale(top1.vel, 0.97f);
        top2.vel = VScale(top2.vel, 0.97f);

        // 移動
        top1.pos = VAdd(top1.pos, top1.vel);
        top2.pos = VAdd(top2.pos, top2.vel);

        // 衝突判定
        ResolveCollision(top1, top2);
        WallCollision(top1);
        WallCollision(top2);

        // 回転処理
        top1.angle += top1.spin * 0.02f;
        top2.angle += top2.spin * 0.02f;

        // 自然な減速（スピンが速いほど減りにくく、遅くなると一気に止まる設定もアリ）
        top1.spin *= 0.998f;
        top2.spin *= 0.998f;

        // --- 描画 ---

        // 床
        DrawBox3D(VGet(-45, -1, -45), VGet(45, 0, 45), GetColor(60, 60, 60), TRUE);

        // コマの描画
        Top* tops[2] = { &top1, &top2 };
        for (int i = 0; i < 2; i++) {
            // 本体
            DrawSphere3D(tops[i]->pos, tops[i]->radius, 16, tops[i]->color, GetColor(255, 255, 255), TRUE);

            // 回転がわかるように線を描画
            VECTOR lineEnd = VGet(
                tops[i]->pos.x + cosf(tops[i]->angle) * tops[i]->radius,
                tops[i]->pos.y + 2.0f, // 少し上に浮かせる
                tops[i]->pos.z + sinf(tops[i]->angle) * tops[i]->radius
            );
            DrawLine3D(tops[i]->pos, lineEnd, GetColor(255, 255, 255));
        }

        // UI
        DrawFormatString(20, 20, GetColor(255, 255, 255), "P1 Spin: %.1f", top1.spin);
        DrawFormatString(20, 40, GetColor(255, 255, 255), "P2 Spin: %.1f", top2.spin);
        if (top1.spin <= 0) DrawString(20, 70, "P1 STOPPED!", GetColor(255, 0, 0));
        if (top2.spin <= 0) DrawString(20, 90, "P2 STOPPED!", GetColor(255, 0, 0));

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}