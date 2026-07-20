#include <Adafruit_MCP3008.h>
#include <Servo.h>

// --- ハードウェア定義 ---
Adafruit_MCP3008 adc;
Servo servoL;
Servo servoR;
Servo arm; 
const int PIN_MOTOR_L = 5;
const int PIN_MOTOR_R = 4;

// --- 追加: 空車判定用 ---
const int PIN_IR_FRONT = A5;
const int EMPTY_THRESHOLD = 400;

// --- 調整用パラメータ ---
int baseSpeedL = 143; 
int baseSpeedR = 31;  

//float Kp_align = 0.10;
//align用のゲイン
float Kp_align = 0.04;
//P制御用のゲイン
float Kp = 0.05; 
//I制御用のゲイン
float Ki = 0.005; 
int CROSS_THRESHOLD = 3700; 

int v[6] = {0, 0, 0, 0, 0, 0}; 
long integral = 0;             

void setup() {
  Serial.begin(9600);
  adc.begin();
  
  servoL.attach(PIN_MOTOR_L);
  servoR.attach(PIN_MOTOR_R);
  arm.attach(8);
  arm.write(90);
  
  servoL.write(93);
  servoR.write(91);
  delay(2000); 
}

// ==========================================
// レイヤ1: ハードウェア抽象化 (HAL)
// ==========================================
void readLineSensors() {
  for (int i = 0; i < 6; i++) {
    v[i] = adc.readADC(i);
  }
  int sum = 0;
  for (int i = 0; i < 6; i++) {
    sum += v[i];
    //Serial.println(v[i]);
  }
/*
  Serial.println(v[2]);
  Serial.println(v[3]);
*/
  
}

void setMotors(float valL, float valR) {
  if(valL > 180) valL = 180;
  if(valL < 0)   valL = 0;
  if(valR > 180) valR = 180;
  if(valR < 0)   valR = 0;
  servoL.write(valL);
  servoR.write(valR);
}

// ==========================================
// レイヤ2: 制御ロジック
// ==========================================
//6連センサーの値を読み取り、ズレを検知する
//中央を0として左にずれると負をとり、右にずれると正をとる
int calculateError() {
  return (-3 * v[0]) + (-2 * v[1]) + (-1 * v[2]) + ( 1 * v[3]) + ( 2 * v[4]) + ( 3 * v[5]);
}

//計算したズレをPIによって抑制する
void traceLinePI() {
  int error = calculateError();
  integral += error;
  if(integral > 1000) integral = 1000;
  else if(integral < -1000) integral = -1000;
  
  float turn = (error * Kp) + (integral * Ki);
  //turnが正のとき右の回転数を上げる。turnが負のとき左の回転数を上げる。
  setMotors(baseSpeedL + turn, baseSpeedR + turn);
}

// 
void alignToLine() {
  //Serial.println("Start Alignment...");
  while (true) {
    readLineSensors();
    int error = calculateError();
    
    //errorの絶対値が5未満だったら停止
    if (abs(error) < 5) {
      setMotors(93, 91);
      //Serial.println("Alignment Complete!");
      break; 
    }
    
    //Kp_align = 0.03
    float turnSpeed = error * Kp_align;
    
    //振り幅の調整
    if (turnSpeed > 30) turnSpeed = 30;
    if (turnSpeed < -30) turnSpeed = -30;
    
    // 大きくずれているときと小さくずれているときで区別する
     if (error >=5 && turnSpeed < 5) turnSpeed = 5; 
     if (error <= -5 && turnSpeed > -5) turnSpeed = -5;

    /*
    int outL = 93;
    int outR = 91;
    if(abs(error) > 100){//誤差が大きいとき片輪だけ動かす
      if(error > 0){//ラインが右側にある時左車輪を動かす
        outL = 93 + turnSpeed;
      }else{//ラインが左側にある時右車輪を動かす
        outR = 91 + turnSpeed;
      }
     setMotors(outL, outR);
     delay(75);
    }else {//誤差が小さいときは両方の車輪を動かす
      outL = 93 + turnSpeed;
      outR = 91 + turnSpeed;
      setMotors(outL, outR);
      delay(75);
    }
  */

    setMotors(93 + turnSpeed, 91 + turnSpeed);
    delay(75);
  }
}

