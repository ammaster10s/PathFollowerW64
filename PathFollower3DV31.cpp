#include "keio_workshop.h"

// ==============================
// Global variables
// ==============================
int ModelHandle_CAR;

long timeA = 0, timeB = 0;
int looptime_draw = 0, looptime_cal = 0;
double etime = 0.0;

// LiDAR toggle
int show_lidar = 8;   // 0:none, 1:center, 2:rear32, 3:left, 4:front, 5:right, 6:rear, 7:OS1, 8:ALL

// Vehicle state (from shared memory)
double Car_X = 0.0;
double Car_Y = 0.0;
double Car_Yaw = 0.0;
double g_vel_kmh = 0.0;
double g_yaw_rate = 0.0;

// Trajectory
double Tra_X[1000];
double Tra_Y[1000];
int Tra_cnt = 0;

// Camera control
double cam_yaw = 0.0;      // camera rotation offset (rad)
double cam_height = 6.0;   // camera height
double cam_dist = 15.0;    // camera distance

// Function prototype
void main2();

// ==============================
// Main
// ==============================
// Added SAL annotations to match Windows headers and avoid C28251 warning.
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    // local loop variable removed from outer scope to avoid accidental reuse
    // (we'll use descriptive locals inside loops)
    int i;

    // ==========================
    // DxLib Initialization
    // ==========================
    SetMainWindowText("Keio Workshop - Mission 1");
    SetAlwaysRunFlag(TRUE);
    ChangeWindowMode(TRUE);
    SetGraphMode(1900, 1050, 32);
    SetFontSize(40);
    ChangeFontType(DX_FONTTYPE_ANTIALIASING_4X4);
    SetWindowSizeChangeEnableFlag(TRUE);

    sharememoryinit();
    velosharememoryinit();
    SetOutApplicationLogValidFlag(FALSE);

    if (DxLib_Init() < 0) return -1;

    ModelHandle_CAR = MV1LoadModel("model/car/hiace.mv1");
    if (ModelHandle_CAR < 0) return -1;

    SetDrawScreen(DX_SCREEN_BACK);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    for (i = 0; i < 1000; i++) {
        Tra_X[i] = -999.0;
        Tra_Y[i] = -999.0;
    }

    timeA = GetTimer();

    // ==========================
    // Main loop
    // ==========================
    while (ProcessMessage() == 0 &&
        CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        looptime_draw = GetTimer() - timeA;
        timeA = GetTimer();

        main2();
        velocheck();
        ClearDrawScreen();

        // ==========================
        // Camera control (arrow keys)
        // ==========================
        if (CheckHitKey(KEY_INPUT_LEFT))  cam_yaw += 0.03;
        if (CheckHitKey(KEY_INPUT_RIGHT)) cam_yaw -= 0.03;
        if (CheckHitKey(KEY_INPUT_UP))    cam_height += 0.2;
        if (CheckHitKey(KEY_INPUT_DOWN))  cam_height -= 0.2;

        if (cam_height < 2.0)  cam_height = 2.0;
        if (cam_height > 20.0) cam_height = 20.0;

        VECTOR CamPos = VGet(
            (float)(Car_X - cam_dist * cos(Car_Yaw + cam_yaw)),
            (float)cam_height,
            (float)(Car_Y - cam_dist * sin(Car_Yaw + cam_yaw))
        );

        VECTOR CamTarget = VGet(
            (float)Car_X,
            1.0f,
            (float)Car_Y
        );

        SetCameraNearFar(3.0f, 1000.0f);
        SetCameraPositionAndTargetAndUpVec(
            CamPos, CamTarget, VGet(0, 1, 0)
        );

        // ==========================
        // Light
        // ==========================
        SetLightDirection(
            VGet((float)-cos(Car_Yaw), -1.0f, (float)-sin(Car_Yaw))
        );

        // ==========================
        // Floor grid
        // ==========================
        for (int gx = -200; gx < 200; gx++) {
            DrawLine3D(VGet(gx, 0, 200), VGet(gx, 0, -200), GetColor(50, 50, 50));
            DrawLine3D(VGet(200, 0, gx), VGet(-200, 0, gx), GetColor(50, 50, 50));
        }

        // ==========================
        // LiDAR toggle (L key)
        // ==========================
        static int old_l_key = 0;
        int now_l_key = CheckHitKey(KEY_INPUT_L);

        if (now_l_key != 0 && old_l_key == 0) {
            show_lidar++;
            if (show_lidar > 8) show_lidar = 0;
        }
        old_l_key = now_l_key;

        // ==========================
        // LiDAR visualization
        // ==========================
        // Use vehicle yaw and position variables to compute transformed LiDAR points.
        double cosang = cos(Car_Yaw);
        double sinang = sin(Car_Yaw);

        // Use the existing toggle variable 'show_lidar' instead of undefined 'showpnt'.
        if (show_lidar == 1 || show_lidar == 8) {
            double tmpx, tmpy;

            for (int i = 0; i < velopointnum; i++) {
                tmpx = Car_X + p[i].x * cosang - p[i].y * sinang;
                tmpy = Car_Y + p[i].x * sinang + p[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p[i].z + 0.1, tmpy), 0.05, 4, GetColor(200, 0, 0), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 2 || show_lidar == 8) {
            double tmpx, tmpy;
            for (int i = 0; i < velopointnum_32B; i++) {
                tmpx = Car_X + p_32B[i].x * cosang - p_32B[i].y * sinang;
                tmpy = Car_Y + p_32B[i].x * sinang + p_32B[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_32B[i].z + 0.1, tmpy), 0.05, 4, GetColor(0, 200, 200), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 3 || show_lidar == 8) {
            for (int i = 0; i < velopointnum_L; i++) {
                double tmpx, tmpy;
                tmpx = Car_X + p_L[i].x * cosang - p_L[i].y * sinang;
                tmpy = Car_Y + p_L[i].x * sinang + p_L[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_L[i].z + 0.1, tmpy), 0.05, 0, GetColor(0, 200, 0), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 4 || show_lidar == 8) {
            for (int i = 0; i < velopointnum_F; i++) {
                double tmpx, tmpy;
                tmpx = Car_X + p_F[i].x * cosang - p_F[i].y * sinang;
                tmpy = Car_Y + p_F[i].x * sinang + p_F[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_F[i].z + 0.1, tmpy), 0.05, 0, GetColor(0, 0, 200), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 5 || show_lidar == 8) {
            for (int i = 0; i < velopointnum_R; i++) {
                double tmpx, tmpy;
                tmpx = Car_X + p_R[i].x * cosang - p_R[i].y * sinang;
                tmpy = Car_Y + p_R[i].x * sinang + p_R[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_R[i].z + 0.1, tmpy), 0.05, 0, GetColor(200, 200, 0), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 6 || show_lidar == 8) {
            for (int i = 0; i < velopointnum_B; i++) {
                double tmpx, tmpy;
                tmpx = Car_X + p_B[i].x * cosang - p_B[i].y * sinang;
                tmpy = Car_Y + p_B[i].x * sinang + p_B[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_B[i].z + 0.1, tmpy), 0.05, 0, GetColor(200, 0, 200), GetColor(255, 255, 255), TRUE);
            }
        }

        if (show_lidar == 7 || show_lidar == 8) {
            for (int i = 0; i < velopointnum_OS1; i++) {
                double tmpx, tmpy;
                tmpx = Car_X + p_OS1[i].x * cosang - p_OS1[i].y * sinang;
                tmpy = Car_Y + p_OS1[i].x * sinang + p_OS1[i].y * cosang;
                DrawSphere3D(VGet(tmpx, p_OS1[i].z + 0.1, tmpy), 0.05, 0, GetColor(200, 200, 200), GetColor(255, 255, 255), TRUE);

            }
        }

        // ==========================
        // Trajectory
        // ==========================
        for (i = 0; i < Tra_cnt; i++) {
            if (Tra_X[i] > -900) {
                DrawSphere3D(
                    VGet((float)Tra_X[i], 0.1f, (float)Tra_Y[i]),
                    0.2f,
                    8,
                    GetColor(0, 255, 0),
                    GetColor(0, 255, 0),
                    TRUE
                );
            }
        }

        // ==========================
        // Draw car model
        // ==========================
        float model_x = (float)(Car_X - POSOFFSET * cos(Car_Yaw));
        float model_z = (float)(Car_Y - POSOFFSET * sin(Car_Yaw));

        MV1SetPosition(ModelHandle_CAR, VGet(model_x, 0.0f, model_z));
        MV1SetRotationXYZ(ModelHandle_CAR, VGet(0, (float)(PI - Car_Yaw + PI), 0));
        MV1DrawModel(ModelHandle_CAR);

        // ==========================
        // Debug info
        // ==========================
        DrawFormatString(10, 10, GetColor(255, 255, 0), "Time=%.2lf Looptime Draw=%d CAL=%d", etime, looptime_draw, looptime_cal);
        DrawFormatString(10, 50, GetColor(255, 255, 0), "Velocity=%.2lf km/h Yaw Rate=%.2lf deg/s", g_vel_kmh, g_yaw_rate); DrawFormatString(10, 90, GetColor(255, 255, 0), "X=%.2lf m Y=%.2lf m Yaw Angle=%.2lf rad", Car_X, Car_Y, Car_Yaw); 

        DrawFormatString(
            10, 130, GetColor(0, 255, 255),
            "show_lidar = %d (Press L to toggle)", show_lidar
        );

        DrawFormatString(
            10, 170, GetColor(255, 0, 0),
            "LIDAR num=%d num32B=%d numL=%d numF=%d numR=%d numB=%d numOS1=%d",
     
            velopointnum,
            velopointnum_32B,
            velopointnum_L,
            velopointnum_F,
            velopointnum_R,
            velopointnum_B,
            velopointnum_OS1
        );

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}

// ==============================
// Shared memory update
// ==============================
void main2()
{
    timeB = GetTimer();
    etime += 0.025;

    g_vel_kmh = sm[64];
    g_yaw_rate = sm[70];

    Car_X = sm[505];
    Car_Y = sm[506];
    Car_Yaw = sm[507];

    static double traj_timer = 0.0;
    traj_timer += 0.025;

    if (traj_timer >= 0.1 && Tra_cnt < 1000) {
        Tra_X[Tra_cnt] = Car_X;
        Tra_Y[Tra_cnt] = Car_Y;
        Tra_cnt++;
        traj_timer = 0.0;
    }

    looptime_cal = GetTimer() - timeB;
}
