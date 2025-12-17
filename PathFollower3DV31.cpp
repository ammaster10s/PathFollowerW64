#include "keio_workshop.h"

// ==============================
// Global variables
// ==============================
int ModelHandle_CAR;

long timeA = 0, timeB = 0;
int looptime_draw = 0, looptime_cal = 0;
double etime = 0.0;

// Vehicle state
double Car_X = 0.0;
double Car_Y = 0.0;
double Car_Yaw = 0.0;
double g_vel_kmh = 0.0;
double g_yaw_rate = 0.0;

// Control limits
#define MAX_SPEED 40.0   // km/h
#define MIN_SPEED 0.0
#define MAX_STEER 0.6    // rad (~34 deg)
#define STEER_STEP 0.02  // rad per frame
#define SPEED_STEP 0.5   // km/h per frame


// Control targets
double target_speed = 20.0;
double target_steer = 0.0;

// Trajectory visualization
double Tra_X[1000];
double Tra_Y[1000];
int Tra_cnt = 0;

// Function prototype
void main2();

// ==============================
// Main
// ==============================
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    int i;

    // ==========================
    // DxLib Initialization
    // ==========================
    SetMainWindowText("Keio Workshop");
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

    // Load car model
    ModelHandle_CAR = MV1LoadModel("model/car/hiace.mv1");
    if (ModelHandle_CAR < 0) return -1;

    SetDrawScreen(DX_SCREEN_BACK);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // Initialize trajectory
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

        // Calculation
        main2();

        ClearDrawScreen();

        // ==========================
        // Camera (chase view)
        // ==========================
        VECTOR CamPos = VGet(
            (float)(Car_X - 15.0 * cos(Car_Yaw)),
            6.0f,
            (float)(Car_Y - 15.0 * sin(Car_Yaw))
        );

        VECTOR CamTarget = VGet(
            (float)Car_X,
            1.0f,
            (float)Car_Y
        );

        SetCameraNearFar(3.0f, 1000.0f);
        SetCameraPositionAndTargetAndUpVec(
            CamPos,
            CamTarget,
            VGet(0.0f, 1.0f, 0.0f)
        );

        // ==========================
        // Light
        // ==========================
        SetLightDirection(
            VGet(
                (float)-cos(Car_Yaw),
                -1.0f,
                (float)-sin(Car_Yaw)
            )
        );

        // ==========================
        // Floor grid
        // ==========================
        for (i = -200; i < 200; i++) {
            DrawLine3D(VGet(i, 0, 200), VGet(i, 0, -200), GetColor(50, 50, 50));
            DrawLine3D(VGet(200, 0, i), VGet(-200, 0, i), GetColor(50, 50, 50));
        }
        for (i = -200; i < 200; i += 10) {
            DrawLine3D(VGet(i, 0, 200), VGet(i, 0, -200), GetColor(150, 150, 150));
            DrawLine3D(VGet(200, 0, i), VGet(-200, 0, i), GetColor(150, 150, 150));
        }

        // ==========================
        // Draw trajectory
        // ==========================
        for (i = 0; i < Tra_cnt; i++) {
            if (Tra_X[i] > -900) {
                DrawSphere3D(
                    VGet((float)Tra_X[i], 0.1f, (float)Tra_Y[i]),
                    0.2f,
                    8,
                    GetColor(255, 0, 0),
                    GetColor(255, 0, 0),
                    TRUE
                );
            }
        }

        // ==========================
        // Draw car model
        // ==========================
        float model_x = (float)(Car_X - POSOFFSET * cos(Car_Yaw));
        float model_z = (float)(Car_Y - POSOFFSET * sin(Car_Yaw));

        MV1SetPosition(
            ModelHandle_CAR,
            VGet(model_x, 0.0f, model_z)
        );

        MV1SetRotationXYZ(
            ModelHandle_CAR,
            VGet(0.0f, (float)(PI - Car_Yaw), 0.0f)
        );

        MV1DrawModel(ModelHandle_CAR);

        DrawFormatString(
            10, 10, GetColor(255, 255, 0),
            "Time=%.2lf  Looptime Draw=%d  CAL=%d",
            etime, looptime_draw, looptime_cal
        );

        DrawFormatString(
            10, 50, GetColor(255, 255, 0),
            "Velocity=%.2lf km/h  Yaw Rate=%.2lf deg/s",
            g_vel_kmh, g_yaw_rate
        );

        DrawFormatString(
            10, 90, GetColor(255, 255, 0),
            "X=%.2lf m  Y=%.2lf m  Yaw Angle=%.2lf rad",
            Car_X, Car_Y, Car_Yaw
        );

        DrawFormatString(
            10, 140, GetColor(0, 255, 255),
            "Target Speed=%.2f km/h  Target Steer=%.3f rad",
            target_speed, target_steer
        );



        ScreenFlip();
    }

    DxLib_End();
    return 0;
}

// ==============================
// Calculation / Shared memory
// ==============================
void main2()
{
    timeB = GetTimer();
    etime += 0.025;

    // Read state from shared memory
    g_vel_kmh = sm[64];
    g_yaw_rate = sm[70];

    Car_X = sm[505];
    Car_Y = sm[506];
    Car_Yaw = sm[507];

    //// Control enable
    //sm[200] = 1;
    //sm[201] = 1;

    //sm[210] = 1;
    //sm[211] = 1;

    // ==========================
// Keyboard control
// ==========================

// Speed control
    //if (CheckHitKey(KEY_INPUT_W)) {
    //    target_speed += SPEED_STEP;
    //}
    //if (CheckHitKey(KEY_INPUT_S)) {
    //    target_speed -= SPEED_STEP;
    //}
    //if (CheckHitKey(KEY_INPUT_SPACE)) {
    //    target_speed = 0.0;
    //}

    //// Steering control
    //if (CheckHitKey(KEY_INPUT_A)) {
    //    target_steer += STEER_STEP;
    //}
    //if (CheckHitKey(KEY_INPUT_D)) {
    //    target_steer -= STEER_STEP;
    //}

    //// Reset steering when no key pressed
    //if (!CheckHitKey(KEY_INPUT_A) && !CheckHitKey(KEY_INPUT_D)) {
    //    target_steer *= 0.9;   // smooth return to center
    //}

    //// Clamp speed
    //if (target_speed > MAX_SPEED) target_speed = MAX_SPEED;
    //if (target_speed < MIN_SPEED) target_speed = MIN_SPEED;

    //// Clamp steering
    //if (target_steer > MAX_STEER) target_steer = MAX_STEER;
    //if (target_steer < -MAX_STEER) target_steer = -MAX_STEER;


    //sm[212] = target_steer;

    //sm[220] = 1;
    //sm[221] = 1;
    //sm[222] = target_speed;

    // ==========================
    // Trajectory logging (0.1 s)
    // ==========================
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