// 線が下に来るまで周り続ける
void turn90Right() {
  setMotors(110, 110); 
  delay(400); // 最初の線を抜ける
  
  while (true) {
    readLineSensors();
    //Serial.println(v[2]);
    //Serial.println(v[3]);
    if (v[2] > 400 && v[3] > 400) break;
    //delay(5);
  }
  
  setMotors(93, 91);
  delay(200);
}

void turn90Left() {
  setMotors(70, 70); 
  delay(400); // 最初の線を抜ける
  
  while (true) {
    readLineSensors();
    //Serial.println(v[2]);
    //Serial.println(v[3]);
    if (v[2] > 400 && v[3] > 400) break;
    //delay(10);
  }
  
  setMotors(93, 91);
  delay(200);
}


/*
void turn180() {
  Serial.println("turn180: Start (Delay version)");
  setMotors(150, 150); 
  // 90度のラインを無視して通り過ぎるため、長めに無条件で回る
  delay(1000); 
  
  Serial.println("turn180: Looking for 180 deg line...");
  // 後ろのラインを見つけるまで回り続ける
  while (true) {
    readLineSensors();
    if (v[0] > 500 || v[1] > 500) break;
    delay(10);
  }
  
  Serial.println("turn180: Line found! Stopping.");
  setMotors(93, 91);
  delay(200);
}
*/



void turn180() {
  //Serial.println("turn180: Start (Delay version)");
  turn90Left();
  delay(1500);
  alignToLine();
  delay(100);
  setMotors(87,98);
  delay(200);
  turn90Left();
  delay(500);
  alignToLine();
}


/*
// 【完全センサー駆動版】改善案（Step 2用）
// 時間(delay)に頼らず、ラインの「本数」を数えて正確に180度回ります
void turn180() {
  Serial.println("turn180: Start (State Machine version)");
  setMotors(150, 150); 
  
  Serial.println("turn180: Step 1 - Clearing 0 deg line...");
  // 1. 今いるライン(0度)からセンサーが外れるのを待つ（turn90と同じ）
  delay(300); 
  
  Serial.println("turn180: Step 2 - Finding 90 deg line...");
  // 2. 1本目のライン（横方向・90度）を見つけるまで回る
  while (true) {
    readLineSensors();
    if (v[2] > 500 || v[3] > 500) break;
    delay(10);
  }

  Serial.println("turn180: Step 3 - Clearing 90 deg line...");
  // 3. 1本目のラインを完全に通り過ぎる（センサーが白になる）まで回る
  while (true) {
    readLineSensors();
    if (v[2] < 300 && v[3] < 300) break;
    delay(10);
  }

  Serial.println("turn180: Step 4 - Finding 180 deg line...");
  // 4. 2本目のライン（後ろ方向・180度）を見つけるまで回る
  while (true) {
    readLineSensors();
    if (v[2] > 500 || v[3] > 500) break;
    delay(10);
  }
  
  Serial.println("turn180: Complete!");
  setMotors(93, 91);
  delay(200);
}
*/


// 空車判定関数（Step 2用）
bool isEmptySpot() {
  //全面センサーの値を取得
  int val = analogRead(PIN_IR_FRONT);
  //Serial.print("Front IR: ");
  //Serial.println(val);
  //EMPTY_THRESHOLD = 400
  //しきい値400では大体10cmぐらい
  return (val < EMPTY_THRESHOLD);
}

// ==========================================
// レイヤ3: 意思決定レイヤ
// ==========================================
bool isCrossroad() {
  int sum = 0;
  for (int i = 0; i < 6; i++) {
    sum += v[i];
  }
  // 曲線での誤検知を防ぐ確実な条件
  //CROSS_THRESHOLD =3000
  int sensor = 550;
  int senser_up = 800;
  if ( /* sum > CROSS_THRESHOLD && */ v[0] > sensor  && v[1] > sensor && v[2] > sensor && v[3] > sensor && v[4] > sensor && v[5] > sensor) {
    return true;
  }
  return false;
}

// ==========================================
// レイヤ4: メインループ (各ステップごとに1つのloop()を有効にしてください)
// ==========================================

// ▼▼▼ Step 1 テスト用: ライントレース -> 交差点検知 -> 90度右旋回 -> 姿勢補正 -> 停止 ▼▼▼
/*
void loop() {
  //下部センサー起動
  readLineSensors();
  if (isCrossroad()) {
    setMotors(93, 91);
    delay(500); // 安定待ち
    Serial.println("Crossroad Detected! Turning Right...");
    setMotors(98,87);
    delay(1200);
    turn90Right();
    setMotors(93, 91);
    delay(500); // 安定待ち
    
    alignToLine();
    
    Serial.println("Test 1 (Turn & Align) Complete.");
    
    Serial.println("All Tests Complete. Stopping.");
    setMotors(93, 91);
    while (true) { delay(100); } // テスト終了で永遠に停止
  }else {
    traceLinePI();
  }

  delay(10);
}
*/
// ▲▲▲ Step 1 はここまで ▲▲▲

// ▼▼▼ Step 2 テスト用: Step 1 + 空車判定 -> 180度旋回 -> バック駐車 -> 停止 ▼▼▼
/*
void loop() {
  readLineSensors();

  if (isCrossroad()) {
    setMotors(93, 91);
    delay(500); // 安定待ち
    Serial.println("Crossroad Detected! Turning Right...");
    setMotors(98,87);
    delay(1200);
    turn90Right();
    setMotors(93, 91);
    delay(500); // 安定待ち
    
    alignToLine();
    
    Serial.println("Test 1 (Turn & Align) Complete.");
    
    Serial.println(">>> CHECKING SPOT <<<");
    setMotors(93, 91);
    delay(2000); // 【区切り】動作確認用に2秒停止

    if (isEmptySpot()) {
      Serial.println("Spot is EMPTY! Starting Parking Sequence...");
      int spotVal = analogRead(PIN_IR_FRONT);
      
      // 180度旋回
      Serial.println(">>> TURNING 180 <<<");
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      turn180();
      setMotors(93, 91);
      delay(500);
      alignToLine();
      
      Serial.println(">>> READY TO BACK UP <<<");
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      
      // バック計算
      float D = 0.00224 * spotVal * spotVal - 2.2585 * spotVal + 654.5;
      float targetDistance = D ;//- 180.0; 
      int backTimeMs = targetDistance * 6.0;
      //係数は調整必須（モータの速度に依存するのとその速度での単位時間あたりの進む距離が必要） 
      if (backTimeMs < 0) backTimeMs = 0;
      
      Serial.print("Target Dist: "); Serial.println(targetDistance);
      Serial.print("Back Time: "); Serial.println(backTimeMs);
      
      Serial.println(">>> REVERSING <<<");
      setMotors(30, 140); // バック出力
      delay(backTimeMs);
      
      Serial.println(">>> PARKING COMPLETE! <<<");
    } else {
      Serial.println("Spot is FULL.");
    }
    
    Serial.println("All Tests Complete. Stopping.");
    setMotors(93, 91);
    while (true) { delay(100); } // テスト終了で永遠に停止
  } else {
    traceLinePI();
  }
  
  delay(10);
}
*/
// ▲▲▲ Step 2 はここまで ▲▲▲

// ▼▼▼ Step 3 テスト用: Step 2 + 駐車完了後 -> 枠脱出 -> コース復帰 -> トレース再開 ▼▼▼

void loop() {
  readLineSensors();

  if (isCrossroad()) {
    setMotors(93, 91);
    delay(500); // 安定待ち
    //Serial.println("Crossroad Detected! Turning Right...");
    setMotors(98,87);
    delay(1200);
    turn90Right();
    setMotors(93, 91);
    delay(500); // 安定待ち
    
    alignToLine();
    
    //Serial.println("Test 1 (Turn & Align) Complete.");
    
    //Serial.println(">>> CHECKING SPOT <<<");
    setMotors(93, 91);
    delay(2000); // 【区切り】動作確認用に2秒停止

    if (isEmptySpot()) {
      //Serial.println("Spot is EMPTY! Starting Parking Sequence...");
      int spotVal = analogRead(PIN_IR_FRONT);
      
      // 180度旋回
      //Serial.println(">>> TURNING 180 <<<");
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      setMotors(87,98);
      delay(200);
      turn180();
      setMotors(93, 91);
      delay(500);
      alignToLine();
      
      //Serial.println(">>> READY TO BACK UP <<<");
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      
      // バック計算
      float D = 0.00224 * spotVal * spotVal - 2.2585 * spotVal + 654.5;
      float targetDistance = D ;//- 180.0; 
      int backTimeMs = targetDistance * 6.3;
      //係数は調整必須（モータの速度に依存するのとその速度での単位時間あたりの進む距離が必要） 
      if (backTimeMs < 0) backTimeMs = 0;
      
      //Serial.print("Target Dist: "); Serial.println(targetDistance);
      //Serial.print("Back Time: "); Serial.println(backTimeMs);
      
      //Serial.println(">>> REVERSING <<<");
      setMotors(36, 143); // バック出力
      delay(backTimeMs);
      
      //Serial.println(">>> PARKING COMPLETE! <<<");
      
      //Serial.println(">>> WAITING 3 SECONDS... <<<");
      setMotors(93, 91);
      delay(3000); // 駐車完了のアピール時間
      
      //Serial.println(">>> LEAVING SPOT (MOVING FORWARD) <<<");
      setMotors(baseSpeedL, baseSpeedR); // 前進して交差点を探す
      
      // 交差点(メインライン)を見つけるまで前進し続ける
      
      while (isCrossroad() == false) {
        readLineSensors();
        traceLinePI();
        delay(10);
      }
      
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      //Serial.println(">>> BACK ON MAIN LINE. TURNING RIGHT TO RESUME <<<");
      
      // 右枠にバック駐車した状態から前進してきたため、今はメインラインに対して「左」を向いています。
      // 元の進行方向を向くには「右」に90度旋回します。
      setMotors(98,87);
      delay(800);
      turn90Right();
      setMotors(93, 91);
      delay(500);
      
      alignToLine();
      
      //Serial.println(">>> READY TO RESUME TRACE <<<");
      setMotors(93, 91);
      delay(2000); // 【区切り】動作確認用に2秒停止
      
      // 旋回直後に今いる交差点を再び誤検知しないよう、少しだけ強制前進（不感時間）
      setMotors(baseSpeedL, baseSpeedR);
      delay(300);
      
      //Serial.println(">>> STEP 3 COMPLETE. RESUMING TRACE! <<<");
    } else {
      //Serial.println("Spot is FULL.");
      //Serial.println("All Tests Complete. Stopping.");
      turn90Left();
      delay(200);
    }
  } else {
    traceLinePI();
  }
  
  //delay(10);
}
// ▲▲▲ Step 3 はここまで ▲▲▲

// ▼▼▼ バック駐車専用テスト用 (機体を駐車枠の前に前向きに置き、開始) ▼▼▼
/*
void loop() {
  Serial.println(">>> BACK PARKING TEST STARTING IN 3 SECONDS... <<<");
  delay(3000); // 手を離すための猶予時間
  
  // 1. 前向きでセンサ値を読み取る
  int spotVal = analogRead(PIN_IR_FRONT);
  Serial.print("Front IR (Distance Sensor): "); Serial.println(spotVal);
  
  // 2. 180度旋回して後ろを向く
  Serial.println(">>> TURNING 180 <<<");
  turn180();
  setMotors(93, 91);
  delay(500);
  alignToLine(); // 姿勢補正
  
  Serial.println(">>> READY TO BACK UP <<<");
  setMotors(93, 91);
  delay(2000); // 【区切り】動作確認用に2秒停止
  
  // 3. バック計算
  float D = 0.00224 * spotVal * spotVal - 2.2585 * spotVal + 654.5;
  float targetDistance = D - 190.0; 
  int backTimeMs = targetDistance * 10.0;
  
  if (backTimeMs < 0) backTimeMs = 0;
  
  Serial.print("Target Dist: "); Serial.println(targetDistance);
  Serial.print("Back Time: "); Serial.println(backTimeMs);
  
  // 4. バック
  Serial.println(">>> REVERSING <<<");
  setMotors(30, 140); // バック出力
  delay(backTimeMs);
  
  Serial.println(">>> PARKING COMPLETE! <<<");
  setMotors(93, 91);
  
  while (true) { delay(100); } // 永遠に停止
}
*/
// ▲▲▲ バック駐車専用テスト はここまで ▲▲▲
